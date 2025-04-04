#include "ShaderInclude.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix viewMatrix;
    matrix projMatrix;
};



// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel_Sky main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel_Sky output;

    matrix view = viewMatrix;
    view._14 = 0;
    view._24 = 0;
    view._34 = 0;
    
    matrix updateView = mul(projMatrix,view);
    output.position = mul(updateView, float4(input.Position, 1.0f));
    
    output.position.z = output.position.w;
    output.sampleDir = input.Position;
    
    return output;
}