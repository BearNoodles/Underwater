
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
		XMMATRIX lightView[DIRCOUNT];
		XMMATRIX lightProjection[DIRCOUNT];
	};

	struct TessellationBufferType
	{
		float tessellationFactorE;
		float tessellationFactorI;
		XMFLOAT2 padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[DIRCOUNT];
		XMFLOAT4 diffuse[DIRCOUNT];
		XMFLOAT4 direction[DIRCOUNT];
		//float padding;
	};

	struct HeightBufferType
	{
		float time;
		float speed;
		float height;
		float frequency;
	};

public:

	HeightShader(ID3D11Device* device, HWND hwnd);
	~HeightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, Light* dLights, float* wave, float tess);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow1;
	ID3D11SamplerState* sampleStateShadow2;
	ID3D11Buffer* tessBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* heightBuffer;
};

#endif