#include "DebugTriangleSimulation.h"

void DebugTriangleSimulation::PopulateCommandList()
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

    DXGI_SWAP_CHAIN_DESC1 desc;
    m_SwapChain->GetDesc1(&desc);
    uint width = desc.Width;
    uint height = desc.Height;

    m_Triangle->Draw(m_CommandList.Get(), rtvHandle, width, height);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets[m_FrameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    m_CommandList->ResourceBarrier(1, &barrier);

    m_CommandList->Close();
}

void DebugTriangleSimulation::PostInit()
{
    m_Triangle = MakeUnique<DebugTriangle>(m_Device);
}
