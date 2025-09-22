#pragma once

#include "Engine/Simulation.h"
#include "Engine/Camera.h"
#include "Engine/Object.h"
#include "Graphics/Mesh.h"

class MeshTestSimulation : public Simulation
{
protected:
    void PopulateCommandList() final;
    void PostInit() final;

private:
    static constexpr uint m_MeshCountX = 10;
    static constexpr uint m_MeshCountY = 10;
    static constexpr uint m_MeshCountZ = 10;
    static constexpr uint m_TotalMeshCount = m_MeshCountX * m_MeshCountY * m_MeshCountZ;

    static constexpr float m_MeshSpacing = 2.5f;

    const float3 m_LightDirection = Normalize(float3{ -1.0f, -1.0f, -1.0f });

    UniquePtr<Object> m_Object;
    SharedPtr<MeshPipeline> m_MeshPipeline;
    UniquePtr<Camera> m_Camera;


    struct FrameData
    {
        XMMATRIX ViewProjection;
        float3 CameraPosition;
        float Padding;
        float3 LightDirection;
        float Padding2;
    };
    static_assert(sizeof(FrameData) % 16 == 0, "FrameData must be 16-byte aligned");

    ComPtr<ID3D12Resource> m_FrameData;
    
};