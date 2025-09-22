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
    float3 albedo;              // Base color
    float metallic;             // 0 = dielectric, 1 = metal
    float roughness;            // 0 = smooth, 1 = rough

    // Texture resource handles
    ComPtr<ID3D12Resource> albedoRoughnessMap;
    ComPtr<ID3D12Resource> normalMap;
    ComPtr<ID3D12Resource> metallicMap;
    ComPtr<ID3D12Resource> roughnessMap;

    // UV transform for shifting/scaling/rotating texture coordinates
    float2 uvOffset = { 0.0f, 0.0f };
    float2 uvScale = { 1.0f, 1.0f };
};

class Mesh
{
public:
    Mesh() = default;

    void AddVertex(const MeshVertex& v)
    {
        m_vertices.push_back(v);
    }

    void AddTriangle(uint32_t i0, uint32_t i1, uint32_t i2)
    {
        m_indices.push_back(i0);
        m_indices.push_back(i1);
        m_indices.push_back(i2);
    }

    void SetMaterial(const Material& material)
    {
        m_material = material;
    }

    const std::vector<MeshVertex>& GetVertices() const { return m_vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_indices; }
    const Material& GetMaterial() const { return m_material; }
    size_t GetVertexCount() const { return m_vertices.size(); }
    size_t GetIndexCount() const { return m_indices.size(); }

private:
    std::vector<MeshVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    Material m_material;
};
