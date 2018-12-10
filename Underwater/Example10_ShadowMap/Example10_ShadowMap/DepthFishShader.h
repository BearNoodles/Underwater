
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthFishShader : public BaseShader
{
private:


public:

	DepthFishShader(ID3D11Device* device, HWND hwnd);
	~DepthFishShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
};
