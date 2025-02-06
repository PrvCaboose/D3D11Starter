#include "Transform.h"

using namespace DirectX;

Transform::Transform() : 
	position(0, 0, 0),
	pitchYawRoll(0, 0, 0),
	scale(1, 1, 1)
{
	//XMStoreFloat3();
}

void Transform::MoveAbsolute(float x, float y, float z) {
	//XMStoreFloat3(&position, XMLoadFloat3(&position)+)
}
