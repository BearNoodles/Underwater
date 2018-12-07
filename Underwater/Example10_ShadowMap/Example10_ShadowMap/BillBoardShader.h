
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class BillboardShader : public BaseShader
{

public:

	BillboardShader(ID3D11Device* device, HWND hwnd);
	~BillboardShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
};
