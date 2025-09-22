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

cbuffer ObjectData : register(b0)
{
    float4x4 Model;
    float4x4 ViewProj;
    float2 UvOffset;
    float2 UvScale;
};

PSInput main(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(Model, float4(input.Position, 1.0f));
    output.Position = mul(ViewProj, worldPos);
    output.WorldPos = worldPos.xyz;
    output.Normal = mul(Model, float4(input.Normal, 1.0f)).xyz;
    output.UV = input.UV * UvScale + UvOffset;
    return output;
}