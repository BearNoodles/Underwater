// depth shader.cpp
#include "depthheightshader.h"


DepthHeightShader::DepthHeightShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depthHeight_vs.cso", L"depthHeight_hs.cso", L"depthHeight_ds.cso", L"depthHeight_ps.cso");
}



void DepthHeightShader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	initShader(vsFilename, psFilename);

	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


DepthHeightShader::~DepthHeightShader()
{
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

	//Release base shader components
	BaseShader::~BaseShader();
}


void DepthHeightShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC tessBufferDesc;
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
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	//samplerDesc.BorderColor[0] = 1.0f;
	//samplerDesc.BorderColor[1] = 1.0f;
	//samplerDesc.BorderColor[2] = 1.0f;
	//samplerDesc.BorderColor[3] = 1.0f;
	//renderer->CreateSamplerState(&samplerDesc, &sampleState);


	tessBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessBufferDesc.MiscFlags = 0;
	tessBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessBufferDesc, NULL, &tessBuffer);


	// Setup wave buffer
	heightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightBufferDesc.ByteWidth = sizeof(HeightBufferType);
	heightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	heightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightBufferDesc.MiscFlags = 0;
	heightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&heightBufferDesc, NULL, &heightBuffer);

}


void DepthHeightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* heightTexture, float* wave, float tess)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TessellationBufferType* tessPtr;
	HeightBufferType* heightPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	//// Lock the constant buffer so it can be written to.
	//deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//dataPtr = (MatrixBufferType*)mappedResource.pData;
	//dataPtr->world = tworld;// worldMatrix;
	//dataPtr->view = tview;
	//dataPtr->projection = tproj;
	//deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);


	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->Map(tessBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessellationBufferType*)mappedResource.pData;

	tessPtr->tessellationFactorE = tess;
	tessPtr->tessellationFactorI = tess;
	tessPtr->padding = { 0.0f,0.0f };
	deviceContext->Unmap(tessBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessBuffer);


	deviceContext->Map(heightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	heightPtr = (HeightBufferType*)mappedResource.pData;

	heightPtr->time = wave[0];
	heightPtr->speed = wave[1];
	heightPtr->height = wave[2];
	heightPtr->padding = 0.0f;
	deviceContext->Unmap(heightBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &heightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->DSSetShaderResources(0, 1, &heightTexture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

}


