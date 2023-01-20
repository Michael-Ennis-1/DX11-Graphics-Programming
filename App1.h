// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "TessellationShader.h"
#include "Tplane.h"
#include "DepthTessellationShader.h"
#include "BasicShader.h"
#include "CombinedBlurShader.h"
#include "DepthOfFieldShader.h"
#include "DepthShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	// Initializes all of the lights
	void initLight(float sceneWidth, float sceneHeight);

	bool frame();

protected:
	// Calculates depth from the Directional Light's Viewpoint
	void depthPass1();

	// Calculates depth from the Spot Light's Viewpoint
	void depthPass2();

	// Calculates depth from the Camera's Viewpoint
	void cameraDepthPass();

	// Renders the screen to a texture for use in Post Processing
	void screenPass();

	// Blurs the screen texture
	void blurPass();

	// Passes through Depth Of Field shader and determines final screen texture to render
	void finalPass();

	bool render();
	void gui();

private:
	// Tessellation Shader and Mesh
	TessellationShader* tessellationShader;
	TPlane* TplaneMesh;

	// Simple Depth Shader, Tessellation Shader and shadowmaps for both Spot Light and Directional Light
	DepthShader* depthShader;
	DepthTessellationShader* depthTessellationShader;
	ShadowMap* shadowMap[2];

	// DepthOfField and Combined Blur shaders used for Post Processing
	DepthOfFieldShader* depthOfFieldShader;
	CombinedBlurShader* combinedBlurShader;

	// Variables used to affect the Depth Of Field post process
	// Weighting multiplies the lerp value, cutoff decides how far percentage wise a pixel's depth must be before it's completely blurred
	bool activeDOF = true;
	float weighting = 1.0f;
	float cutoff = 0.15f;
	float percentage = 0.001f;

	// Render Textures used for camera depth, screen texture and screen blur
	RenderTexture* depthTexture;
	RenderTexture* screenTexture;
	RenderTexture* blurTexture;

	// Orthomesh used for showing post process to screen
	OrthoMesh* screenOrthoMesh;

	// Meshes to show the positions of point light and spot light, and Simple Shader to show them
	BasicShader* basicShader;
	SphereMesh* pointlightMesh;
	SphereMesh* spotlightMesh;

	// Light array and specific values setup in arrays, so that the values can be changed easily
	Light* lightArray[3];
	
	// Directional Light initial values
	float lightDir1[3] = { 1.0f, -0.7f, 0.0f };
	float lightDif1[4] = { 0.5f, 0.5f, 0.5f, 0.7f };
	float lightAmb1[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	// Point Light initial values
	float lightPos2[3] = { 64, 18, 68 };
	float lightDif2[4] = { 0.8f, 0.0f, 0.8f, 0.7f };
	float lightAmb2[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float dropoff2 = 0.05f;
	float specIntensity = 0.15f;
	float specExponent = 1;

	// Spot Light initial values
	float lightDir3[3] = { 0.0f, -0.6f, -1.0f };
	float lightPos3[3] = { 41, 30, 72 };
	float lightDif3[4] = { 0.8f, 0.8f, 0.0f, 1.0f };
	float lightAmb3[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float cutOffAngle = 60.0f;

	// Boolean array passed to the pixel shader allows for specific lights to be turned on or off
	bool activeLight[3];

	// Decides the inside and outside factor when tessellating, decides whether to use bumpmap or vertex normals
	int tessFactor = 10;
	bool pixelNormals = true;

	// Mesh and it's position
	CubeMesh* cube1;
	float cubePos[3] = { 37, 18, 46 };
};

#endif