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
	terrainPlane = new TessellatedQuad(renderer->getDevice(), renderer->getDeviceContext(), 100);
	surfacePlane = new TessellatedQuad(renderer->getDevice(), renderer->getDeviceContext(), 100);
	fishMesh = new PointCubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/teapot.obj");

	textureMgr->loadTexture("brick", L"res/brick1.dds");
	textureMgr->loadTexture("terrainHeight", L"res/terrainHeight.png");
	textureMgr->loadTexture("waveHeight", L"res/waveHeight.png");
	textureMgr->loadTexture("terrainNormal", L"res/terrainNormal.png");
	textureMgr->loadTexture("waveNormal", L"res/waveNormal.png");
	textureMgr->loadTexture("water", L"res/blue2.png");

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	waterShader = new UnderwaterShader(renderer->getDevice(), hwnd);
	heightShader = new HeightShader(renderer->getDevice(), hwnd);
	depthHeightShader = new DepthHeightShader(renderer->getDevice(), hwnd);
	surfaceShader = new SurfaceShader(renderer->getDevice(), hwnd);
	billboardShader = new BillboardShader(renderer->getDevice(), hwnd);

	ortho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	ortho2 = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, -screenWidth/2.7f, screenHeight/2.7f);

	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;
	int sceneWidth = 200;
	int sceneHeight = 200;

	modelRot = 0;

	// This is your shadow map
	shadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap2 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	playerDepthMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);

	waterTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	 
	dLights = new Light[DIRCOUNT]();

	dLights[0].setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	dLights[0].setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	dLights[0].setDirection(-1.0f, -1.0f, 0.0f);
	dLights[0].setPosition(0.0f, 40.0f, 0.0f);
	dLights[0].generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	dLights[1].setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	dLights[1].setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);// shadows on hills?
	dLights[1].setDirection(1.0f, -1.0f, 0.0f);
	dLights[1].setPosition(0.0f , 40.0f, 0.0f);
	dLights[1].generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	lightDir0 = new float[3]{ 1.0f, -1.0f, 0.0f };
	lightDir1 = new float[3]{ -1.0f, -1.0f, 0.0f };

	terrainTess = 1;
	waterTess = 1;

	wave = new float[3];
	wave[0] = 0; //Time
	wave[1] = 5.0f; //Speed
	wave[2] = 5.0f; //Height
	wave[3] = 1.0f; //frequency

	noWave = new float[2];
	noWave[0] = 0;
	noWave[1] = 0;
	noWave[2] = 40;
	noWave[3] = 0;

	fog = new float[1];
	fog[0] = 0;
	fog[1] = 100;
	screenW = screenWidth;
	screenH = screenHeight;

	waterPos = { -50.0f, -10.0f, 0.0f };
	isUnderwater = false;

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
	currentTime += timer->getTime();
	wave[0] = currentTime;
	dLights[0].setDirection(lightDir0[0], lightDir0[1], lightDir0[2]);
	dLights[1].setDirection(lightDir1[0], lightDir1[1], lightDir1[2]);

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

	worldMatrix = positionFloor();
	// Render floor
	terrainPlane->sendData(renderer->getDeviceContext());
	depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix0, lightProjectionMatrix0, textureMgr->getTexture("terrainHeight"), noWave, terrainTess);
	depthHeightShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());

	//worldMatrix = positionSurface();
	//
	//surfacePlane->sendData(renderer->getDeviceContext());
	//depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix0, lightProjectionMatrix0, textureMgr->getTexture("waterHeight"), wave, waterTess);
	//depthHeightShader->render(renderer->getDeviceContext(), surfacePlane->getIndexCount());
	
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

	worldMatrix = positionFloor();
	// Render floor
	terrainPlane->sendData(renderer->getDeviceContext());

	depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1, textureMgr->getTexture("terrainHeight"), noWave, terrainTess);
	depthHeightShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());

	//worldMatrix = positionSurface();
	
	//surfacePlane->sendData(renderer->getDeviceContext());
	//
	//depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1, textureMgr->getTexture("waterHeight"), wave, waterTess);
	//depthHeightShader->render(renderer->getDeviceContext(), surfacePlane->getIndexCount());

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
	playerDepthMap->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = positionFloor();
	// Render floor
	terrainPlane->sendData(renderer->getDeviceContext());

	depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("terrainHeight"), noWave, terrainTess);
	depthHeightShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());

	worldMatrix = positionSurface();
	
	surfacePlane->sendData(renderer->getDeviceContext());
	
	depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("waterHeight"), wave, waterTess);
	depthHeightShader->render(renderer->getDeviceContext(), surfacePlane->getIndexCount());

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

	//renderer->setBackBufferRenderTarget();
	//renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	camera->update();
	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = positionFloor();
	// Render floor
	terrainPlane->sendData(renderer->getDeviceContext());
	
	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("brick"), textureMgr->getTexture("terrainHeight"), textureMgr->getTexture("terrainNormal"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), dLights, noWave, terrainTess);
	heightShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());
	
	worldMatrix = positionSurface();
	// Render surface
	surfacePlane->sendData(renderer->getDeviceContext());
	
	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("water"), textureMgr->getTexture("waveHeight"), textureMgr->getTexture("waveNormal"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), dLights, wave, waterTess);
	heightShader->render(renderer->getDeviceContext(), surfacePlane->getIndexCount());
	
	worldMatrix = positionModel();
	model->sendData(renderer->getDeviceContext());

	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("brick"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), dLights);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());

	worldMatrix = positionFish();
	fishMesh->sendData(renderer->getDeviceContext());

	billboardShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"));
	billboardShader->render(renderer->getDeviceContext(), fishMesh->getIndexCount());
	
	renderer->setBackBufferRenderTarget();
}

