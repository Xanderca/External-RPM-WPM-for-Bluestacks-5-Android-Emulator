#ifndef BLUESTACKS_CPP
#define BLUESTACKS_CPP

#include <impl/includes.hpp>

namespace offsets {
    constexpr u64 vma_vm_start = 0x0;
    constexpr u64 vma_vm_next  = 0x10;
    constexpr u64 vma_vm_file  = 0xA0;
}

namespace {

    void log_verbose(bool verbose, const std::string& message)
    {
        if (verbose) {
            std::cout << message << '\n';
        }
    }
}

namespace bstk {

    bool c_bluestacks::setup(const std::string& package_name, bool verbose)
    {
        package_name_value = package_name;
        task_value         = { };
        guest_cr3_value    = 0;

        constexpr s32 k_setup_attempts = 20;
        constexpr u32 k_setup_delay_ms = 500;

        memory->destroy();

        for (s32 attempt = 0; attempt < k_setup_attempts; ++attempt) {
            log_verbose(verbose, "[bstk] setup attempt " + std::to_string(attempt + 1));
            memory->clear_cr3();

            if (memory->setup(verbose)) {
                log_verbose(verbose, "[bstk] memory setup ok");
                if (find_task(package_name, task_value, verbose)) {
                    log_verbose(verbose, "[bstk] task ok pid=" + std::to_string(task_value.pid) + " mm=" + utility::hex(task_value.mm) + " comm=" + task_value.comm + " cmdline=" + task_value.cmdline);
                    guest_cr3_value = memory->get_cr3(task_value.mm, verbose);
                    if (guest_cr3_value != 0) {
                        log_verbose(verbose, "[bstk] cr3=" + utility::hex(guest_cr3_value));
                        memory->set_cr3(guest_cr3_value);
                        return true;
                    }

                    log_verbose(verbose, "[bstk] cr3 read failed");
                } else {
                    log_verbose(verbose, "[bstk] task not found");
                }
            } else {
                log_verbose(verbose, "[bstk] memory setup failed");
            }

            memory->destroy();

            if (attempt + 1 < k_setup_attempts) {
                Sleep(k_setup_delay_ms);
            }
        }

        package_name_value.clear();
        task_value      = { };
        guest_cr3_value = 0;
        return false;
    }

    void c_bluestacks::destroy()
    {
        package_name_value.clear();
        task_value      = { };
        guest_cr3_value = 0;
        memory->destroy();
    }

    u64 c_bluestacks::get_cr3() const
    {
        return guest_cr3_value;
    }

    u64 c_bluestacks::get_module_base(const std::string& module_name, usize map_index, bool verbose)
    {
        if (task_value.mm == 0 || guest_cr3_value == 0 || map_index == 0)
            return 0;

        constexpr usize k_max_vmas = 8192;

        u64 current_vma { };
        if (!memory->read_kernel(task_value.mm + offsetof(kernel::c_guest_mm_struct, mmap), current_vma, verbose))
            return 0;

        std::vector<u64> seen_vmas     { };
        std::vector<u64> module_bases  { };
        u64 matched_file  { };

        while (current_vma != 0 && seen_vmas.size() < k_max_vmas) {
            if (std::find(seen_vmas.begin(), seen_vmas.end(), current_vma) != seen_vmas.end()) {
                break;
            }

            seen_vmas.push_back(current_vma);

            u64 vm_start { };
            u64 vm_next { };
            u64 vm_file { };

            if (!memory->read_kernel(current_vma + offsets::vma_vm_start, vm_start, verbose) || !memory->read_kernel(current_vma + offsets::vma_vm_next, vm_next, verbose) || !memory->read_kernel(current_vma + offsets::vma_vm_file, vm_file, verbose)) {
                break;
            }

            if (vm_file != 0) {
                const auto file_name = read_file_name(vm_file, verbose);
                const bool file_match = utility::matches_library_name(file_name, module_name);
                if (file_match && matched_file == 0) {
                    matched_file = vm_file;
                }

                if (file_match || (matched_file != 0 && vm_file == matched_file)) {
                    if (std::find(module_bases.begin(), module_bases.end(), vm_start) == module_bases.end()) {
                        module_bases.push_back(vm_start);
                    }
                }
            }

            if (vm_next == 0 || vm_next == current_vma) {
                break;
            }

            current_vma = vm_next;
        }

        if (module_bases.size() < map_index)
            return 0;

        return module_bases[map_index - 1];
    }

