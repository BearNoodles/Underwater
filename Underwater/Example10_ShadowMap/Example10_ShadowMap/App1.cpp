// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

#define PI 3.14159265359

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create appropriate meshes for each object
	terrainPlane = new TessellatedQuad(renderer->getDevice(), renderer->getDeviceContext(), 100);
	surfacePlane = new TessellatedQuad(renderer->getDevice(), renderer->getDeviceContext(), 100);
	fishMesh = new PointCubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/teapot.obj");

	//Load Textures
	textureMgr->loadTexture("brick", L"res/rock.jpg");
	textureMgr->loadTexture("terrainHeight", L"res/terrainHeight.png");
	textureMgr->loadTexture("waveHeight", L"res/waveHeight.png");
	textureMgr->loadTexture("terrainNormal", L"res/terrainNormal.png");
	textureMgr->loadTexture("waveNormal", L"res/waveNormal.png");
	textureMgr->loadTexture("water", L"res/blue.png");
	textureMgr->loadTexture("fish", L"res/jelly.png");


	//Initialise shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	waterShader = new UnderwaterShader(renderer->getDevice(), hwnd);
	heightShader = new HeightShader(renderer->getDevice(), hwnd);
	depthHeightShader = new DepthHeightShader(renderer->getDevice(), hwnd);
	//surfaceShader = new SurfaceShader(renderer->getDevice(), hwnd);
	billboardShader = new BillboardShader(renderer->getDevice(), hwnd);
	depthFishShader = new DepthFishShader(renderer->getDevice(), hwnd);

	//Initialise ortho mesh
	ortho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);

	//High value for the shadow map size to give detailed shadows
	int shadowmapWidth = 8192;
	int shadowmapHeight = 8192;
	int sceneWidth = 150;
	int sceneHeight = 150;


	//Shadow maps
	shadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap2 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap3 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);

	//Texture to be rendered to the ortho mesh
	waterTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);


	//Initialise arrarys of each light type (directional and point)
	dLights = new Light[DIRCOUNT]();
	pLights = new Light[POINTCOUNT]();

	//Red directional light pointing down and to the right
	dLights[0].setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	dLights[0].setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	dLights[0].setDirection( 1.0f, -0.5f, 0.0f );
	dLights[0].setPosition(0.0f, 40.0f, 50.0f);
	dLights[0].generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	//Blue directional light pointing down and to the left
	dLights[1].setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	dLights[1].setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	dLights[1].setDirection(-1.0f, -0.5f, 0.0f);
	dLights[1].setPosition(0.0f, 40.0f, 50.0f);
	dLights[1].generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	//White point light positioned at the spout of the teapot model.  Switch off as default as it doesnt cast shadows
	pLights[0].setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	pLights[0].setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	pLights[0].setDirection(0.0001f, -1.0f, 1.0f);
	pLights[0].setPosition(30.0f, 30.0f, 50.0f);
	pLights[0].generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	//Variables so light directions can be changed (position for point light)
	lightDir0 = new float[3]{ 1.0f, -0.5f, 0.0f };
	lightDir1 = new float[3]{ 0.0f, -0.5f, -1.0f };
	pLightPos = new float[3]{ 30.0f, 30.0f, 50.0f };

	//Variable for light colours
	lightColour0 = new float[4]{ 1.0f, 0.0f, 0.0f, 1.0f };
	lightColour1 = new float[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
	pLightColour = new float[4]{ 1.0f, 1.0f, 1.0f, 1.0f };

	//Black and white light colours to set the point light as
	blackColour = new float[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
	whiteColour = new float[4]{ 1.0f, 1.0f, 1.0f, 1.0f };

	//point light is off by default
	pLightOn = false;

	//Tessellation starts as 1 (no tesselation) and can be increased using the imgui menu
	terrainTess = 10;
	waterTess = 10;

	//Floats to controls the waves passed into shaders with height mapping

	//Water wave
	wave = new float[3];
	wave[0] = 0; //Time
	wave[1] = 5.0f; //Speed
	wave[2] = 5.0f; //Height
	wave[3] = 0.4f; //frequency

	//Terrain wave
	//terrain only needs height value
	noWave = new float[3];
	noWave[0] = 0; //Time 
	noWave[1] = 0; //Speed
	noWave[2] = 40; //Height
	noWave[3] = 0; //Frequency

	//Start positions for each object
	fishPos = {-50.0f, -30.0f, 0.0f};
	floorPos = { -50.0f, -30.0f, 0.0f };
	waterPos = { -50.0f, -10.0f, 0.0f };
	modelPos = { -30.0f, 10.0f, 105.0f };
	
	//Position camera to better starting position
	camera->setPosition(camera->getPosition().x, camera->getPosition().y + 10, camera->getPosition().z);
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

	// Perform depth passes to create shadow maps
	depthPass1();
	depthPass2();

	//Only do depth pass for point light if it is switched on
	if (pLightOn)
	{
		depthPass3();
	}

	// Render scene to render texture
	WaterTexturePass();

	//Apply post processing effect to render texture
	PostProcessingPass();

	return true;
}

void App1::depthPass1()
{
	// Set the render target to be the first shadowmap render to texture.
	shadowMap->setRenderTarget(renderer->getDeviceContext());
	shadowMap->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the 1st directional light
	dLights[0].generateViewMatrix();
	XMMATRIX lightViewMatrix0 = dLights[0].getViewMatrix();
	XMMATRIX lightProjectionMatrix0 = dLights[0].getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	//Tranform world matrix to position the terrain
	worldMatrix = positionFloor();

	// Render floor depth for 1st directional light
	terrainPlane->sendData(renderer->getDeviceContext());
	depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix0, lightProjectionMatrix0, textureMgr->getTexture("terrainHeight"), noWave, terrainTess);
	depthHeightShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());

	//Tranform world matrix to position the teapot model
	worldMatrix = positionModel();

	// Render model depth for 1st directional light
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix0, lightProjectionMatrix0);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());


	//Tranform world matrix to position the fish
	worldMatrix = positionFish();

	// Render fish mesh depth for 1st directional light
	fishMesh->sendData(renderer->getDeviceContext());
	depthFishShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix0, lightProjectionMatrix0);
	depthFishShader->render(renderer->getDeviceContext(), fishMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::depthPass2()
{
	// Set the render target to be the second shadowmap render to texture.
	shadowMap2->setRenderTarget(renderer->getDeviceContext());
	shadowMap2->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the 2nd directional light
	dLights[1].generateViewMatrix();
	XMMATRIX lightViewMatrix1 = dLights[1].getViewMatrix();
	XMMATRIX lightProjectionMatrix1 = dLights[1].getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	//Tranform world matrix to position the terrain
	worldMatrix = positionFloor();

	// Render floor depth for 2nd directional light
	terrainPlane->sendData(renderer->getDeviceContext());
	depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1, textureMgr->getTexture("terrainHeight"), noWave, terrainTess);
	depthHeightShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());

	//Tranform world matrix to position the model
	worldMatrix = positionModel();

	// Render model depth for 2nd directional light
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	//Tranform world matrix to position the fish
	worldMatrix = positionFish();

	// Render fish depth for 2nd directional light
	fishMesh->sendData(renderer->getDeviceContext());
	depthFishShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1);
	depthFishShader->render(renderer->getDeviceContext(), fishMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::depthPass3()
{
	// Set the render target to be the third shadowmap render to texture.
	shadowMap3->setRenderTarget(renderer->getDeviceContext());
	shadowMap3->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the point light
	pLights[0].generateViewMatrix();
	XMMATRIX lightViewMatrix2 = pLights[0].getViewMatrix();
	XMMATRIX lightProjectionMatrix2 = pLights[0].getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	//Tranform world matrix to position the terrain
	worldMatrix = positionFloor();

	// Render floor depth for point light
	terrainPlane->sendData(renderer->getDeviceContext());
	depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2, textureMgr->getTexture("terrainHeight"), noWave, terrainTess);
	depthHeightShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());

	//Tranform world matrix to position the water surface
	worldMatrix = positionSurface();

	// Render water surface depth for point light
	surfacePlane->sendData(renderer->getDeviceContext());
	depthHeightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2, textureMgr->getTexture("waterHeight"), wave, waterTess);
	depthHeightShader->render(renderer->getDeviceContext(), surfacePlane->getIndexCount());

	//Tranform world matrix to position the model
	worldMatrix = positionModel();

	// Render model depth for point light
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	//Tranform world matrix to position the fish mesh
	worldMatrix = positionFish();

	// Render fish mesh depth for point light
	fishMesh->sendData(renderer->getDeviceContext());
	depthFishShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	depthFishShader->render(renderer->getDeviceContext(), fishMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


void App1::WaterTexturePass()
{
	// Set the render target to be the render to texture and clear it
	waterTexture->setRenderTarget(renderer->getDeviceContext());
	waterTexture->clearRenderTarget(renderer->getDeviceContext(), 1.39f, 0.58f, 0.92f, 1.0f);

	//Update camera
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	//Tranform world matrix to position the terrain
	worldMatrix = positionFloor();

	// Render terrain with height and shadows
	terrainPlane->sendData(renderer->getDeviceContext());
	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("brick"), textureMgr->getTexture("terrainHeight"), textureMgr->getTexture("terrainNormal"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), shadowMap3->getShaderResourceView(), dLights, pLights, noWave, terrainTess);
	heightShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());

	//Tranform world matrix to position the water surface
	worldMatrix = positionSurface();

	// Render water surface with height and shadows
	surfacePlane->sendData(renderer->getDeviceContext());
	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("water"), textureMgr->getTexture("waveHeight"), textureMgr->getTexture("waveNormal"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), shadowMap3->getShaderResourceView(), dLights, pLights, wave, waterTess);
	heightShader->render(renderer->getDeviceContext(), surfacePlane->getIndexCount());

	//Tranform world matrix to position the model
	worldMatrix = positionModel();

	// Render model with shadows
	model->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("brick"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), shadowMap3->getShaderResourceView(), dLights, pLights);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());

	//Tranform world matrix to position the fish mesh
	worldMatrix = positionFish();

	// Render fish mesh to create fish  at each point and light them
	fishMesh->sendData(renderer->getDeviceContext());
	billboardShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("fish"),shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), shadowMap3->getShaderResourceView(), dLights, pLights, camera->getPosition());
	billboardShader->render(renderer->getDeviceContext(), fishMesh->getIndexCount());
	
	//If wireframe is active at this point turn it off
	renderer->setWireframeMode(false);
	renderer->setBackBufferRenderTarget();
}

