//HeightMapping Shader
#include "HeightMappingShader.h"



HeightMappingShader::HeightMappingShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	//Initiate the Shaders to use. (Uses the ShadowShader's PixelShader to apply shadows.)
	initShader(L"hmap_vs.cso", L"shadow_ps.cso");
}


HeightMappingShader::~HeightMappingShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}
	//Release base shader components
	BaseShader::~BaseShader();
}


void HeightMappingShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	//Setup Buffer Descriptions
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	//Setup Sampler Descriptions
	D3D11_SAMPLER_DESC samplerDesc;


	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup Light Buffer Description
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);


}


void HeightMappingShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* textureShadow, ID3D11ShaderResourceView* textureHeight, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* light, Light*light2, Light*light3)
{
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* lightPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld, tview, tproj, tLightViewMatrix, tLightProjectionMatrix;
	XMMATRIX tLightViewMatrix2, tLightProjectionMatrix2;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	//Get View and Ortho Matrices from the First Light
	tLightViewMatrix = XMMatrixTranspose(light->getViewMatrix());
	tLightProjectionMatrix = XMMatrixTranspose(light->getOrthoMatrix());


	//Get View and Projection Matrices from the Second Light
	tLightViewMatrix2 = XMMatrixTranspose(light2->getViewMatrix());
	tLightProjectionMatrix2 = XMMatrixTranspose(light2->getProjectionMatrix());

	//Get View and Projection Matrices from the Third Light
	XMMATRIX tLightViewMatrix3 = XMMatrixTranspose(light3->getViewMatrix());
	XMMATRIX tLightProjectionMatrix3 = XMMatrixTranspose(light3->getProjectionMatrix());

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//Pass in Matrix Data
	//World Matrix
	dataPtr->world = tworld;
	//View Matrix
	dataPtr->view = tview;
	//Projection Matrix
	dataPtr->projection = tproj;
	
	//Pass in the Buffer the View and Projection Matrix from the First Light
	dataPtr->lightView[0] = tLightViewMatrix;
	dataPtr->lightProjection[0] = tLightProjectionMatrix;

	//Pass in the Buffer the View and Projection Matrix from the Second Light
	dataPtr->lightView[1] = tLightViewMatrix2;
	dataPtr->lightProjection[1] = tLightProjectionMatrix2;
	
	//Pass in the Buffer the View and Projection Matrix from the Third Light
	dataPtr->lightView[2] = tLightViewMatrix3;
	dataPtr->lightProjection[2] = tLightProjectionMatrix3;

	//Set the buffer in the Vertex Shader
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);


	//Set data from the Light to the Buffer to be send to the Pixel Shader.
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient[0] = light->getAmbientColour();
	lightPtr->diffuse[0] = light->getDiffuseColour();
	lightPtr->position = light->getPosition();
	lightPtr->direction = light->getDirection();
	lightPtr->padding = 0.f;
	lightPtr->padding3 = 0.0f;
	lightPtr->padding4 = 0.0f;

	//Set data from the second Light to the Buffer to be send to the Pixel Shader.
	lightPtr->ambient[1] = light2->getAmbientColour();
	lightPtr->diffuse[1] = light2->getDiffuseColour();
	lightPtr->position2 = light2->getPosition();
	lightPtr->direction2 = light2->getDirection();
	lightPtr->padding2 = 0.f;

	
	//Set data from the third Light to the Buffer to be send to the Pixel Shader.
	lightPtr->ambient[2] = light3->getAmbientColour();
	lightPtr->diffuse[2] = light3->getDiffuseColour();
	lightPtr->position3 = light3->getPosition();
	lightPtr->direction3 = light3->getDirection();
	lightPtr->padding5 = 0.f;
	lightPtr->padding6 = 0.f;


	//Set the buffer to the Pixel Shader, sending the data.
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Set shader texture resources in the Pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &textureShadow);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &depthMap2);
	deviceContext->PSSetShaderResources(3, 1, &depthMap3);

	//Set the Samplers in the Pixel Shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
	deviceContext->PSSetSamplers(2, 1, &sampleStateShadow2);
	deviceContext->PSSetSamplers(3, 1, &sampleStateShadow3);

	// Set shader texture and sampler resource in the Vertex shader for Height Mapping.
	deviceContext->VSSetShaderResources(0, 1, &textureHeight);
	deviceContext->VSSetSamplers(0, 1, &sampleStateHeight);
}





