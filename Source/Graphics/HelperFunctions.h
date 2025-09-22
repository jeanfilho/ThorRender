#pragma once

#include <DirectXMath.h>
#include "Engine/BaseTypes.h"
#include "Engine/Camera.h"
#include "Graphics/Mesh.h"


inline MeshTemplate CreateCubeMesh(float size = 1.0f)
{
    MeshTemplate cube;
    float halfSize = size * 0.5f;
    
    // Front face vertices
    cube.AddVertex({ float3{-halfSize, -halfSize,  halfSize}, float3{ 0.0f,  0.0f,  1.0f}, float2{0.0f, 1.0f} });
    cube.AddVertex({ float3{ halfSize, -halfSize,  halfSize}, float3{ 0.0f,  0.0f,  1.0f}, float2{1.0f, 1.0f} });
    cube.AddVertex({ float3{ halfSize,  halfSize,  halfSize}, float3{ 0.0f,  0.0f,  1.0f}, float2{1.0f, 0.0f} });
    cube.AddVertex({ float3{-halfSize,  halfSize,  halfSize}, float3{ 0.0f,  0.0f,  1.0f}, float2{0.0f, 0.0f} });

    // Back face vertices
    cube.AddVertex({ float3{ halfSize, -halfSize, -halfSize}, float3{ 0.0f,  0.0f, -1.0f}, float2{0.0f, 1.0f} });
    cube.AddVertex({ float3{-halfSize, -halfSize, -halfSize}, float3{ 0.0f,  0.0f, -1.0f}, float2{1.0f, 1.0f} });
    cube.AddVertex({ float3{-halfSize,  halfSize, -halfSize}, float3{ 0.0f,  0.0f, -1.0f}, float2{1.0f, 0.0f} });
    cube.AddVertex({ float3{ halfSize,  halfSize, -halfSize}, float3{ 0.0f,  0.0f, -1.0f}, float2{0.0f, 0.0f} });

    // Top face vertices
    cube.AddVertex({ float3{-halfSize,  halfSize, -halfSize}, float3{ 0.0f,  1.0f,  0.0f}, float2{0.0f, 1.0f} });
    cube.AddVertex({ float3{-halfSize,  halfSize,  halfSize}, float3{ 0.0f,  1.0f,  0.0f}, float2{0.0f, 0.0f} });
    cube.AddVertex({ float3{ halfSize,  halfSize,  halfSize}, float3{ 0.0f,  1.0f,  0.0f}, float2{1.0f, 0.0f} });
    cube.AddVertex({ float3{ halfSize,  halfSize, -halfSize}, float3{ 0.0f,  1.0f,  0.0f}, float2{1.0f, 1.0f} });

    // Bottom face vertices
    cube.AddVertex({ float3{-halfSize, -halfSize, -halfSize}, float3{ 0.0f, -1.0f,  0.0f}, float2{0.0f, 0.0f} });
    cube.AddVertex({ float3{ halfSize, -halfSize, -halfSize}, float3{ 0.0f, -1.0f,  0.0f}, float2{1.0f, 0.0f} });
    cube.AddVertex({ float3{ halfSize, -halfSize,  halfSize}, float3{ 0.0f, -1.0f,  0.0f}, float2{1.0f, 1.0f} });
    cube.AddVertex({ float3{-halfSize, -halfSize,  halfSize}, float3{ 0.0f, -1.0f,  0.0f}, float2{0.0f, 1.0f} });

    // Right face vertices
    cube.AddVertex({ float3{ halfSize, -halfSize, -halfSize}, float3{ 1.0f,  0.0f,  0.0f}, float2{0.0f, 1.0f} });
    cube.AddVertex({ float3{ halfSize,  halfSize, -halfSize}, float3{ 1.0f,  0.0f,  0.0f}, float2{0.0f, 0.0f} });
    cube.AddVertex({ float3{ halfSize,  halfSize,  halfSize}, float3{ 1.0f,  0.0f,  0.0f}, float2{1.0f, 0.0f} });
    cube.AddVertex({ float3{ halfSize, -halfSize,  halfSize}, float3{ 1.0f,  0.0f,  0.0f}, float2{1.0f, 1.0f} });

    // Left face vertices
    cube.AddVertex({ float3{-halfSize, -halfSize,  halfSize}, float3{-1.0f,  0.0f,  0.0f}, float2{0.0f, 1.0f} });
    cube.AddVertex({ float3{-halfSize,  halfSize,  halfSize}, float3{-1.0f,  0.0f,  0.0f}, float2{0.0f, 0.0f} });
    cube.AddVertex({ float3{-halfSize,  halfSize, -halfSize}, float3{-1.0f,  0.0f,  0.0f}, float2{1.0f, 0.0f} });
    cube.AddVertex({ float3{-halfSize, -halfSize, -halfSize}, float3{-1.0f,  0.0f,  0.0f}, float2{1.0f, 1.0f} });

    // Add triangles for each face (2 triangles per face, 6 faces)
    // Front face
    cube.AddTriangle(0, 1, 2);
    cube.AddTriangle(2, 3, 0);

    // Back face
    cube.AddTriangle(4, 5, 6);
    cube.AddTriangle(6, 7, 4);

    // Top face
    cube.AddTriangle(8, 9, 10);
    cube.AddTriangle(10, 11, 8);

    // Bottom face
    cube.AddTriangle(12, 13, 14);
    cube.AddTriangle(14, 15, 12);

    // Right face
    cube.AddTriangle(16, 17, 18);
    cube.AddTriangle(18, 19, 16);

    // Left face
    cube.AddTriangle(20, 21, 22);
    cube.AddTriangle(22, 23, 20);

    return cube;
}

