// Combines a single horizontal and vertical blur, using gaussian blur technique
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class CombinedBlurShader : public BaseShader
{
private:

	// Stores the screen's height and width
	struct ScreenSizeBufferType
	{
		float screenWidth;
		float screenHeight;
		XMFLOAT2 padding;
	};

public:

	CombinedBlurShader(ID3D11Device* device, HWND hwnd);
	~CombinedBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float width, float height);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
};