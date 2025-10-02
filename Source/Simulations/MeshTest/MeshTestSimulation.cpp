#include "MeshTestSimulation.h"

#include "Graphics/HelperFunctions.h"

void MeshTestSimulation::PopulateCommandList()
{
    // Calculate frame data
    FrameData frameData{};
    frameData.LightDirection = m_LightDirection;
    frameData.ViewPosition = m_Camera->GetPosition();
    frameData.ViewProj = XMMatrixTranspose(m_Camera->GetViewProjectionMatrix());
    frameData.LightColor = m_LightColor;
    frameData.InvView = XMMatrixTranspose(XMMatrixInverse(nullptr, m_Camera->GetViewMatrix()));

    // Copy frame data to the buffer
    {
        memcpy(m_MappedFrameData + m_FrameDataSize * m_FrameIndex, &frameData, sizeof(FrameData));
    }

    // Reset command allocator and command list
    m_CommandAllocator[m_FrameIndex]->Reset();
    m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), nullptr);

    // Transition final frame to render target
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets[m_FrameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    m_CommandList->ResourceBarrier(1, &barrier);

    // Clear the render target & depth Buffers
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
        FLOAT clearColor[] = { 0.2f, 0.4f, 0.6f, 1.0f };
        m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DsvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_DsvDescriptorSize);
        m_CommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    // Set render target & depth buffer
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DsvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_DsvDescriptorSize);
        m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    }

    // Set viewport and scissor rect
    {
        D3D12_VIEWPORT viewport = {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(m_RenderTargets[m_FrameIndex]->GetDesc().Width);
        viewport.Height = static_cast<float>(m_RenderTargets[m_FrameIndex]->GetDesc().Height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        m_CommandList->RSSetViewports(1, &viewport);

        D3D12_RECT scissorRect = {};
        scissorRect.left = 0;
        scissorRect.top = 0;
        scissorRect.right = static_cast<LONG>(viewport.Width);
        scissorRect.bottom = static_cast<LONG>(viewport.Height);
        m_CommandList->RSSetScissorRects(1, &scissorRect);
    }

    // Draw mesh
    {
        m_MeshPipeline->Bind(m_CommandList.Get());

        // Set frame data constant buffer (b0)
        m_CommandList->SetGraphicsRootConstantBufferView(0, m_FrameData->GetGPUVirtualAddress() + m_FrameDataSize * m_FrameIndex);

        m_Object->Draw(m_CommandList.Get());
    }

    // Transition final frame to present
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets[m_FrameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    m_CommandList->ResourceBarrier(1, &barrier);

    m_CommandList->Close();
}

void MeshTestSimulation::PostResize()
{
    if (m_Camera)
    {
        D3D12_RESOURCE_DESC renderDesc = m_RenderTargets[0]->GetDesc();
        float aspectRatio = static_cast<float>(renderDesc.Width) / static_cast<float>(renderDesc.Height);
        m_Camera->SetPerspective(m_FovHorizontal, aspectRatio, 0.1f, 100.0f);
    }
}

void MeshTestSimulation::PostInit()
{
    // Create camera
    {
        m_Camera = MakeUnique<Camera>();
        m_Camera->SetPosition(float3{ 0.0f, 0.0f, -5.0f});
        D3D12_RESOURCE_DESC renderDesc = m_RenderTargets[0]->GetDesc();
        float aspectRatio = static_cast<float>(renderDesc.Width) / static_cast<float>(renderDesc.Height);
        m_Camera->SetPerspective(m_FovHorizontal, aspectRatio, 0.1f, 100.0f);
    }

    // Create sphere
    {
        D3D12_RESOURCE_DESC depthDesc, renderDesc;
        renderDesc = m_RenderTargets[0]->GetDesc();
        depthDesc = m_DepthBuffers[0]->GetDesc();

        // Create mesh, object and pipeline
        m_Object = MakeUnique<Object>();
        m_Object->Initialize(m_Device.Get());

        m_MeshPipeline = MakeShared<MeshPipeline>();
        m_MeshPipeline->Initialize(m_Device.Get(), renderDesc.Format, depthDesc.Format);

        MeshTemplate meshTemplate = CreateSphereMesh(1);
        SharedPtr<Mesh> mesh = MakeShared<Mesh>(meshTemplate, m_Device.Get());
        m_Object->SetMesh(mesh);
    }

    // Create frame data buffer
    {
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        m_FrameDataSize = AlignUp(sizeof(FrameData), 256);
        resourceDesc.Width = m_FrameDataSize * Simulation::s_FrameCount;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        HRESULT hr = m_Device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_FrameData));

        if (FAILED(hr))
            throw std::runtime_error("Failed to create frame data buffer");

        // Map the buffer
        if (FAILED(m_FrameData->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedFrameData))))
            throw std::runtime_error("Failed to map frame data buffer");
    }
}

void MeshTestSimulation::PreRelease()
{
    if (m_FrameData)
    {
        m_FrameData->Unmap(0, nullptr);
        m_MappedFrameData = nullptr;
        m_FrameData.Reset();
    }
    if (m_Object)
    {
        m_Object->Release();
        m_Object.reset();
    }
    if (m_MeshPipeline)
    {
        m_MeshPipeline.reset();
    }
    if (m_Camera)
    {
        m_Camera.reset();
    }
}
