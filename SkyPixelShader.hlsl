#include "ShaderInclude.hlsli"

TextureCube cube : register(t0);
SamplerState sample : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
    return cube.Sample(sample, input.sampleDir);
}