// Depth Tessellation Shader follows similar steps to Tessellation Shader, but generates depth values from the mesh instead of calculating lighting and normals
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthTessellationShader : public BaseShader
{

public:

	DepthTessellationShader(ID3D11Device* device, HWND hwnd);
	~DepthTessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMap, int tessFactor);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tessBuffer;
	ID3D11SamplerState* sampleState;

	// Stores the inside and outside factor, which determines how the quad is sliced
	struct TessBufferType
	{
		float insideFactor;
		float outsideFactor;
		XMFLOAT2 padding;
	};
};

