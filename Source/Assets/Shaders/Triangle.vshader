// Simple vertex shader for triangle
struct VSInput {
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct PSInput {
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

PSInput main(VSInput input) {
    PSInput output;
    output.Position = float4(input.Position, 1.0f);
    output.Color = input.Color;
    return output;
}
