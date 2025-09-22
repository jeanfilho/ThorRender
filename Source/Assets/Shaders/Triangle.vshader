// Simple vertex shader for triangle
struct VSInput {
    float3 position : POSITION;
    float3 color : COLOR;
};
struct PSInput {
    float4 position : SV_POSITION;
    float3 color : COLOR;
};
PSInput main(VSInput input) {
    PSInput output;
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    return output;
}
