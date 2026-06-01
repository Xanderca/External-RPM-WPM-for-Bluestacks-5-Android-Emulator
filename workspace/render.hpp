#ifndef RENDER_HPP
#define RENDER_HPP

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

#include <memory>

#include <workspace/types.hpp>

namespace renderer {

    class c_render {
    public:
        c_render() = default;
        ~c_render();

    private:
        HWND                    overlay_window     { };
        WNDCLASSEXW             window_class       { };
        IDXGISwapChain*         swap_chain         { };
        ID3D11Device*           device             { };
        ID3D11DeviceContext*    device_context     { };
        ID3D11RenderTargetView* render_target_view { };
        bool                    is_setup_done      { };
        bool                    is_frame_started   { };
        s32                     window_x           { };
        s32                     window_y           { };
        s32                     window_width       { };
        s32                     window_height      { };

    public:
        bool setup();
        void destroy();
        bool start();
        void end();

    private:
        bool create_window();
        bool create_device();
        void destroy_window();
        void destroy_device();
        void create_render_target();
        void destroy_render_target();
        void process_messages();
        bool update_window_rect();
        void apply_window_rect();
    };
}

inline auto render = std::make_shared<renderer::c_render>();

#endif
