#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include "directx/d3dx12.h"

#include "Engine/BaseTypes.h"
#include "IO/Files.h"

class MeshPipeline
{
public:
    MeshPipeline() = default;
    ~MeshPipeline() = default;

    // Initialize the pipeline with shaders and render target format
    void Initialize(ID3D12Device* device, DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT);

    // Bind the pipeline to the command list
    void Bind(ID3D12GraphicsCommandList* commandList) const;

    // Get the root signature for setting constant buffers and resources
    ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

    // Check if pipeline is initialized
    bool IsInitialized() const { return m_PipelineState != nullptr; }

private:
    void CreateRootSignature(ID3D12Device* device);
    void CreatePipelineState(ID3D12Device* device, DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthStencilFormat);

private:
    ComPtr<ID3D12RootSignature> m_RootSignature;
    ComPtr<ID3D12PipelineState> m_PipelineState;
};