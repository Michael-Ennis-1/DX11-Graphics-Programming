#include "tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessellation_quad_vs.cso", L"tessellation_quad_hs.cso", L"tessellation_quad_ds.cso", L"tessellation_quad_ps.cso");
}


TessellationShader::~TessellationShader()
{
	// Release the sampler
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	// Release the matrix buffer
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	// Release the tessellation buffer
	if (tessBuffer)
	{
		tessBuffer->Release();
		tessBuffer = 0;
	}
	// Release the light buffer
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}
	// Release layout
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	
	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix buffer.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup the description of then tessellation buffer.
	D3D11_BUFFER_DESC tessBufferDesc;
	tessBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessBufferDesc.ByteWidth = sizeof(TessBufferType);
	tessBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessBufferDesc.MiscFlags = 0;
	tessBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessBufferDesc, NULL, &tessBuffer);

	// Setup description of the Texture Sampler.
	D3D11_SAMPLER_DESC samplerDesc;
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

	// Setup description of the Light Buffer
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* heightMap, ID3D11ShaderResourceView* shadowMap1, ID3D11ShaderResourceView* shadowMap2, int tessFactor, Light* lights[], bool active[], float dropoff2, bool bumpMapping, float specInt, float specExp, Camera* cam, float cutOffAngle)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Set matrix buffer with both world, view and projection matrices and send to Domain Shader
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->worldMatrix = tworld;// worldMatrix;
	dataPtr->viewMatrix = tview;
	dataPtr->projectionMatrix = tproj;

	// Set directional light view matrix and ortho matrix for use with generating shadows
	dataPtr->lightViewMatrix1 = XMMatrixTranspose(lights[0]->getViewMatrix());
	dataPtr->lightProjectionMatrix1 = XMMatrixTranspose(lights[0]->getOrthoMatrix());

	// Set spot light view matrix and projection matrix for use with generating shadows
	dataPtr->lightViewMatrix2 = XMMatrixTranspose(lights[2]->getViewMatrix());
	dataPtr->lightProjectionMatrix2 = XMMatrixTranspose(lights[2]->getProjectionMatrix());

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	// Set tessellation factors and send to Hull Shader, Domain Shader and Pixel Shader
	TessBufferType* tessPtr;
	deviceContext->Map(tessBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessBufferType*)mappedResource.pData;
	tessPtr->insideFactor = tessFactor;
	tessPtr->outsideFactor = tessFactor;
	tessPtr->padding = XMFLOAT2(0.0f, 0.0f);
	deviceContext->Unmap(tessBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessBuffer);
	deviceContext->DSSetConstantBuffers(1, 1, &tessBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &tessBuffer);

	// Set light data and send to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;

	// Directional Light data
	lightPtr->ambient1 = lights[0]->getAmbientColour();
	lightPtr->diffuse1 = lights[0]->getDiffuseColour();
	lightPtr->direction1 = lights[0]->getDirection();
	lightPtr->active1 = active[0];

	// Point Light data
	lightPtr->ambient2 = lights[1]->getAmbientColour();
	lightPtr->diffuse2 = lights[1]->getDiffuseColour();
	lightPtr->position2 = lights[1]->getPosition();
	lightPtr->active2 = active[1];
	lightPtr->dropoff2 = dropoff2;
	lightPtr->bumpNormals = bumpMapping;
	lightPtr->specIntensity = specInt;
	lightPtr->specExponent = specExp;
	lightPtr->camPos = cam->getPosition();
	lightPtr->padding = 0.0f;

	// Spot Light data
	lightPtr->ambient3 = lights[2]->getAmbientColour();
	lightPtr->diffuse3 = lights[2]->getDiffuseColour();
	lightPtr->position3 = lights[2]->getPosition();
	lightPtr->active3 = active[2];
	lightPtr->direction3 = lights[2]->getDirection();
	lightPtr->cutoff = cutOffAngle;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Set sampler and texture for use in the Domain Shader
	deviceContext->DSSetSamplers(0, 1, &sampleState);
	deviceContext->DSSetShaderResources(0, 1, &heightMap);

	// Set sampler and textures for use in the Pixel Shader
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetShaderResources(0, 1, &heightMap);
	deviceContext->PSSetShaderResources(1, 1, &shadowMap1);
	deviceContext->PSSetShaderResources(2, 1, &shadowMap2);
}


