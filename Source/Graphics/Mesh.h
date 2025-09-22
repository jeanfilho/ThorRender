#pragma once
#include <vector>
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>

#include "Engine/BaseTypes.h"

// Vertex: position + normal + uv
struct MeshVertex
{
    float3 Position;
    float3 Normal;
    float2 Uv;
};

// Material: BRDF parameters + texture resource handles
class Material
{
public:
    float3 Albedo = { 0,0,0 };
    float Metallic = 0;
    float Roughness = 0;

    // Texture resource handles
    ComPtr<ID3D12Resource> AlbedoRoughnessMap = nullptr;
    ComPtr<ID3D12Resource> NormalMap = nullptr;
    ComPtr<ID3D12Resource> MetallicMap = nullptr;
    ComPtr<ID3D12Resource> RoughnessMap = nullptr;

    float2 UvOffset = { 0.0f, 0.0f };
    float2 UvScale = { 1.0f, 1.0f };
};

class MeshTemplate
{
public:
    MeshTemplate() = default;

    void AddVertex(const MeshVertex& v)
    {
        m_Vertices.push_back(v);
    }

    void AddTriangle(uint32_t i0, uint32_t i1, uint32_t i2)
    {
        m_Indices.push_back(i0);
        m_Indices.push_back(i1);
        m_Indices.push_back(i2);
    }

    void SetMaterial(const Material& material)
    {
        m_Material = material;
    }

    const Vector<MeshVertex>& GetVertices() const { return m_Vertices; }
    const Vector<uint32>& GetIndices() const { return m_Indices; }
    const Material& GetMaterial() const { return m_Material; }
    size_t GetVertexCount() const { return m_Vertices.size(); }
    size_t GetIndexCount() const { return m_Indices.size(); }

private:
    Vector<MeshVertex> m_Vertices;
    Vector<uint32> m_Indices;
    Material m_Material;
};

class Mesh
{
public:
    Mesh(const MeshTemplate& meshTemplate, ID3D12Device* device);

    void Draw(ID3D12GraphicsCommandList* commandList) const;

    const ComPtr<ID3D12Resource>& GetVertexBuffer() const { return m_VertexBuffer; }
    const ComPtr<ID3D12Resource>& GetIndexBuffer() const { return m_IndexBuffer; }
    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return m_VertexBufferView; }
    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return m_IndexBufferView; }

private:
    ComPtr<ID3D12Resource> m_VertexBuffer = nullptr;
    ComPtr<ID3D12Resource> m_IndexBuffer = nullptr;
    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
    D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
    uint m_IndexCount = 0;
};
