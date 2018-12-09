// Light shader.h
// Basic single light shader setup
#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_

#define DIRCOUNT 2
#define SPOTCOUNT 1

#include "DXF.h"

using namespace std;
using namespace DirectX;


class ShadowShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[DIRCOUNT + SPOTCOUNT];
		XMMATRIX lightProjection[DIRCOUNT + SPOTCOUNT];
	};

	struct DirLightBufferType
	{
		XMFLOAT4 ambient[DIRCOUNT];
		XMFLOAT4 diffuse[DIRCOUNT];
		XMFLOAT4 direction[DIRCOUNT];
		//float padding;
	};
	struct SpotLightBufferType
	{
		XMFLOAT4 ambient[SPOTCOUNT];
		XMFLOAT4 diffuse[SPOTCOUNT];
		XMFLOAT4 position[SPOTCOUNT];
		XMFLOAT4 direction[SPOTCOUNT];
		XMFLOAT4 attenuation[SPOTCOUNT];//4th value is max range of spotlight
		XMFLOAT4 cone[SPOTCOUNT];
	};

public:

	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* dLights, Light* sLights);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow1;
	ID3D11SamplerState* sampleStateShadow2;
	ID3D11Buffer* dLightBuffer;
	ID3D11Buffer* sLightBuffer;
};

#endif