#include "Engine/Simulation.h"
#include <stdexcept>

Simulation::Simulation() :
    m_RtvDescriptorSize(0),
    m_DsvDescriptorSize(0),
    m_FrameIndex(0),
    m_Device(nullptr),
    m_SwapChain(nullptr),
    m_CommandQueue(nullptr),
    m_RtvHeap(nullptr),
    m_DsvHeap(nullptr),
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
    PostInit();
}

void Simulation::Release()
{
    PreRelease();
    CloseHandle(m_FenceEvent);
}

void Simulation::ResizeScreen(const uint width, const uint height)
{
    if (!m_SwapChain) return;

    // Wait until all previous GPU work is complete
    WaitForPreviousFrame();

    // Reset render targets to recreate them
    for (uint i = 0; i < s_FrameCount; ++i)
        m_RenderTargets[i].Reset();

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    m_SwapChain->GetDesc1(&desc);
    m_SwapChain->ResizeBuffers(s_FrameCount, width, height, desc.Format, desc.Flags);

    m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

    // Recreate render target views
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (uint i = 0; i < s_FrameCount; ++i)
    {
        m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
        m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, m_RtvDescriptorSize);
    }

    // Recreate depth buffers
    {
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        D3D12_RESOURCE_DESC depthDesc = {};
        depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthDesc.Alignment = 0;
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.DepthOrArraySize = 1;
        depthDesc.MipLevels = 1;
        depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DsvHeap->GetCPUDescriptorHandleForHeapStart());
        for (uint i = 0; i < s_FrameCount; ++i) {
            if (FAILED(m_Device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &depthDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &clearValue,
                IID_PPV_ARGS(&m_DepthBuffers[i]))))
            {
                throw std::runtime_error("Failed to create depth buffer.");
            }
            m_Device->CreateDepthStencilView(m_DepthBuffers[i].Get(), nullptr, dsvHandle);
            dsvHandle.Offset(1, m_DsvDescriptorSize);
        }
    }

    PostResize();
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
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
    }

    // Depth buffers
    {
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        D3D12_RESOURCE_DESC depthDesc = {};
        depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthDesc.Alignment = 0;
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.DepthOrArraySize = 1;
        depthDesc.MipLevels = 1;
        depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;
        for (uint i = 0; i < s_FrameCount; ++i) {
            if (FAILED(m_Device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &depthDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &clearValue,
                IID_PPV_ARGS(&m_DepthBuffers[i]))))
            {
                throw std::runtime_error("Failed to create depth buffer.");
            }
        }
    }

    // Swap chain
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = s_FrameCount;
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
    }

    // Render targets and allocators
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = s_FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap));
        m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = s_FrameCount;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DsvHeap));
        m_DsvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DsvHeap->GetCPUDescriptorHandleForHeapStart());
        for (uint i = 0; i < s_FrameCount; ++i)
        {
            m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
            m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_RtvDescriptorSize);

            m_Device->CreateDepthStencilView(m_DepthBuffers[i].Get(), nullptr, dsvHandle);
            dsvHandle.Offset(1, m_DsvDescriptorSize);

            m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator[i]));
            m_FenceValue[i] = 0;
        }
    }

    // Create UAV SRV CBV descriptor heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 1000000; // Max value for Tier 1 and 2 hardware
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (FAILED(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_CbvUavSrvHeap))))
            throw std::runtime_error("Failed to create CBV SRV UAV descriptor heap");
    }

    // Create sampler descriptor heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 16; // Max value for Tier 1 hardware
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (FAILED(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_SamplerHeap))))
            throw std::runtime_error("Failed to create sampler descriptor heap");
    }

    // Command list
    {
        m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator[m_FrameIndex].Get(), nullptr, IID_PPV_ARGS(&m_CommandList));
        m_CommandList->Close();
    }

    // Fence
    {
        m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_FenceEvent) throw std::runtime_error("Failed to create fence event.");
    }
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
