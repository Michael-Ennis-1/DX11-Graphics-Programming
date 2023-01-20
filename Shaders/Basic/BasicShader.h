// Simple shader that calculates lighting and shadows only, does not Tessellate or manipulate the vertices in any way
#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class BasicShader : public BaseShader
{
public:
	BasicShader(ID3D11Device* device, HWND hwnd);
	~BasicShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* meshTexture, ID3D11ShaderResourceView* shadowMap1, ID3D11ShaderResourceView* shadowMap2, Light* lights[], bool active[], float dropoff2, bool bumpMapping, float specInt, float specExp, Camera* cam, float cutOffAngle);

private:

	// Initialization function
	void initShader(const wchar_t* cs, const wchar_t* ps);

	// Defines new Buffer pointers and a Sampler pointer
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;

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
