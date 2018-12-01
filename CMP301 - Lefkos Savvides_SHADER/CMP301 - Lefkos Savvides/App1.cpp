#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	//--------------------------------------
	// Create Mesh object and the quad that will be tessellated.
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	quad = new TexturedQuad(renderer->getDevice(), renderer->getDeviceContext());
	//--------------------------------------
	//Create the two Models and load the model.obj files.
	MetalGearRay = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/MGR.obj");
	MetalGearRex = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/Rex.obj");

	//--------------------------------------
	//Load Textures into the texture Manager.
	textureMgr->loadTexture("brick", L"res/snowdirt.jpg");
	textureMgr->loadTexture("t_Ray", L"res/t_Ray.png");
	textureMgr->loadTexture("t_Rex", L"res/t_Rex.jpg");
	textureMgr->loadTexture("height", L"res/dirt.png");
	textureMgr->loadTexture("fire", L"res/fire.png");

	//--------------------------------------
	//Initiate all the shaders.
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	heightShader = new HeightMappingShader(renderer->getDevice(), hwnd);
	sepiaShader = new SepiaShader(renderer->getDevice(), hwnd);
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);
	depth_tes = new Depth_Tes_Shader(renderer->getDevice(), hwnd);

	//--------------------------------------
	//Initiate the Render Textures.
	debug = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	pptex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH); 

	//--------------------------------------
	//Initiate the Orthomeshe
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);

	//--------------------------------------
	//Set the ShadowMap values.
	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;

	//--------------------------------------
	//Set the Scene values
	int sceneWidth = 100;
	int sceneHeight = 100;

	//--------------------------------------
	//ShadowMap Declarations for all three lights.
	shadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap2 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap3 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);

	//--------------------------------------
	//First light Set Up
	light = new Light;
	light->setAmbientColour(-50.f, 0.f, 0.f, 1.0f);
	light->setDiffuseColour(.3f, 0.3f, 0.3f, 1.0f);
	light->setDirection(.7f, -0.7f, 0.7f);
	light->setPosition(0.f, 0.f, -10.f);
	light->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	//--------------------------------------
	//Second Light Set Up
	light2 = new Light;
	light2->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light2->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light2->setDirection(0.f, -1.0f, 0.5f);
	light2->setPosition(8.f, 17.f, 10.f);
	light2->generateProjectionMatrix( 0.1f, 100.f);

	//--------------------------------------
	//Third Light Set Up
	light3 = new Light;
	light3->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light3->setDiffuseColour(0.f, 1.0f, 1.0f, .0f);
	light3->setDirection(0.f, -1.0f, 1.f);
	light3->setPosition(8.f, 17.f, 10.f);
	light3->generateProjectionMatrix(0.1f, 100.f);

	//--------------------------------------
	//Variables to change the first light's position.
	splightx = light->getPosition().x;
	splighty = light->getPosition().y;
	splightz = light->getPosition().z;

	//--------------------------------------
	//Variables to change the first light's direction.
	splightdx = light->getDirection().x;
	splightdy = light->getDirection().y;
	splightdz = light->getDirection().z;

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
	//Updates the light's variables based on imGUI changes.
	light->setPosition(splightx,splighty,splightz);
	light->setDirection(splightdx, splightdy, splightdz);
	//--------------------------------------
	//Calculate DeltaTime for HeightMapping
	dt += timer->getTime();
	//--------------------------------------
	//--------------------------------------
	//MultiPass Rendering Stage
	//Depth Passes
	depthPass();
	depthPass_2();
	depthPass_3();
	//--------------------------------------
	//HeightMappingPass
	hmpass();
	//--------------------------------------
	//Post Processing Pass
	pppass();
	//--------------------------------------
	//Final Render Pass
	finalPass();
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------
//------------------------------------------DEPTH PASS FOR LIGHT 1-------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMap->setRenderTarget(renderer->getDeviceContext());

	//Clear the Render Target
	shadowMap->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	//Generate the View Matrix of the first Light
	light->generateViewMatrix();
	//Get the World, View, and Projection matrices from the camera and d3d objects.
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	//---------------------------------------------------------------------------------------------------------------------

	//Apply translation
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);

	//Send the mesh data to be passed into the Shaders.
	mesh->sendData(renderer->getDeviceContext());

	//Pass the floor through the DepthShader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------

	//World Matrix Translations
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-7.f, 10.f, -25.f);
	//Declare the scale and rotation Matrix for the appropriate functions.
	XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(160);
	//Apply the scaling and rotations.
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);

	//Send the Metal Gear Ray data to be passed into the DepthShader.
	MetalGearRay->sendData(renderer->getDeviceContext());
	//Pass the Data into the Shader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), MetalGearRay->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------

	//Reset the worldMatrix.
	worldMatrix = renderer->getWorldMatrix();
	//World Matrix Translations.
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);
	//Scaling Calculations.
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	
	//Send the Metal Gear Rex data to be passed into the DepthShader.
	MetalGearRex->sendData(renderer->getDeviceContext());
	//Pass the Data into the Shader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), MetalGearRex->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------
	//Reset the worldMatrix.
	worldMatrix = renderer->getWorldMatrix();
	//World Matrix Translations.
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);

	//Send the Tessellated Quad data to be passed into the Shader.
	quad->sendData(renderer->getDeviceContext());

	//Pass the Data into the Tessellation Depth-Shader.
	depth_tes->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("height"), dt);
	//Render the Shadow.
	depth_tes->render(renderer->getDeviceContext(), quad->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------
	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

//---------------------------------------------------------------------------------------------------------------------
//------------------------------------------DEPTH PASS FOR LIGHT 2-----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
void App1::depthPass_2()
{
	// Set the render target to be the render to texture.
	shadowMap2->setRenderTarget(renderer->getDeviceContext());
	shadowMap2->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	//Get the World, View, and Projection matrices from the camera and d3d objects.
	light2->generateViewMatrix();
	XMMATRIX lightViewMatrix = light2->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light2->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();


	//Apply translation
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);

	//Send the mesh data to be passed into the Shaders.
	mesh->sendData(renderer->getDeviceContext());

	//Pass the floor through the DepthShader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------

	//World Matrix Translations
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-7.f, 10.f, -25.f);
	//Declare the scale and rotation Matrix for the appropriate functions.
	XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(160);
	//Apply the scaling and rotations.
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);

	//Send the Metal Gear Ray data to be passed into the DepthShader.
	MetalGearRay->sendData(renderer->getDeviceContext());
	//Pass the Data into the Shader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), MetalGearRay->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------

	//Reset the worldMatrix.
	worldMatrix = renderer->getWorldMatrix();
	//World Matrix Translations.
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);
	//Scaling Calculations.
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	//Send the Metal Gear Rex data to be passed into the DepthShader.
	MetalGearRex->sendData(renderer->getDeviceContext());
	//Pass the Data into the Shader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), MetalGearRex->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------
	//Reset the worldMatrix.
	worldMatrix = renderer->getWorldMatrix();
	//World Matrix Translations.
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);

	//Send the Tessellated Quad data to be passed into the Shader.
	quad->sendData(renderer->getDeviceContext());

	//Pass the Data into the Tessellation Depth-Shader.
	depth_tes->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("height"), dt);
	//Render the Shadow.
	depth_tes->render(renderer->getDeviceContext(), quad->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------
	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}
