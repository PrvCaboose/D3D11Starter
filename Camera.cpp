#include "Camera.h"

Camera::Camera(
	DirectX::XMFLOAT3 initPos,
	float aspectRatio,
	float fov,
	float nearClipDist,
	float farClipDist,
	float moveSpeed,
	float lookSpeed
):
	aspectRatio(aspectRatio),
	fov(fov),
	nearClipDist(nearClipDist),
	farClipDist(farClipDist),
	moveSpeed(moveSpeed),
	lookSpeed(lookSpeed)
{
	transform = std::make_shared<Transform>();
	transform->SetPosition(initPos);

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 pos = transform->GetPosition();
	DirectX::XMFLOAT3 forward = transform->GetForward();

	DirectX::XMMATRIX mat = DirectX::XMMatrixLookToLH(
		DirectX::XMLoadFloat3(&pos),
		DirectX::XMLoadFloat3(&forward),
		DirectX::XMVectorSet(0,1,0,0));
	DirectX::XMStoreFloat4x4(&viewMatrix, mat);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, 0.01f, 1000.0f));
}

void Camera::Update(float dt)
{
	// Inputs
	float dtSpeed = moveSpeed * dt;

	// Set speed
	if (Input::KeyDown(VK_SHIFT)) { dtSpeed *= 2; }
	if (Input::KeyDown(VK_CONTROL)) { dtSpeed *= 0.5; }

	// Move
	if (Input::KeyDown('W')) { transform->MoveRelative(0, 0, dtSpeed); }
	if (Input::KeyDown('S')) { transform->MoveRelative(0, 0, -dtSpeed); }
	if (Input::KeyDown('A')) { transform->MoveRelative(-dtSpeed, 0, 0); }
	if (Input::KeyDown('D')) { transform->MoveRelative(dtSpeed, 0, 0); }
	if (Input::KeyDown(' ')) { transform->MoveRelative(0, dtSpeed, 0); }
	if (Input::KeyDown('X')) { transform->MoveRelative(0, -dtSpeed, 0); }

	if (Input::MouseLeftDown())
	{
		float cursorMovementX = Input::GetMouseXDelta() * lookSpeed;
		float cursorMovementY = Input::GetMouseYDelta() * lookSpeed;

		transform->Rotate(cursorMovementY, cursorMovementX, 0);

		// Clamp y rotation
		DirectX::XMFLOAT3 rotation = transform->GetPitchYawRoll();
		if (rotation.x > (3.1415962f / 2.0f)) { rotation.x = (3.1415962f / 2.0f); }
		if (rotation.x < -(3.1415962f / 2.0f)) { rotation.x = -(3.1415962f / 2.0f); }
		transform->SetRotation(rotation);
	}

	UpdateViewMatrix();
}

DirectX::XMFLOAT4X4 Camera::GetView()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProj()
{
	return projectionMatrix;
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
	return transform->GetPosition();
}

float Camera::GetFov()
{
	return fov;
}
