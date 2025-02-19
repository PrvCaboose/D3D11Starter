#include "GameEntity.h"
#include "BufferStructs.h"
#include "Graphics.h"
#include "Camera.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh) :
	mesh(mesh)
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

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer, std::shared_ptr<Camera> cam)
{
	// Copy data into constant buffer
	ConstantBufferData cbData;
	cbData.colorTint = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbData.worldMatrix = transform->GetWorldMatrix();
	cbData.viewMatrix = cam->GetView();
	cbData.projMatrix = cam->GetProj();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &cbData, sizeof(cbData));
	Graphics::Context->Unmap(cBuffer.Get(), 0);
	mesh->Draw();
}
