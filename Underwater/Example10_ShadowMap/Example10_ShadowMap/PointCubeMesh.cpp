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

	//Set number of rows columns and layers of control point
	quadRows = 3;
	quadCols = 10;
	quadLayers = 10;

	//Vertex and index count 
	vertexCount = quadRows * quadCols * quadLayers;
	indexCount = quadRows * quadCols * quadLayers;

	//Set gap between each control point
	quadGap = 10.0f;

	//Initialise vertex and index count array
	VertexType* vertices = new VertexType[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	// Load the vertex array with data.

	//Loop over number of column then rows then layers
	for (int i = 0; i < quadCols; i++)
	{
		for (int j = 0; j < quadRows; j++)
		{
			for (int k = 0; k < quadLayers; k++)
			{
				//x, y and z are added to the current vertex
				int x = i * quadRows * quadLayers;
				int y = j * quadLayers;
				int z = k;
				//Position each point based on row, column and layer number multiplied by the specified gap between them
				vertices[x + y + z].position = XMFLOAT3(i * quadGap, j * quadGap, k * quadGap);

				//Add this vertex to the index array
				indices[x + y + z] = x + y + z;
			}
		}
	}


	//Setup buffer description and create them for vertices and indices
	D3D11_BUFFER_DESC vertexBufferDesc = { sizeof(VertexType) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
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
	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}
