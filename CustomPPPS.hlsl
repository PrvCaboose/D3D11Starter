#define NUM_LIGHTS 5

cbuffer ExternalData : register(b0)
{
    matrix worldMatrix;
    // focusPoint;
    bool enabled;
}
struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D Pixels : register(t0);
SamplerState BasicSampler : register(s0); // "s" registers for samplers

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    float3 worldPosition = mul(worldMatrix, float4(input.position.xyz, 1)).xyz;
    
    if (enabled)
        return Pixels.Sample(BasicSampler, input.uv);
    
    float redOffset = 0.022;
    float blueOffset = -0.011;
    float greenOffset = 0.009;
    
    float2 direction = input.uv - worldPosition.xy;
    
    float4 color;
    
    color.r = Pixels.Sample(BasicSampler, input.uv + (direction * float2(redOffset, 0))).r;
    color.g = Pixels.Sample(BasicSampler, input.uv + (direction * float2(greenOffset, 0))).g;
    color.ba = Pixels.Sample(BasicSampler, input.uv + (direction * float2(blueOffset, 0))).ba;
    
    return color;
}