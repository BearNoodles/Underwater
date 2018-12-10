#pragma once

#include "DXF.h"

using namespace DirectX;

class TessellatedQuad : public BaseMesh
{
public:
	TessellatedQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int quads);
	~TessellatedQuad();

	virtual void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
protected:
	void initBuffers(ID3D11Device* device);

private:
	//Number of quads in each row and column that make up the mesh
	int quadCount;
};

