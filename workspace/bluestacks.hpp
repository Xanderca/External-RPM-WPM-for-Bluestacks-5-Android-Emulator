#ifndef BLUESTACKS_HPP
#define BLUESTACKS_HPP

#include <memory>
#include <string>

#include <workspace/memory.hpp>
#include <workspace/types.hpp>

namespace bstk {

    class c_bluestacks {
    public:
        c_bluestacks() = default;
        ~c_bluestacks() = default;

    public:
        bool setup(const std::string& package_name, bool verbose = false);
        void destroy();

    public:
        u64 get_module_base(const std::string& module_name, usize map_index = 1, bool verbose = false);
        u64 get_cr3() const;

    private:
        struct c_task {
            u64         task    { };
            u64         mm      { };
            u32         pid     { };
            u32         tgid    { };
            std::string comm    { };
            std::string cmdline { };
        };

    private:
        bool read_task(u64 task, c_task& out_task, bool verbose);
        std::string read_task_comm(u64 task, bool verbose);
        std::string read_task_cmdline(u64 mm, u64 guest_cr3, bool verbose);
        std::string read_kernel_string(u64 address, usize max_length, bool verbose);
        std::string read_user_string(u64 guest_cr3, u64 address, usize max_length, bool verbose);
        std::string read_dentry_name(u64 dentry, bool verbose);
        std::string read_file_name(u64 file, bool verbose);
        s32 score_task(const c_task& task, const std::string& package_name) const;
        bool find_task(const std::string& package_name, c_task& out_task, bool verbose);

    private:
        std::string package_name_value { };
        c_task      task_value         { };
        u64         guest_cr3_value    { };
    };
}

inline auto bluestacks = std::make_shared<bstk::c_bluestacks>();

#endif
