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

Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
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
    
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.UV).rgb * 2 - 1;
    float3 tangent = normalize(input.Tangent);
    input.Normal = normalize(input.Normal);
    input.UV = input.UV * uvScale + uvOffset;
    
    tangent = normalize(tangent - input.Normal * dot(tangent, input.Normal));
    float3 bi_tangent = cross(tangent, input.Normal);
    float3x3 tbn = float3x3(tangent, bi_tangent, input.Normal);
    input.Normal = normalize(mul(unpackedNormal, tbn));
    
    float3 totalLight = float3(0, 0, 0) + (colorTint.rgb / 3);
    
    // roughness map
    float roughness = RoughnessMap.Sample(BasicSampler, input.UV).r;
    
    // metalness map
    float metalness = MetalnessMap.Sample(BasicSampler, input.UV).r;
    
    // Get surface color
    float4 albedoColor = pow(Albedo.Sample(BasicSampler, input.UV), 2.2f);
    
    // specular
    float4 specularColor = lerp(F0_NON_METAL, albedoColor, metalness);
    
    for (int i = 0; i < NUM_LIGHTS;i++)
    {
        // Normalize the light direction
        Light currentLight = lights[i];
        currentLight.Direction = normalize(currentLight.Direction);
       
        // switch case for each type of light
        switch (currentLight.Type)
        {
            case LIGHT_DIRECTIONAL_TYPE:
                // Increment total light for each light
                totalLight += DirLight(currentLight, input.worldPosition, input.Normal, float3(albedoColor.rgb), roughness, cameraPosition, specularColor.rgb, metalness);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(currentLight, input.worldPosition, input.Normal, float3(albedoColor.rgb), roughness, cameraPosition, specularColor.rgb, metalness);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(currentLight, input.worldPosition, input.Normal, float3(albedoColor.rgb), roughness, cameraPosition, specularColor.rgb, metalness);
                break;
        }
    }
    
    return float4(pow(totalLight.rgb, 1.0f / 2.2f), 1);
}