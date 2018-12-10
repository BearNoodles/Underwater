// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "UnderwaterShader.h"
#include "FogShader.h"
#include "heightShader.h"
#include "depthheightshader.h"
#include "surfaceShader.h"
#include "BillboardShader.h"
#include "DepthFishShader.h"
#include "TessellatedQuad.h"
#include "PointCubeMesh.h"

#define DIRCOUNT 2

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();

	//Positions each object in the scene
	XMMATRIX positionFloor();
	XMMATRIX positionSurface();
	XMMATRIX positionModel();
	XMMATRIX positionFish();

	//Returns true if the camera is below the water level
	bool checkUnderwater();

	//Each depth pass creates a depth map texture 1 of the lights
	void depthPass1();
	void depthPass2();
	void depthPass3();

	//Final pass renders 
	void WaterTexturePass();
	void PostProcessingPass();
	void gui();

private:
	//Meshes for objects in the scene
	PointCubeMesh * fishMesh;
	TessellatedQuad* terrainPlane;
	TessellatedQuad* surfacePlane;
	Model* model;

	//Object start positions
	XMFLOAT3 fishPos;
	XMFLOAT3 floorPos;
	XMFLOAT3 waterPos;
	XMFLOAT3 modelPos;


	//Pointer for arrays of lights and their directions/position
	Light* dLights;
	Light* pLights;
	float* lightDir0;
	float* lightDir1;
	float* pLightPos;

	//True when the point light is switched on
	bool pLightOn;

	//Changeable colour for each directional light. Black and white for the point light
	float* lightColour0;
	float* lightColour1;
	float* pLightColour;
	float* blackColour;
	float* whiteColour;

	//Shaders
	TextureShader* textureShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	UnderwaterShader* waterShader;
	HeightShader* heightShader;
	DepthHeightShader* depthHeightShader;
	SurfaceShader* surfaceShader;
	BillboardShader* billboardShader;
	DepthFishShader* depthFishShader;

	//Render textures for shadow maps and 1 for the final scene to be rendered onto
	RenderTexture* shadowMap;
	RenderTexture* shadowMap2;
	RenderTexture* shadowMap3;
	RenderTexture* waterTexture;
	//Orthomesh for the final render texture
	OrthoMesh* ortho;

	//Changeable esselation values for the water and the terrain
	float terrainTess;
	float waterTess;

	//how much time has passed since the beginning
	float currentTime;

	//Pointer to float4 of values to control the time, speed height and frequency
	//To pass into the shaders with height mapping
	float* wave;
	float* noWave;
	float* fishWave;

	//Screen width and height
	int screenW;
	int screenH;
};

#endif