#pragma once
#include <Windows.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <stdexcept>

#include <d3d12.h>
#include "directx/d3dx12.h"

#include "IO/Files.h"
#include "Engine/BaseTypes.h"


// DebugTriangle: draws a colored triangle using shaders loaded from Temp/Shaders/Pixel and Temp/Shaders/Vertex
class DebugTriangle
{
public:
    DebugTriangle(ComPtr<ID3D12Device> device)
    {
        // Load shaders
        Vector<uint8> vsBytecode, psBytecode;
        LOAD_VERTEX_SHADER("Triangle", vsBytecode);
        LOAD_PIXEL_SHADER("Triangle", psBytecode);

        // Create root signature
        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
        rootSigDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        ComPtr<ID3DBlob> sigBlob, errorBlob;
        if (FAILED(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errorBlob)))
            throw std::runtime_error("Failed to serialize root signature");
        if (FAILED(device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature))))
            throw std::runtime_error("Failed to create root signature");

        // Input layout
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        // Pipeline state
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
        psoDesc.pRootSignature = m_RootSignature.Get();
        psoDesc.VS = { vsBytecode.data(), vsBytecode.size() };
        psoDesc.PS = { psBytecode.data(), psBytecode.size() };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        if (FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState))))
            throw std::runtime_error("Failed to create pipeline state");

        // Vertex buffer
        struct Vertex { float position[3]; float color[3]; };
        Vertex triangleVertices[] = {
            { { 0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } }, // Top (red)
            { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } }, // Right (green)
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // Left (blue)
            { { -0.25f,  0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } }, // Top-Left
            { { 0.25f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f } }, // Top-Right
            { { 0.0f, -0.5f, 0.0f }, { 0.0f, 1.0f, 1.0f } }, // Right-Left
        };
        const uint vbSize = sizeof(triangleVertices);
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);
        if (FAILED(device->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &vbDesc,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr, IID_PPV_ARGS(&m_VertexBuffer))))
            throw std::runtime_error("Failed to create vertex buffer");
        
        void* pData;
        m_VertexBuffer->Map(0, nullptr, &pData);
        memcpy(pData, triangleVertices, vbSize);
        m_VertexBuffer->Unmap(0, nullptr);

        m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
        m_VertexBufferView.SizeInBytes = vbSize;
        m_VertexBufferView.StrideInBytes = sizeof(Vertex);

        unsigned short indices[] = { 
            0, 4, 3,
            3, 5, 2,
            4, 1, 5,
        };
        const uint ibSize = sizeof(indices);
        CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(ibSize);
        if(FAILED(device->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &ibDesc,
            D3D12_RESOURCE_STATE_INDEX_BUFFER, nullptr, IID_PPV_ARGS(&m_IndexBuffer))))
            throw std::runtime_error("Failed to create index buffer");

    m_IndexBuffer->Map(0, nullptr, &pData);
    memcpy(pData, indices, ibSize);
    m_IndexBuffer->Unmap(0, nullptr);

    m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
    m_IndexBufferView.SizeInBytes = ibSize;
    m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
    }

    // Draws the triangle into the current render target
    void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE rtv, uint width, uint height)
    {
        cmdList->SetGraphicsRootSignature(m_RootSignature.Get());
        cmdList->SetPipelineState(m_PipelineState.Get());
        CD3DX12_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
        CD3DX12_RECT scissorRect(0, 0, width, height);
        cmdList->RSSetViewports(1, &viewport);
        cmdList->RSSetScissorRects(1, &scissorRect);
        cmdList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmdList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
        cmdList->IASetIndexBuffer(&m_IndexBufferView);
        cmdList->DrawIndexedInstanced(m_IndexBufferView.SizeInBytes / sizeof(unsigned short), 1, 0, 0, 0);
    }

private:
    ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
    ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
    ComPtr<ID3D12Resource> m_VertexBuffer = nullptr;
    ComPtr<ID3D12Resource> m_IndexBuffer = nullptr;
    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
    D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
};