// tessellation shader.cpp
#include "Tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"Tessellation_vs.cso", L"Tessellation_hs.cso", L"tessellation_ds.cso", L"tessellation_ps.cso");
}


TessellationShader::~TessellationShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (tessellationBuffer)
	{
		tessellationBuffer->Release();
		tessellationBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC timerBufferDesc;

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


	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);


	// Setup light buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);


	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	timerBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timerBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timerBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timerBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timerBufferDesc.MiscFlags = 0;
	timerBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timerBufferDesc, NULL, &timeBuffer);
}

void TessellationShader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* light, Light* light2, Light* light3, float dtimed)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	LightBufferType* lightPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(light->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light->getOrthoMatrix());

	XMMATRIX tLightViewMatrix2 = XMMatrixTranspose(light2->getViewMatrix());
	XMMATRIX tLightProjectionMatrix2 = XMMatrixTranspose(light2->getProjectionMatrix());


	XMMATRIX tLightViewMatrix3 = XMMatrixTranspose(light3->getViewMatrix());
	XMMATRIX tLightProjectionMatrix3 = XMMatrixTranspose(light3->getProjectionMatrix());


	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	//Light1
	dataPtr->lightView[0] = tLightViewMatrix;
	dataPtr->lightProjection[0] = tLightProjectionMatrix;

	//Light1
	dataPtr->lightView[1] = tLightViewMatrix2;
	dataPtr->lightProjection[1] = tLightProjectionMatrix2;

	//Light1
	dataPtr->lightView[2] = tLightViewMatrix3;
	dataPtr->lightProjection[2] = tLightProjectionMatrix3;


	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);


	//Additional
	// Send light data to pixel shader
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient[0] = light->getAmbientColour();
	lightPtr->diffuse[0] = light->getDiffuseColour();
	lightPtr->position = light->getPosition();
	lightPtr->direction = light->getDirection();
	lightPtr->padding = 0.f;
	lightPtr->padding3 = 0.0f;
	lightPtr->padding4 = 0.0f;

	//Light2
	lightPtr->ambient[1] = light2->getAmbientColour();
	lightPtr->diffuse[1] = light2->getDiffuseColour();
	lightPtr->position2 = light2->getPosition();
	lightPtr->direction2 = light2->getDirection();
	lightPtr->padding2 = 0.f;

	//Light3
	//Light3
	lightPtr->ambient[2] = light3->getAmbientColour();
	lightPtr->diffuse[2] = light3->getDiffuseColour();
	lightPtr->position3 = light3->getPosition();
	lightPtr->direction3 = light3->getDirection();
	lightPtr->padding5 = 0.f;
	lightPtr->padding6 = 0.f;

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &depthMap2);
	deviceContext->PSSetShaderResources(3, 1, &depthMap3);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
	deviceContext->PSSetSamplers(2, 1, &sampleStateShadow2);
	deviceContext->PSSetSamplers(3, 1, &sampleStateShadow3);



	//Timer
	TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->dtime = dtimed;
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &timeBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &timeBuffer);

	// Set shader texture and sampler resource in the domain shader for Height Mapping.
	deviceContext->DSSetShaderResources(0, 1, &texture);
	deviceContext->DSSetShaderResources(0, 1, &texture2);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

}



