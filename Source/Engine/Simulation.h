#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include "directx/d3dx12.h"

#include "Engine/BaseTypes.h"

#ifdef _DEBUG
#include "Debug/DebugLayer.h"
#endif

class Simulation
{
public:
    static constexpr uint s_FrameCount = 2;

public:
    Simulation();

    // Forward declarations
    void Init(const uint width, const uint height, const HWND hwnd);
    void Release();
    void ResizeScreen(const uint width, const uint height);
    void Render();

protected:
    virtual void PopulateCommandList() = 0;
    virtual void PostInit() {};
    virtual void PreRelease() {};
    virtual void PostResize() {};
    void WaitForPreviousFrame();
    void InitD3D12(const uint width, const uint height, const HWND hwnd);

protected:
    ComPtr<ID3D12Device> m_Device;
    ComPtr<IDXGISwapChain3> m_SwapChain;
    ComPtr<ID3D12CommandQueue> m_CommandQueue;
    ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
    ComPtr<ID3D12DescriptorHeap> m_CbvUavSrvHeap;
    ComPtr<ID3D12DescriptorHeap> m_SamplerHeap;
    ComPtr<ID3D12Resource> m_RenderTargets[s_FrameCount];
    ComPtr<ID3D12Resource> m_DepthBuffers[s_FrameCount];
    ComPtr<ID3D12CommandAllocator> m_CommandAllocator[s_FrameCount];
    ComPtr<ID3D12GraphicsCommandList> m_CommandList;
    uint m_RtvDescriptorSize;
    uint m_DsvDescriptorSize;
    uint m_FrameIndex;

    // Fence objects
    ComPtr<ID3D12Fence> m_Fence;
    uint64 m_FenceValue[s_FrameCount];
    HANDLE m_FenceEvent;

#ifdef _DEBUG
    D3D12DebugLayer m_DebugLayer;
#endif
};
