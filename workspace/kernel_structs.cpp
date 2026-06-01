#ifndef KERNEL_STRUCTS_CPP
#define KERNEL_STRUCTS_CPP

#include <impl/includes.hpp>

static_assert(sizeof(void*) == 8, "Xander-external requires x64");
static_assert(offsetof(kernel::c_guest_dentry, d_name) == 0x20, "c_guest_dentry::d_name offset mismatch");
static_assert(offsetof(kernel::c_guest_dentry, d_inode) == 0x30, "c_guest_dentry::d_inode offset mismatch");
static_assert(offsetof(kernel::c_guest_file, f_path) == 0x10, "c_guest_file::f_path offset mismatch");
static_assert(offsetof(kernel::c_guest_file, f_count) == 0x38, "c_guest_file::f_count offset mismatch");
static_assert(offsetof(kernel::c_guest_inode, i_ino) == 0x40, "c_guest_inode::i_ino offset mismatch");
static_assert(offsetof(kernel::c_guest_mm_struct, pgd) == 0x50, "c_guest_mm_struct::pgd offset mismatch");
static_assert(offsetof(kernel::c_guest_mm_struct, arg_start) == 0x138, "c_guest_mm_struct::arg_start offset mismatch");
static_assert(offsetof(kernel::c_guest_mm_struct, env_end) == 0x150, "c_guest_mm_struct::env_end offset mismatch");
static_assert(offsetof(kernel::c_guest_mm_struct, exe_file) == 0x3A8, "c_guest_mm_struct::exe_file offset mismatch");
static_assert(offsetof(kernel::c_guest_task_struct, tasks) == 0x470, "c_guest_task_struct::tasks offset mismatch");
static_assert(offsetof(kernel::c_guest_task_struct, mm) == 0x4C0, "c_guest_task_struct::mm offset mismatch");
static_assert(offsetof(kernel::c_guest_task_struct, pid) == 0x570, "c_guest_task_struct::pid offset mismatch");
static_assert(offsetof(kernel::c_guest_task_struct, comm) == 0x720, "c_guest_task_struct::comm offset mismatch");

namespace utility {

    usize bounded_str_len(const char* text, usize limit)
    {
        usize length { };
        while (length < limit && text[length] != '\0') {
            ++length;
        }
        return length;
    }

    std::string hex(u64 value)
    {
        std::ostringstream output { };
        output << "0x" << std::hex << std::uppercase << value;
        return output.str();
    }

    std::string hex_lower(u64 value, usize min_width)
    {
        std::ostringstream output { };
        output << std::hex << std::nouppercase;
        if (min_width != 0) {
            output << std::setw(static_cast<int>(min_width)) << std::setfill('0');
        }
        output << value;
        return output.str();
    }

    std::string sanitize_guest_string(std::string value)
    {
        value.erase(std::remove_if(value.begin(), value.end(), [](unsigned char character) {
            return character == '\0' || character == '\r' || character == '\n';
        }), value.end());
        return value;
    }

    bool equals_case_insensitive(std::string_view left, std::string_view right)
    {
        if (left.size() != right.size())
            return false;

        for (usize index = 0; index < left.size(); ++index) {
            const auto left_character  = static_cast<unsigned char>(left[index]);
            const auto right_character = static_cast<unsigned char>(right[index]);
            if (std::tolower(left_character) != std::tolower(right_character))
                return false;
        }

        return true;
    }

    bool contains_case_insensitive(std::string_view haystack, std::string_view needle)
    {
        if (needle.empty())
            return true;

        if (needle.size() > haystack.size())
            return false;

        for (usize index = 0; index + needle.size() <= haystack.size(); ++index) {
            if (equals_case_insensitive(haystack.substr(index, needle.size()), needle))
                return true;
        }

        return false;
    }

    bool matches_task_name(std::string_view task_name, std::string_view package_name)
    {
        if (package_name.empty())
            return true;

        if (equals_case_insensitive(task_name, package_name))
            return true;

        if (contains_case_insensitive(task_name, package_name))
            return true;

        if (task_name.size() >= 6 && contains_case_insensitive(package_name, task_name))
            return true;

        if (task_name.size() == 15 && package_name.size() >= task_name.size())
            return equals_case_insensitive(task_name, package_name.substr(0, task_name.size()));

        const auto last_dot = package_name.rfind('.');
        if (last_dot != std::string_view::npos && last_dot + 1 < package_name.size()) {
            const auto leaf = package_name.substr(last_dot + 1);
            if (equals_case_insensitive(task_name, leaf))
                return true;

            if (contains_case_insensitive(task_name, leaf))
                return true;

            if (contains_case_insensitive(leaf, task_name))
                return true;
        }

        if (package_name.size() < task_name.size())
            return equals_case_insensitive(task_name.substr(0, package_name.size()), package_name);

        return false;
    }

    bool matches_library_name(std::string_view library_name, std::string_view wanted_library_name)
    {
        if (wanted_library_name.empty())
            return true;

        if (library_name.empty())
            return false;

        if (equals_case_insensitive(library_name, wanted_library_name))
            return true;

        return contains_case_insensitive(library_name, wanted_library_name);
    }

    bool try_convert_kernel_virtual_to_physical(u64 kernel_va, u64& out_phys)
    {
        if (kernel_va >= offsets::linux_direct_map_base && kernel_va <= offsets::linux_direct_map_end) {
            out_phys = kernel_va - offsets::linux_direct_map_base;
            return true;
        }

        if (kernel_va >= offsets::linux_legacy_direct_map_base && kernel_va <= offsets::linux_legacy_direct_map_end) {
            out_phys = kernel_va - offsets::linux_legacy_direct_map_base;
            return true;
        }

        if (kernel_va >= offsets::linux_kernel_image_base && kernel_va < offsets::linux_kernel_image_end) {
            out_phys = kernel_va - offsets::linux_kernel_image_base;
            return true;
        }

        return false;
    }

    bool is_likely_guest_kernel_pointer(u64 value)
    {
        return value >= 0xFFFF800000000000ULL;
    }
}

#endif
