#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> simpleVertexShader, std::shared_ptr<SimplePixelShader> simplePixelShader, DirectX::XMFLOAT2 uvScale, DirectX::XMFLOAT2 uvOfsett,float roughness) :
    colorTint(colorTint),
    simpleVertexShader(simpleVertexShader),
    simplePixelShader(simplePixelShader),
    uvScale(uvScale),
    uvOffset(uvOfsett),
    roughness(roughness)
{
}

void Material::AddTextureSRV(std::string textureName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs.insert({ textureName,srv });
}

void Material::AddSampler(std::string textureName, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState)
{
    samplers.insert({ textureName,samplerState });
}

void Material::PrepareMaterial()
{
    for (auto& t : textureSRVs) { simplePixelShader->SetShaderResourceView(t.first.c_str(),t.second); }
    for (auto& s : samplers) { simplePixelShader->SetSamplerState(s.first.c_str(), s.second); }
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
    return simpleVertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return simplePixelShader;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
    return uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
    return uvOffset;
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Material::GetTextureMap()
{
    return textureSRVs;
}

float Material::GetRoughness()
{
    return roughness;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
    this->colorTint = colorTint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> simpleVertexShader)
{
    this->simpleVertexShader = simpleVertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> simplePixelShader)
{
    this->simplePixelShader = simplePixelShader;
}

void Material::SetUVScale(DirectX::XMFLOAT2 scale)
{
    uvScale = scale;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 offset)
{
    uvOffset = offset;
}

void Material::SetRoughness(float roughness)
{
    this->roughness = roughness;
}