    bool c_bluestacks::read_task(u64 task, c_task& out_task, bool verbose)
    {
        out_task.task = task;

        if (!memory->read_kernel(task + offsetof(kernel::c_guest_task_struct, mm), out_task.mm, verbose) ||
            !memory->read_kernel(task + offsetof(kernel::c_guest_task_struct, pid), out_task.pid, verbose) ||
            !memory->read_kernel(task + offsetof(kernel::c_guest_task_struct, tgid), out_task.tgid, verbose)) {
            return false;
        }

        out_task.comm = read_task_comm(task, verbose);
        if (out_task.comm.empty())
            return false;

        if (out_task.mm != 0 && utility::is_likely_guest_kernel_pointer(out_task.mm)) {
            const auto task_cr3 = memory->get_cr3(out_task.mm, verbose);
            if (task_cr3 != 0) {
                out_task.cmdline = read_task_cmdline(out_task.mm, task_cr3, verbose);
            }
        }

        return true;
    }

    std::string c_bluestacks::read_task_comm(u64 task, bool verbose)
    {
        char comm[16] { };
        if (!memory->read_kernel(task + offsetof(kernel::c_guest_task_struct, comm), comm, sizeof(comm), verbose))
            return { };

        return utility::sanitize_guest_string(std::string(comm, utility::bounded_str_len(comm, sizeof(comm))));
    }

    std::string c_bluestacks::read_task_cmdline(u64 mm, u64 guest_cr3, bool verbose)
    {
        u64 arg_start { };
        u64 arg_end   { };
        if (!memory->read_kernel(mm + offsetof(kernel::c_guest_mm_struct, arg_start), arg_start, verbose) ||
            !memory->read_kernel(mm + offsetof(kernel::c_guest_mm_struct, arg_end), arg_end, verbose) ||
            arg_start == 0 ||
            arg_end == 0 ||
            arg_end <= arg_start) {
            return { };
        }

        auto span = arg_end - arg_start;
        if (span > 0x2000ULL) {
            span = 0x2000ULL;
        }

        if (span == 0)
            return { };

        std::vector<char> data(static_cast<usize>(span), '\0');
        if (!memory->read_user(guest_cr3, arg_start, data.data(), data.size(), verbose))
            return { };

        return utility::sanitize_guest_string(std::string(data.data(), utility::bounded_str_len(data.data(), data.size())));
    }

    std::string c_bluestacks::read_kernel_string(u64 address, usize max_length, bool verbose)
    {
        if (address == 0 || max_length == 0)
            return { };

        std::vector<char> data(max_length + 1, '\0');
        if (!memory->read_kernel(address, data.data(), max_length, verbose))
            return { };

        return utility::sanitize_guest_string(std::string(data.data(), utility::bounded_str_len(data.data(), max_length)));
    }

    std::string c_bluestacks::read_user_string(u64 guest_cr3, u64 address, usize max_length, bool verbose)
    {
        if (address == 0 || max_length == 0)
            return { };

        std::vector<char> data(max_length + 1, '\0');
        if (!memory->read_user(guest_cr3, address, data.data(), max_length, verbose))
            return { };

        return utility::sanitize_guest_string(std::string(data.data(), utility::bounded_str_len(data.data(), max_length)));
    }

    std::string c_bluestacks::read_dentry_name(u64 dentry, bool verbose)
    {
        kernel::c_guest_dentry guest_dentry { };
        if (!memory->read_kernel(dentry, guest_dentry, verbose))
            return { };

        if (guest_dentry.d_name.len > 0 && guest_dentry.d_name.len <= 256 && guest_dentry.d_name.name != 0) {
            const auto exact_name = read_kernel_string(guest_dentry.d_name.name, guest_dentry.d_name.len, verbose);
            if (!exact_name.empty())
                return exact_name;
        }

        return utility::sanitize_guest_string(std::string(guest_dentry.d_iname, utility::bounded_str_len(guest_dentry.d_iname, sizeof(guest_dentry.d_iname))));
    }

