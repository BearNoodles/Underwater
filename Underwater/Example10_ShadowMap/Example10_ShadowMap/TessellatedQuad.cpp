#include "TessellatedQuad.h"

// ColourTriangle.cpp
// Simple triangle mesh for example purposes. With colour, no texture coordinates or normals.

// Initialise buffers
TessellatedQuad::TessellatedQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int quads)
{
	quadCount = quads;
	initBuffers(device);
}

// Release resources.
TessellatedQuad::~TessellatedQuad()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build shape and fill buffers.
void TessellatedQuad::initBuffers(ID3D11Device* device)
{
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//Vertex count and index count for square quad plane
	vertexCount = 4 * quadCount * quadCount;
	indexCount = 4 * quadCount * quadCount;

	//Initialise vertex and index arrays
	VertexType_Texture* vertices = new VertexType_Texture[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	//Loop over numver of rows and column (both same)
	for (int i = 0; i < quadCount; i++)
	{
		for (int j = 0; j < quadCount; j++)
		{
			//x and y keep the current vertex number updated
			int x = 4 * i * quadCount;
			int y = 4 * j;

			//Position each corner of the quad then add i the the x value and j to the z value
			//Texture coordinate then moves i or j divided by quadcount of a fraction along 
			vertices[0 + x + y].position = XMFLOAT3(0.0f + i, 0.0f, 1.0f + j);  // top left.
			vertices[0 + x + y].texture = XMFLOAT2((0.0f + i) / (quadCount ), (0.0f - j) / (quadCount));

			vertices[1 + x + y].position = XMFLOAT3(0.0f + i, 0.0f, 0.0f + j);  // bottom left.
			vertices[1 + x + y].texture = XMFLOAT2((0.0f + i) / (quadCount ), (1.0f - j) / (quadCount));

			vertices[2 + x + y].position = XMFLOAT3(1.0f + i, 0.0f, 0.0f + j);  // bottom right.
			vertices[2 + x + y].texture = XMFLOAT2((1.0f + i) / (quadCount ), (1.0f - j) / (quadCount));

			vertices[3 + x + y].position = XMFLOAT3(1.0f + i, 0.0f, 1.0f + j);  // top right.
			vertices[3 + x + y].texture = XMFLOAT2((1.0f + i) / (quadCount ), (0.0f - j) / (quadCount));

			//Update index numbers
			indices[0 + x + y] = 0 + x + y;
			indices[1 + x + y] = 1 + x + y;
			indices[2 + x + y] = 2 + x + y;
			indices[3 + x + y] = 3 + x + y;
		}

	}

	//initialise vertex and index buffer descriptions and create the buffers
	D3D11_BUFFER_DESC vertexBufferDesc = { sizeof(VertexType_Texture) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
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
void TessellatedQuad::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType_Texture);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}
