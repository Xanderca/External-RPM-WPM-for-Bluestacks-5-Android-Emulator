#ifndef GVA_MEMORY_BRIDGE_HPP
#define GVA_MEMORY_BRIDGE_HPP


#include <string>
#include <type_traits>

#include <workspace/memory_engine.hpp>
#include <workspace/types.hpp>

namespace mem {

    class c_gva_memory_bridge {
    public:
        c_gva_memory_bridge() = default;
        ~c_gva_memory_bridge() = default;

    public:
        void attach(c_memory_engine* memory_engine, u64 guest_cr3);
        bool is_ready() const;
        u64 cr3() const;

    public:
        template <typename t_type>
        t_type read(u64 guest_va, bool verbose = false) const
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            t_type result { };
            if (!read(guest_va, result, verbose))
                return t_type { };

            return result;
        }

        template <typename t_type>
        bool read(u64 guest_va, t_type& out_value, bool verbose = false) const
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            if (!is_ready())
                return false;

            return memory_engine_value->read_gva(guest_cr3_value, guest_va, out_value, verbose);
        }

        template <typename t_type>
        bool write(u64 guest_va, const t_type& value, bool verbose = false) const
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            if (!is_ready())
                return false;

            return memory_engine_value->write_gva(guest_cr3_value, guest_va, value, verbose);
        }

        bool read_array(u64 guest_va, void* buffer, usize byte_count, bool verbose = false) const;
        bool write_array(u64 guest_va, const void* buffer, usize byte_count, bool verbose = false) const;
        std::string read_string(u64 guest_va, s32 size, bool unicode, bool verbose = false) const;
        void clear_translation_caches() const;

    private:
        c_memory_engine* memory_engine_value { };
        u64 guest_cr3_value { };
    };
}

#endif
