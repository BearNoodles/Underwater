#ifndef _POINTCUBEMESH_H_
#define _POINTCUBEMESH_H_

#include "BaseMesh.h"

using namespace DirectX;

class PointCubeMesh : public BaseMesh
{

public:
	PointCubeMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~PointCubeMesh();

	virtual void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

protected:
	void initBuffers(ID3D11Device* device);

	//Number of quads in each row column and layer
	int quadRows;
	int quadCols;
	int quadLayers;

	//Gap between each control points
	float quadGap;

};

#endif