#pragma once
#include <DirectXMath.h>

#define LIGHT_DIRECTIONAL_TYPE 0
#define LIGHT_TYPE_POINT	   1
#define LIGHT_TYPE_SPOT        2

struct Light{
	int Type;
	DirectX::XMFLOAT3 Direction;
	float Range;
	DirectX::XMFLOAT3 Position;
	float Intensity;
	DirectX::XMFLOAT3 Color;
	float SpotInnerAngle;
	float SpotOuterAngle;
	DirectX::XMFLOAT2 Padding;
};