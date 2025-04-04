#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

class Sky
{
public:
	Sky(const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back,
		std::shared_ptr<Mesh> mesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleOptions,
		std::shared_ptr<SimplePixelShader> pixelShader,
		std::shared_ptr<SimpleVertexShader> vertexShader);

	~Sky();

	void Draw(std::shared_ptr<Camera> camera);

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleOptions;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSrv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;

	// --- HEADER ---

	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};

