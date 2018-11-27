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

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	quad = new TexturedQuad(renderer->getDevice(), renderer->getDeviceContext());	
	MetalGearRay = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/MGR.obj");
	MetalGearRex = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/Rex.obj");

	

	//Textures
	textureMgr->loadTexture("brick", L"res/snowdirt.jpg");
	textureMgr->loadTexture("t_Ray", L"res/t_Ray.png");
	textureMgr->loadTexture("t_Rex", L"res/t_Rex.jpg");
	textureMgr->loadTexture("height", L"res/dirt.png");
	textureMgr->loadTexture("fire", L"res/fire.png");




	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	heightShader = new HeightMappingShader(renderer->getDevice(), hwnd);
	sepiaShader = new SepiaShader(renderer->getDevice(), hwnd);
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);

	debug = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	pptex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH); 

	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);

	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// This is your shadow map
	shadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);

	light = new Light;
	light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(.7f, -0.7f, 0.7f);
	light->setPosition(0.f, 0.f, -10.f);
	light->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);
	
	splightx = light->getPosition().x;
	splighty = light->getPosition().y;
	splightz = light->getPosition().z;

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
	//Updates
	light->setPosition(splightx,splighty,splightz);
	light->setDirection(splightdx, splightdy, splightdz);

	dt += timer->getTime();

	// Perform depth pass

	depthPass();
	hmpass();
	pppass();
	finalPass();

	//test();
	return true;
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMap->setRenderTarget(renderer->getDeviceContext());
	shadowMap->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());

	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	

	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	

	//calculations for Shadow
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-7.f, 10.f, -25.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(160);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);



	// Render MetalGearRay

	
	MetalGearRay->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), MetalGearRay->getIndexCount());

	//---------------------------------------------------------------------------------------------------------------------

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//Render Metal Gear Rex

	MetalGearRex->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), MetalGearRex->getIndexCount());


	//---------------------
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-4.f, 10.f, 35.f);

	quad->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), quad->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::hmpass()
{
	// Set the render target to be the render to texture and clear it
	debug->setRenderTarget(renderer->getDeviceContext());
	debug->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());

	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), textureMgr->getTexture("height"), shadowMap->getShaderResourceView(), light);
	heightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	//Render bullet

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-4.f, 13.5f, -38.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(10.0f, 10.75f, 10.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(160);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);


	// Render MetalGearRay
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-7.f, 10.f, -25.f);
	scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	rotationMatrix = XMMatrixRotationY(160);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);
	MetalGearRay->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("t_Ray"), shadowMap->getShaderResourceView(), light);
	shadowShader->render(renderer->getDeviceContext(), MetalGearRay->getIndexCount());

	//Render Metal Gear Rex

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//Render Metal Gear Rex

	MetalGearRex->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("t_Rex"), shadowMap->getShaderResourceView(), light);
	shadowShader->render(renderer->getDeviceContext(), MetalGearRex->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-4.f, 10.f, 35.f);

	quad->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,/*texture*/ textureMgr->getTexture("fire"), /*heihhtmapping*/ textureMgr->getTexture("height"), shadowMap->getShaderResourceView(), light, dt);
	tessellationShader->render(renderer->getDeviceContext(), quad->getIndexCount());





	//heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), textureMgr->getTexture("height"), shadowMap->getShaderResourceView(), light);
	//heightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}
void App1::pppass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	pptex->setRenderTarget(renderer->getDeviceContext());
	pptex->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = debug->getOrthoMatrix();

	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	sepiaShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, debug->getShaderResourceView());
	sepiaShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();

}
void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.f, 0.f, 0.f, 1.0f);
	//camera->update();
	renderer->setZBuffer(false);
	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();


	///UNCOMMENT FOR DEBUGGING WIREFRAME MODE
	
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);

	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	

	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), textureMgr->getTexture("height"), shadowMap->getShaderResourceView(), light);
	heightShader->render(renderer->getDeviceContext(), mesh->getIndexCount());




	// Render MetalGearRay
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-7.f, 10.f, -25.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(160);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);
	MetalGearRay->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("t_Ray"), shadowMap->getShaderResourceView(), light);
	shadowShader->render(renderer->getDeviceContext(), MetalGearRay->getIndexCount());


	//Render Metal Gear Rex

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(15.f, 0.f, 200.f);
	scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	//Render Metal Gear Rex

	MetalGearRex->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("t_Rex"), shadowMap->getShaderResourceView(), light);
	shadowShader->render(renderer->getDeviceContext(), MetalGearRex->getIndexCount());

	
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-4.f, 10.f, 30.f);

	quad->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), textureMgr->getTexture("fire"), shadowMap->getShaderResourceView(), light, dt);
	tessellationShader->render(renderer->getDeviceContext(), quad->getIndexCount());
	
	///ENDOF


	//DEBUG
	

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, pptex->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	gui();
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

	// Send geometry data, set shader parameters, render object with shader
	

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
	ImGui::Text("Spot Light: ");
	ImGui::SliderFloat("X Position: %.1f", &splightx, -50, 50);
	ImGui::SliderFloat("Y Position: %.1f", &splighty, -50, 50);
	ImGui::SliderFloat("Z Position: %.1f", &splightz, -50, 50);

	ImGui::SliderFloat("X Direction: %.1f", &splightdx, -75,75);
	ImGui::SliderFloat("Y Direction: %.1f", &splightdy, -75, 75);
	ImGui::SliderFloat("Z Direction: %.1f", &splightdz, -75, 75);


	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

