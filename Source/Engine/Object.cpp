#include "Object.h"
#include "../Graphics/Mesh.h"
#include "directx/d3dx12.h"
#include <stdexcept>
#include "Simulation.h"

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
    m_ConstantBufferDirty = 2;
}

void Object::SetUvScale(const float2& uvScale)
{
    m_UvScale = uvScale;
    m_ConstantBufferDirty = 2;
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
    m_ObjectDataBufferSize = AlignUp(sizeof(ObjectData), 256);
    resourceDesc.Width = m_ObjectDataBufferSize * Simulation::s_FrameCount;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


    if (FAILED(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_ObjectDataBuffer))))
    {
        throw std::runtime_error("Failed to create object constant buffer");
    }


    // Map the buffer and keep it mapped for the lifetime of the resource
    if (FAILED(m_ObjectDataBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedObjectData))))
    {
        throw std::runtime_error("Failed to map object data buffer");
    }

    UpdateWorldMatrix();

    m_ConstantBufferDirty = 2;
}

void Object::Release()
{
    if (m_ObjectDataBuffer)
    {
        m_ObjectDataBuffer->Unmap(0, nullptr);
        m_MappedObjectData = nullptr;
        m_ObjectDataBuffer.Reset();
    }
    m_Mesh.reset();
}

void Object::UpdateConstantBuffer()
{
    if (!m_ObjectDataBuffer || m_ConstantBufferDirty == 0)
    {
        return;
    }

    // Prepare object data
    ObjectData objectData{};
    objectData.Model = m_WorldMatrix;
    objectData.UvOffset = m_UvOffset;
    objectData.UvScale = m_UvScale;
    objectData.Normal = m_NormalMatrix;

    // Copy data to the buffer
    static constexpr size_t dataSize = sizeof(ObjectData);
    size_t offset = m_CurrentFrameIndex * m_ObjectDataBufferSize;
    memcpy(m_MappedObjectData + offset, &objectData, dataSize);

    m_ConstantBufferDirty--;
}

void Object::UpdateWorldMatrix()
{
    // Create transformation matrices
    XMMATRIX translation = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

    // Combine transformations in SRT order
    XMMATRIX worldMatrix = scale * rotation * translation;

    // Remove the transpose here - let the shader handle it
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(worldMatrix));

    // Compute normal matrix: inverse transpose of the 3x3 part (no extra transpose needed)
    XMMATRIX upperLeft3x3 = XMMatrixSet(
        XMVectorGetX(worldMatrix.r[0]), XMVectorGetY(worldMatrix.r[0]), XMVectorGetZ(worldMatrix.r[0]), 0,
        XMVectorGetX(worldMatrix.r[1]), XMVectorGetY(worldMatrix.r[1]), XMVectorGetZ(worldMatrix.r[1]), 0,
        XMVectorGetX(worldMatrix.r[2]), XMVectorGetY(worldMatrix.r[2]), XMVectorGetZ(worldMatrix.r[2]), 0,
        0, 0, 0, 1
    );
    
    XMMATRIX normalMatrix = XMMatrixInverse(nullptr, upperLeft3x3);
    XMStoreFloat4x4(&m_NormalMatrix, normalMatrix);

    m_ConstantBufferDirty = 2;
}

void Object::Draw(ID3D12GraphicsCommandList* commandList)
{
    if (m_Mesh && m_ObjectDataBuffer)
    {
        // Advance to the next frame index
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % Simulation::s_FrameCount;

        // Update constant buffer if needed
        UpdateConstantBuffer();

        D3D12_GPU_VIRTUAL_ADDRESS bufferAddress = m_ObjectDataBuffer->GetGPUVirtualAddress() + m_CurrentFrameIndex * m_ObjectDataBufferSize;
        commandList->SetGraphicsRootConstantBufferView(1, bufferAddress);

        // Draw the mesh
        m_Mesh->Draw(commandList);
    }
}