// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Shader objects
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);
	depthTessellationShader = new DepthTessellationShader(renderer->getDevice(), hwnd);
	basicShader = new BasicShader(renderer->getDevice(), hwnd);
	combinedBlurShader = new CombinedBlurShader(renderer->getDevice(), hwnd);
	depthOfFieldShader = new DepthOfFieldShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);

	// Create Mesh objects
	TplaneMesh = new TPlane(renderer->getDevice(), renderer->getDeviceContext(), 100);
	pointlightMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 20);
	spotlightMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 20);

	// Create empty shadow maps with high resolution
	shadowMap[0] = new ShadowMap(renderer->getDevice(), 8192, 8192);
	shadowMap[1] = new ShadowMap(renderer->getDevice(), 8192, 8192);

	// Create new render textures with same size as the screen
	screenTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	blurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	depthTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Create new ortho mesh to display the screen
	screenOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);

	// Load textures to the texture manager
	textureMgr->loadTexture(L"heightMap", L"res/height.png");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	// Initialize Lights
	initLight(screenWidth, screenHeight);

	// Initialize Mesh
	cube1 = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext(), 20);
}

void App1::initLight(float sceneWidth, float sceneHeight)
{
	// Configure Directional Light
	lightArray[0] = new Light();
	lightArray[0]->setAmbientColour(lightAmb1[0], lightAmb1[1], lightAmb1[2], lightAmb1[3]);
	lightArray[0]->setDiffuseColour(lightDif1[0], lightDif1[1], lightDif1[2], lightDif1[3]);
	lightArray[0]->setDirection(lightDir1[0], lightDir1[1], lightDir1[2]);
	lightArray[0]->setPosition(0, 0, 0);
	lightArray[0]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 150.0f);
	activeLight[0] = true;

	// Configure Point Light
	lightArray[1] = new Light();
	lightArray[1]->setAmbientColour(lightAmb2[0], lightAmb2[1], lightAmb2[2], lightAmb2[3]);
	lightArray[1]->setDiffuseColour(lightDif2[0], lightDif2[1], lightDif2[2], lightDif2[3]);
	lightArray[1]->setDirection(0, -1, 0);
	lightArray[1]->setPosition(lightPos2[0], lightPos2[1], lightPos2[2]);
	activeLight[1] = true;

	// Configure Spot Light
	lightArray[2] = new Light();
	lightArray[2]->setAmbientColour(lightAmb3[0], lightAmb3[1], lightAmb3[2], lightAmb3[3]);
	lightArray[2]->setDiffuseColour(lightDif3[0], lightDif3[1], lightDif3[2], lightDif3[3]);
	lightArray[2]->setDirection(lightDir3[0], lightDir3[1], lightDir3[2]);
	lightArray[2]->setPosition(lightPos3[0], lightPos3[1], lightPos3[2]);
	activeLight[2] = true;
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Delete the shader pointers, to prevent memory leak
	if (tessellationShader)
	{
		delete tessellationShader;
		tessellationShader = 0;
	}
	if (depthTessellationShader)
	{
		delete depthTessellationShader;
		depthTessellationShader = 0;
	}
	if (basicShader)
	{
		delete basicShader;
		basicShader = 0;
	}
	if (combinedBlurShader)
	{
		delete combinedBlurShader;
		combinedBlurShader = 0;
	}
	if (depthOfFieldShader)
	{
		delete depthOfFieldShader;
		depthOfFieldShader = 0;
	}
	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}

	// Delete the mesh pointers, to prevent memory leak
	if (TplaneMesh)
	{
		delete TplaneMesh;
		TplaneMesh = 0;
	}
	if (pointlightMesh)
	{
		delete pointlightMesh;
		pointlightMesh = 0;
	}
	if (spotlightMesh)
	{
		delete spotlightMesh;
		spotlightMesh = 0;
	}
	if (cube1)
	{
		delete cube1;
		cube1 = 0;
	}

	// Delete screen textures and depth map pointers, to prevent memory leak
	if (shadowMap[0])
	{
		delete shadowMap[0];
		shadowMap[0] = 0;
	}
	if (shadowMap[1])
	{
		delete shadowMap[1];
		shadowMap[1] = 0;
	}
	if (screenTexture)
	{
		delete screenTexture;
		screenTexture = 0;
	}
	if (blurTexture)
	{
		delete blurTexture;
		blurTexture = 0;
	}
	if (depthTexture)
	{
		delete depthTexture;
		depthTexture = 0;
	}
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	// Depth pass for Directional Light
	depthPass1();

	// Depth pass for Spot Light
	depthPass2();

	// Depth pass for Camera
	cameraDepthPass();

	// Render pass to screen texture
	screenPass();

	// Blur pass
	blurPass();

	// Depth of Field pass and render to screen
	finalPass();

	return true;
}

