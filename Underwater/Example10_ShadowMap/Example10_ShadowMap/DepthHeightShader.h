
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthHeightShader : public BaseShader
{
public:

	DepthHeightShader(ID3D11Device* device, HWND hwnd);
	~DepthHeightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* heightTexture);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
};
