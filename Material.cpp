#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> simpleVertexShader, std::shared_ptr<SimplePixelShader> simplePixelShader) :
    colorTint(colorTint),
    simpleVertexShader(simpleVertexShader),
    simplePixelShader(simplePixelShader)

{
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
