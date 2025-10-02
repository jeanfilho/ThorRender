struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 WorldPos : WORLDPOS;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
};

cbuffer FrameData : register(b0)
{
    float4x4 ViewProj;
    float4x4 InvView;
    float3 LightDirection;
    float __Padding0;
    float3 LightColor;
    float __Padding1;
    float3 ViewPosition;
    float __Padding2;
};

cbuffer ObjectData : register(b1)
{
    float4x4 Model;
    float4x4 Normal;
    float2 UvOffset;
    float2 UvScale;
};

PSInput main(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.Position, 1.0f), Model);
    output.Position = mul(worldPos, ViewProj);
    output.WorldPos = worldPos.xyz;
    output.Normal = mul(float4(input.Normal, 0.0f), Normal).xyz;
    output.UV = input.UV * UvScale + UvOffset;
    
    return output;
}