//---------------------------------------------------------------------------------------------------------------------
//------------------------------------------DEPTH PASS FOR LIGHT 3-----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
void App1::depthPass_3()
{
	// Set the render target to be the render to texture.
	shadowMap3->setRenderTarget(renderer->getDeviceContext());
	shadowMap3->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	//Get the World, View, and Projection matrices from the camera and d3d objects.
	light3->generateViewMatrix();
	XMMATRIX lightViewMatrix = light3->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light3->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();


	//Apply translation
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);

	//Send the mesh data to be passed into the Shaders.
	mesh->sendData(renderer->getDeviceContext());

	//Pass the floor through the DepthShader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------

	//World Matrix Translations
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-7.f, 10.f, -25.f);
	//Declare the scale and rotation Matrix for the appropriate functions.
	XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(160);
	//Apply the scaling and rotations.
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);

	//Send the Metal Gear Ray data to be passed into the DepthShader.
	MetalGearRay->sendData(renderer->getDeviceContext());
	//Pass the Data into the Shader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), MetalGearRay->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------

	//Reset the worldMatrix.
	worldMatrix = renderer->getWorldMatrix();
	//World Matrix Translations.
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);
	//Scaling Calculations.
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	//Send the Metal Gear Rex data to be passed into the DepthShader.
	MetalGearRex->sendData(renderer->getDeviceContext());
	//Pass the Data into the Shader.
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//Render the Shadow.
	depthShader->render(renderer->getDeviceContext(), MetalGearRex->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------
	//Reset the worldMatrix.
	worldMatrix = renderer->getWorldMatrix();
	//World Matrix Translations.
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);

	//Send the Tessellated Quad data to be passed into the Shader.
	quad->sendData(renderer->getDeviceContext());

	//Pass the Data into the Tessellation Depth-Shader.
	depth_tes->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("height"), dt);
	//Render the Shadow.
	depth_tes->render(renderer->getDeviceContext(), quad->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------
	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}
