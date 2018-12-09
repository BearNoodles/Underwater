
#ifndef _HEIGHTSHADER_H_
#define _HEIGHTSHADER_H_
#define DIRCOUNT 2
#define POINTCOUNT 1

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

	struct DirLightBufferType
	{
		XMFLOAT4 ambient[DIRCOUNT];
		XMFLOAT4 diffuse[DIRCOUNT];
		XMFLOAT4 direction[DIRCOUNT];
		//float padding;
	};
	struct PointLightBufferType
	{
		XMFLOAT4 ambient[POINTCOUNT];
		XMFLOAT4 diffuse[POINTCOUNT];
		XMFLOAT4 position[POINTCOUNT];
		XMFLOAT4 attenuation[POINTCOUNT];
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

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* dLights, Light* sLights, float* wave, float tess);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow1;
	ID3D11Buffer* tessBuffer;
	ID3D11Buffer* dLightBuffer;
	ID3D11Buffer* pLightBuffer;
	ID3D11Buffer* heightBuffer;
};

#endif