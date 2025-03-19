#include "GameEntity.h"
#include "BufferStructs.h"
#include "Graphics.h"
#include "Camera.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
	mesh(mesh), material(material)
{
	transform = std::make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return transform;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}

void GameEntity::Draw(std::shared_ptr<Camera> cam)
{
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	//material->GetVertexShader()->SetFloat4("colorTint", material->GetColorTint());
	material->GetVertexShader()->SetMatrix4x4("worldMatrix", transform->GetWorldMatrix());
	material->GetVertexShader()->SetMatrix4x4("viewMatrix", cam->GetView());
	material->GetVertexShader()->SetMatrix4x4("projMatrix", cam->GetProj());

	material->GetVertexShader()->CopyAllBufferData();

	material->GetPixelShader()->SetFloat4("colorTint", material->GetColorTint());
	material->GetPixelShader()->SetFloat2("uvScale", material->GetUVScale());
	material->GetPixelShader()->SetFloat2("uvOffset", material->GetUVOffset());
	material->GetPixelShader()->CopyAllBufferData();

	material->PrepareMaterial();

	mesh->Draw();
}
