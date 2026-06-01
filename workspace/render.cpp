#ifndef RENDER_CPP
#define RENDER_CPP

#include <impl/includes.hpp>

#include <dwmapi.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

namespace {

    LRESULT CALLBACK overlay_wnd_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
    {
        ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam);

        if (message == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(window, message, wparam, lparam);
    }
}

namespace renderer {

    c_render::~c_render()
    {
        destroy();
    }

    bool c_render::setup()
    {
        if (is_setup_done)
            return true;

        if (!create_window())
            return false;

        if (!create_device()) {
            destroy();
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        auto& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        ImGui::StyleColorsDark();
        ImGui::GetStyle().WindowBorderSize = 0.0f;
        ImGui::GetStyle().WindowRounding = 0.0f;

        ImGui_ImplWin32_Init(overlay_window);
        ImGui_ImplWin32_EnableAlphaCompositing(overlay_window);
        ImGui_ImplDX11_Init(device, device_context);

        is_setup_done = true;
        return true;
    }

    void c_render::destroy()
    {
        if (is_frame_started) {
            ImGui::EndFrame();
            is_frame_started = false;
        }

        if (ImGui::GetCurrentContext() != nullptr) {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        destroy_device();
        destroy_window();

        is_setup_done = false;
    }

    bool c_render::start()
    {
        if (!is_setup_done)
            return false;

        process_messages();

        if (!is_setup_done)
            return false;

        if (!update_window_rect())
            return false;

        SetWindowPos(overlay_window, HWND_TOPMOST, window_x, window_y, window_width, window_height, SWP_NOACTIVATE | SWP_SHOWWINDOW);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        is_frame_started = true;
        return true;
    }

    void c_render::end()
    {
        if (!is_setup_done)
            return;

        if (!is_frame_started)
            return;

        ImGui::Render();

        const float clear_color[4] { 0.0f, 0.0f, 0.0f, 0.0f };

        device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
        device_context->ClearRenderTargetView(render_target_view, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swap_chain->Present(1, 0);

        is_frame_started = false;
    }

    bool c_render::create_window()
    {
        update_window_rect();

        window_class = { };
        window_class.cbSize = sizeof(WNDCLASSEXW);
        window_class.lpfnWndProc = overlay_wnd_proc;
        window_class.hInstance = GetModuleHandleW(nullptr);
        window_class.lpszClassName = L"astrum_overlay_window";

        RegisterClassExW(&window_class);

        overlay_window = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, window_class.lpszClassName, L"astrum_overlay_window", WS_POPUP, window_x, window_y, window_width, window_height, nullptr, nullptr, window_class.hInstance, nullptr);
        if (!overlay_window)
            return false;

        SetLayeredWindowAttributes(overlay_window, 0, 255, LWA_ALPHA);

        MARGINS margins { -1, -1, -1, -1 };
        DwmExtendFrameIntoClientArea(overlay_window, &margins);

        ShowWindow(overlay_window, SW_SHOW);
        UpdateWindow(overlay_window);
        SetWindowPos(overlay_window, HWND_TOPMOST, window_x, window_y, window_width, window_height, SWP_NOACTIVATE | SWP_SHOWWINDOW);

        return true;
    }

    bool c_render::create_device()
    {
        DXGI_SWAP_CHAIN_DESC swap_chain_desc { };
        swap_chain_desc.BufferCount = 2;
        swap_chain_desc.BufferDesc.Width = static_cast<UINT>(window_width);
        swap_chain_desc.BufferDesc.Height = static_cast<UINT>(window_height);
        swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.OutputWindow = overlay_window;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.Windowed = TRUE;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        constexpr D3D_FEATURE_LEVEL feature_levels[] {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0
        };

        D3D_FEATURE_LEVEL feature_level { };

        auto result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, feature_levels, static_cast<UINT>(std::size(feature_levels)), D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, &feature_level, &device_context);

        if (FAILED(result)) {
            result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, feature_levels, static_cast<UINT>(std::size(feature_levels)), D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, &feature_level, &device_context);

            if (FAILED(result))
                return false;
        }

        create_render_target();
        return render_target_view != nullptr;
    }

    void c_render::destroy_window()
    {
        if (overlay_window) {
            DestroyWindow(overlay_window);
            overlay_window = nullptr;
        }

        if (window_class.hInstance && window_class.lpszClassName) {
            UnregisterClassW(window_class.lpszClassName, window_class.hInstance);
            window_class = { };
        }
    }

    void c_render::destroy_device()
    {
        destroy_render_target();

        if (swap_chain) {
            swap_chain->Release();
            swap_chain = nullptr;
        }

        if (device_context) {
            device_context->Release();
            device_context = nullptr;
        }

        if (device) {
            device->Release();
            device = nullptr;
        }
    }

    void c_render::create_render_target()
    {
        if (!swap_chain)
            return;

        ID3D11Texture2D* back_buffer { };

        if (FAILED(swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer))))
            return;

        device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
        back_buffer->Release();
    }

    void c_render::destroy_render_target()
    {
        if (!render_target_view)
            return;

        render_target_view->Release();
        render_target_view = nullptr;
    }

    void c_render::process_messages()
    {
        MSG message { };

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);

            if (message.message == WM_QUIT) {
                is_setup_done = false;
                break;
            }
        }
    }

    bool c_render::update_window_rect()
    {
        const auto new_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        const auto new_y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        const auto new_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        const auto new_height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        if (new_width <= 0 || new_height <= 0)
            return false;

        if (new_x == window_x && new_y == window_y && new_width == window_width && new_height == window_height)
            return true;

        window_x = new_x;
        window_y = new_y;
        window_width = new_width;
        window_height = new_height;

        apply_window_rect();

        if (swap_chain) {
            destroy_render_target();
            swap_chain->ResizeBuffers(0, static_cast<UINT>(window_width), static_cast<UINT>(window_height), DXGI_FORMAT_UNKNOWN, 0);
            create_render_target();
        }

        return true;
    }

    void c_render::apply_window_rect()
    {
        if (!overlay_window)
            return;

        SetWindowPos(overlay_window, HWND_TOPMOST, window_x, window_y, window_width, window_height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
}

#endif
