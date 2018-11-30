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
#include "Depth_Tes_Shader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:

	bool render();

//All passes created.
	//Depth Passes
	void depthPass();
	void depthPass_2();
	void depthPass_3();
	//HeightMappingPass
	void hmpass();
	//Final Pass to Render
	void finalPass();
	//Post Processing Pass
	void pppass();
	//Gui
	void gui();
	//Pass made for debugging
	void test();

private:

//Shader Initializations
	//Texture Shader
	TextureShader* textureShader;
	TessellationShader* tessellationShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	HeightMappingShader* heightShader;
	SepiaShader* sepiaShader;
	Depth_Tes_Shader* depth_tes;


//Mesh Initialization
	PlaneMesh* mesh;
	TessellationMesh* meshh;

//Lights Iitialization
	Light* light;
	Light* light2;
	Light* light3;

//Models Initialization
	Model* MetalGearRay;
	Model* MetalGearRex;
	
//Quad for Tessellation
	TexturedQuad* quad;

	

//Render to Texture Initializations
	//Shadow Maps
	RenderTexture* shadowMap;
	RenderTexture* shadowMap2;
	RenderTexture* shadowMap3;

	//HeightMap
	RenderTexture* heightMap;

	//Post Processing RTT
	RenderTexture* pptex;
	RenderTexture* debug;
	RenderTexture* PostProc;

	//Orthomesh
	OrthoMesh* orthoMesh;
	
//Variables for imGUI 
	//Light 1 Controls
	float splightx, splighty, splightz;
	float splightdx, splightdy, splightdz;

//Delta Time for Height Mapping Tessellation
	float dt;
};

#endif