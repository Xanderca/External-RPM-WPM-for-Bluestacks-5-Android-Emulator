#ifndef GVA_MEMORY_BRIDGE_CPP
#define GVA_MEMORY_BRIDGE_CPP

#include <impl/includes.hpp>

namespace mem {

    void c_gva_memory_bridge::attach(c_memory_engine* memory_engine, u64 guest_cr3)
    {
        memory_engine_value = memory_engine;
        guest_cr3_value     = guest_cr3;
    }

    bool c_gva_memory_bridge::is_ready() const
    {
        return memory_engine_value != nullptr && guest_cr3_value != 0;
    }

    u64 c_gva_memory_bridge::cr3() const
    {
        return guest_cr3_value;
    }

    bool c_gva_memory_bridge::read_array(u64 guest_va, void* buffer, usize byte_count, bool verbose) const
    {
        if (!is_ready() || buffer == nullptr || byte_count == 0)
            return false;

        return memory_engine_value->read_gva(guest_cr3_value, guest_va, buffer, byte_count, verbose);
    }

    bool c_gva_memory_bridge::write_array(u64 guest_va, const void* buffer, usize byte_count, bool verbose) const
    {
        if (!is_ready() || buffer == nullptr || byte_count == 0)
            return false;

        return memory_engine_value->write_gva(guest_cr3_value, guest_va, buffer, byte_count, verbose);
    }

    std::string c_gva_memory_bridge::read_string(u64 guest_va, s32 size, bool unicode, bool verbose) const
    {
        if (!is_ready() || guest_va == 0 || size <= 0)
            return { };

        std::vector<u8> string_bytes(static_cast<usize>(size), 0);
        if (!read_array(guest_va, string_bytes.data(), string_bytes.size(), verbose))
            return { };

        if (!unicode) {
            std::string result(reinterpret_cast<const char*>(string_bytes.data()), string_bytes.size());
            const auto terminator = result.find('\0');
            if (terminator != std::string::npos) {
                result.resize(terminator);
            }
            return utility::sanitize_guest_string(std::move(result));
        }

        const auto* wide_ptr = reinterpret_cast<const wchar_t*>(string_bytes.data());
        const auto wide_len  = size / static_cast<s32>(sizeof(wchar_t));
        if (wide_len <= 0)
            return { };

        const auto utf8_length = WideCharToMultiByte(CP_UTF8, 0, wide_ptr, wide_len, nullptr, 0, nullptr, nullptr);
        if (utf8_length <= 0)
            return { };

        std::string output(static_cast<usize>(utf8_length), '\0');
        WideCharToMultiByte(CP_UTF8, 0, wide_ptr, wide_len, output.data(), utf8_length, nullptr, nullptr);

        const auto terminator = output.find('\0');
        if (terminator != std::string::npos) {
            output.resize(terminator);
        }

        return utility::sanitize_guest_string(std::move(output));
    }

    void c_gva_memory_bridge::clear_translation_caches() const
    {
        if (memory_engine_value != nullptr) {
            memory_engine_value->clear_runtime_caches();
        }
    }
}

#endif