//---------------------------------------------------------------------------------------------------------------------
//----------------------------------------------HEIGHT MAP PASS--------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

void App1::hmpass()
{
	// Set the render target to be the render to texture and clear it.
	debug->setRenderTarget(renderer->getDeviceContext());
	debug->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	camera->update();

	//Get the World, View, and Projection matrices from the camera and d3d objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	
	//Trnalsations
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	//Send the data to pass through the shader.
	mesh->sendData(renderer->getDeviceContext());
	
	//Pass the data through the HeightMapping Shader.
	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), textureMgr->getTexture("height"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), shadowMap3->getShaderResourceView(), light, light2, light3);
	//Render the HeightMapped Floor.
	heightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//Reset WorldMatrix.
	worldMatrix = renderer->getWorldMatrix();
	//Translation.
	worldMatrix = XMMatrixTranslation(-4.f, 13.5f, -38.f);
	//Declaration of Scale and Rotation Matrices.
	XMMATRIX scaleMatrix = XMMatrixScaling(10.0f, 10.75f, 10.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(160);
	//Application of Scale and Rotation Matrices.
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);

	//Reset the World Matrix
	worldMatrix = renderer->getWorldMatrix();
	//Set Translation
	worldMatrix = XMMatrixTranslation(-7.f, 10.f, -25.f);
	//Set Scaling
	scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	//Set Rotation on the Y axis
	rotationMatrix = XMMatrixRotationY(160);
	//Apply above transforms
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);

	//Send data to be passed into the Shadow Shader
	MetalGearRay->sendData(renderer->getDeviceContext());

	//Pass data into the ShadowShader (Shadow Maps for all three lights, and the three lights.)
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("t_Ray"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), shadowMap3->getShaderResourceView(), light, light2, light3);
	shadowShader->render(renderer->getDeviceContext(), MetalGearRay->getIndexCount());
	
	//Reset WorldMatrix
	worldMatrix = renderer->getWorldMatrix();
	//Set Translation
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);
	//Set Scaling
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	//Apply Scaling
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	//Send data to be passed into the Shadow Shader
	MetalGearRex->sendData(renderer->getDeviceContext());
	//Pass data into the ShadowShader (Shadow Maps for all three lights, and the three lights.)
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("t_Rex"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), shadowMap3->getShaderResourceView(), light, light2, light3);
	shadowShader->render(renderer->getDeviceContext(), MetalGearRex->getIndexCount());

	//Reset the World Matrix
	worldMatrix = renderer->getWorldMatrix();
	//Translation
	worldMatrix = XMMatrixTranslation(-4.f, 10.f, 35.f);

	//Send data to be passed into the Tessellation Shader
	quad->sendData(renderer->getDeviceContext());
	//Pass Data into the Tessellation Shader (Matrices, Texture to apply on object, Texture to Sample and Heightmap, ShadowMaps, Lights and Delta Time.)
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,/*texture*/ textureMgr->getTexture("brick"), /*heightmapping*/ textureMgr->getTexture("height"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), shadowMap3->getShaderResourceView(), light, light2, light3, dt);
	tessellationShader->render(renderer->getDeviceContext(), quad->getIndexCount());

	//Set the BackBuffer as Render Targer.
	renderer->setBackBufferRenderTarget();
}
//---------------------------------------------------------------------------------------------------------------------
//--------------------------------------------POST PROCESSING PASS-----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
void App1::pppass()
{
	//Matrices Declaration
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	//Set the render target to be the render to texture and clear it.
	pptex->setRenderTarget(renderer->getDeviceContext());
	pptex->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	
	//Get the Matrices values from the renderer, camera and RTT.
	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = debug->getOrthoMatrix();

	//Turn the ZBuffer off.
	renderer->setZBuffer(false);

	//Send the orthoMesh data to be passed into the Post Processing Shader.
	orthoMesh->sendData(renderer->getDeviceContext());
	//Send the Data to the post processing Shader.
	sepiaShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, debug->getShaderResourceView());
	//Render the processed scene.
	sepiaShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	//Turn the ZBuffer true again.
	renderer->setZBuffer(true);

	//Set the BackBuffer as Render Targer.
	renderer->setBackBufferRenderTarget();

}
void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.f, 0.f, 0.f, 1.0f);
	//camera->update();
	renderer->setZBuffer(false);
	//Get the World, View, and Projection matrices from the camera and d3d objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();


	///UNCOMMENT FOR DEBUGGING WIREFRAME MODE
	
	//XMMATRIX viewMatrix = camera->getViewMatrix();
	//XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	//worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);

	//// Render floor
	//mesh->sendData(renderer->getDeviceContext());
	//

	//heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), textureMgr->getTexture("height"), shadowMap->getShaderResourceView(), light);
	//heightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());




	//// Render MetalGearRay
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(-7.f, 10.f, -25.f);
	//XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	//XMMATRIX rotationMatrix = XMMatrixRotationY(160);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);
	//MetalGearRay->sendData(renderer->getDeviceContext());
	//shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("t_Ray"), shadowMap->getShaderResourceView(), light);
	//shadowShader->render(renderer->getDeviceContext(), MetalGearRay->getIndexCount());


	////Render Metal Gear Rex

	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);
	//scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	////Render Metal Gear Rex

	//MetalGearRex->sendData(renderer->getDeviceContext());
	//shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("t_Rex"), shadowMap->getShaderResourceView(), light);
	//shadowShader->render(renderer->getDeviceContext(), MetalGearRex->getIndexCount());

	//
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(-4.f, 10.f, 30.f);

	//quad->sendData(renderer->getDeviceContext());
	//tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), textureMgr->getTexture("fire"), shadowMap->getShaderResourceView(), light, dt);
	//tessellationShader->render(renderer->getDeviceContext(), quad->getIndexCount());
	
	///ENDOF


	//DEBUG
	
	//Send the data to be passed through the Texture Shader.
	orthoMesh->sendData(renderer->getDeviceContext());
	//Pass the orthoMesh data to display the RTT.
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, pptex->getShaderResourceView());
	//Render the scene on a texture.
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	//Set the ZBuffer true.
	renderer->setZBuffer(true);

	//Run GUI.
	gui();
	//End
	renderer->endScene();
}

