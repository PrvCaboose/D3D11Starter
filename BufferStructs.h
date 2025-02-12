#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

struct ConstantBufferData {
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 worldMatrix;
};