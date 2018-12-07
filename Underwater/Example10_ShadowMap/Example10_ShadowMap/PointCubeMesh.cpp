#include "PointCubeMesh.h"



PointCubeMesh::PointCubeMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	initBuffers(device);
}

// Release resources.
PointCubeMesh::~PointCubeMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build shape and fill buffers.
void PointCubeMesh::initBuffers(ID3D11Device* device)
{
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	quadRows = 10;
	quadCols = 10;
	quadLayers = 10;

	vertexCount = quadRows * quadCols * quadLayers;
	indexCount = quadRows * quadCols * quadLayers;

	quadGap = 2.5f;

	VertexType_Colour* vertices = new VertexType_Colour[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	// Load the vertex array with data.

	for (int i = 0; i < quadCols; i++)
	{
		for (int j = 0; j < quadRows; j++)
		{
			for (int k = 0; k < quadLayers; k++)
			{
				int x = i * quadRows * quadLayers;
				int y = j * quadLayers;
				int z = k;
				vertices[x + y + z].position = XMFLOAT3(i * quadGap, j * quadGap, k * quadGap);
				vertices[x + y + z].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

				indices[x + y + z] = x + y + z;
			}
		}
	}


	///

	//vertices[4].position = XMFLOAT3(1.0f, 1.0f, -1.0f);  // Top right.
	//vertices[4].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//
	//vertices[5].position = XMFLOAT3(0.0f, 1.0f, -1.0f);  // Top left.
	//vertices[5].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//
	//vertices[6].position = XMFLOAT3(3.0f, 0.0f, -1.0f);  // bottom right.
	//vertices[6].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//
	//vertices[7].position = XMFLOAT3(1.0f, 0.0f, -1.0f);  // bottom left.
	//vertices[7].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	// Load the index array with data.


	D3D11_BUFFER_DESC vertexBufferDesc = { sizeof(VertexType_Colour) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	vertexData = { vertices, 0 , 0 };
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc = { sizeof(unsigned long) * indexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	indexData = { indices, 0, 0 };
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

// Send Geometry data to the GPU
void PointCubeMesh::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType_Colour);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}
