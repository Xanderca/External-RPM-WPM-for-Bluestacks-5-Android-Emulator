#ifndef MEMORY_ENGINE_CPP
#define MEMORY_ENGINE_CPP

#include <impl/includes.hpp>


namespace offsets {
    constexpr u64 vm_cpu_count             = 0x3C;
    constexpr u64 vm_read_counter          = 0x10AFF0;
    constexpr u64 vm_direct_ram_window     = 0x104000;
    constexpr u64 vm_direct_ram_tag        = 0x106000;
    constexpr u64 vm_ram_range_mode        = 0x106019;
    constexpr u64 vm_range_cache           = 0x106050;
    constexpr u64 vm_range_tree_root       = 0x106098;
    constexpr u64 vm_mmio2_table           = 0x1060B0;
    constexpr u64 vm_chunk_cache           = 0x106860;
    constexpr u64 vm_chunk_tree_root       = 0x106C60;
    constexpr u64 vm_page_map_cache        = 0x106C80;
    constexpr u64 vm_cpu_array             = 0x11FE00;
    constexpr u64 bstk_map_chunk_rva       = 0xDA070;//for bluestacks latest or version above 5.22.130+++ use this offset // 0xA24C0;// and before that replace driver
    constexpr u64 expected_pvcpu_delta     = 0x120000;
    constexpr u64 peb_ldr                  = 0x18;
    constexpr u64 range_start              = 0x0;
    constexpr u64 range_size               = 0x8;
    constexpr u64 range_host_base          = 0x30;
    constexpr u64 range_left               = 0x50;
    constexpr u64 range_right              = 0x58;
    constexpr u64 range_page_desc_table    = 0x70;
    constexpr u64 range_page_desc_stride   = 0x10;
    constexpr u64 page_desc_state          = 0x0;
    constexpr u64 page_desc_info           = 0x8;
    constexpr u64 chunk_node_left          = 0x0;
    constexpr u64 chunk_node_right         = 0x8;
    constexpr u64 chunk_node_key           = 0x10;
    constexpr u64 chunk_host_base          = 0x28;
    constexpr u64 mmio_range_host_base     = 0x70;
    constexpr u64 page_map_cache_stride    = 0x20;
    constexpr u64 page_map_cached_tag      = 0x0;
    constexpr u64 page_map_page_desc       = 0x8;
    constexpr u64 page_map_host_base       = 0x18;
    constexpr u64 remote_stub_pvm          = 6;
    constexpr u64 remote_stub_chunk_id     = 15;
    constexpr u64 remote_stub_result_ptr_0 = 21;
    constexpr u64 remote_stub_target_fn    = 31;
    constexpr u64 remote_stub_result_ptr_1 = 43;
}

namespace utility {

    constexpr u64 k_long_mode_page_frame_mask = 0x000FFFFFFFFFF000ULL;
    constexpr u64 k_page_size                 = 0x1000ULL;
    constexpr u64 k_large_page_1gb_mask       = 0x000FFFFFFFE00000ULL;
    constexpr u64 k_large_page_2mb_mask       = 0x000FFFFFFFE00000ULL;
    constexpr u64 k_user_pointer_min          = 0x10000ULL;
    constexpr u64 k_user_pointer_max          = 0x00007FFFFFFFFFFFULL;
    constexpr u64 k_read_counter_limit        = 1000000000000ULL;
    constexpr u64 k_direct_state_mask_high    = 0x7000000000000ULL;
    constexpr u64 k_direct_state_mask_low     = 0x0FFFFFFFFFF000ULL;
    constexpr u64 k_present_bit               = 1ULL << 0;
    constexpr u64 k_page_size_bit             = 1ULL << 7;
    constexpr u64 k_page_mask                 = ~0xFFFULL;
    constexpr u64 k_page_offset_mask          = 0xFFFULL;
    constexpr u32 k_chunk_cache_mask          = 0x3F;
    constexpr u64 k_range_cache_mask          = 0x7ULL;
    constexpr u64 k_page_map_cache_mask       = 0xFFULL;
    constexpr usize k_remote_stub_size        = 64;

    enum class e_system_information_class : ULONG {
        system_process_information = 5
    };

    enum class e_process_info_class : ULONG {
        process_basic_information = 0
    };

    struct c_unicode_string;
    struct c_object_attributes;
    struct c_client_id;

    using t_nt_query_system_information = LONG (NTAPI*)(e_system_information_class, PVOID, ULONG, PULONG);
    using t_nt_open_process = LONG (NTAPI*)(PHANDLE, ACCESS_MASK, c_object_attributes*, c_client_id*);
    using t_nt_query_information_process = LONG (NTAPI*)(HANDLE, e_process_info_class, PVOID, ULONG, PULONG);

    struct c_unicode_string {
        USHORT length         { };
        USHORT maximum_length { };
        PWSTR  buffer         { };
    };

    struct c_object_attributes {
        ULONG             length                   { };
        HANDLE            root_directory           { };
        c_unicode_string* object_name              { };
        ULONG             attributes               { };
        PVOID             security_descriptor      { };
        PVOID             security_quality_service { };
    };

    struct c_client_id {
        HANDLE unique_process { };
        HANDLE unique_thread  { };
    };

    struct c_system_process_information {
        ULONG            next_entry_offset           { };
        ULONG            number_of_threads           { };
        BYTE             reserved_0[48]             { };
        c_unicode_string image_name                 { };
        LONG             base_priority              { };
        HANDLE           unique_process_id          { };
        PVOID            reserved_1                 { };
        ULONG            handle_count               { };
        ULONG            session_id                 { };
        PVOID            reserved_2                 { };
        SIZE_T           peak_virtual_size          { };
        SIZE_T           virtual_size               { };
        ULONG            reserved_3                 { };
        SIZE_T           peak_working_set_size      { };
        SIZE_T           working_set_size           { };
        PVOID            reserved_4                 { };
        SIZE_T           quota_paged_pool_usage     { };
        PVOID            reserved_5                 { };
        SIZE_T           quota_non_paged_pool_usage { };
        SIZE_T           pagefile_usage             { };
        SIZE_T           peak_pagefile_usage        { };
        SIZE_T           private_page_count         { };
        LARGE_INTEGER    read_operation_count       { };
        LARGE_INTEGER    write_operation_count      { };
        LARGE_INTEGER    other_operation_count      { };
        LARGE_INTEGER    read_transfer_count        { };
        LARGE_INTEGER    write_transfer_count       { };
        LARGE_INTEGER    other_transfer_count       { };
    };

