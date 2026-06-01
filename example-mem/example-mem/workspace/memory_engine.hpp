#ifndef MEMORY_ENGINE_HPP
#define MEMORY_ENGINE_HPP


#include <Windows.h>

#include <optional>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <unordered_map>

#include <workspace/kernel_structs.hpp>
#include <workspace/types.hpp>

namespace mem {

    struct c_module_info {
        std::wstring name { };
        std::wstring path { };
        u64          base { };
        u32          size { };
    };

    struct c_vm_info {
        u64 pvm          { };
        u64 pvcpu_0      { };
        u32 cpu_count    { };
        u64 read_counter { };
        u64 region_size  { };
    };

    struct c_translation_result {
        u64 guest_physical { };
        u64 host_pointer   { };
    };

    class c_memory_engine {
    public:
        c_memory_engine() = default;
        ~c_memory_engine();

    public:
        c_memory_engine(const c_memory_engine&) = delete;
        c_memory_engine& operator=(const c_memory_engine&) = delete;

    public:
        bool initialize(bool verbose = false);
        void shutdown();
        bool is_initialized() const;

    public:
        HANDLE process() const;
        u32 process_id() const;
        const c_module_info& module() const;
        const c_vm_info& vm_info() const;
        u64 pvm() const;
        u64 pvcpu() const;
        void clear_runtime_caches();

    public:
        std::optional<c_translation_result> resolve_guest_physical(u64 guest_phys, bool verbose = false);
        bool read_guest_physical(u64 guest_phys, void* buffer, usize size, bool verbose = false);
        bool write_guest_physical(u64 guest_phys, const void* buffer, usize size, bool verbose = false);
        bool read_guest_kernel(u64 kernel_va, void* buffer, usize size, bool verbose = false);
        bool write_guest_kernel(u64 kernel_va, const void* buffer, usize size, bool verbose = false);
        std::optional<std::string> read_guest_kernel_c_string(u64 kernel_va, usize max_chars, bool verbose = false);
        std::optional<std::string> read_guest_kernel_string(u64 kernel_va, usize size, bool verbose = false);
        std::optional<u64> read_mm_cr3(u64 mm_va, bool verbose = false);
        std::optional<u64> translate_guest_va(u64 guest_cr3, u64 guest_va, bool verbose = false);
        bool read_guest_user(u64 guest_cr3, u64 guest_va, void* buffer, usize size, bool verbose = false);
        bool write_guest_user(u64 guest_cr3, u64 guest_va, const void* buffer, usize size, bool verbose = false);
        std::optional<std::string> read_guest_user_c_string(u64 guest_cr3, u64 guest_va, usize max_chars, bool verbose = false);
        bool read_gva(u64 guest_cr3, u64 guest_va, void* buffer, usize size, bool verbose = false);
        bool write_gva(u64 guest_cr3, u64 guest_va, const void* buffer, usize size, bool verbose = false);

    public:
        template <typename t_type>
        bool read_guest_physical(u64 guest_phys, t_type& out_value, bool verbose = false)
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            return read_guest_physical(guest_phys, &out_value, sizeof(t_type), verbose);
        }

