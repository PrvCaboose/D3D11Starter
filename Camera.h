#pragma once
#include "Input.h"
#include "math.h"
#include "memory"
#include "Transform.h"
#include <DirectXMath.h>

class Camera
{
private:
	std::shared_ptr<Transform> transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	float fov;
	float aspectRatio;
	float nearClipDist;
	float farClipDist;
	float moveSpeed;
	float lookSpeed;
public:
	Camera(DirectX::XMFLOAT3 initPos,float aspectRatio,float fov,float nearClipDist,float farClipDist,float moveSpeed,float lookSpeed);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);
	void Update(float dt);
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProj();
	DirectX::XMFLOAT3 GetPosition();
	float GetFov();
};

