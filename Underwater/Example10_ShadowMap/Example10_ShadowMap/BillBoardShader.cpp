// geometry shader.cpp
#include "BillboardShader.h"


BillboardShader::BillboardShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"fiShader_vs.cso", L"fiShader_gs.cso", L"fiShader_ps.cso");
}


BillboardShader::~BillboardShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (sampleStateShadow)
	{
		sampleStateShadow->Release();
		sampleStateShadow = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
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
	if (pLightBuffer)
	{
		pLightBuffer->Release();
		pLightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void BillboardShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC dLightBufferDesc;
	D3D11_BUFFER_DESC pLightBufferDesc;

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

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);


	// Setup light buffer
	dLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	dLightBufferDesc.ByteWidth = sizeof(DirLightBufferType);
	dLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dLightBufferDesc.MiscFlags = 0;
	dLightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&dLightBufferDesc, NULL, &dLightBuffer);

	// Setup light buffer
	pLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pLightBufferDesc.ByteWidth = sizeof(PointLightBufferType);
	pLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pLightBufferDesc.MiscFlags = 0;
	pLightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&pLightBufferDesc, NULL, &pLightBuffer);

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

	// Create the texture sampler state.
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
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

}

void BillboardShader::initShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadGeometryShader(gsFilename);
}


void BillboardShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* dLights, Light* pLights, XMFLOAT3 camPos)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	CameraBufferType* camPtr;
	DirLightBufferType* dLightPtr;
	PointLightBufferType* pLightPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix0 = XMMatrixTranspose(dLights[0].getViewMatrix());
	XMMATRIX tLightProjectionMatrix0 = XMMatrixTranspose(dLights[0].getOrthoMatrix());
	XMMATRIX tLightViewMatrix1 = XMMatrixTranspose(dLights[1].getViewMatrix());
	XMMATRIX tLightProjectionMatrix1 = XMMatrixTranspose(dLights[1].getOrthoMatrix());
	XMMATRIX tLightViewMatrix2 = XMMatrixTranspose(pLights[0].getViewMatrix());
	XMMATRIX tLightProjectionMatrix2 = XMMatrixTranspose(pLights[0].getOrthoMatrix());

	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->GSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->camerPos = camPos;
	camPtr->padding = 0.0f;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->GSSetConstantBuffers(1, 1, &cameraBuffer);


	// Additional
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


	//point light
	deviceContext->Map(pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	pLightPtr = (PointLightBufferType*)mappedResource.pData;

	pLightPtr->ambient[0] = pLights[0].getAmbientColour();
	pLightPtr->diffuse[0] = pLights[0].getDiffuseColour();
	pLightPtr->position[0] = { pLights[0].getPosition().x, pLights[0].getPosition().y, pLights[0].getPosition().z, 0.0f };
	pLightPtr->attenuation[0] = { 1.0f, 0.125f, 0.0f, 0.0f }; //Constant, Linear and Quadratic Factors and padding

	deviceContext->Unmap(pLightBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &pLightBuffer);

	// Set shader texture resource in the pixel shader.
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &depthMap2);
	deviceContext->PSSetShaderResources(3, 1, &depthMap3);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);

}

void BillboardShader::render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);

	// Base render function.
	BaseShader::render(deviceContext, indexCount);
}



