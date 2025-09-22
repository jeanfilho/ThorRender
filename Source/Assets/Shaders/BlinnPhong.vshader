struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : WORLDPOS;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

cbuffer ObjectData : register(b0)
{
    float4x4 model;
    float4x4 viewProj;
    float2 uvOffset;
    float2 uvScale;
};

PSInput main(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(model, float4(input.position, 1.0f));
    output.position = mul(viewProj, worldPos);
    output.worldPos = worldPos.xyz;
    output.normal = mul(model, float4(input.normal, 1.0f)).xyz;
    output.uv = input.uv * uvScale + uvOffset;
    return output;
}