void App1::PostProcessingPass()
{
	//Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	
	
	//RENDER THE RENDER TEXTURE SCENE
	//Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();// Default camera position for orthographic rendering

	ortho->sendData(renderer->getDeviceContext());

	//Check if camera is lower than water level
	if (checkUnderwater())
	{
		//Apply water shader effect to the render texture
		waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, waterTexture->getShaderResourceView(), currentTime);
		waterShader->render(renderer->getDeviceContext(), ortho->getIndexCount());
	}

	else
	{
		//Render the render texture to the back buffer as normal
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, waterTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), ortho->getIndexCount());
	}
	
	//Turn off 2d rendering
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
	worldMatrix = XMMatrixTranslation(floorPos.x, floorPos.y, floorPos.z);

	return worldMatrix;
}

XMMATRIX App1::positionSurface()
{
	// position water surface
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(waterPos.x, waterPos.y, waterPos.z);

	return worldMatrix;
}

XMMATRIX App1::positionModel()
{
	// position model
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(modelPos.x, modelPos.y, modelPos.z);

	//Scale the model size down
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	//Rotate the model 180 degrees in Y
	XMMATRIX rotMatrix = XMMatrixRotationY(PI);
	worldMatrix = XMMatrixMultiply(rotMatrix, worldMatrix);
	return worldMatrix;
}

