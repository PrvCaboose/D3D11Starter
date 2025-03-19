#pragma once
#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"
#include <unordered_map>

class Material
{
public:
	Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> simpleVertexShader, std::shared_ptr<SimplePixelShader> simplePixelShader, DirectX::XMFLOAT2 uvScale, DirectX::XMFLOAT2 uvOffset);
	void AddTextureSRV(std::string textureName,Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string textureName, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState);
	void PrepareMaterial();

	// Getters
	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureMap();

	// Setters
	void SetColorTint(DirectX::XMFLOAT4 colorTint);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> simpleVertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> simplePixelShader);
	void SetUVScale(DirectX::XMFLOAT2 scale);
	void SetUVOffset(DirectX::XMFLOAT2 offset);

private:
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	std::shared_ptr<SimpleVertexShader> simpleVertexShader;
	std::shared_ptr<SimplePixelShader> simplePixelShader;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

};

