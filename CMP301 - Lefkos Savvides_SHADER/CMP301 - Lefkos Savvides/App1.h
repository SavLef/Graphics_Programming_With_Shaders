// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "HeightMappingShader.h"
#include "SepiaShader.h"
#include "TessellationShader.h"
#include "LaserBeam.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass();
	void hmpass();
	void finalPass();
	void FirstPass();
	void pppass();
	void gui();
	void test();

private:
	TextureShader* textureShader;
	PlaneMesh* mesh;
	//SphereMesh* quad;

	TessellationMesh* meshh;


	Light* light;
	Model* MetalGearRay;
	Model* MetalGearRex;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	HeightMappingShader* heightShader;
	SepiaShader* sepiaShader;

	TexturedQuad* quad;

	

	TessellationShader* tessellationShader;

	RenderTexture* shadowMap;
	RenderTexture* heightMap;
	RenderTexture* pptex;

	RenderTexture* debug;
	RenderTexture* PostProc;

	OrthoMesh* orthoMesh;
	
	float splightx, splighty, splightz;
	float splightdx, splightdy, splightdz;
};

#endif