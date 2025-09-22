#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>
#include <algorithm>
#include <stdexcept>

#include "Simulations/DebugTriangle/DebugTriangleSimulation.h"
#include "Simulations/MeshTest/MeshTestSimulation.h"

HWND g_HWND = nullptr;

uint g_Width = 800;
uint g_Height = 600;

UniquePtr<Simulation> g_Simulation;

LRESULT CALLBACK WindowProc(HWND hWnd, uint message, WPARAM wParam, LPARAM lParam)
{

    switch(message)
    {
    case WM_SIZE:
        g_Width = LOWORD(lParam);
        g_Height = HIWORD(lParam);
        if(g_Simulation) g_Simulation->ResizeScreen(g_Width, g_Height);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "DX12SampleWindow";
    RegisterClass(&wc);

    g_HWND = CreateWindowEx(
        0,
        wc.lpszClassName,
        "DirectX 12 Template",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, g_Width, g_Height,
        nullptr, nullptr, hInstance, nullptr
    );

    ShowWindow(g_HWND, nCmdShow);
}

// Entry point
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    // Silence "unreferenced parameter" warnings for parameters we don't use.
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitWindow(hInstance, nShowCmd);

    g_Simulation = MakeUnique<MeshTestSimulation>();

    g_Simulation->Init(g_Width, g_Height, g_HWND);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            g_Simulation->Render();
        }
    }

    g_Simulation->Release();

    return 0;
}