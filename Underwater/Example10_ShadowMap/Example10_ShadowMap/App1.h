// Application.h
#ifndef _APP1_H
#define _APP1_H

#include <stdlib.h> 
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
	XMMATRIX positionFloor();
	XMMATRIX positionSurface();
	XMMATRIX positionModel();
	XMMATRIX positionFish();
	bool checkUnderwater();

	void depthPass1();
	void depthPass2();
	void depthPass3();
	void depthPass4();
	void finalPass();
	void finalPass2();
	void gui();

private:
	TextureShader* textureShader;
	TessellatedQuad* terrainPlane;
	PointCubeMesh* fishMesh;
	//PlaneMesh* surfacePlane;
	TessellatedQuad* surfacePlane;
	Model* model;

	float modelRot;
	XMFLOAT3 fishPos;
	float fishRot;
	XMFLOAT3 waterPos;

	Light* dLights;
	Light* pLights;
	float* lightDir0;
	float* lightDir1;
	float* pLightPos;

	ShadowShader* shadowShader;
	DepthShader* depthShader;
	UnderwaterShader* waterShader;
	HeightShader* heightShader;
	DepthHeightShader* depthHeightShader;
	SurfaceShader* surfaceShader;
	BillboardShader* billboardShader;
	DepthFishShader* depthFishShader;


	RenderTexture* shadowMap;
	RenderTexture* shadowMap2;
	RenderTexture* shadowMap3;
	RenderTexture* playerDepthMap;
	RenderTexture* waterTexture;
	OrthoMesh* ortho;
	OrthoMesh* ortho2;

	float terrainTess;
	float waterTess;

	float currentTime;

	float* wave;
	float* noWave;
	float* fishWave;
	int randF[100];

	float* fog;

	int screenW;
	int screenH;
	
	bool isUnderwater;
};

#endif