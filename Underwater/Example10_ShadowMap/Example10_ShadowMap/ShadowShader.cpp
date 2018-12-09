// texture shader.cpp
#include "shadowshader.h"


ShadowShader::ShadowShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"shadow_vs.cso", L"shadow_ps.cso");
}


ShadowShader::~ShadowShader()
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
	if (dLightBuffer)
	{
		dLightBuffer->Release();
		dLightBuffer = 0;
	}
	if (sLightBuffer)
	{
		sLightBuffer->Release();
		sLightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void ShadowShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC dLightBufferDesc;
	D3D11_BUFFER_DESC sLightBufferDesc;

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
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
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

	// Setup directional light buffer
	dLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	dLightBufferDesc.ByteWidth = sizeof(DirLightBufferType);
	dLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dLightBufferDesc.MiscFlags = 0;
	dLightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&dLightBufferDesc, NULL, &dLightBuffer);

	// Setup spot light buffer
	sLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	sLightBufferDesc.ByteWidth = sizeof(SpotLightBufferType);
	sLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sLightBufferDesc.MiscFlags = 0;
	sLightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&sLightBufferDesc, NULL, &sLightBuffer);

}


void ShadowShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* dLights, Light* sLights)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	DirLightBufferType* dLightPtr;
	SpotLightBufferType* sLightPtr;
	
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix0 = XMMatrixTranspose(dLights[0].getViewMatrix());
	XMMATRIX tLightProjectionMatrix0 = XMMatrixTranspose(dLights[0].getOrthoMatrix());
	XMMATRIX tLightViewMatrix1 = XMMatrixTranspose(dLights[1].getViewMatrix());
	XMMATRIX tLightProjectionMatrix1 = XMMatrixTranspose(dLights[1].getOrthoMatrix());
	XMMATRIX tLightViewMatrix2 = XMMatrixTranspose(sLights[0].getViewMatrix());
	XMMATRIX tLightProjectionMatrix2 = XMMatrixTranspose(sLights[0].getOrthoMatrix());
	
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
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	//Additional
	// Send light data to pixel shader
	deviceContext->Map(dLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dLightPtr = (DirLightBufferType*)mappedResource.pData;

	//1st Directional light
	dLightPtr->ambient[0] = dLights[0].getAmbientColour();
	dLightPtr->diffuse[0] = dLights[0].getDiffuseColour();
	dLightPtr->direction[0] = { dLights[0].getDirection().x, dLights[0].getDirection().y, dLights[0].getDirection().z, 0 };

	//2nd directional light
	dLightPtr->ambient[1] = dLights[1].getAmbientColour();
	dLightPtr->diffuse[1] = dLights[1].getDiffuseColour();
	dLightPtr->direction[1] = { dLights[1].getDirection().x, dLights[1].getDirection().y, dLights[1].getDirection().z, 0 };
	//dlightPtr->padding[1] = 0;
	deviceContext->Unmap(dLightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &dLightBuffer);


	//spot light
	deviceContext->Map(sLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	sLightPtr = (SpotLightBufferType*)mappedResource.pData;

	sLightPtr->ambient[0] = sLights[0].getAmbientColour();
	sLightPtr->diffuse[0] = sLights[0].getDiffuseColour();
	sLightPtr->position[0] = { sLights[0].getPosition().x, sLights[0].getPosition().y, sLights[0].getPosition().z, 0.0f };
	sLightPtr->attenuation[0] = { 0.1f, 0.005f, 0.0f, 1000.0f }; //Constant, Linear and Quadratic Factors and max range of spotlight
	sLightPtr->direction[0] = { sLights[0].getDirection().x, sLights[0].getDirection().y, sLights[0].getDirection().z, 0 };
	sLightPtr->cone[0] = { 10.0f, 0.0f, 0.0f, 0.0f }; //float4 for padding
	
	deviceContext->Unmap(sLightBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &sLightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &depthMap2);
	deviceContext->PSSetShaderResources(3, 1, &depthMap3);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow1);
}

