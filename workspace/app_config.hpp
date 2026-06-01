#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#include <workspace/types.hpp>

namespace config {
    inline constexpr wchar_t k_default_process_name[]       = L"HD-Player.exe";
    inline constexpr wchar_t k_default_module_name[]        = L"BstkVMM.dll";
    inline constexpr u32     k_remote_thread_timeout_ms     = 5000;
    inline constexpr usize   k_default_dump_size            = 64;
    inline constexpr usize   k_default_string_character_cap = 256;
}

#endif
