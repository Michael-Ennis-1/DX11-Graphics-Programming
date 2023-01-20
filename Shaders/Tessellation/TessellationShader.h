// Tessellates the heightmap to generate more realistic proportions, calculates lighting and shadows on the new proportions
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class TessellationShader : public BaseShader
{

public:

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* heightMap, ID3D11ShaderResourceView* shadowMap1, ID3D11ShaderResourceView* shadowMap2, int tessFactor, Light* lights[], bool active[], float dropoff2, bool bumpMapping, float specInt, float specExp, Camera* cam, float cutOffAngle);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tessBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* sampleState;

	// Stores matrices to be used in vertex manipulation and shadow generation
	struct MatrixBufferType
	{
		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;

		XMMATRIX lightViewMatrix1;
		XMMATRIX lightProjectionMatrix1;

		XMMATRIX lightViewMatrix2;
		XMMATRIX lightProjectionMatrix2;
	};

	// Stores tessellation values to determine the amount of slices
	struct TessBufferType
	{
		float insideFactor;
		float outsideFactor;
		XMFLOAT2 padding;
	};

	// Stores light values to calculate lighting
	struct LightBufferType
	{
		XMFLOAT4 ambient1;
		XMFLOAT4 diffuse1;
		XMFLOAT3 direction1;
		float active1;

		XMFLOAT4 ambient2;
		XMFLOAT4 diffuse2;
		XMFLOAT3 position2;
		float active2;
		float dropoff2;
		float bumpNormals;
		float specIntensity;
		float specExponent;
		XMFLOAT3 camPos;
		float padding;

		XMFLOAT4 ambient3;
		XMFLOAT4 diffuse3;
		XMFLOAT3 position3;
		float active3;
		XMFLOAT3 direction3;
		float cutoff;
	};
};
