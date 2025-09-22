#include "Mesh.h"
#include "directx/d3dx12.h"

Mesh::Mesh(const MeshTemplate& meshTemplate, ID3D12Device* device)
{
    const auto& vertices = meshTemplate.GetVertices();
    const auto& indices = meshTemplate.GetIndices();
    m_IndexCount = static_cast<uint>(indices.size());

    // Create vertex buffer
    const UINT vertexBufferSize = vertices.size() * sizeof(MeshVertex);
    {
        // Create default heap buffer
        auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
        device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            nullptr,
            IID_PPV_ARGS(&m_VertexBuffer));

        // Copy data to the buffer
        {
            void* pData;
            CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
            HRESULT hr = m_VertexBuffer->Map(0, &readRange, &pData);
            if (FAILED(hr))
                throw std::runtime_error("Failed to map frame data buffer");
            memcpy(pData, &vertices, vertexBufferSize);
            m_VertexBuffer->Unmap(0, nullptr);
        }

        // Setup vertex buffer view
        m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
        m_VertexBufferView.StrideInBytes = sizeof(MeshVertex);
        m_VertexBufferView.SizeInBytes = vertexBufferSize;
    }

    // Create index buffer
    const UINT indexBufferSize = indices.size() * sizeof(uint32_t);
    {
        // Create default heap buffer
        auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
        device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_INDEX_BUFFER,
            nullptr,
            IID_PPV_ARGS(&m_IndexBuffer));

        // Copy data to the buffer
        {
            void* pData;
            CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
            HRESULT hr = m_IndexBuffer->Map(0, &readRange, &pData);
            if (FAILED(hr))
                throw std::runtime_error("Failed to map frame data buffer");
            memcpy(pData, &vertices, indexBufferSize);
            m_IndexBuffer->Unmap(0, nullptr);
        }

        // Setup index buffer view
        m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
        m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
        m_IndexBufferView.SizeInBytes = indexBufferSize;
    }
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList) const
{
    commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    commandList->IASetIndexBuffer(&m_IndexBufferView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}