XMMATRIX App1::positionFish()
{
	//Update fish movement in each direction with sin and cos wave to give the movement
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-50.0f + -sinf(-currentTime), -30.0f + sinf(-3.0f * currentTime / 10.0f), cosf(-currentTime / 2.0f));

	return worldMatrix;
}

bool App1::checkUnderwater()
{
	//Returns true if camera is under the water surface
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
	//Wireframe toggle
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	//Point light toggle
	ImGui::Checkbox("Point Light", &pLightOn);

	//Sliders for tessellating the water and terrain
	ImGui::SliderFloat("TesselationW", &waterTess, 1.0f, 32.0f);
	ImGui::SliderFloat("TesselationT", &terrainTess, 1.0f, 32.0f);

	//Controls for the 1st directional light direction and colour
	if (ImGui::CollapsingHeader("Directional Light 1"))
	{

		ImGui::SliderFloat("Directional Light X direction", &lightDir0[0], -1, 1);
		ImGui::SliderFloat("Directional Light Y direction", &lightDir0[1], -1, 1);
		ImGui::SliderFloat("Directional Light Z direction", &lightDir0[2], -1, 1);
		ImGui::ColorPicker4("Colour", lightColour0);
		//Dont let it do the zero thing
		if (lightDir0[0] == 0)
		{
			lightDir0[0] = 0.0001f;
		}
	}

	//Controls for the 2nd directional light direction and colour
	if (ImGui::CollapsingHeader("Directional Light 2"))
	{
		ImGui::SliderFloat("Directional Light X direction", &lightDir1[0], -1, 1);
		ImGui::SliderFloat("Directional Light Y direction", &lightDir1[1], -1, 1);
		ImGui::SliderFloat("Directional Light Z direction", &lightDir1[2], -1, 1);
		ImGui::ColorPicker4("Colour", lightColour1);
		//Dont let it do the zero thing
		if (lightDir1[0] == 0)
		{
			lightDir1[0] = 0.0001f;
		}
	}

	//Creates point light menu in UI if point light is on
	if (pLightOn)
	{
		//Sets the point light colour back to white
		pLightColour = whiteColour;

		//Controls for the point light position
		if (ImGui::CollapsingHeader("Point Light Pos"))
		{
			ImGui::SliderFloat("PointPosX", &pLightPos[0], 0, 100);
			ImGui::SliderFloat("PointPosY", &pLightPos[1], 0, 30);
			ImGui::SliderFloat("PointPosZ", &pLightPos[2], 0, 100);
		}
	}
	//If point light is switched off set the colour to black
	else
	{
		pLightColour = blackColour;
	}
	
	//Update directional lights position based on their direction (so the view point of the light is still on the scene)
	dLights[0].setPosition(-lightDir0[0] * 40, -lightDir0[1] * 40, -lightDir0[2] * 40 + 50);
	dLights[1].setPosition(-lightDir1[0] * 40, -lightDir1[1] * 40, -lightDir1[2] * 40 + 50);

	//Controls for the wave values of the water
	if (ImGui::CollapsingHeader("Wave Control"))
	{
		ImGui::SliderFloat("Wave Speed", &wave[1], 0, 10);
		ImGui::SliderFloat("Wave Height", &wave[2], 0, 100);
		ImGui::SliderFloat("Wave Frequency", &wave[3], 0, 2);
	}


	//Update direction and position of lights
	dLights[0].setDirection(lightDir0[0], lightDir0[1], lightDir0[2]);
	dLights[1].setDirection(lightDir1[0], lightDir1[1], lightDir1[2]);
	pLights[0].setPosition(pLightPos[0], pLightPos[1], pLightPos[2]);

	//Update colours of lights
	dLights[0].setDiffuseColour(lightColour0[0], lightColour0[1], lightColour0[2], 1.0f);
	dLights[1].setDiffuseColour(lightColour1[0], lightColour1[1], lightColour1[2], 1.0f);
	pLights[0].setDiffuseColour(pLightColour[0], pLightColour[1], pLightColour[2], 1.0f);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

