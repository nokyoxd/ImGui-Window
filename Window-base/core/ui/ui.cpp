#include "ui.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam
);

// Forward declare message handler
LRESULT WINAPI WndProc(
    HWND hWnd,
    UINT uMessage,
    WPARAM wParam,
    LPARAM lParam
);

void UI::Setup()
{
    CreateHWindow("Window Name", "Window Class Name");
    CreateDevice();
    CreateImGui();

    while (bRun)
    {
        CreateRender();
        Render();
        DestroyRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    DestroyImGui();
    DestroyDevice();
    DestroyHWindow();
}

void UI::CreateHWindow(const char* szWindowName, const char* szWindowClassName)
{
    /* Center window pos */
    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN) / 2 - iWidth / 2;
    int iScreenHeight = GetSystemMetrics(SM_CYSCREEN) / 2 - iHeight / 2;

    WndClass.cbSize = sizeof(WNDCLASSEX);
    WndClass.style = CS_CLASSDC;
    WndClass.lpfnWndProc = WndProc;
    WndClass.cbClsExtra = 0L;
    WndClass.cbWndExtra = 0L;
    WndClass.hInstance = GetModuleHandle(NULL);
    WndClass.hIcon = NULL;
    WndClass.hCursor = NULL;
    WndClass.hbrBackground = NULL;
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = szWindowClassName;
    WndClass.hIconSm = NULL;

    RegisterClassEx(&WndClass);
    HWnd = CreateWindow(
        WndClass.lpszClassName,
        szWindowName,
        WS_POPUP,
        iScreenWidth,
        iScreenHeight,
        iWidth, 
        iHeight, 
        NULL, 
        NULL, 
        WndClass.hInstance,
        NULL
    );

    /* Windows margins */
    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(HWnd, &margins);

    ShowWindow(HWnd, SW_SHOWDEFAULT);
    UpdateWindow(HWnd);
}

void UI::DestroyHWindow()
{
    DestroyWindow(HWnd);
    UnregisterClass(WndClass.lpszClassName, WndClass.hInstance);
}

bool UI::CreateDevice() 
{
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D)
        return false;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT, 
        D3DDEVTYPE_HAL,
        HWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &g_d3dpp,
        &g_pd3dDevice) < 0)
            return false;

    return true;
}

void UI::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void UI::DestroyDevice()
{
    if (g_pd3dDevice) 
    { 
        g_pd3dDevice->Release(); 
        g_pd3dDevice = nullptr;
    }

    if (g_pD3D) 
    { 
        g_pD3D->Release(); 
        g_pD3D = nullptr; 
    }
}

void UI::CreateImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    /* Disable file log */
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;             

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(HWnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);
}

void UI::DestroyImGui()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UI::CreateRender()
{
    MSG message;
    while (PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            bRun = !bRun;
            return;
        }
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void UI::Render()
{
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ UI::iWidth, UI::iHeight });
    ImGui::Begin("##window", &bRun, ImGuiWindowFlags_NoResize);
    {
        

        
    }
    ImGui::End();
}

void UI::DestroyRender()
{
    ImGui::EndFrame();

    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    g_pd3dDevice->Clear(
        0, 
        NULL, 
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
        0, 
        1.0f, 
        0
    );

    if (g_pd3dDevice->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        g_pd3dDevice->EndScene();
    }

    HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
    if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}

LRESULT WINAPI WndProc(
    HWND hWnd,
    UINT uMessage,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMessage, wParam, lParam))
        return true;

    switch (uMessage)
    {
    case WM_SIZE:
    {
        if (UI::g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            UI::g_d3dpp.BackBufferWidth = LOWORD(lParam);
            UI::g_d3dpp.BackBufferHeight = HIWORD(lParam);
            UI::ResetDevice();
        }
    }  return 0;
    case WM_SYSCOMMAND:
    {
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
    } break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
    } return 0;
    case WM_LBUTTONDOWN:
    {
        UI::Pos = MAKEPOINTS(lParam);
    } return 0;
    case WM_MOUSEMOVE:
    {
        /* Window movement */
        if (wParam == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(lParam);
            RECT rect{ };

            GetWindowRect(UI::HWnd, &rect);
            rect.left += points.x - UI::Pos.x;
            rect.top += points.y - UI::Pos.y;

            if (UI::Pos.x >= 0 &&
                UI::Pos.x <= UI::iWidth &&
                UI::Pos.y >= 0 && UI::Pos.y <= 20)
                SetWindowPos(
                    UI::HWnd,
                    HWND_TOPMOST,
                    rect.left,
                    rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
        }
    } return 0;
    }

    return DefWindowProc(hWnd, uMessage, wParam, lParam);
}