#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__


#define LIGHT_DIRECTIONAL_TYPE 0
#define LIGHT_TYPE_POINT	   1
#define LIGHT_TYPE_SPOT        2
#define MAX_SPECULAR_EXPONENT 256.0f

// ALL of your code pieces (structs, functions, etc.) go here!
// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 Position : POSITION; // XYZ position
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 worldPosition : POSITION;
};

struct Light
{
    int Type;
    float3 Direction;
    
    float Range;
    float3 Position;
    
    float Intensity;
    float3 Color;
    
    float SpotInnerAngle;
    float SpotOuterAngle;
    float2 Padding;
};

float GetSpecular(float3 normal, float roughness, float3 toCam, float3 toLight)
{
    float3 reflection = reflect(-toLight, normal);
    
    return roughness == 1 ? 0.0 : pow(max(dot(toCam, reflection), 0.0f), (1.0f - roughness) * MAX_SPECULAR_EXPONENT);
}
float GetDiffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float3 DirLight(Light light, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float3 camPos)
{
    // get the opposite direction of the incoming light
    float3 dirToLight = normalize(-light.Direction);
    float3 viewVec = normalize(camPos-worldPos);
    float3 reflectVec = reflect(light.Direction, normal);
    
    // get diffuse color
    float diffuseColor = GetDiffuse(normal,dirToLight);
    float spec = GetSpecular(normal, roughness, viewVec, dirToLight);
    
    // final pixel color
    return (diffuseColor + spec) * surfaceColor * light.Color * light.Intensity;
};

float3 PointLight(Light light, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float3 camPos)
{
    // get the direction of the incoming light
    float3 toLight = normalize(light.Position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    
    // get diffuse color
    float atten = Attenuate(light, worldPos);
    float diffuseColor = GetDiffuse(normal, toLight);
    float spec = GetSpecular(normal, roughness, toCam, toLight);
    
    // final pixel color
    return (diffuseColor * surfaceColor + spec) * atten * light.Color * light.Intensity;
}

float3 SpotLight(Light light, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float3 camPos)
{
    
    float3 toLight = normalize(light.Position - worldPos);
    float angle = saturate(dot(-toLight, light.Direction));
    
    
    
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float fallOffRange = cosOuter - cosInner;
    
    
    float spotTerm = saturate((cosOuter - angle) / fallOffRange);
    
    
    
    return PointLight(light, worldPos, normal, surfaceColor, roughness, camPos) * spotTerm;
}


#endif