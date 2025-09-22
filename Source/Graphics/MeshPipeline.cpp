#include "MeshPipeline.h"
#include <stdexcept>

void MeshPipeline::Initialize(ID3D12Device* device, DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthStencilFormat)
{
    if (!device)
    {
        throw std::invalid_argument("Device cannot be null");
    }

    CreateRootSignature(device);
    CreatePipelineState(device, renderTargetFormat, depthStencilFormat);
}

void MeshPipeline::Bind(ID3D12GraphicsCommandList* commandList) const
{
    if (!IsInitialized())
    {
        throw std::runtime_error("Pipeline is not initialized");
    }

    commandList->SetGraphicsRootSignature(m_RootSignature.Get());
    commandList->SetPipelineState(m_PipelineState.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void MeshPipeline::CreateRootSignature(ID3D12Device* device)
{
    // Root parameters for constant buffers and texture resources
    CD3DX12_ROOT_PARAMETER1 rootParameters[5] = {};

    // Frame data constant buffer (b0) - used by both vertex and pixel shaders
    rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_ALL);

    // Object data constant buffer (b1) - used by vertex shader
    rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

    // Material data constant buffer (b1) - used by pixel shader
    rootParameters[2].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_PIXEL);

    // Texture descriptor table for material textures (t0-t3)
    CD3DX12_DESCRIPTOR_RANGE1 textureRange;
    textureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
    rootParameters[3].InitAsDescriptorTable(1, &textureRange, D3D12_SHADER_VISIBILITY_PIXEL);

    // Sampler descriptor table (s0)
    CD3DX12_DESCRIPTOR_RANGE1 samplerRange;
    samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    rootParameters[4].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);

    // Create versioned root signature
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error);

    if (FAILED(hr))
    {
        if (error)
        {
            String errorMsg(static_cast<const char*>(error->GetBufferPointer()), error->GetBufferSize());
            throw std::runtime_error("Failed to serialize root signature: " + errorMsg);
        }
        throw std::runtime_error("Failed to serialize root signature");
    }

    hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create root signature");
    }
}

void MeshPipeline::CreatePipelineState(ID3D12Device* device, DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthStencilFormat)
{
    // Input layout description matching MeshVertex structure
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Pipeline state description
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = m_RootSignature.Get();

    // Shader bytecode
    Vector<uint8_t> vertexShaderBytecode, pixelShaderBytecode;
    LOAD_VERTEX_SHADER("BlinnPhong", vertexShaderBytecode);
    LOAD_PIXEL_SHADER("BlinnPhong", pixelShaderBytecode);
    psoDesc.VS = { vertexShaderBytecode.data(), vertexShaderBytecode.size() };
    psoDesc.PS = { pixelShaderBytecode.data(), pixelShaderBytecode.size() };

    // Blend state (no blending)
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // Rasterizer state
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    psoDesc.RasterizerState.FrontCounterClockwise = FALSE;

    // Depth stencil state
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

    // Input layout
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };

    // Primitive topology
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // Render target and depth stencil formats
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = renderTargetFormat;
    psoDesc.DSVFormat = depthStencilFormat;

    // Multisampling
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.SampleMask = UINT_MAX;

    // Create pipeline state
    HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create graphics pipeline state");
    }
}