    std::string c_bluestacks::read_file_name(u64 file, bool verbose)
    {
        kernel::c_guest_file guest_file { };
        if (!memory->read_kernel(file, guest_file, verbose))
            return { };

        if (guest_file.f_path.dentry == 0)
            return { };

        return read_dentry_name(guest_file.f_path.dentry, verbose);
    }

    s32 c_bluestacks::score_task(const c_task& task, const std::string& package_name) const
    {
        if (package_name.empty())
            return 1;

        s32 score { };

        if (!task.cmdline.empty()) {
            if (utility::equals_case_insensitive(task.cmdline, package_name)) {
                score = (std::max)(score, 100);
            } else if (utility::contains_case_insensitive(task.cmdline, package_name)) {
                score = (std::max)(score, 90);
            } else if (utility::contains_case_insensitive(package_name, task.cmdline)) {
                score = (std::max)(score, 80);
            }
        }

        if (utility::equals_case_insensitive(task.comm, package_name)) {
            score = (std::max)(score, 60);
        } else if (utility::matches_task_name(task.comm, package_name)) {
            score = (std::max)(score, 50);
        }

        if (task.pid == task.tgid && score > 0) {
            ++score;
        }

        return score;
    }

    bool c_bluestacks::find_task(const std::string& package_name, c_task& out_task, bool verbose)
    {
        constexpr usize k_max_tasks = 4096;

        const auto init_tasks_head = kernel::k_init_task + offsetof(kernel::c_guest_task_struct, tasks);

        u64 init_tasks_next { };
        u64 init_tasks_prev { };
        if (!memory->read_kernel(init_tasks_head + offsetof(kernel::c_guest_list_head, next), init_tasks_next, verbose) ||
            !memory->read_kernel(init_tasks_head + offsetof(kernel::c_guest_list_head, prev), init_tasks_prev, verbose)) {
            return false;
        }

        std::vector<u64> seen_heads { };
        c_task best_task { };
        s32 best_score { };

        auto walk = [&](u64 start_head, bool reverse) {
            u64 current_head = start_head;

            while (current_head != 0 && current_head != init_tasks_head && seen_heads.size() < k_max_tasks) {
                if (current_head < offsetof(kernel::c_guest_task_struct, tasks)) {
                    break;
                }

                if (std::find(seen_heads.begin(), seen_heads.end(), current_head) != seen_heads.end()) {
                    break;
                }

                seen_heads.push_back(current_head);

                c_task current_task { };
                if (read_task(current_head - offsetof(kernel::c_guest_task_struct, tasks), current_task, verbose) &&
                    current_task.mm != 0 &&
                    utility::is_likely_guest_kernel_pointer(current_task.mm)) {
                    const auto current_score = score_task(current_task, package_name);
                    if (current_score > best_score) {
                        log_verbose(verbose, "[bstk] best task pid=" + std::to_string(current_task.pid) + " score=" + std::to_string(current_score) + " comm=" + current_task.comm + " cmdline=" + current_task.cmdline);
                        best_score = current_score;
                        best_task  = current_task;
                    }
                }

                u64 next_head { };
                const auto link_offset = reverse ? offsetof(kernel::c_guest_list_head, prev) : offsetof(kernel::c_guest_list_head, next);
                if (!memory->read_kernel(current_head + link_offset, next_head, false)) {
                    break;
                }

                if (next_head == 0 || next_head == current_head) {
                    break;
                }

                current_head = next_head;
            }
        };

        walk(init_tasks_next, false);
        walk(init_tasks_prev, true);

        if (best_score == 0 || best_task.mm == 0 || !utility::is_likely_guest_kernel_pointer(best_task.mm))
            return false;

        out_task = best_task;
        return true;
    }
}

#endif