        template <typename t_type>
        bool read_guest_kernel(u64 kernel_va, t_type& out_value, bool verbose = false)
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            return read_guest_kernel(kernel_va, &out_value, sizeof(t_type), verbose);
        }

        template <typename t_type>
        bool read_guest_user(u64 guest_cr3, u64 guest_va, t_type& out_value, bool verbose = false)
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            return read_guest_user(guest_cr3, guest_va, &out_value, sizeof(t_type), verbose);
        }

        template <typename t_type>
        bool write_guest_user(u64 guest_cr3, u64 guest_va, const t_type& value, bool verbose = false)
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            return write_guest_user(guest_cr3, guest_va, &value, sizeof(t_type), verbose);
        }

        template <typename t_type>
        bool read_gva(u64 guest_cr3, u64 guest_va, t_type& out_value, bool verbose = false)
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            return read_gva(guest_cr3, guest_va, &out_value, sizeof(t_type), verbose);
        }

        template <typename t_type>
        bool write_gva(u64 guest_cr3, u64 guest_va, const t_type& value, bool verbose = false)
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            return write_gva(guest_cr3, guest_va, &value, sizeof(t_type), verbose);
        }

    private:
        struct c_gva_page_key {
            u64 cr3_page { };
            u64 va_page  { };

            bool operator==(const c_gva_page_key& other) const noexcept
            {
                return cr3_page == other.cr3_page && va_page == other.va_page;
            }
        };

        struct c_gva_page_key_hash {
            usize operator()(const c_gva_page_key& key) const noexcept
            {
                const u64 mixed = key.cr3_page ^ (key.va_page + 0x9E3779B97F4A7C15ULL + (key.cr3_page << 6) + (key.cr3_page >> 2));
#if SIZE_MAX == UINT64_MAX
                return static_cast<usize>(mixed);
#else
                return static_cast<usize>(mixed ^ (mixed >> 32));
#endif
            }
        };

    private:
        void reset();
        bool read_host_bytes(u64 address, void* buffer, usize size) const;
        bool write_host_bytes(u64 address, const void* buffer, usize size) const;

        template <typename t_type>
        bool read_host_value(u64 address, t_type& out_value) const
        {
            static_assert(std::is_trivially_copyable_v<t_type>);
            return read_host_bytes(address, &out_value, sizeof(t_type));
        }

    private:
        std::optional<c_vm_info> locate_vm_info(bool verbose) const;
        std::optional<u64> find_chunk_from_cache(u32 chunk_id) const;
        std::optional<u64> find_chunk_from_tree(u32 chunk_id, bool verbose) const;
        std::optional<u64> find_physical_range(u64 guest_phys) const;
        std::optional<u64> try_page_map_cache(u64 guest_phys, bool verbose) const;
        std::optional<u64> map_physical_page_to_host_base(u64 guest_phys, bool verbose);
        std::optional<u64> map_physical_to_host(u64 guest_phys, bool verbose);
        bool map_chunk_via_bstk_vmm(u32 chunk_id, u64& out_chunk_pointer, bool verbose);

    private:
        std::optional<u64> lookup_cached_physical_page(u64 guest_cr3, u64 guest_va) const;
        void store_cached_physical_page(u64 guest_cr3, u64 guest_va, u64 guest_phys_page);
        void invalidate_cached_physical_page(u64 guest_cr3, u64 guest_va);
        std::optional<u64> lookup_cached_host_page_by_gva(u64 guest_cr3, u64 guest_va) const;
        void store_cached_host_page_by_gva(u64 guest_cr3, u64 guest_va, u64 host_page_base);
        void invalidate_cached_host_page_by_gva(u64 guest_cr3, u64 guest_va);
        std::optional<u64> lookup_cached_host_page(u64 guest_phys) const;
        void store_cached_host_page(u64 guest_phys, u64 host_page_base);
        void invalidate_cached_host_page(u64 guest_phys);
        void clear_translation_caches();

    private:
        static std::optional<u32> find_process_id_by_name(const std::wstring& process_name);
        static std::optional<c_module_info> find_module_in_process(u32 process_id, const std::wstring& module_name);

    private:
        HANDLE process_handle { };
        u32 process_id_value  { };
        c_module_info module_info_value { };
        c_vm_info vm_info_value { };
        u64 bstk_module_base   { };

    private:
        mutable std::shared_mutex cache_mutex { };
        std::unordered_map<c_gva_page_key, u64, c_gva_page_key_hash> gva_to_physical_page_cache { };
        std::unordered_map<c_gva_page_key, u64, c_gva_page_key_hash> gva_to_host_page_cache { };
        std::unordered_map<u64, u64> phys_to_host_page_cache { };

    private:
        static constexpr usize k_max_gva_cache_entries      = 262144;
        static constexpr usize k_max_gva_host_cache_entries = 262144;
        static constexpr usize k_max_phys_cache_entries     = 262144;
    };
}

#endif
