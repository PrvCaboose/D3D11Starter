#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics.h"
#include "Vertex.h"

class Mesh
{
	private :
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		int numIndices;
		int numVertices;

	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() {
			return vertexBuffer;
		}

		Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() {
			return indexBuffer;
		}

		int GetIndexCount() {
			return numIndices;
		}

		int GetVertexCount() {
			return numVertices;
		}

		Mesh(Vertex vertices[], unsigned int indices[], int numVertices, int numIndices);
		void Draw();
		~Mesh();
};