    struct c_process_basic_information {
        LONG      exit_status          { };
        PVOID     peb_base_address     { };
        ULONG_PTR affinity_mask        { };
        LONG      base_priority        { };
        ULONG_PTR unique_process_id    { };
        ULONG_PTR inherited_process_id { };
    };

    struct c_peb_ldr_data {
        ULONG      length                        { };
        BOOLEAN    initialized                   { };
        HANDLE     ss_handle                     { };
        LIST_ENTRY in_load_order_module_list     { };
        LIST_ENTRY in_memory_order_module_list   { };
        LIST_ENTRY in_initialization_module_list { };
    };

    struct c_ldr_data_table_entry {
        LIST_ENTRY       in_load_order_links           { };
        LIST_ENTRY       in_memory_order_links         { };
        LIST_ENTRY       in_initialization_order_links { };
        PVOID            dll_base                      { };
        PVOID            entry_point                   { };
        ULONG            size_of_image                 { };
        c_unicode_string full_dll_name                 { };
        c_unicode_string base_dll_name                 { };
    };

    inline bool nt_success(LONG status)
    {
        return status >= 0;
    }

    inline bool is_likely_user_pointer(u64 value)
    {
        return value >= k_user_pointer_min && value < k_user_pointer_max;
    }

    void verbose_log(bool verbose, const std::string& message)
    {
        if (verbose) {
            std::cout << message << '\n';
        }
    }

    std::wstring to_lower_wide(std::wstring value)
    {
        std::transform(value.begin(), value.end(), value.begin(), [](wchar_t character) {
            return static_cast<wchar_t>(::towlower(character));
        });
        return value;
    }

    template <typename t_type>
    t_type get_nt_function(const char* name)
    {
        static const auto ntdll_module = GetModuleHandleW(L"ntdll.dll");
        if (ntdll_module == nullptr)
            return nullptr;

        return reinterpret_cast<t_type>(GetProcAddress(ntdll_module, name));
    }

    HANDLE open_process_nt(u32 process_id, ACCESS_MASK access_mask)
    {
        static const auto nt_open_process = get_nt_function<t_nt_open_process>("NtOpenProcess");
        if (nt_open_process == nullptr)
            return nullptr;

        HANDLE process_handle { };
        c_object_attributes object_attributes { };
        object_attributes.length = sizeof(object_attributes);

        c_client_id client_id { };
        client_id.unique_process = reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(process_id));

        if (!nt_success(nt_open_process(&process_handle, access_mask, &object_attributes, &client_id)))
            return nullptr;

        return process_handle;
    }
}

namespace mem {

    c_memory_engine::~c_memory_engine()
    {
        shutdown();
    }

    bool c_memory_engine::initialize(bool verbose)
    {
        reset();

        const auto process_id_result = find_process_id_by_name(config::k_default_process_name);
        if (!process_id_result.has_value()) {
            utility::verbose_log(verbose, "[mem] process not found");
            return false;
        }

        utility::verbose_log(verbose, "[mem] process id=" + std::to_string(*process_id_result));

        process_handle = utility::open_process_nt(*process_id_result, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION);
        if (process_handle == nullptr) {
            utility::verbose_log(verbose, "[mem] open process failed");
            reset();
            return false;
        }

        process_id_value = *process_id_result;

        const auto module_result = find_module_in_process(process_id_value, config::k_default_module_name);
        if (!module_result.has_value()) {
            utility::verbose_log(verbose, "[mem] module not found");
            reset();
            return false;
        }

        module_info_value = *module_result;
        bstk_module_base  = module_info_value.base;
        utility::verbose_log(verbose, "[mem] module base=" + utility::hex(module_info_value.base));

        const auto vm_result = locate_vm_info(verbose);
        if (!vm_result.has_value()) {
            utility::verbose_log(verbose, "[mem] vm info not found");
            reset();
            return false;
        }

        vm_info_value = *vm_result;

        {
            std::unique_lock<std::shared_mutex> lock(cache_mutex);
            gva_to_physical_page_cache.reserve(65536);
            gva_to_host_page_cache.reserve(65536);
            phys_to_host_page_cache.reserve(65536);
        }

        utility::verbose_log(verbose, "memory engine initialized");
        return true;
    }

    void c_memory_engine::shutdown()
    {
        reset();
    }

    bool c_memory_engine::is_initialized() const
    {
        return process_handle != nullptr && process_id_value != 0 && vm_info_value.pvm != 0 && vm_info_value.pvcpu_0 != 0;
    }

    HANDLE c_memory_engine::process() const
    {
        return process_handle;
    }

    u32 c_memory_engine::process_id() const
    {
        return process_id_value;
    }

    const c_module_info& c_memory_engine::module() const
    {
        return module_info_value;
    }

    const c_vm_info& c_memory_engine::vm_info() const
    {
        return vm_info_value;
    }

    u64 c_memory_engine::pvm() const
    {
        return vm_info_value.pvm;
    }

    u64 c_memory_engine::pvcpu() const
    {
        return vm_info_value.pvcpu_0;
    }

    void c_memory_engine::clear_runtime_caches()
    {
        clear_translation_caches();
    }

    std::optional<u64> c_memory_engine::lookup_cached_physical_page(u64 guest_cr3, u64 guest_va) const
    {
        const c_gva_page_key key { guest_cr3 & utility::k_long_mode_page_frame_mask, guest_va & utility::k_page_mask };
        std::shared_lock<std::shared_mutex> lock(cache_mutex);

        const auto cache_entry = gva_to_physical_page_cache.find(key);
        if (cache_entry == gva_to_physical_page_cache.end())
            return std::nullopt;

        return cache_entry->second;
    }

    void c_memory_engine::store_cached_physical_page(u64 guest_cr3, u64 guest_va, u64 guest_phys_page)
    {
        const c_gva_page_key key { guest_cr3 & utility::k_long_mode_page_frame_mask, guest_va & utility::k_page_mask };
        std::unique_lock<std::shared_mutex> lock(cache_mutex);

        if (gva_to_physical_page_cache.size() >= k_max_gva_cache_entries) {
            gva_to_physical_page_cache.clear();
        }

        gva_to_physical_page_cache[key] = guest_phys_page & utility::k_page_mask;
    }

    void c_memory_engine::invalidate_cached_physical_page(u64 guest_cr3, u64 guest_va)
    {
        const c_gva_page_key key { guest_cr3 & utility::k_long_mode_page_frame_mask, guest_va & utility::k_page_mask };
        std::unique_lock<std::shared_mutex> lock(cache_mutex);
        gva_to_physical_page_cache.erase(key);
        gva_to_host_page_cache.erase(key);
    }

