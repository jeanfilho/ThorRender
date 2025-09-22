#include "Engine/Simulation.h"
#include <stdexcept>

Simulation::Simulation() :
    m_RtvDescriptorSize(0),
    m_FrameIndex(0),
    m_Device(nullptr),
    m_SwapChain(nullptr),
    m_CommandQueue(nullptr),
    m_RtvHeap(nullptr),
    m_CommandList(nullptr),
    m_Fence(nullptr),
    m_RenderTargets{},
    m_CommandAllocator{},
    m_FenceValue{},
    m_FenceEvent(nullptr)
#ifdef _DEBUG
    // Debug layer is automatically initialized by its constructor
    , m_DebugLayer(true)
#endif
{

}

void Simulation::Init(const uint width, const uint height, const HWND hwnd)
{
    InitD3D12(width, height, hwnd);
}

void Simulation::Release()
{
    // Cleanup
    CloseHandle(m_FenceEvent);
}

void Simulation::ResizeScreen(const uint width, const uint height)
{
    if (!m_SwapChain) return;

    // Wait until all previous GPU work is complete
    WaitForPreviousFrame();

    // Reset render targets to recreate them
    for (uint i = 0; i < FrameCount; ++i)
        m_RenderTargets[i].Reset();

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    m_SwapChain->GetDesc1(&desc);
    m_SwapChain->ResizeBuffers(FrameCount, width, height, desc.Format, desc.Flags);

    m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

    // Recreate render target views
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (uint i = 0; i < FrameCount; ++i)
    {
        m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
        m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, m_RtvDescriptorSize);
    }
}

void Simulation::InitD3D12(const uint width, const uint height, const HWND hwnd)
{
    // Device
    ComPtr<IDXGIFactory7> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    ComPtr<IDXGIAdapter1> adapter;
    factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter1), &adapter);
    D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_Device));

#ifdef _DEBUG
    // Attach debug layer to device
    m_DebugLayer.AttachToDevice(m_Device);
#endif

    // Command queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));

    // Swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

    ComPtr<IDXGISwapChain1> swapChain1;
    factory->CreateSwapChainForHwnd(
        m_CommandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1
    );
    swapChain1.As(&m_SwapChain);
    m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

    // RTV heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap));
    m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Render targets and allocators
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (uint i = 0; i < FrameCount; ++i) {
        m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
        m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, m_RtvDescriptorSize);

        m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator[i]));
        m_FenceValue[i] = 0;
    }

    // Command list
    m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator[m_FrameIndex].Get(), nullptr, IID_PPV_ARGS(&m_CommandList));
    m_CommandList->Close();

    // Fence
    m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
    m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_FenceEvent) throw std::runtime_error("Failed to create fence event.");
}

void Simulation::PopulateCommandList()
{
    m_CommandAllocator[m_FrameIndex]->Reset();
    m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), nullptr);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets[m_FrameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    m_CommandList->ResourceBarrier(1, &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
    FLOAT clearColor[] = { 0.2f, 0.4f, 0.6f, 1.0f };
    m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets[m_FrameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    m_CommandList->ResourceBarrier(1, &barrier);

    m_CommandList->Close();
}

void Simulation::WaitForPreviousFrame()
{
    // Signal and increment the fence value.
    const uint64 fenceToWaitFor = ++m_FenceValue[m_FrameIndex];
    m_CommandQueue->Signal(m_Fence.Get(), fenceToWaitFor);

    // Wait until the previous frame is finished.
    if (m_Fence->GetCompletedValue() < fenceToWaitFor) {
        m_Fence->SetEventOnCompletion(fenceToWaitFor, m_FenceEvent);
        WaitForSingleObject(m_FenceEvent, INFINITE);
    }

    m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void Simulation::Render()
{
    PopulateCommandList();

    ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
    m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    m_SwapChain->Present(1, 0);

    WaitForPreviousFrame();

#ifdef _DEBUG
    // Dump debug messages to Output window
    m_DebugLayer.DumpStoredMessages();
#endif
}
