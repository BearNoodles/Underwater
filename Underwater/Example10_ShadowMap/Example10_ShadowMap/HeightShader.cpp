
#include "heightShader.h"


HeightShader::HeightShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"height_vs.cso", L"height_hs.cso", L"height_ds.cso", L"height_ps.cso");
}



void HeightShader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	initShader(vsFilename, psFilename);

	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


HeightShader::~HeightShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}
	if (tessBuffer)
	{
		tessBuffer->Release();
		tessBuffer = 0;
	}
	if (heightBuffer)
	{
		heightBuffer->Release();
		heightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void HeightShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC tessBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC heightBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow1);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow2);

	tessBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessBufferDesc.MiscFlags = 0;
	tessBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessBufferDesc, NULL, &tessBuffer);

	// Setup light buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	// Setup wave buffer
	heightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightBufferDesc.ByteWidth = sizeof(HeightBufferType);
	heightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	heightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightBufferDesc.MiscFlags = 0;
	heightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&heightBufferDesc, NULL, &heightBuffer);

}


void HeightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, Light* dLights, float* wave, float tess)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TessellationBufferType* tessPtr;
	LightBufferType* dLightPtr;
	HeightBufferType* heightPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);
	XMMATRIX tLightViewMatrix0 = XMMatrixTranspose(dLights[0].getViewMatrix());
	XMMATRIX tLightProjectionMatrix0 = XMMatrixTranspose(dLights[0].getOrthoMatrix());
	XMMATRIX tLightViewMatrix1 = XMMatrixTranspose(dLights[1].getViewMatrix());
	XMMATRIX tLightProjectionMatrix1 = XMMatrixTranspose(dLights[1].getOrthoMatrix());

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView[0] = tLightViewMatrix0;
	dataPtr->lightProjection[0] = tLightProjectionMatrix0;
	dataPtr->lightView[1] = tLightViewMatrix1;
	dataPtr->lightProjection[1] = tLightProjectionMatrix1;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->Map(tessBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessellationBufferType*)mappedResource.pData;

	tessPtr->tessellationFactorE = tess;
	tessPtr->tessellationFactorI = tess;
	tessPtr->padding = { 0.0f,0.0f };
	deviceContext->Unmap(tessBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessBuffer);

	//Additional
	// Send light data to pixel shader
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dLightPtr = (LightBufferType*)mappedResource.pData;

	//1st Directional light
	dLightPtr->ambient[0] = dLights[0].getAmbientColour();
	dLightPtr->diffuse[0] = dLights[0].getDiffuseColour();
	dLightPtr->direction[0] = { dLights[0].getDirection().x, dLights[0].getDirection().y, dLights[0].getDirection().z, 0 };

	//2nd directional light
	dLightPtr->ambient[1] = dLights[1].getAmbientColour();
	dLightPtr->diffuse[1] = dLights[1].getDiffuseColour();
	dLightPtr->direction[1] = { dLights[1].getDirection().x, dLights[1].getDirection().y, dLights[1].getDirection().z, 0 };
	//dlightPtr->padding[1] = 0;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);


	deviceContext->Map(heightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	heightPtr = (HeightBufferType*)mappedResource.pData;

	heightPtr->time = wave[0];
	heightPtr->speed = wave[1];
	heightPtr->height = wave[2];
	heightPtr->frequency = wave[3];
	deviceContext->Unmap(heightBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &heightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &depthMap2);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow1);
	deviceContext->PSSetSamplers(2, 1, &sampleStateShadow2);

	// Set shader texture resource in the pixel shader.
	deviceContext->DSSetShaderResources(0, 1, &heightTexture);
	deviceContext->DSSetShaderResources(1, 1, &normalTexture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);
}

