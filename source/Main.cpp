#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "directx/d3dx12.h"
#include <algorithm>
#include <stdexcept>

#ifdef _DEBUG
#include "Debug/DebugLayer.h"
#endif

using namespace Microsoft::WRL;

constexpr UINT FrameCount = 2;

// Globals
UINT g_Width = 800;
UINT g_Height = 600;

HWND g_hwnd = nullptr;
ComPtr<ID3D12Device> g_device;
ComPtr<IDXGISwapChain3> g_swapChain;
ComPtr<ID3D12CommandQueue> g_commandQueue;
ComPtr<ID3D12DescriptorHeap> g_rtvHeap;
ComPtr<ID3D12Resource> g_renderTargets[FrameCount];
ComPtr<ID3D12CommandAllocator> g_commandAllocator[FrameCount];
ComPtr<ID3D12GraphicsCommandList> g_commandList;
UINT g_rtvDescriptorSize;
UINT g_frameIndex;

// Fence objects
ComPtr<ID3D12Fence> g_fence;
UINT64 g_fenceValue[FrameCount] = {};
HANDLE g_fenceEvent = nullptr;

#ifdef _DEBUG
// Global debug layer instance
D3D12DebugLayer g_debugLayer(true);
#endif


// Debug Triangle
#include "Debug/DebugTriangle.h"
std::unique_ptr<DebugTriangle> g_debugTriangle;

// Forward declarations
void InitWindow(HINSTANCE hInstance, int nCmdShow);
void InitD3D12();
void PopulateCommandList();
void WaitForPreviousFrame();
void Render();
void ResizeScreen();

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    // Alt+Enter fullscreen toggle
    switch(message)
    {
    case WM_SIZE:
        g_Width = LOWORD(lParam);
        g_Height = HIWORD(lParam);
        ResizeScreen();
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

    g_hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "DirectX 12 Template",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, g_Width, g_Height,
        nullptr, nullptr, hInstance, nullptr
    );

    ShowWindow(g_hwnd, nCmdShow);
}

void ResizeScreen()
{
    if (!g_swapChain) return;

    // Wait until all previous GPU work is complete
    WaitForPreviousFrame();

    // Reset render targets to recreate them
    for (UINT i = 0; i < FrameCount; ++i)
        g_renderTargets[i].Reset();

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    g_swapChain->GetDesc1(&desc);
    g_swapChain->ResizeBuffers(FrameCount, g_Width, g_Height, desc.Format, desc.Flags);

    g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();

    // Recreate render target views
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < FrameCount; ++i)
    {
        g_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_renderTargets[i]));
        g_device->CreateRenderTargetView(g_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, g_rtvDescriptorSize);
    }
}

void InitD3D12()
{
    // Device
    ComPtr<IDXGIFactory7> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    ComPtr<IDXGIAdapter1> adapter;
    factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter1), &adapter);
    D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&g_device));

#ifdef _DEBUG
    // Attach debug layer to device
    g_debugLayer.AttachToDevice(g_device);
#endif

    // Command queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    g_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_commandQueue));

    // Swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = g_Width;
    swapChainDesc.Height = g_Height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

    ComPtr<IDXGISwapChain1> swapChain1;
    factory->CreateSwapChainForHwnd(
        g_commandQueue.Get(),
        g_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1
    );
    swapChain1.As(&g_swapChain);
    g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();

    // RTV heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    g_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_rtvHeap));
    g_rtvDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Render targets and allocators
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < FrameCount; ++i) {
        g_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_renderTargets[i]));
        g_device->CreateRenderTargetView(g_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, g_rtvDescriptorSize);

        g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocator[i]));
        g_fenceValue[i] = 0;
    }

    // Command list
    g_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocator[g_frameIndex].Get(), nullptr, IID_PPV_ARGS(&g_commandList));
    g_commandList->Close();

    // Fence
    g_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence));
    g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_fenceEvent) throw std::runtime_error("Failed to create fence event.");
}

void PopulateCommandList()
{
    g_commandAllocator[g_frameIndex]->Reset();
    g_commandList->Reset(g_commandAllocator[g_frameIndex].Get(), nullptr);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        g_renderTargets[g_frameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    g_commandList->ResourceBarrier(1, &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_rtvHeap->GetCPUDescriptorHandleForHeapStart(), g_frameIndex, g_rtvDescriptorSize);
    FLOAT clearColor[] = { 0.2f, 0.4f, 0.6f, 1.0f };
    g_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);


    g_debugTriangle->Draw(g_commandList.Get(),
        rtvHandle,
        g_renderTargets[g_frameIndex]->GetDesc().Width,
        g_renderTargets[g_frameIndex]->GetDesc().Height);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        g_renderTargets[g_frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    g_commandList->ResourceBarrier(1, &barrier);

    g_commandList->Close();
}

void InitApp()
{
    g_debugTriangle = std::make_unique<DebugTriangle>(g_device);
}

void WaitForPreviousFrame()
{
    // Signal and increment the fence value.
    const UINT64 fenceToWaitFor = ++g_fenceValue[g_frameIndex];
    g_commandQueue->Signal(g_fence.Get(), fenceToWaitFor);

    // Wait until the previous frame is finished.
    if (g_fence->GetCompletedValue() < fenceToWaitFor) {
        g_fence->SetEventOnCompletion(fenceToWaitFor, g_fenceEvent);
        WaitForSingleObject(g_fenceEvent, INFINITE);
    }

    g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();
}

void Render()
{
    PopulateCommandList();

    ID3D12CommandList* ppCommandLists[] = { g_commandList.Get() };
    g_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    g_swapChain->Present(1, 0);

    WaitForPreviousFrame();

#ifdef _DEBUG
    // Dump debug messages to Output window
    g_debugLayer.DumpStoredMessages();
#endif
}

// Entry point
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    // Silence "unreferenced parameter" warnings for parameters we don't use.
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitWindow(hInstance, nShowCmd);
    InitD3D12();
    InitApp();

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            Render();
        }
    }

    // Cleanup
    CloseHandle(g_fenceEvent);

    return 0;
}