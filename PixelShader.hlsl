#include "ShaderInclude.hlsli"
#define NUM_LIGHTS 5

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    
    float2 uvScale;
    float2 uvOffset;
    
    float3 cameraPosition;
    float roughness;
    
    float3 ambient;
    float padding;
    
    Light lights[NUM_LIGHTS];
}

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
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
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    input.Normal = normalize(input.Normal);
    input.UV = input.UV * uvScale + uvOffset;
    float3 totalLight = float3(0, 0, 0) + ambient;
    for (int i = 0; i < NUM_LIGHTS;i++)
    {
        // Normalize the light direction
        Light currentLight = lights[i];
        currentLight.Direction = normalize(currentLight.Direction);
        // Get surface color
        float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.UV);
        // switch case for each type of light
        switch (currentLight.Type)
        {
            case LIGHT_DIRECTIONAL_TYPE:
                // Increment total light for each light
                totalLight += DirLight(currentLight, input.worldPosition, input.Normal, float3(surfaceColor.rgb), roughness, cameraPosition);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(currentLight, input.worldPosition, input.Normal, float3(surfaceColor.rgb), roughness, cameraPosition);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(currentLight, input.worldPosition, input.Normal, float3(surfaceColor.rgb), roughness, cameraPosition);
                break;
        }
    }
    
    return float4(totalLight.rgb, 1);
}