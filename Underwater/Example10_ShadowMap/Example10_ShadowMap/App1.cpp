// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

#define PIPI 6.28318530718

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/teapot.obj");
	textureMgr->loadTexture("brick", L"res/brick1.dds");
	textureMgr->loadTexture("height", L"res/height.png");

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	waterShader = new UnderwaterShader(renderer->getDevice(), hwnd);
	heightShader = new HeightShader(renderer->getDevice(), hwnd);

	ortho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	ortho2 = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, 0, 0);

	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;
	int sceneWidth = 100;
	int sceneHeight = 100;

	modelRot = 0;

	// This is your shadow map
	shadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap2 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	playerDepthMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);

	waterTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	dLights = new Light[DIRCOUNT]();

	dLights[0].setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	dLights[0].setDiffuseColour(1.0f, 1.0f, 0.0f, 1.0f);
	dLights[0].setDirection(1.0f, -1.0f, 0.0f);
	dLights[0].setPosition(-20.0f, 20.0f, 0.0f);
	dLights[0].generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	dLights[1].setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	dLights[1].setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	dLights[1].setDirection(-1.0f, -1.0f, 0.0f);
	dLights[1].setPosition(20.0f, 20.0f, 0.0f);
	dLights[1].generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	fog = new float[2];
	fog[0] = 0;
	fog[1] = 100;
	screenW = screenWidth;
	screenH = screenHeight;

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

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

	// Perform depth pass
	depthPass1();
	depthPass2();
	depthPass3();
	// Render scene
	finalPass();
	finalPass2();

	return true;
}

void App1::depthPass1()
{
	// Set the render target to be the render to texture.
	shadowMap->setRenderTarget(renderer->getDeviceContext());
	shadowMap->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	dLights[0].generateViewMatrix();
	XMMATRIX lightViewMatrix0 = dLights[0].getViewMatrix();
	XMMATRIX lightProjectionMatrix0 = dLights[0].getOrthoMatrix();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -20.0f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix0, lightProjectionMatrix0);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = positionModel();
	model->sendData(renderer->getDeviceContext());

	//Get depth for 1st light
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix0, lightProjectionMatrix0);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::depthPass2()
{
	// Set the render target to be the render to texture.
	shadowMap2->setRenderTarget(renderer->getDeviceContext());
	shadowMap2->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	dLights[1].generateViewMatrix();
	XMMATRIX lightViewMatrix1 = dLights[1].getViewMatrix();
	XMMATRIX lightProjectionMatrix1 = dLights[1].getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -20.0f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());

	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = positionModel();
	model->sendData(renderer->getDeviceContext());

	//Get depth for light

	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::depthPass3()
{
	// Set the render target to be the render to texture.
	playerDepthMap->setRenderTarget(renderer->getDeviceContext());
	playerDepthMap->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -20.0f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());

	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = positionModel();
	model->sendData(renderer->getDeviceContext());

	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


void App1::finalPass()
{
	// Set the render target to be the render to texture and clear it
	waterTexture->setRenderTarget(renderer->getDeviceContext());
	waterTexture->clearRenderTarget(renderer->getDeviceContext(), 1.39f, 0.58f, 0.92f, 1.0f);

	camera->update();
	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -20.0f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());

	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("height"));
	heightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, 
		textureMgr->getTexture("brick"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), dLights);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = positionModel();
	model->sendData(renderer->getDeviceContext());

	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("brick"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), dLights);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());
	
	renderer->setBackBufferRenderTarget();
}

void App1::finalPass2()
{

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	currentTime += timer->getTime();


	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	ortho->sendData(renderer->getDeviceContext());
	waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, waterTexture->getShaderResourceView(), currentTime);
	waterShader->render(renderer->getDeviceContext(), ortho->getIndexCount());

	ortho2->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, playerDepthMap->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), ortho->getIndexCount());

	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}

XMMATRIX App1::positionModel()
{
	// Render model
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	XMMATRIX rotMatrix = XMMatrixRotationX(modelRot);
	worldMatrix = XMMatrixMultiply(rotMatrix, worldMatrix);

	return worldMatrix;
}




void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	ImGui::SliderFloat("Teapot rotation", &modelRot, 0, PIPI);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

