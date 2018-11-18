#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class FogShader : public BaseShader
{
private:
	struct FogBufferType
	{
		XMFLOAT2 cameraPosition;
		float fogStart;
		float fogEnd;
	};



public:
	FogShader(ID3D11Device* device, HWND hwnd);
	~FogShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float* fog, float* cameraPos);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* fogBuffer;
};

