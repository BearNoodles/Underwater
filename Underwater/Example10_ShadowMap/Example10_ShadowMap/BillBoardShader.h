
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

#define DIRCOUNT 2
#define POINTCOUNT 1


class BillboardShader : public BaseShader
{
private:
	//Matrix buffer with extra matrices for the Lights passed in
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[DIRCOUNT + POINTCOUNT];
		XMMATRIX lightProjection[DIRCOUNT + POINTCOUNT];
	};

	//Buffer for passing the camera position
	struct CameraBufferType
	{
		XMFLOAT3 camerPos;
		float padding;
	};

	//Buffer for array of directional lights
	struct DirLightBufferType
	{
		XMFLOAT4 ambient[DIRCOUNT];
		XMFLOAT4 diffuse[DIRCOUNT];
		XMFLOAT4 direction[DIRCOUNT];
		//float padding;
	};

	//Buffer for array of point lights
	struct PointLightBufferType
	{
		XMFLOAT4 ambient[POINTCOUNT];
		XMFLOAT4 diffuse[POINTCOUNT];
		XMFLOAT4 position[POINTCOUNT];
		XMFLOAT4 attenuation[POINTCOUNT];
	};


public:

	BillboardShader(ID3D11Device* device, HWND hwnd);
	~BillboardShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* dLights, Light* sLights, XMFLOAT3 camPos);

	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	//Buffers to pass into shaders
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer * cameraBuffer;
	ID3D11Buffer* dLightBuffer;
	ID3D11Buffer* pLightBuffer;
};