    std::optional<u64> c_memory_engine::lookup_cached_host_page_by_gva(u64 guest_cr3, u64 guest_va) const
    {
        const c_gva_page_key key { guest_cr3 & utility::k_long_mode_page_frame_mask, guest_va & utility::k_page_mask };
        std::shared_lock<std::shared_mutex> lock(cache_mutex);

        const auto cache_entry = gva_to_host_page_cache.find(key);
        if (cache_entry == gva_to_host_page_cache.end())
            return std::nullopt;

        return cache_entry->second;
    }

    void c_memory_engine::store_cached_host_page_by_gva(u64 guest_cr3, u64 guest_va, u64 host_page_base)
    {
        const c_gva_page_key key { guest_cr3 & utility::k_long_mode_page_frame_mask, guest_va & utility::k_page_mask };
        std::unique_lock<std::shared_mutex> lock(cache_mutex);

        if (gva_to_host_page_cache.size() >= k_max_gva_host_cache_entries) {
            gva_to_host_page_cache.clear();
        }

        gva_to_host_page_cache[key] = host_page_base & utility::k_page_mask;
    }

    void c_memory_engine::invalidate_cached_host_page_by_gva(u64 guest_cr3, u64 guest_va)
    {
        const c_gva_page_key key { guest_cr3 & utility::k_long_mode_page_frame_mask, guest_va & utility::k_page_mask };
        std::unique_lock<std::shared_mutex> lock(cache_mutex);
        gva_to_host_page_cache.erase(key);
    }

    std::optional<u64> c_memory_engine::lookup_cached_host_page(u64 guest_phys) const
    {
        const auto guest_phys_page = guest_phys & utility::k_page_mask;
        std::shared_lock<std::shared_mutex> lock(cache_mutex);

        const auto cache_entry = phys_to_host_page_cache.find(guest_phys_page);
        if (cache_entry == phys_to_host_page_cache.end())
            return std::nullopt;

        return cache_entry->second;
    }

    void c_memory_engine::store_cached_host_page(u64 guest_phys, u64 host_page_base)
    {
        const auto guest_phys_page = guest_phys & utility::k_page_mask;
        std::unique_lock<std::shared_mutex> lock(cache_mutex);

        if (phys_to_host_page_cache.size() >= k_max_phys_cache_entries) {
            phys_to_host_page_cache.clear();
        }

        phys_to_host_page_cache[guest_phys_page] = host_page_base & utility::k_page_mask;
    }

    void c_memory_engine::invalidate_cached_host_page(u64 guest_phys)
    {
        const auto guest_phys_page = guest_phys & utility::k_page_mask;
        std::unique_lock<std::shared_mutex> lock(cache_mutex);
        phys_to_host_page_cache.erase(guest_phys_page);
    }

    void c_memory_engine::clear_translation_caches()
    {
        std::unique_lock<std::shared_mutex> lock(cache_mutex);
        gva_to_physical_page_cache.clear();
        gva_to_host_page_cache.clear();
        phys_to_host_page_cache.clear();
    }

    void c_memory_engine::reset()
    {
        clear_translation_caches();

        if (process_handle != nullptr) {
            CloseHandle(process_handle);
            process_handle = nullptr;
        }

        process_id_value  = 0;
        module_info_value = { };
        vm_info_value     = { };
        bstk_module_base  = 0;
    }

    bool c_memory_engine::read_host_bytes(u64 address, void* buffer, usize size) const
    {
        if (process_handle == nullptr || buffer == nullptr || size == 0)
            return false;

        SIZE_T bytes_read { };
        if (!ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(address), buffer, size, &bytes_read))
            return false;

