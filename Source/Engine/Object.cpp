#include "Object.h"
#include "../Graphics/Mesh.h"
#include "directx/d3dx12.h"
#include <stdexcept>

void Object::SetPosition(const float3& position)
{
    m_Position = position;
    UpdateWorldMatrix();
}

void Object::SetRotation(const float3& rotation)
{
    m_Rotation = rotation;
    UpdateWorldMatrix();
}

void Object::SetScale(const float3& scale)
{
    m_Scale = scale;
    UpdateWorldMatrix();
}

void Object::SetUvOffset(const float2& uvOffset)
{
    m_UvOffset = uvOffset;
    m_ConstantBufferDirty = true;
}

void Object::SetUvScale(const float2& uvScale)
{
    m_UvScale = uvScale;
    m_ConstantBufferDirty = true;
}

void Object::Initialize(ID3D12Device* device)
{
    // Create constant buffer for ObjectData
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = sizeof(ObjectData);
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_ObjectDataBuffer));

    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create object constant buffer");
    }

    m_ConstantBufferDirty = true;
}

void Object::UpdateConstantBuffer()
{
    if (!m_ObjectDataBuffer || !m_ConstantBufferDirty)
    {
        return;
    }

    // Prepare object data
    ObjectData objectData;
    objectData.Model = m_WorldMatrix;
    objectData.UvOffset = m_UvOffset;
    objectData.UvScale = m_UvScale;

    // Map and copy data to the buffer
    void* pData;
    CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
    HRESULT hr = m_ObjectDataBuffer->Map(0, &readRange, &pData);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to map object data buffer");
    }

    memcpy(pData, &objectData, sizeof(ObjectData));
    m_ObjectDataBuffer->Unmap(0, nullptr);

    m_ConstantBufferDirty = false;
}

void Object::UpdateWorldMatrix()
{
    // Create transformation matrices
    XMMATRIX translation = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

    // Combine transformations in SRT order (Scale * Rotation * Translation)
    XMMATRIX worldMatrix = scale * rotation * translation;

    // Store the result (transpose for HLSL column-major format)
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(worldMatrix));

    m_ConstantBufferDirty = true;
}

void Object::Draw(ID3D12GraphicsCommandList* commandList)
{
    if (m_Mesh && m_ObjectDataBuffer)
    {
        // Update constant buffer if needed
        UpdateConstantBuffer();

        // Set the object constant buffer (ObjectData at register b1)
        commandList->SetGraphicsRootConstantBufferView(1, m_ObjectDataBuffer->GetGPUVirtualAddress());

        // Draw the mesh
        m_Mesh->Draw(commandList);
    }
}