// Blurs the screen based on how far away a pixel's depth is from the centre pixel
#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class DepthOfFieldShader : public BaseShader
{
public:
	DepthOfFieldShader(ID3D11Device* device, HWND hwnd);
	~DepthOfFieldShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* blurTexture, ID3D11ShaderResourceView* depthTexture, float weighting, float cutOff, float lerpPercent, bool activeDOF);

private:

	// Stores values that affect the Depth of Field calculation
	struct ActiveBufferType
	{
		float percentage;
		float cutoff;
		float weight;
		float active;
	};

	// Initialization function
	void initShader(const wchar_t* cs, const wchar_t* ps);

	// Defines new Buffer pointers and a Sampler pointer
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* activeBuffer;
};
#pragma once
