#pragma once
#include "Engine/BaseTypes.h"
#include <d3d12.h>

struct MaterialData
{
    float3 Albedo;
    float Metallic;
    float Roughness;
    float Padding[3];
};

struct Material
{
    float3 Albedo = { 1,0,0 };
    float Metallic = .5;
    float Roughness = .5;

    // Texture resource handles
    ComPtr<ID3D12Resource> AlbedoMap = nullptr;
    ComPtr<ID3D12Resource> NormalMap = nullptr;
    ComPtr<ID3D12Resource> MetallicMap = nullptr;
    ComPtr<ID3D12Resource> RoughnessMap = nullptr;
    float2 UvOffset = { 0.0f, 0.0f };
    float2 UvScale = { 1.0f, 1.0f };
};