void App1::depthPass1()
{
	// Empties the shadow map and prepares it for use
	shadowMap[0]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// Generates a view matrix from the light's perspective
	lightArray[0]->generateViewMatrix();

	// Offsets the direction light depending on it's direction, so the shadow map covers the screen
	XMFLOAT3 lightOffset = lightArray[0]->getDirection();
	lightOffset = XMFLOAT3(-lightOffset.x * 50, -lightOffset.y * 50, -lightOffset.z * 50);
	lightArray[0]->setPosition(lightOffset.x + 50.0f, lightOffset.y, lightOffset.z + 50.0f);

	// Generates an ortho and view matrix from the Direction Light, and gets the world matrix
	XMMATRIX lightViewMatrix = lightArray[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lightArray[0]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX translate = XMMatrixIdentity();

	// Sends the data to the Depth Tessellation Shader and returns a depth value
	TplaneMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	depthTessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"), tessFactor);
	depthTessellationShader->render(renderer->getDeviceContext(), TplaneMesh->getIndexCount());

	// Moves to the cube mesh's position
	translate *= XMMatrixTranslation(cubePos[0], cubePos[1], cubePos[2]);
	worldMatrix = worldMatrix * translate;

	// Sends the data to the Depth Shader and returns a depth value
	cube1->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cube1->getIndexCount());

	// Resets the viewport and stops writing to the Shadow Map
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::depthPass2()
{
	// Empties the shadow map and prepares it for use
	shadowMap[1]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// Generates a view matrix from the light's perspective
	lightArray[2]->generateViewMatrix();

	// Generates a projection matrix from the light's perspective, using the SCREEN_NEAR and SCREEN_FAR values 
	lightArray[2]->generateProjectionMatrix(0.1f, 200.0f);

	// Generates an projection and view matrix from the Spot Light, and gets the world matrix
	XMMATRIX lightViewMatrix = lightArray[2]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lightArray[2]->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX translate = XMMatrixIdentity();

	// Sends the plane data to the Depth Tessellation Shader and returns a depth value
	TplaneMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	depthTessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"), tessFactor);
	depthTessellationShader->render(renderer->getDeviceContext(), TplaneMesh->getIndexCount());

	// Moves to the cube mesh's position
	translate *= XMMatrixTranslation(cubePos[0], cubePos[1], cubePos[2]);
	worldMatrix = worldMatrix * translate;

	// Sends the data to the Depth Shader and returns a depth value
	cube1->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cube1->getIndexCount());

	// Resets the viewport and stops writing to the Shadow Map
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::cameraDepthPass()
{
	// Empties the depth texture and sets it as render target
	depthTexture->setRenderTarget(renderer->getDeviceContext());
	depthTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 0.0f);
	
	// Generates a view matrix from the camera's perspective, as well as a projection and world matrix from the renderer
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX translate = XMMatrixIdentity();

	// Sends the plane data to the Depth Tessellation Shader and returns a depth value
	TplaneMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	depthTessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heightMap"), tessFactor);
	depthTessellationShader->render(renderer->getDeviceContext(), TplaneMesh->getIndexCount());

	// Moves to the cube mesh's position
	translate *= XMMatrixTranslation(cubePos[0], cubePos[1], cubePos[2]);
	worldMatrix = worldMatrix * translate;

	// Sends the data to the Depth Shader and returns a depth value
	cube1->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cube1->getIndexCount());

	// Resets the viewport and stops writing to the Shadow Map
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::screenPass()
{
	// Empties the screen texture and sets it as render target
	screenTexture->setRenderTarget(renderer->getDeviceContext());
	screenTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Generates a view matrix from the camera's perspective, as well as a projection and world matrix from the renderer
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translate;
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Sends the plane data to the Tessellation Shader, which tessellates the height map and appropriately calculates lighting and shadows
	TplaneMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heightMap"), shadowMap[0]->getDepthMapSRV(), shadowMap[1]->getDepthMapSRV(), tessFactor, lightArray, activeLight, dropoff2, pixelNormals, specIntensity, specExponent, camera, cutOffAngle);
	tessellationShader->render(renderer->getDeviceContext(), TplaneMesh->getIndexCount());

	// Place the point light mesh at the Point Light's Position
	translate = XMMatrixIdentity();
	translate *= XMMatrixTranslation(lightPos2[0], lightPos2[1], lightPos2[2]);
	worldMatrix = worldMatrix * translate;

	// Only render the point light if it's active
	if (activeLight[1])
	{
		pointlightMesh->sendData(renderer->getDeviceContext());
		basicShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap[0]->getDepthMapSRV(), shadowMap[1]->getDepthMapSRV(), lightArray, activeLight, dropoff2, pixelNormals, specIntensity, specExponent, camera, cutOffAngle);
		basicShader->render(renderer->getDeviceContext(), pointlightMesh->getIndexCount());
	}

	// Translate back to original coordinates
	translate = XMMatrixIdentity();
	translate *= XMMatrixTranslation(-lightPos2[0], -lightPos2[1], -lightPos2[2]);
	worldMatrix = worldMatrix * translate;

	// Place the spot light mesh at the Spot Light's Position
	translate = XMMatrixIdentity();
	translate *= XMMatrixTranslation(lightPos3[0], lightPos3[1], lightPos3[2]);
	worldMatrix = worldMatrix * translate;

	// Only render the spot light if it's active
	if (activeLight[2])
	{
		spotlightMesh->sendData(renderer->getDeviceContext());
		basicShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap[0]->getDepthMapSRV(), shadowMap[1]->getDepthMapSRV(), lightArray, activeLight, dropoff2, pixelNormals, specIntensity, specExponent, camera, cutOffAngle);
		basicShader->render(renderer->getDeviceContext(), spotlightMesh->getIndexCount());
	}

	// Translate back to original coordinates
	translate = XMMatrixIdentity();
	translate *= XMMatrixTranslation(-lightPos3[0], -lightPos3[1], -lightPos3[2]);
	worldMatrix = worldMatrix * translate;

	// Translate to cube's position
	translate = XMMatrixIdentity();
	translate *= XMMatrixTranslation(cubePos[0], cubePos[1], cubePos[2]);
	worldMatrix = worldMatrix * translate;

	// Sends the data to the Basic Shader and calculates lighting/shadows
	cube1->sendData(renderer->getDeviceContext());
	basicShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap[0]->getDepthMapSRV(), shadowMap[1]->getDepthMapSRV(), lightArray, activeLight, dropoff2, pixelNormals, specIntensity, specExponent, camera, cutOffAngle);
	basicShader->render(renderer->getDeviceContext(), cube1->getIndexCount());

	// Resets the viewport and stops writing to the Shadow Map
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::blurPass()
{
	// Empties the blur texture and sets it as the render target
	blurTexture->setRenderTarget(renderer->getDeviceContext());
	blurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Gets the screen's size based on the blurTexture's height and width
	float screenSizeX = (float)blurTexture->getTextureWidth();
	float screenSizeY = (float)blurTexture->getTextureHeight();

	// Generates a view matrix from the camera's perspective, as well as an ortho matrix of the blur texture, and world matrix from the renderer
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();

	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = blurTexture->getOrthoMatrix();

	// Sends the screen texture to be blurred with the depth buffer turned off
	renderer->setZBuffer(false);
	screenOrthoMesh->sendData(renderer->getDeviceContext());
	combinedBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, screenTexture->getShaderResourceView(), screenSizeX, screenSizeY);
	combinedBlurShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::finalPass()
{
	// Begins rendering the scene
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Gets the World, Orth and Projection matrix from the renderer, and generates and Orhto and View matrix from the camera's perspective
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Renders the heightmap to expose it to wireframe mode
	if (wireframeToggle)
	{
		TplaneMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heightMap"), shadowMap[0]->getDepthMapSRV(), shadowMap[1]->getDepthMapSRV(), tessFactor, lightArray, activeLight, dropoff2, pixelNormals, specIntensity, specExponent, camera, cutOffAngle);
		tessellationShader->render(renderer->getDeviceContext(), TplaneMesh->getIndexCount());
	}

	// Renders the screen orthomesh to the screen, ignoring the z buffer
	// and uses the Post Processing technique Depth of Field to lerp between the original and blurred texture
	renderer->setZBuffer(false);
	screenOrthoMesh->sendData(renderer->getDeviceContext());
	depthOfFieldShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, screenTexture->getShaderResourceView(), blurTexture->getShaderResourceView(), depthTexture->getShaderResourceView(), weighting, cutoff, percentage, activeDOF);
	depthOfFieldShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Move the camera based on user input
	camera->update();

	gui();

	// Ends rendering the scene
	renderer->endScene();
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Information too small to put in a Collapsing Header
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::DragInt("Tessellation", &tessFactor, 1, 1, 64);
	ImGui::Checkbox("Bump Mapping", &pixelNormals);
	ImGui::DragFloat3("CubePos", cubePos, 1.0f, 0.0f, 1000.0f);

	// Directional light UI attributes
	if (ImGui::CollapsingHeader("Directional Light 1"))
	{
		ImGui::Checkbox("Activate Directional Light", &activeLight[0]);
		ImGui::DragFloat3("D. Light Direction", lightDir1, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat4("D. Diffuse", lightDif1, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat4("D. Ambient", lightAmb1, 0.01f, 0.01f, 1.0f);
	}
	lightArray[0]->setDirection(lightDir1[0], lightDir1[1], lightDir1[2]);
	lightArray[0]->setAmbientColour(lightAmb1[0], lightAmb1[1], lightAmb1[2], lightAmb1[3]);
	lightArray[0]->setDiffuseColour(lightDif1[0], lightDif1[1], lightDif1[2], lightDif1[3]);

	// Point Light UI attributes
	if (ImGui::CollapsingHeader("Point Light"))
	{
		ImGui::Checkbox("Activate Point Light", &activeLight[1]);
		ImGui::DragFloat3("P. Position", lightPos2, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat4("P. Diffuse", lightDif2, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat4("P. Ambient", lightAmb2, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("P. Attenuation", &dropoff2, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("P. Spec Intensity", &specIntensity, 0.01f, 0.0f, 5.0f);
		ImGui::DragFloat("P. Spec Exponent", &specExponent, 0.01f, 0.0f, 5.0f);
	}
	lightArray[1]->setPosition(lightPos2[0], lightPos2[1], lightPos2[2]);
	lightArray[1]->setAmbientColour(lightAmb2[0], lightAmb2[1], lightAmb2[2], lightAmb2[3]);
	lightArray[1]->setDiffuseColour(lightDif2[0], lightDif2[1], lightDif2[2], lightDif2[3]);

	// Spot Light UI attributes
	if (ImGui::CollapsingHeader("Spot Light"))
	{
		ImGui::Checkbox("Activate Spot Light", &activeLight[2]);
		ImGui::DragFloat3("S. Light Direction", lightDir3, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat3("S. Light Position", lightPos3, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat4("S. Diffuse", lightDif3, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat4("S. Ambient", lightAmb3, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("S. Cutoff", &cutOffAngle, 1.0f, 0.0f, 360.0f);
	}
	lightArray[2]->setDirection(lightDir3[0], lightDir3[1], lightDir3[2]);
	lightArray[2]->setPosition(lightPos3[0], lightPos3[1], lightPos3[2]);
	lightArray[2]->setAmbientColour(lightAmb3[0], lightAmb3[1], lightAmb3[2], lightAmb3[3]);
	lightArray[2]->setDiffuseColour(lightDif3[0], lightDif3[1], lightDif3[2], lightDif3[3]);

	// Depth Of Field UI attributes
	if (ImGui::CollapsingHeader("Depth Of Field"))
	{
		ImGui::Checkbox("Activate Depth Of Field", &activeDOF);
		ImGui::DragFloat("Weighting", &weighting, 0.1f, 0.0f, 15.0f);
		ImGui::DragFloat("Cutoff", &cutoff, 0.01f, 0.0f, 1.0f);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