void App1::test()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();
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

	ImGui::Text("Camera: ");
	ImGui::Text("X Position: %.1f", camera->getPosition().x);
	ImGui::Text("Y Position: %.1f", camera->getPosition().y);
	ImGui::Text("Z Position: %.1f", camera->getPosition().z);

	ImGui::Text("  Rotation: %.1f", camera->getRotation());
	ImGui::Text("Directional Light: ");
	//ImGui::SliderFloat("X Position: %.1f", &splightx, -50, 50);
	//ImGui::SliderFloat("Y Position: %.1f", &splighty, -50, 50);
	//ImGui::SliderFloat("Z Position: %.1f", &splightz, -50, 50);

	ImGui::SliderFloat("X Direction: %.1f", &splightdx, -75,75);
	ImGui::SliderFloat("Y Direction: %.1f", &splightdy, -75, 75);
	ImGui::SliderFloat("Z Direction: %.1f", &splightdz, -75, 75);

	/*ImGui::Text("Tessellation: ");
	ImGui::SliderFloat("Edge 1 %.1f", &splightdx, 5, 20);
	ImGui::SliderFloat("Edge 2: %.1f", &splightdy, 5, 20);
	ImGui::SliderFloat("Edge 3: %.1f", &splightdz, 5, 20);
	ImGui::SliderFloat("Edge 4: %.1f", &splightdx, 5, 20);
	ImGui::SliderFloat("Inside 1: %.1f", &splightdy, 5, 20);
	ImGui::SliderFloat("Inside 2: %.1f", &splightdz, 5, 20);*/


	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