MeshTemplate CreateSphereMesh(float radius = 1.0f, uint32 latitudeSegments = 10, uint32 longitudeSegments = 10)
{
    MeshTemplate mesh;
    for (uint32 lat = 1; lat <= latitudeSegments; ++lat)
    {
        float theta = PI * lat / latitudeSegments;
        float sinTheta = Sin(theta);
        float cosTheta = Cos(theta);
        for (uint32 lon = 0; lon <= longitudeSegments; ++lon)
        {
            float phi = lon * 2 * PI / longitudeSegments;
            float sinPhi = Sin(phi);
            float cosPhi = Cos(phi);

            MeshVertex vertex;
            vertex.Position = { radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi };
            vertex.Normal = { sinTheta * cosPhi, cosTheta, sinTheta * sinPhi };
            vertex.Uv = { 1.0f - (float)lon / longitudeSegments, 1.0f - (float)lat / latitudeSegments };
            mesh.AddVertex(vertex);
        }
    }
    for (uint32 lat = 0; lat < latitudeSegments; ++lat)
    {
        for (uint32 lon = 0; lon < longitudeSegments; ++lon)
        {
            uint32 first = (lat * (longitudeSegments + 1)) + lon;
            uint32 second = first + longitudeSegments + 1;
            mesh.AddTriangle(first, second, first + 1);
            mesh.AddTriangle(second, second + 1, first + 1);
        }
    }
    return mesh;
}

inline XMMATRIX GetViewMatrix(const float3& position, const float3& target, const float3& up)
{
    return XMMatrixLookAtLH(
        XMLoadFloat3(&position),
        XMLoadFloat3(&target),
        XMLoadFloat3(&up)
    );
}

inline XMMATRIX GetViewMatrix(const Camera& camera)
{
    return GetViewMatrix(camera.GetPosition(), camera.GetTarget(), camera.GetUp());
}

inline XMMATRIX GetProjectionMatrix(float fovY, float aspect, float nearZ, float farZ)
{
    return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
}

inline XMMATRIX GetProjectionMatrix(const Camera& camera)
{
    return GetProjectionMatrix(camera.GetFovY(), camera.GetAspect(), camera.GetNearZ(), camera.GetFarZ());
}
