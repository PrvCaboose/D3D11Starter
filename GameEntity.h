#pragma once
#include "stdio.h"
#include "memory"
#include "Mesh.h"
#include "Transform.h"

class GameEntity
{
private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Transform> transform;
public:
	GameEntity(std::shared_ptr<Mesh> mesh);

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();

	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer);
};

