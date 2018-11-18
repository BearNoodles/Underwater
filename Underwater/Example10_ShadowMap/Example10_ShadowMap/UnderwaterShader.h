#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class UnderwaterShader : public BaseShader
{
private:
	struct UnderwaterBufferType
	{
		float time;
		XMFLOAT3 padding;
	};

	//struct FogBufferType
	//{
	//	XMFLOAT3 cameraPosition;
	//	float fogStart;
	//	float fogEnd;
	//	XMFLOAT3 padding;
	//};

	

public:
	UnderwaterShader(ID3D11Device* device, HWND hwnd);
	~UnderwaterShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float time_);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* waterBuffer;
	//ID3D11Buffer* fogBuffer;
};

