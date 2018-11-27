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
	XMMATRIX positionModel();
	void depthPass1();
	void depthPass2();
	void depthPass3();
	void finalPass();
	void finalPass2();
	void firstPass2();
	void gui();

private:
	TextureShader* textureShader;
	PlaneMesh* mesh;
	Model* model;
	float modelRot;

	Light *dLights;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	UnderwaterShader* waterShader;
	HeightShader* heightShader;


	RenderTexture* shadowMap;
	RenderTexture* shadowMap2;
	RenderTexture* playerDepthMap;
	RenderTexture* waterTexture;
	OrthoMesh* ortho;
	OrthoMesh* ortho2;

	float currentTime;
	float* fog;

	int screenW;
	int screenH;
};

#endif