
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthHeightShader : public BaseShader
{
private:
	//Buffer to hold values for tessellation
	struct TessellationBufferType
	{
		float tessellationFactorE;
		float tessellationFactorI;
		XMFLOAT2 padding;
	};

	//Buffer to hold vaules control the waves on height maps
	struct HeightBufferType
	{
		float time;
		float speed;
		float height;
		float padding;
	};

public:

	DepthHeightShader(ID3D11Device* device, HWND hwnd);
	~DepthHeightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* heightTexture, float* wave, float tess);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11Buffer* tessBuffer;
	ID3D11Buffer * heightBuffer;
};