void App1::finalPass2()
{

	//Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	
	
	//RENDER THE RENDER TEXTURE SCENE
	//Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	ortho->sendData(renderer->getDeviceContext());
	if (checkUnderwater())
	{
		//waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, waterTexture->getShaderResourceView(), currentTime);
		//waterShader->render(renderer->getDeviceContext(), ortho->getIndexCount());

		waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, waterTexture->getShaderResourceView(), currentTime);
		waterShader->render(renderer->getDeviceContext(), ortho->getIndexCount());

	}

	else
	{
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, waterTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), ortho->getIndexCount());
	}
	ortho2->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, playerDepthMap->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), ortho2->getIndexCount());
	
	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}

XMMATRIX App1::positionFloor()
{
	// position floor
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-50.0f, -30.0f, 0.0f);
	//XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//XMMATRIX rotMatrix = XMMatrixRotationX(modelRot);
	//worldMatrix = XMMatrixMultiply(rotMatrix, worldMatrix);

	return worldMatrix;
}

XMMATRIX App1::positionSurface()
{
	// position water surface
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(waterPos.x, waterPos.y, waterPos.z);
	//XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//XMMATRIX rotMatrix = XMMatrixRotationX(modelRot);
	//worldMatrix = XMMatrixMultiply(rotMatrix, worldMatrix);

	return worldMatrix;
}

XMMATRIX App1::positionModel()
{
	// position model
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, 7.0f, 5.0f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	XMMATRIX rotMatrix = XMMatrixRotationX(modelRot);
	worldMatrix = XMMatrixMultiply(rotMatrix, worldMatrix);

	return worldMatrix;
}

XMMATRIX App1::positionFish()
{
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, -20.0f, 0.0f);

	return worldMatrix;
}

bool App1::checkUnderwater()
{
	if (camera->getPosition().y < waterPos.y + wave[2] / 2)
	{
		return true;
	}

	return false;
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

	ImGui::SliderFloat("TesselationW", &waterTess, 1.0f, 32.0f);
	ImGui::SliderFloat("TesselationT", &terrainTess, 1.0f, 32.0f);

	ImGui::SliderFloat("LightDir0X", &lightDir0[0], -1, 1);
	//Dont let it do the zero thing
	if (lightDir0[0] == 0)
	{
		lightDir0[0] = 0.0001f;
	}
	ImGui::SliderFloat("LightDir0Y", &lightDir0[1], -1, 1);
	ImGui::SliderFloat("LightDir0Z", &lightDir0[2], -1, 1);


	ImGui::SliderFloat("LightDir1X", &lightDir1[0], -1, 1);
	//Dont let it do the zero thing
	if (lightDir1[0] == 0)
	{
		lightDir1[0] = 0.0001f;
	}
	ImGui::SliderFloat("LightDir1Y", &lightDir1[1], -1, 1);
	ImGui::SliderFloat("LightDir1Z", &lightDir1[2], -1, 1);

	
	dLights[0].setPosition(-lightDir0[0] * 40, -lightDir0[1] * 40, -lightDir0[2] * 40);
	dLights[1].setPosition(-lightDir1[0] * 40, -lightDir1[1] * 40, -lightDir1[2] * 40);

	ImGui::SliderFloat("Teapot rotation", &modelRot, 0, PIPI);
	ImGui::SliderFloat("Wave Speed", &wave[1], 0, 10);
	ImGui::SliderFloat("Wave Height", &wave[2], 0, 100);
	ImGui::SliderFloat("Wave Frequency", &wave[3], 0, 2);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

