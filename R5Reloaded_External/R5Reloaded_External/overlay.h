#pragma once
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"
#include <dwmapi.h>
#include <d3d11.h>
#include "Utils\sdk.h"
#include "Utils\globals.h"
#include "Utils\sdk.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Shcore.lib")

#define GREEN ImColor(0, 255, 0)
#define RED ImColor(255, 0, 0)
#define BLUE ImColor(0, 0, 255)
#define SKYBLUE ImColor(0, 255, 255)
#define WHITE ImColor(1.f, 1.f, 1.f, 1.f)

class Overlay
{
public:
    RECT GameSize = {};
    POINT GamePoint = {};

    // Menu
    bool ShowMenu = false;
    bool ESP = true;
    bool DummyESP = false;
    bool TeamESP = false;

    bool ESP_Box = true;
    int BoxType = 1;
    bool ESP_Line = false;
    bool ESP_Distance = true;
    float MaxDistance = 500.f;
    bool ESP_HealthBar = true;

    bool DrawFov = true;

    ImColor ESP_Normal = { 1.f, 0.f, 0.0f, 1.f };
    ImColor ESP_Visible = { 1.f, 1.f, 1.f, 1.f };
    ImColor ESP_Team = { 0.f, 1.f, 1.f, 1.f };

    void m_Info();
    void m_Menu();
    void m_ESP();

    void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width)
    {
        ImGui::GetWindowDrawList()->AddLine(a, b, color, width);
    }
    void DrawBox(ImColor color, float x, float y, float w, float h)
    {
        DrawLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.0f);
        DrawLine(ImVec2(x, y), ImVec2(x, y + h), color, 1.0f);
        DrawLine(ImVec2(x + w, y), ImVec2(x + w, y + h), color, 1.0f);
        DrawLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, 1.0f);
    }
    void Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
    {
        ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, color, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
    }
    void String(ImVec2 pos, ImColor color, const char* text)
    {
        Text(pos, color, text, text + strlen(text), 200, 0);
    }
    void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
    }
    void ProgressBar(float x, float y, float w, float h, int value, int v_max)
    {
        ImColor barColor = ImColor(min(510 * (v_max - value) / 100, 255), min(510 * value / 100, 255), 25, 255);

        RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), barColor, 0.0f, 0);
    }
    void BaseBar(float x, float y, float w, float h)
    {
        RectFilled(x, y, x + w, y + ((h / float(100)) * (float)100), ImColor(0.f, 0.f, 0.f, 1.f), 0.f, 0);
    }
    void Circle(float x, float y, float fov_size, ImColor color, float size)
    {
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(x / 2.f, y / 2.f), fov_size, color, 100, size);
    }
};

static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

static bool IsKeyDown(int VK)
{
    return (GetAsyncKeyState(VK) & 0x8000) != 0;
}

static void LoadStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Border
    style.WindowBorderSize = 0.f;
    style.FrameBorderSize = 0.f;
    style.ChildBorderSize = 1.f;

    // Roundering
    style.WindowRounding = 6.f;
    style.FrameRounding = 2.f;
    style.ChildRounding = 6.f;
    style.PopupRounding = 3.f;

    style.WindowPadding = ImVec2(12.f, 12.f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.GrabMinSize = 6;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.89f, 0.10f, 0.44f, 1.00f);

    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.50f, 0.92f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.14f);
    colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.17f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.94f, 1.00f, 0.28f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.06f, 0.45f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.f, 0.f, 0.f, 0.f);

    colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.01f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.06f, 0.45f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.59f, 0.98f, 1.00f);
}

static void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

static void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

static void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

static bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hWnd, msg, wParam, lParam);
}