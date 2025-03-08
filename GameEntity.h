#pragma once
#include "stdio.h"
#include "memory"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"

class GameEntity
{
private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Material> material;
public:
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();

	void SetMaterial(std::shared_ptr<Material> material);

	void Draw(std::shared_ptr<Camera> cam);
};

