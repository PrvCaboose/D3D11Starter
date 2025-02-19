#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	pitchYawRoll(0, 0, 0),
	scale(1, 1, 1),
	matrixChanged(false)
{
	// Init matricies
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	matrixChanged = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
	matrixChanged = true;
}

void Transform::SetRotation(float p, float y, float r)
{
	pitchYawRoll.x = p;
	pitchYawRoll.y = y;
	pitchYawRoll.z = r;
	matrixChanged = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll = rotation;
	matrixChanged = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	matrixChanged = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
	matrixChanged = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (!matrixChanged)
	{
		return worldMatrix;
	}
	XMMATRIX world =
		(XMMatrixScaling(scale.x,scale.y,scale.z)) *
		(XMMatrixRotationRollPitchYaw(pitchYawRoll.x,pitchYawRoll.y,pitchYawRoll.z)) *
		(XMMatrixTranslation(position.x, position.y, position.z));
	XMStoreFloat4x4(&worldMatrix, world);
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	XMVECTOR rot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	XMVECTOR right = XMVectorSet(1,0,0,0);
	XMFLOAT3 dir;
	XMStoreFloat3(&dir,XMVector3Rotate(right, rot));
	return dir;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	XMVECTOR rot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMFLOAT3 dir;
	XMStoreFloat3(&dir, XMVector3Rotate(up, rot));
	return dir;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	XMVECTOR rot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	XMVECTOR foreward = XMVectorSet(0, 0, 1, 0);
	XMFLOAT3 dir;
	XMStoreFloat3(&dir, XMVector3Rotate(foreward, rot));
	return dir;
}

void Transform::MoveAbsolute(float x, float y, float z) {
	position.x += x;
	position.y += y;
	position.z += z;
	matrixChanged = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;
	matrixChanged = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR vec = XMVectorSet(x, y, z, 0);
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(
		this->GetPitchYawRoll().x,
		this->GetPitchYawRoll().y,
		this->GetPitchYawRoll().z
	);
	XMVECTOR dir = XMVector3Rotate(vec,quat);
	XMStoreFloat3(&position,XMLoadFloat3(&position) + dir);
	matrixChanged = true;
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
	XMVECTOR vec = XMVectorSet(offset.x,offset.y,offset.z,0);
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(
		this->GetPitchYawRoll().x,
		this->GetPitchYawRoll().y,
		this->GetPitchYawRoll().z
	);
	XMVECTOR dir = XMVector3Rotate(vec, quat);
	XMStoreFloat3(&position, XMLoadFloat3(&position) + dir);
	matrixChanged = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;
	matrixChanged = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll.x += rotation.x;
	pitchYawRoll.y += rotation.y;
	pitchYawRoll.z += rotation.z;
	matrixChanged = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x += x;
	scale.y += y;
	scale.z += z;
	matrixChanged = true;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	this->scale.x += scale.x;
	this->scale.y += scale.y;
	this->scale.z += scale.z;
	matrixChanged = true;
}
