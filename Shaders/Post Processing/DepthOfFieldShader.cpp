#include "DepthOfFieldShader.h"

DepthOfFieldShader::DepthOfFieldShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depth_of_field_vs.cso", L"depth_of_field_ps.cso");
}

DepthOfFieldShader::~DepthOfFieldShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
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

	// Release the light constant buffer.
	if (activeBuffer)
	{
		activeBuffer->Release();
		activeBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthOfFieldShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC activeBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the matrix buffer, to be used in vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup active buffer for use in pixel shader
	activeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	activeBufferDesc.ByteWidth = sizeof(ActiveBufferType);
	activeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	activeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	activeBufferDesc.MiscFlags = 0;
	activeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&activeBufferDesc, NULL, &activeBuffer);
}

void DepthOfFieldShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* blurTexture, ID3D11ShaderResourceView* depthTexture, float weighting, float cutOff, float lerpPercent, bool activeDOF)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Set matrix buffer and send to the Vertex Shader
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Set active buffer and send to the Pixel Shader
	ActiveBufferType* activePtr;
	deviceContext->Map(activeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	activePtr = (ActiveBufferType*)mappedResource.pData;
	activePtr->percentage = lerpPercent;
	activePtr->cutoff = cutOff;
	activePtr->weight = weighting;
	activePtr->active = activeDOF;
	deviceContext->Unmap(activeBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &activeBuffer);

	// Set Textures and the sampler in the Pixel Shader.
	deviceContext->PSSetShaderResources(0, 1, &normalTexture);
	deviceContext->PSSetShaderResources(1, 1, &blurTexture);
	deviceContext->PSSetShaderResources(2, 1, &depthTexture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	// Set texture and sampler in the Vertex Shader
	deviceContext->VSSetShaderResources(0, 1, &normalTexture);
	deviceContext->VSSetSamplers(0, 1, &sampleState);
}