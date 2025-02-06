#pragma once
#include <DirectXMath.h>

class Transform
{
public:
	Transform();

	// Setters
	void SetPosition(float x, float y, float z);
	//void SetPosition(DirectX::XMFLOAT3 pos);
	void SetRotation(float p, float y, float r);
	void SetScale(float x, float y, float z);

	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(float x, float y, float z);

	void Rotate(float p, float y, float r);
	void Scale(float x, float y, float z);

	// Getters
	//void DirectX::XMFLOAT3 

private:
	// Transofrm Data
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 pitchYawRoll;

	// Matrix
	DirectX::XMFLOAT4X4 worldMatrix;
};

