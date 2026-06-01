#ifndef MAIN_CPP
#define MAIN_CPP

#include <impl/includes.hpp>

uptr libunity_base{ };

namespace offsets {
    constexpr uptr base_world_typeinfo_ref{ 0xD2B1EA8 };
    constexpr uptr player_camera_typeinfo_ref{ 0xD29E928 };

    constexpr uptr il2cpp_object_header{ 0x10 };
    constexpr uptr il2cpp_class_static_fields{ 0x80 };
    constexpr uptr base_world_instance{ 0x0 };
    constexpr uptr base_world_game{ 0x18 };
    constexpr uptr base_game_enemy_pawns{ 0x178 };
    constexpr uptr list_items{ 0x10 };
    constexpr uptr list_size{ 0x18 };
    constexpr uptr array_items{ 0x20 };
    constexpr uptr player_camera_world_camera{ 0x10 };
    constexpr uptr camera_view_matrix{ 0xD0 };
    constexpr uptr pawn_is_alive{ 0x548 };
    constexpr uptr pawn_position{ 0x5D4 };
}

struct vec2_t {
    float x, y;

    vec2_t() : x(0.f), y(0.f) {}
    vec2_t(float px, float py) : x(px), y(py) {}
};

struct vec3_t {
    float x, y, z;

    vec3_t() : x(0.f), y(0.f), z(0.f) {}
    vec3_t(float px, float py, float pz) : x(px), y(py), z(pz) {}

    bool is_zero() const { 
        return x == 0.f && y == 0.f && z == 0.f;
    }
};

struct matrix4x4_t {
    float m00{}, m10{}, m20{}, m30{};
    float m01{}, m11{}, m21{}, m31{};
    float m02{}, m12{}, m22{}, m32{};
    float m03{}, m13{}, m23{}, m33{};
};

uptr get_static_fields(uptr typeinfo_ref)
{
    auto typeinfo_ptr = memory->read<uptr>(libunity_base + typeinfo_ref);
    if (!typeinfo_ptr) 
        return 0;

    auto klass = memory->read<uptr>(typeinfo_ptr);
    if (!klass) 
        return 0;

    return memory->read<uptr>(klass + offsets::il2cpp_class_static_fields);
}

matrix4x4_t get_view_matrix()
{
    auto static_fields = get_static_fields(offsets::player_camera_typeinfo_ref);
    if (!static_fields) 
        return {};

    auto world_camera = memory->read<uptr>(static_fields + offsets::player_camera_world_camera);
    if (!world_camera) 
        return {};

    return memory->read<matrix4x4_t>(memory->read<uptr>(world_camera + offsets::il2cpp_object_header) + offsets::camera_view_matrix);
}

bool world_to_screen(matrix4x4_t vm, vec3_t& wp, vec2_t& out)
{
    auto ds = ImGui::GetIO().DisplaySize;
    if (ds.x <= 0.f || ds.y <= 0.f)
        return false;

    float cx = wp.x * vm.m00 + wp.y * vm.m01 + wp.z * vm.m02 + vm.m03;
    float cy = wp.x * vm.m10 + wp.y * vm.m11 + wp.z * vm.m12 + vm.m13;
    float cw = wp.x * vm.m30 + wp.y * vm.m31 + wp.z * vm.m32 + vm.m33;

    if (cw <= 0.01f)
        return false;

    float scx = ds.x * 0.5f;
    float scy = ds.y * 0.5f;

    out = vec2_t{ scx + (cx / cw) * scx, scy - (cy / cw) * scy };
    return true;
}

void func()
{
    auto view_matrix = get_view_matrix();

    auto base_world = memory->read<uptr>(get_static_fields(offsets::base_world_typeinfo_ref) + offsets::base_world_instance);
    if (!base_world)
        return;

    auto base_game = memory->read<uptr>(base_world + offsets::base_world_game);
    if (!base_game)
        return;

    auto enemy_list = memory->read<uptr>(base_game + offsets::base_game_enemy_pawns);
    auto enemy_array = memory->read<uptr>(enemy_list + offsets::list_items);
    auto enemy_count = memory->read<i32>(enemy_list + offsets::list_size);

    for (i32 i{}; i < enemy_count; ++i) {
        auto pawn = memory->read<uptr>(enemy_array + offsets::array_items + static_cast<uptr>(i) * sizeof(uptr));
        if (!pawn)
            continue;

        auto is_alive = memory->read<bool>(pawn + offsets::pawn_is_alive);
        if (!is_alive)
            continue;

        vec2_t screen_pos;
        vec3_t pos = memory->read<vec3_t>(pawn + offsets::pawn_position);
        if (!world_to_screen(view_matrix, pos, screen_pos))
            continue;

        ImGui::GetBackgroundDrawList()->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y), ImVec2(screen_pos.x, screen_pos.y), IM_COL32(255, 50, 50, 200), 1.5f);
    }
}

int main()
{
    if (!bluestacks->setup("com.activision.callofduty.shooter"))
        return 1;

    libunity_base = bluestacks->get_module_base("libunity.so", 1);
    if (!libunity_base) 
        return 1;

    std::cout << "libunity base: 0x" << std::hex << libunity_base << std::endl;

    if (!render->setup()) 
        return 1;

    while (!GetAsyncKeyState(VK_END))
    {
        render->start();

        func();

        render->end();
    }

    return 0;
}

#endif