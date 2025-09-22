#pragma once
#include <vector>
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>

#include "Engine/BaseTypes.h"

// Vertex: position + normal + uv
struct MeshVertex
{
    float3 position;
    float3 normal;
    float2 uv;
};

// Material: BRDF parameters + texture resource handles
struct Material
{
    float3 Albedo;
    float Metallic;
    float Roughness;

    // Texture resource handles
    ComPtr<ID3D12Resource> AlbedoRoughnessMap;
    ComPtr<ID3D12Resource> NormalMap;
    ComPtr<ID3D12Resource> MetallicMap;
    ComPtr<ID3D12Resource> RoughnessMap;

    float2 UvOffset = { 0.0f, 0.0f };
    float2 UvScale = { 1.0f, 1.0f };
};

class Mesh
{
public:
    Mesh() = default;

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
