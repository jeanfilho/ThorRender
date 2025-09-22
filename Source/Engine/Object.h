#pragma once

#include <memory>

#include "Engine/BaseTypes.h"
#include "Graphics/Mesh.h"
#include "Graphics/MeshPipeline.h"

class Object {
public:

    const float3& GetPosition() const { return m_Position; }
    const float3& GetRotation() const { return m_Rotation; }
    const float3& GetScale() const { return m_Scale; }
    const float4x4& GetWorldMatrix() const { return m_WorldMatrix; }
    const float2& GetUvOffset() const { return m_UvOffset; }
    const float2& GetUvScale() const { return m_UvScale; }
    const SharedPtr<Mesh>& GetMesh() const { return m_Mesh; }

    void SetPosition(const float3& position);
    void SetRotation(const float3& rotation);
    void SetScale(const float3& scale);
    void SetUvOffset(const float2& uvOffset);
    void SetUvScale(const float2& uvScale);
    void SetMesh(SharedPtr<Mesh> mesh) { m_Mesh = mesh; }

    void InitializeConstantBuffer(ID3D12Device* device);
    void Draw(ID3D12GraphicsCommandList* commandList) const;

private:
    void UpdateConstantBuffer();
    void UpdateWorldMatrix();

    // Object transformation data matching ObjectData cbuffer in shader
    struct ObjectData
    {
        float4x4 Model;
        float2 UvOffset;
        float2 UvScale;
    };
    static_assert(sizeof(ObjectData) % 16 == 0, "ObjectData must be 16-byte aligned");

    float3 m_Position = {0.0f, 0.0f, 0.0f};
    float3 m_Rotation = {0.0f, 0.0f, 0.0f}; // In radians
    float3 m_Scale = {1.0f, 1.0f, 1.0f};
    float4x4 m_WorldMatrix;
    
    float2 m_UvOffset = {0.0f, 0.0f};
    float2 m_UvScale = {1.0f, 1.0f};
    
    SharedPtr<Mesh> m_Mesh;
    SharedPtr<MeshPipeline> m_MeshPipeline;
    
    // D3D12 constant buffer
    ComPtr<ID3D12Resource> m_ObjectDataBuffer;
    bool m_ConstantBufferDirty = true;
};

