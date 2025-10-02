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
    void PostResize() final;
    void PreRelease() final;

private:
    static constexpr uint m_MeshCountX = 10;
    static constexpr uint m_MeshCountY = 10;
    static constexpr uint m_MeshCountZ = 10;
    static constexpr uint m_TotalMeshCount = m_MeshCountX * m_MeshCountY * m_MeshCountZ;

    static constexpr float m_MeshSpacing = 2.5f;

    const float3 m_LightDirection = Normalize(-float3{ -1, -1, 1 });
    const float3 m_LightColor = float3{ 1, 1, 1};
    const uint m_FovHorizontal = 90;

    UniquePtr<Object> m_Object = nullptr;
    SharedPtr<MeshPipeline> m_MeshPipeline = nullptr;
    UniquePtr<Camera> m_Camera = nullptr;

    struct FrameData
    {
        XMMATRIX ViewProj;
        XMMATRIX InvView;
        float3 LightDirection;
        float __Padding0;
        float3 LightColor;
        float __Padding1;
        float3 ViewPosition;
        float __Padding2;
    };

    ComPtr<ID3D12Resource> m_FrameData = nullptr;
    uint8* m_MappedFrameData = nullptr;
    size_t m_FrameDataSize = 0;
};