// Light shader.h
// Basic single light shader setup
#ifndef _HEIGHTSHADER_H_
#define _HEIGHTSHADER_H_
#define DIRCOUNT 2

#include "DXF.h"

using namespace std;
using namespace DirectX;


class HeightShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[DIRCOUNT];
		XMFLOAT4 diffuse[DIRCOUNT];
		XMFLOAT4 direction[DIRCOUNT];
		//float padding;
	};

public:

	HeightShader(ID3D11Device* device, HWND hwnd);
	~HeightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightTexture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, Light* dLights);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow1;
	ID3D11SamplerState* sampleStateShadow2;
	ID3D11Buffer* lightBuffer;
};

#endif