
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class BillboardShader : public BaseShader
{
private:
	struct CameraBufferType
	{
		XMFLOAT3 camerPos;
		float padding;
	};

	struct HeightBufferType
	{
		float time;
		float speed;
		float height;
		float frequency;
	};

public:

	BillboardShader(ID3D11Device* device, HWND hwnd);
	~BillboardShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 camPos, float* wave);

	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer * cameraBuffer;
	ID3D11Buffer * heightBuffer;
};