        return bytes_read == size;
    }

    bool c_memory_engine::write_host_bytes(u64 address, const void* buffer, usize size) const
    {
        if (process_handle == nullptr || buffer == nullptr || size == 0)
            return false;

        SIZE_T bytes_written { };
        if (!WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(address), buffer, size, &bytes_written))
            return false;

        return bytes_written == size;
    }

    std::optional<c_vm_info> c_memory_engine::locate_vm_info(bool verbose) const
    {
        MEMORY_BASIC_INFORMATION memory_info { };
        auto* current_address = static_cast<u8*>(nullptr);
        std::optional<c_vm_info> fallback_result { };

        while (VirtualQueryEx(process_handle, current_address, &memory_info, sizeof(memory_info)) == sizeof(memory_info)) {
            const auto protection = memory_info.Protect & 0xFF;
            const bool readable_writable = protection == PAGE_READWRITE || protection == PAGE_EXECUTE_READWRITE;

            if (memory_info.State == MEM_COMMIT && readable_writable && memory_info.RegionSize >= (128 * 1024)) {
                const auto candidate = static_cast<u64>(reinterpret_cast<uptr>(memory_info.BaseAddress));

                u32 cpu_count { };
                u64 cpu_0 { };
                u64 read_counter { };

                const bool cpu_count_ok = read_host_value(candidate + offsets::vm_cpu_count, cpu_count) && cpu_count >= 1 && cpu_count <= 8;
                const bool cpu_window_ok = offsets::vm_cpu_array + sizeof(u64) <= static_cast<u64>(memory_info.RegionSize);
                const bool cpu_0_read_ok = read_host_value(candidate + offsets::vm_cpu_array, cpu_0) && utility::is_likely_user_pointer(cpu_0);
                const bool cpu_0_ok = cpu_0_read_ok && cpu_0 == candidate + offsets::expected_pvcpu_delta;
                const bool read_counter_read_ok = read_host_value(candidate + offsets::vm_read_counter, read_counter);
                const bool read_counter_ok = read_counter_read_ok && read_counter != 0 && read_counter <= utility::k_read_counter_limit;

                if (cpu_count_ok && cpu_window_ok && cpu_0_ok && read_counter_ok) {
                    c_vm_info result { };
                    result.pvm          = candidate;
                    result.pvcpu_0      = cpu_0;
                    result.cpu_count    = cpu_count;
                    result.read_counter = read_counter;
                    result.region_size  = static_cast<u64>(memory_info.RegionSize);
                    return result;
                }

                if (!fallback_result.has_value() &&
                    cpu_count_ok &&
                    cpu_window_ok &&
                    cpu_0_read_ok &&
                    read_counter_ok &&
                    static_cast<u64>(memory_info.RegionSize) == offsets::expected_pvcpu_delta) {
                    c_vm_info result { };
                    result.pvm          = candidate;
                    result.pvcpu_0      = cpu_0;
                    result.cpu_count    = cpu_count;
                    result.read_counter = read_counter;
                    result.region_size  = static_cast<u64>(memory_info.RegionSize);
                    fallback_result     = result;
                }
            }

            const auto next_address_value = static_cast<uptr>(reinterpret_cast<uptr>(current_address) + memory_info.RegionSize);
            if (next_address_value <= reinterpret_cast<uptr>(current_address)) {
                break;
            }

            current_address = reinterpret_cast<u8*>(next_address_value);
        }

        if (fallback_result.has_value()) {
            utility::verbose_log(verbose, "[mem] using fallback vm candidate base=" + utility::hex(fallback_result->pvm));
            return fallback_result;
        }

        return std::nullopt;
    }

    std::optional<u32> c_memory_engine::find_process_id_by_name(const std::wstring& process_name)
    {
        static const auto nt_query_system_information = utility::get_nt_function<utility::t_nt_query_system_information>("NtQuerySystemInformation");
        if (nt_query_system_information == nullptr)
            return std::nullopt;

        ULONG buffer_size { };
        nt_query_system_information(utility::e_system_information_class::system_process_information, nullptr, 0, &buffer_size);
        if (buffer_size == 0)
            return std::nullopt;

        std::vector<u8> buffer(static_cast<usize>(buffer_size) + 0x1000, 0);
        if (!utility::nt_success(nt_query_system_information(utility::e_system_information_class::system_process_information, buffer.data(), static_cast<ULONG>(buffer.size()), &buffer_size)))
            return std::nullopt;

        const auto wanted_name = utility::to_lower_wide(process_name);
        auto* current_process = reinterpret_cast<utility::c_system_process_information*>(buffer.data());

        while (true) {
            if (current_process->image_name.buffer != nullptr) {
                std::wstring image_name(current_process->image_name.buffer, current_process->image_name.length / sizeof(wchar_t));
                if (utility::to_lower_wide(std::move(image_name)) == wanted_name)
                    return static_cast<u32>(reinterpret_cast<ULONG_PTR>(current_process->unique_process_id));
            }

            if (current_process->next_entry_offset == 0) {
                break;
            }

            current_process = reinterpret_cast<utility::c_system_process_information*>(reinterpret_cast<u8*>(current_process) + current_process->next_entry_offset);
        }

        return std::nullopt;
    }

    std::optional<c_module_info> c_memory_engine::find_module_in_process(u32 process_id, const std::wstring& module_name)
    {
        static const auto nt_query_information_process = utility::get_nt_function<utility::t_nt_query_information_process>("NtQueryInformationProcess");
        if (nt_query_information_process == nullptr)
            return std::nullopt;

        const auto process_handle = utility::open_process_nt(process_id, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);
        if (process_handle == nullptr)
            return std::nullopt;

        utility::c_process_basic_information process_information { };
        if (!utility::nt_success(nt_query_information_process(process_handle, utility::e_process_info_class::process_basic_information, &process_information, sizeof(process_information), nullptr))) {
            CloseHandle(process_handle);
            return std::nullopt;
        }

        if (process_information.peb_base_address == nullptr) {
            CloseHandle(process_handle);
            return std::nullopt;
        }

        utility::c_peb_ldr_data loader_data { };
        PVOID loader_pointer { };
        if (!ReadProcessMemory(process_handle, reinterpret_cast<PVOID>(reinterpret_cast<ULONG_PTR>(process_information.peb_base_address) + offsets::peb_ldr), &loader_pointer, sizeof(loader_pointer), nullptr) || loader_pointer == nullptr) {
            CloseHandle(process_handle);
            return std::nullopt;
        }

        if (!ReadProcessMemory(process_handle, loader_pointer, &loader_data, sizeof(loader_data), nullptr)) {
            CloseHandle(process_handle);
            return std::nullopt;
        }

        const auto wanted_name = utility::to_lower_wide(module_name);
        auto* start_node = &loader_data.in_load_order_module_list;
        auto* current_node = loader_data.in_load_order_module_list.Flink;

        for (s32 index = 0; index < 512 && current_node != start_node && current_node != nullptr; ++index) {
            utility::c_ldr_data_table_entry loader_entry { };
            if (!ReadProcessMemory(process_handle, current_node, &loader_entry, sizeof(loader_entry), nullptr)) {
                break;
            }

            if (loader_entry.base_dll_name.buffer != nullptr) {
                std::wstring base_name(loader_entry.base_dll_name.length / sizeof(wchar_t), L'\0');
                if (ReadProcessMemory(process_handle, loader_entry.base_dll_name.buffer, base_name.data(), loader_entry.base_dll_name.length, nullptr) && utility::to_lower_wide(base_name) == wanted_name) {
                    c_module_info result { };
                    result.name = std::move(base_name);
                    result.base = static_cast<u64>(reinterpret_cast<uptr>(loader_entry.dll_base));
                    result.size = loader_entry.size_of_image;

                    if (loader_entry.full_dll_name.buffer != nullptr && loader_entry.full_dll_name.length != 0) {
                        std::wstring full_path(loader_entry.full_dll_name.length / sizeof(wchar_t), L'\0');
                        if (ReadProcessMemory(process_handle, loader_entry.full_dll_name.buffer, full_path.data(), loader_entry.full_dll_name.length, nullptr)) {
                            result.path = std::move(full_path);
                        }
                    }

                    CloseHandle(process_handle);
                    return result;
                }
            }

            current_node = loader_entry.in_load_order_links.Flink;
        }

        CloseHandle(process_handle);
        return std::nullopt;
    }

    bool c_memory_engine::map_chunk_via_bstk_vmm(u32 chunk_id, u64& out_chunk_pointer, bool verbose)
    {
        out_chunk_pointer = 0;
        if (bstk_module_base == 0 || process_id_value == 0)
            return false;

        const auto remote_process = utility::open_process_nt(process_id_value, PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE);
        if (remote_process == nullptr)
            return false;

        struct c_remote_chunk_map_result {
            u64 chunk_pointer { };
            s64 status        { };
        };

        const auto total_size = sizeof(c_remote_chunk_map_result) + utility::k_remote_stub_size;
        auto cleanup = [&](LPVOID remote_memory) {
            if (remote_memory != nullptr) {
                VirtualFreeEx(remote_process, remote_memory, 0, MEM_RELEASE);
            }
            CloseHandle(remote_process);
        };

        auto* remote_memory = static_cast<u8*>(VirtualAllocEx(remote_process, nullptr, total_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
        if (remote_memory == nullptr) {
            cleanup(nullptr);
            return false;
        }

        const auto remote_result = static_cast<u64>(reinterpret_cast<uptr>(remote_memory));
        const auto remote_code   = remote_result + sizeof(c_remote_chunk_map_result);
        const auto remote_fn     = bstk_module_base + offsets::bstk_map_chunk_rva;

        std::array<u8, utility::k_remote_stub_size> stub {
            0x48, 0x83, 0xEC, 0x28,
            0x48, 0xB9,
            0, 0, 0, 0, 0, 0, 0, 0,
            0xBA,
            0, 0, 0, 0,
            0x49, 0xB8,
            0, 0, 0, 0, 0, 0, 0, 0,
            0x48, 0xB8,
            0, 0, 0, 0, 0, 0, 0, 0,
            0xFF, 0xD0,
            0x48, 0xB9,
            0, 0, 0, 0, 0, 0, 0, 0,
            0x48, 0x63, 0xC0,
            0x48, 0x89, 0x41, 0x08,
            0x48, 0x83, 0xC4, 0x28,
            0xC3
        };

        auto patch_u64 = [&](u64 offset, u64 value) {
            std::memcpy(stub.data() + offset, &value, sizeof(value));
        };

        auto patch_u32 = [&](u64 offset, u32 value) {
            std::memcpy(stub.data() + offset, &value, sizeof(value));
        };

        patch_u64(offsets::remote_stub_pvm, vm_info_value.pvm);
        patch_u32(offsets::remote_stub_chunk_id, chunk_id);
        patch_u64(offsets::remote_stub_result_ptr_0, remote_result);
        patch_u64(offsets::remote_stub_target_fn, remote_fn);
        patch_u64(offsets::remote_stub_result_ptr_1, remote_result);

        c_remote_chunk_map_result initial_result { };
        SIZE_T bytes_written { };
        if (!WriteProcessMemory(remote_process, remote_memory, &initial_result, sizeof(initial_result), &bytes_written) || bytes_written != sizeof(initial_result) || !WriteProcessMemory(remote_process, reinterpret_cast<LPVOID>(remote_code), stub.data(), stub.size(), &bytes_written) || bytes_written != stub.size()) {
            cleanup(remote_memory);
            return false;
        }

        const auto remote_thread = CreateRemoteThread(remote_process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(remote_code), nullptr, 0, nullptr);
        if (remote_thread == nullptr) {
            cleanup(remote_memory);
            return false;
        }

        const auto wait_result = WaitForSingleObject(remote_thread, config::k_remote_thread_timeout_ms);
        CloseHandle(remote_thread);
        if (wait_result != WAIT_OBJECT_0) {
            cleanup(remote_memory);
            return false;
        }

        c_remote_chunk_map_result final_result { };
        SIZE_T bytes_read { };
        const bool read_ok = ReadProcessMemory(remote_process, remote_memory, &final_result, sizeof(final_result), &bytes_read) && bytes_read == sizeof(final_result);
        cleanup(remote_memory);

        if (!read_ok || final_result.status < 0 || final_result.chunk_pointer == 0) {
            if (verbose) {
                std::cout << "[map] remote chunk map failed for chunk " << chunk_id << " status=" << utility::hex(static_cast<u64>(final_result.status)) << '\n';
            }
            return false;
        }

        if (verbose) {
            std::cout << "[map] remote chunk map succeeded for chunk " << chunk_id << " chunk_ptr=" << utility::hex(final_result.chunk_pointer) << '\n';
        }

        out_chunk_pointer = final_result.chunk_pointer;
        return true;
    }

    std::optional<u64> c_memory_engine::find_chunk_from_cache(u32 chunk_id) const
    {
        const auto slot = chunk_id & utility::k_chunk_cache_mask;
        const auto slot_address = vm_info_value.pvm + offsets::vm_chunk_cache + static_cast<u64>(slot) * 0x10;

        u32 cached_id { };
        u64 chunk_pointer { };
        if (!read_host_value(slot_address, cached_id) || !read_host_value(slot_address + 0x8, chunk_pointer))
            return std::nullopt;

        if (cached_id != chunk_id || !utility::is_likely_user_pointer(chunk_pointer))
            return std::nullopt;

        return chunk_pointer;
    }

    std::optional<u64> c_memory_engine::find_chunk_from_tree(u32 chunk_id, bool verbose) const
    {
        u64 node { };
        if (!read_host_value(vm_info_value.pvm + offsets::vm_chunk_tree_root, node))
            return std::nullopt;

        while (node != 0) {
            u32 key { };
            if (!read_host_value(node + offsets::chunk_node_key, key))
                return std::nullopt;

            if (verbose) {
                std::cout << "[map] chunk-tree node=" << utility::hex(node) << " key=" << key << '\n';
            }

            if (key == chunk_id)
                return node;

            const auto next_offset = chunk_id < key ? offsets::chunk_node_left : offsets::chunk_node_right;
            if (!read_host_value(node + next_offset, node))
                return std::nullopt;
        }

        return std::nullopt;
    }

    std::optional<u64> c_memory_engine::find_physical_range(u64 guest_phys) const
    {
        const auto slot = (guest_phys >> 20) & utility::k_range_cache_mask;
        const auto cache_address = vm_info_value.pvm + offsets::vm_range_cache + slot * sizeof(u64);

        u64 node { };
        if (!read_host_value(cache_address, node))
            return std::nullopt;

        auto range_contains = [&](u64 candidate) -> std::optional<bool> {
            if (candidate == 0)
                return false;

            u64 range_start { };
            u64 range_size { };
            if (!read_host_value(candidate + offsets::range_start, range_start) || !read_host_value(candidate + offsets::range_size, range_size))
                return std::nullopt;

            return guest_phys >= range_start && guest_phys - range_start < range_size;
        };

        if (node != 0) {
            const auto cache_result = range_contains(node);
            if (!cache_result.has_value())
                return std::nullopt;

            if (*cache_result)
                return node;
        }

        if (!read_host_value(vm_info_value.pvm + offsets::vm_range_tree_root, node))
            return std::nullopt;

        while (node != 0) {
            u64 range_start { };
            u64 range_size { };
            if (!read_host_value(node + offsets::range_start, range_start) || !read_host_value(node + offsets::range_size, range_size))
                return std::nullopt;

            if (guest_phys >= range_start && guest_phys - range_start < range_size)
                return node;

            const auto next_offset = guest_phys < range_start ? offsets::range_left : offsets::range_right;
            if (!read_host_value(node + next_offset, node))
                return std::nullopt;
        }

        return std::nullopt;
    }

    std::optional<u64> c_memory_engine::try_page_map_cache(u64 guest_phys, bool verbose) const
    {
        const auto slot = (guest_phys >> 12) & utility::k_page_map_cache_mask;
        const auto entry_address = vm_info_value.pvm + offsets::vm_page_map_cache + slot * offsets::page_map_cache_stride;
        const auto page_tag = guest_phys & utility::k_page_mask;

        u64 cached_tag { };
        u64 page_description { };
        u64 host_base { };
        if (!read_host_value(entry_address + offsets::page_map_cached_tag, cached_tag) || !read_host_value(entry_address + offsets::page_map_page_desc, page_description) || !read_host_value(entry_address + offsets::page_map_host_base, host_base)) {
            utility::verbose_log(verbose, "[map] failed to read page-map cache entry");
            return std::nullopt;
        }

        if (verbose) {
            std::cout << "[map] page-cache slot=" << slot << " tag=" << utility::hex(cached_tag) << " desc=" << utility::hex(page_description) << " host=" << utility::hex(host_base) << '\n';
        }

        if (cached_tag == page_tag && utility::is_likely_user_pointer(host_base)) {
            utility::verbose_log(verbose, "[map] page-cache hit");
            return host_base | (guest_phys & utility::k_page_offset_mask);
        }

        utility::verbose_log(verbose, "[map] page-cache miss");
        return std::nullopt;
    }

    std::optional<u64> c_memory_engine::map_physical_page_to_host_base(u64 guest_phys, bool verbose)
    {
        const auto range_result = find_physical_range(guest_phys);
        if (!range_result.has_value()) {
            utility::verbose_log(verbose, "[map] no physical range found for GPA " + utility::hex(guest_phys));
            return std::nullopt;
        }

        u64 range_start { };
        if (!read_host_value(*range_result + offsets::range_start, range_start))
            return std::nullopt;

        const auto page_index = (guest_phys - range_start) >> 12;
        const auto page_description = *range_result + offsets::range_page_desc_table + page_index * offsets::range_page_desc_stride;

        u64 state { };
        u32 info { };
        if (!read_host_value(page_description + offsets::page_desc_state, state) || !read_host_value(page_description + offsets::page_desc_info, info))
            return std::nullopt;

        const auto page_class = static_cast<u32>((state >> 51) & 0x7);
        const auto read_map_kind = static_cast<u32>((state >> 48) & 0x3);

        u8 ram_range_mode { };
        if (!read_host_value(vm_info_value.pvm + offsets::vm_ram_range_mode, ram_range_mode)) {
            ram_range_mode = 0;
        }

        if (verbose) {
            std::cout << "[map] desc=" << utility::hex(page_description) << " state=" << utility::hex(state) << " info=" << utility::hex(info) << " page_class=" << page_class << " read_map_kind=" << read_map_kind << " ram_range_mode=" << static_cast<unsigned>(ram_range_mode) << '\n';
        }

        if (page_class != 2 && page_class != 3 && ram_range_mode != 0) {
            u64 range_host_base { };
            if (!read_host_value(*range_result + offsets::range_host_base, range_host_base) || !utility::is_likely_user_pointer(range_host_base))
                return std::nullopt;

            return range_host_base + ((guest_phys - range_start) & utility::k_page_mask);
        }

        if (read_map_kind == 0)
            return vm_info_value.pvm + offsets::vm_direct_ram_window;

        if (page_class == 2 || page_class == 3) {
            const auto mmio_id = info >> 24;
            const auto mmio_page = info & 0x00FFFFFF;

            u64 mmio_range { };
            if (!read_host_value(vm_info_value.pvm + offsets::vm_mmio2_table + static_cast<u64>(mmio_id) * sizeof(u64), mmio_range) || !utility::is_likely_user_pointer(mmio_range))
                return std::nullopt;

            u64 host_base { };
            if (!read_host_value(mmio_range + offsets::mmio_range_host_base, host_base))
                return std::nullopt;

            return host_base + (static_cast<u64>(mmio_page) << 12);
        }

        const auto chunk_id = info >> 9;
        const auto page_in_chunk = info & 0x1FF;
        if (chunk_id != 0) {
            auto chunk_result = find_chunk_from_cache(chunk_id);
            if (!chunk_result.has_value()) {
                chunk_result = find_chunk_from_tree(chunk_id, verbose);
                if (!chunk_result.has_value()) {
                    u64 remote_chunk { };
                    if (map_chunk_via_bstk_vmm(chunk_id, remote_chunk, verbose)) {
                        chunk_result = remote_chunk;
                    }
                }

                if (!chunk_result.has_value())
                    return std::nullopt;
            }

            u64 chunk_host_base { };
            if (!read_host_value(*chunk_result + offsets::chunk_host_base, chunk_host_base))
                return std::nullopt;

            return chunk_host_base + (static_cast<u64>(page_in_chunk) << 12);
        }

        if (info == 0) {
            u64 direct_tag { };
            if (!read_host_value(vm_info_value.pvm + offsets::vm_direct_ram_tag, direct_tag))
                return std::nullopt;

            const bool direct_class = page_class == 4;
            const bool direct_state_match = (state & utility::k_direct_state_mask_high) == 0 && (state & utility::k_direct_state_mask_low) == direct_tag;
            if (direct_class || direct_state_match)
                return vm_info_value.pvm + offsets::vm_direct_ram_window;
        }

        return std::nullopt;
    }

    std::optional<u64> c_memory_engine::map_physical_to_host(u64 guest_phys, bool verbose)
    {
        if (const auto cached_host_page = lookup_cached_host_page(guest_phys); cached_host_page.has_value())
            return *cached_host_page | (guest_phys & utility::k_page_offset_mask);

        const auto page_map_result = try_page_map_cache(guest_phys, verbose);
        if (page_map_result.has_value()) {
            store_cached_host_page(guest_phys, *page_map_result & utility::k_page_mask);
            return page_map_result;
        }

        const auto page_base = map_physical_page_to_host_base(guest_phys, verbose);
        if (!page_base.has_value())
            return std::nullopt;

        store_cached_host_page(guest_phys, *page_base);
        return *page_base | (guest_phys & utility::k_page_offset_mask);
    }

    std::optional<c_translation_result> c_memory_engine::resolve_guest_physical(u64 guest_phys, bool verbose)
    {
        const auto host_pointer = map_physical_to_host(guest_phys, verbose);
        if (!host_pointer.has_value())
            return std::nullopt;

        c_translation_result result { };
        result.guest_physical = guest_phys;
        result.host_pointer   = *host_pointer;
        return result;
    }

    bool c_memory_engine::read_guest_physical(u64 guest_phys, void* buffer, usize size, bool verbose)
    {
        if (buffer == nullptr || size == 0)
            return false;

        auto* output = static_cast<u8*>(buffer);
        usize copied { };

        while (copied < size) {
            const auto current_phys = guest_phys + copied;
            const auto chunk_size = (std::min)(size - copied, static_cast<usize>(utility::k_page_size - (current_phys & utility::k_page_offset_mask)));
            const auto translation_result = resolve_guest_physical(current_phys, verbose);
            if (!translation_result.has_value())
                return false;

            if (!read_host_bytes(translation_result->host_pointer, output + copied, chunk_size)) {
                invalidate_cached_host_page(current_phys);

                const auto retry_result = resolve_guest_physical(current_phys, verbose);
                if (!retry_result.has_value() || !read_host_bytes(retry_result->host_pointer, output + copied, chunk_size))
                    return false;
            }

            copied += chunk_size;
        }

        return true;
    }

    bool c_memory_engine::write_guest_physical(u64 guest_phys, const void* buffer, usize size, bool verbose)
    {
        if (buffer == nullptr || size == 0)
            return false;

        const auto* input = static_cast<const u8*>(buffer);
        usize written { };

        while (written < size) {
            const auto current_phys = guest_phys + written;
            const auto chunk_size = (std::min)(size - written, static_cast<usize>(utility::k_page_size - (current_phys & utility::k_page_offset_mask)));
            const auto translation_result = resolve_guest_physical(current_phys, verbose);
            if (!translation_result.has_value())
                return false;

            if (!write_host_bytes(translation_result->host_pointer, input + written, chunk_size)) {
                invalidate_cached_host_page(current_phys);

                const auto retry_result = resolve_guest_physical(current_phys, verbose);
                if (!retry_result.has_value() || !write_host_bytes(retry_result->host_pointer, input + written, chunk_size))
                    return false;
            }

            written += chunk_size;
        }

        return true;
    }

    bool c_memory_engine::read_guest_kernel(u64 kernel_va, void* buffer, usize size, bool verbose)
    {
        if (buffer == nullptr || size == 0)
            return false;

        auto* output = static_cast<u8*>(buffer);
        usize copied { };

        while (copied < size) {
            const auto current_va = kernel_va + copied;
            const auto chunk_size = (std::min)(size - copied, static_cast<usize>(utility::k_page_size - (current_va & utility::k_page_offset_mask)));

            u64 current_phys { };
            if (!utility::try_convert_kernel_virtual_to_physical(current_va, current_phys))
                return false;

            if (!read_guest_physical(current_phys, output + copied, chunk_size, verbose))
                return false;

            copied += chunk_size;
        }

        return true;
    }

    bool c_memory_engine::write_guest_kernel(u64 kernel_va, const void* buffer, usize size, bool verbose)
    {
        if (buffer == nullptr || size == 0)
            return false;

        const auto* input = static_cast<const u8*>(buffer);
        usize written { };

        while (written < size) {
            const auto current_va = kernel_va + written;
            const auto chunk_size = (std::min)(size - written, static_cast<usize>(utility::k_page_size - (current_va & utility::k_page_offset_mask)));

            u64 current_phys { };
            if (!utility::try_convert_kernel_virtual_to_physical(current_va, current_phys))
                return false;

            if (!write_guest_physical(current_phys, input + written, chunk_size, verbose))
                return false;

            written += chunk_size;
        }

        return true;
    }

    std::optional<std::string> c_memory_engine::read_guest_kernel_c_string(u64 kernel_va, usize max_chars, bool verbose)
    {
        if (kernel_va == 0 || max_chars == 0)
            return std::nullopt;

        std::string output { };
        output.reserve(max_chars);

        for (usize index = 0; index < max_chars; ++index) {
            char character { };
            if (!read_guest_kernel(kernel_va + index, character, verbose)) {
                break;
            }

            if (character == '\0') {
                break;
            }

            output.push_back(character);
        }

        if (output.empty())
            return std::nullopt;

        return utility::sanitize_guest_string(std::move(output));
    }

    std::optional<std::string> c_memory_engine::read_guest_kernel_string(u64 kernel_va, usize size, bool verbose)
    {
        if (kernel_va == 0 || size == 0)
            return std::nullopt;

        std::vector<char> buffer(size + 1, '\0');
        if (!read_guest_kernel(kernel_va, buffer.data(), size, verbose))
            return std::nullopt;

        return utility::sanitize_guest_string(std::string(buffer.data(), size));
    }

    std::optional<u64> c_memory_engine::read_mm_cr3(u64 mm_va, bool verbose)
    {
        u64 pgd_kernel_va { };
        if (!read_guest_kernel(mm_va + offsetof(kernel::c_guest_mm_struct, pgd), pgd_kernel_va, verbose) || pgd_kernel_va == 0)
            return std::nullopt;

        u64 pgd_phys { };
        if (!utility::try_convert_kernel_virtual_to_physical(pgd_kernel_va, pgd_phys))
            return std::nullopt;

        return pgd_phys;
    }

    std::optional<u64> c_memory_engine::translate_guest_va(u64 guest_cr3, u64 guest_va, bool verbose)
    {
        const auto canonical_check = guest_va >> 47;
        if (canonical_check != 0 && canonical_check != 0x1FFFFULL)
            return std::nullopt;

        if (const auto cached_page = lookup_cached_physical_page(guest_cr3, guest_va); cached_page.has_value())
            return *cached_page | (guest_va & utility::k_page_offset_mask);

        const auto pml4_base = guest_cr3 & utility::k_long_mode_page_frame_mask;

        u64 pml4e { };
        if (!read_guest_physical(pml4_base + ((guest_va >> 39) & 0x1FFULL) * sizeof(u64), pml4e, verbose) || (pml4e & utility::k_present_bit) == 0)
            return std::nullopt;

        const auto pdpt_base = pml4e & utility::k_long_mode_page_frame_mask;

        u64 pdpte { };
        if (!read_guest_physical(pdpt_base + ((guest_va >> 30) & 0x1FFULL) * sizeof(u64), pdpte, verbose) || (pdpte & utility::k_present_bit) == 0)
            return std::nullopt;

        if ((pdpte & utility::k_page_size_bit) != 0) {
            const auto resolved = (pdpte & utility::k_large_page_1gb_mask) | (guest_va & 0x3FFFFFFFULL);
            store_cached_physical_page(guest_cr3, guest_va, resolved & utility::k_page_mask);
            return resolved;
        }

        const auto pd_base = pdpte & utility::k_long_mode_page_frame_mask;

        u64 pde { };
        if (!read_guest_physical(pd_base + ((guest_va >> 21) & 0x1FFULL) * sizeof(u64), pde, verbose) || (pde & utility::k_present_bit) == 0)
            return std::nullopt;

        if ((pde & utility::k_page_size_bit) != 0) {
            const auto resolved = (pde & utility::k_large_page_2mb_mask) | (guest_va & 0x1FFFFFULL);
            store_cached_physical_page(guest_cr3, guest_va, resolved & utility::k_page_mask);
            return resolved;
        }

        const auto pt_base = pde & utility::k_long_mode_page_frame_mask;

        u64 pte { };
        if (!read_guest_physical(pt_base + ((guest_va >> 12) & 0x1FFULL) * sizeof(u64), pte, verbose) || (pte & utility::k_present_bit) == 0)
            return std::nullopt;

        const auto resolved = (pte & utility::k_long_mode_page_frame_mask) | (guest_va & utility::k_page_offset_mask);
        store_cached_physical_page(guest_cr3, guest_va, resolved & utility::k_page_mask);
        return resolved;
    }

    bool c_memory_engine::read_guest_user(u64 guest_cr3, u64 guest_va, void* buffer, usize size, bool verbose)
    {
        if (buffer == nullptr || size == 0)
            return false;

        auto* output = static_cast<u8*>(buffer);
        usize copied { };

        while (copied < size) {
            const auto current_va = guest_va + copied;
            const auto chunk_size = (std::min)(size - copied, static_cast<usize>(utility::k_page_size - (current_va & utility::k_page_offset_mask)));
            const auto page_offset = current_va & utility::k_page_offset_mask;

            if (const auto host_page = lookup_cached_host_page_by_gva(guest_cr3, current_va); host_page.has_value()) {
                if (read_host_bytes(*host_page + page_offset, output + copied, chunk_size)) {
                    copied += chunk_size;
                    continue;
                }

                invalidate_cached_host_page_by_gva(guest_cr3, current_va);
            }

            bool read_ok { };
            for (s32 attempt = 0; attempt < 2 && !read_ok; ++attempt) {
                const auto phys_result = translate_guest_va(guest_cr3, current_va, verbose);
                if (!phys_result.has_value()) {
                    invalidate_cached_physical_page(guest_cr3, current_va);
                    continue;
                }

                const auto host_result = map_physical_to_host(*phys_result, verbose);
                if (!host_result.has_value()) {
                    invalidate_cached_physical_page(guest_cr3, current_va);
                    invalidate_cached_host_page(*phys_result);
                    continue;
                }

                if (read_host_bytes(*host_result, output + copied, chunk_size)) {
                    store_cached_host_page_by_gva(guest_cr3, current_va, *host_result & utility::k_page_mask);
                    read_ok = true;
                    break;
                }

                invalidate_cached_host_page_by_gva(guest_cr3, current_va);
                invalidate_cached_physical_page(guest_cr3, current_va);
                invalidate_cached_host_page(*phys_result);
            }

            if (!read_ok)
                return false;

            copied += chunk_size;
        }

        return true;
    }

    bool c_memory_engine::write_guest_user(u64 guest_cr3, u64 guest_va, const void* buffer, usize size, bool verbose)
    {
        if (buffer == nullptr || size == 0)
            return false;

        const auto* input = static_cast<const u8*>(buffer);
        usize written { };

        while (written < size) {
            const auto current_va = guest_va + written;
            const auto chunk_size = (std::min)(size - written, static_cast<usize>(utility::k_page_size - (current_va & utility::k_page_offset_mask)));
            const auto page_offset = current_va & utility::k_page_offset_mask;

            if (const auto host_page = lookup_cached_host_page_by_gva(guest_cr3, current_va); host_page.has_value()) {
                if (write_host_bytes(*host_page + page_offset, input + written, chunk_size)) {
                    written += chunk_size;
                    continue;
                }

                invalidate_cached_host_page_by_gva(guest_cr3, current_va);
            }

            bool write_ok { };
            for (s32 attempt = 0; attempt < 2 && !write_ok; ++attempt) {
                const auto phys_result = translate_guest_va(guest_cr3, current_va, verbose);
                if (!phys_result.has_value()) {
                    invalidate_cached_physical_page(guest_cr3, current_va);
                    continue;
                }

                const auto host_result = map_physical_to_host(*phys_result, verbose);
                if (!host_result.has_value()) {
                    invalidate_cached_physical_page(guest_cr3, current_va);
                    invalidate_cached_host_page(*phys_result);
                    continue;
                }

                if (write_host_bytes(*host_result, input + written, chunk_size)) {
                    store_cached_host_page_by_gva(guest_cr3, current_va, *host_result & utility::k_page_mask);
                    write_ok = true;
                    break;
                }

                invalidate_cached_host_page_by_gva(guest_cr3, current_va);
                invalidate_cached_physical_page(guest_cr3, current_va);
                invalidate_cached_host_page(*phys_result);
            }

            if (!write_ok)
                return false;

            written += chunk_size;
        }

        return true;
    }

    std::optional<std::string> c_memory_engine::read_guest_user_c_string(u64 guest_cr3, u64 guest_va, usize max_chars, bool verbose)
    {
        if (guest_va == 0 || max_chars == 0)
            return std::nullopt;

        std::string output { };
        output.reserve(max_chars);

        for (usize index = 0; index < max_chars; ++index) {
            char character { };
            if (!read_guest_user(guest_cr3, guest_va + index, character, verbose)) {
                break;
            }

            if (character == '\0') {
                break;
            }

            output.push_back(character);
        }

        if (output.empty())
            return std::nullopt;

        return utility::sanitize_guest_string(std::move(output));
    }

    bool c_memory_engine::read_gva(u64 guest_cr3, u64 guest_va, void* buffer, usize size, bool verbose)
    {
        return read_guest_user(guest_cr3, guest_va, buffer, size, verbose);
    }

    bool c_memory_engine::write_gva(u64 guest_cr3, u64 guest_va, const void* buffer, usize size, bool verbose)
    {
        return write_guest_user(guest_cr3, guest_va, buffer, size, verbose);
    }
}

#endif
