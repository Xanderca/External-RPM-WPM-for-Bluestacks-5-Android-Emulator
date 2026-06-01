#ifndef KERNEL_STRUCTS_HPP
#define KERNEL_STRUCTS_HPP


#include <cstddef>
#include <string>

#include <workspace/types.hpp>

namespace offsets {
    constexpr u64 linux_direct_map_base        = 0xFFFF888000000000ULL;
    constexpr u64 linux_direct_map_end         = 0xFFFFC87FFFFFFFFFULL;
    constexpr u64 linux_legacy_direct_map_base = 0xFFFF880000000000ULL;
    constexpr u64 linux_legacy_direct_map_end  = 0xFFFFC7FFFFFFFFFFULL;
    constexpr u64 linux_kernel_image_base      = 0xFFFFFFFF80000000ULL;
    constexpr u64 linux_kernel_image_end       = 0xFFFFFFFFFF000000ULL;
}

namespace kernel {

    inline constexpr u64 k_init_task = 0xFFFFFFFF822147C0ULL;

    struct c_guest_list_head {
        u64 next { };
        u64 prev { };
    };

    struct c_guest_qstr {
        u32 hash { };
        u32 len  { };
        u64 name { };
    };

    struct c_guest_path {
        u64 mnt    { };
        u64 dentry { };
    };

    struct c_guest_vfs_mount {
        u64 mnt_root  { };
        u64 mnt_sb    { };
        s32 mnt_flags { };
    };

    struct c_guest_mount {
        u8                pad_0[0x10] { };
        u64               mnt_parent   { };
        u64               mnt_mountpoint { };
        c_guest_vfs_mount mnt          { };
    };

    struct c_guest_dentry {
        u8           pad_0[0x18] { };
        u64          d_parent    { };
        c_guest_qstr d_name      { };
        u64          d_inode     { };
        char         d_iname[32] { };
        u8           pad_1[0x08] { };
        u64          d_op        { };
    };

    struct c_guest_file {
        u8           pad_0[0x10] { };
        c_guest_path f_path      { };
        u64          f_inode     { };
        u64          f_op        { };
        u8           pad_1[0x08] { };
        u64          f_count     { };
        u32          f_flags     { };
        u32          f_mode      { };
        u8           pad_2[0x10] { };
        u64          f_pos       { };
    };

    struct c_guest_inode {
        u16 i_mode      { };
        u8  pad_0[0x1E] { };
        u64 i_op        { };
        u64 i_sb        { };
        u8  pad_1[0x10] { };
        u64 i_ino       { };
    };

    struct c_guest_mm_struct {
        u64 mmap         { };
        u64 mm_rb_root   { };
        u8  pad_0[0x40]  { };
        u64 pgd          { };
        u8  pad_1[0xE0]  { };
        u64 arg_start    { };
        u64 arg_end      { };
        u64 env_start    { };
        u64 env_end      { };
        u8  pad_2[0x250] { };
        u64 exe_file     { };
    };

    struct c_guest_task_struct {
        u8                pad_0[0x470] { };
        c_guest_list_head tasks         { };
        u8                pad_1[0x40]  { };
        u64               mm            { };
        u64               active_mm     { };
        u8                pad_2[0xA0]  { };
        u32               pid           { };
        u32               tgid          { };
        u64               real_parent   { };
        u64               parent        { };
        u8                pad_3[0x188] { };
        u64               real_cred     { };
        u64               cred          { };
        char              comm[16]      { };
    };
}

namespace utility {
    usize bounded_str_len(const char* text, usize limit);
    std::string hex(u64 value);
    std::string hex_lower(u64 value, usize min_width = 0);
    std::string sanitize_guest_string(std::string value);
    bool equals_case_insensitive(std::string_view left, std::string_view right);
    bool contains_case_insensitive(std::string_view haystack, std::string_view needle);
    bool matches_task_name(std::string_view task_name, std::string_view package_name);
    bool matches_library_name(std::string_view library_name, std::string_view wanted_library_name);
    bool try_convert_kernel_virtual_to_physical(u64 kernel_va, u64& out_phys);
    bool is_likely_guest_kernel_pointer(u64 value);
}

#endif
