// Depth Tessellation shader.cpp
#include "Depth_Tes_Shader.h"


Depth_Tes_Shader::Depth_Tes_Shader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	//Initialise Shaders to use
	initShader(L"Depth_Tes_vs.cso", L"Depth_Tes_hs.cso", L"Depth_Tes_ds.cso", L"Depth_Tes_ps.cso");
}


Depth_Tes_Shader::~Depth_Tes_Shader()
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

void Depth_Tes_Shader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	//Setup Buffer Descriptions
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC timerBufferDesc;
	D3D11_BUFFER_DESC TesValBufferDesc;

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

	// Setup the description of the Time buffer that is in the Domain shader.
	timerBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timerBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timerBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timerBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timerBufferDesc.MiscFlags = 0;
	timerBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timerBufferDesc, NULL, &timeBuffer);

	// Setup the description of the Tessellation Values that is in the Hull shader.
	TesValBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	TesValBufferDesc.ByteWidth = sizeof(TesValuesType);
	TesValBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TesValBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TesValBufferDesc.MiscFlags = 0;
	TesValBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timerBufferDesc, NULL, &valuesBuffer);

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


	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

}

void Depth_Tes_Shader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void Depth_Tes_Shader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix,ID3D11ShaderResourceView* texture, float dtimed, XMINT4 eg, XMINT4 ins)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);


	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Set shader texture and sampler resource in the Domain shader for Height Mapping.
	deviceContext->DSSetShaderResources(0, 1, &texture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	//Timer - Send to the Domain Shader for the displacement
	TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->dtime = dtimed;
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &timeBuffer);


	//Set data from the values inputed to the Values Buffer to be send to the Domain Shader.
	TesValuesType* tesPtr;
	deviceContext->Map(valuesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tesPtr = (TesValuesType*)mappedResource.pData;

	tesPtr->edg.x = eg.x;
	tesPtr->edg.y = eg.y;
	tesPtr->edg.z = eg.z;
	tesPtr->edg.w = eg.w;

	tesPtr->insd.x = ins.x;
	tesPtr->insd.y = ins.y;

	deviceContext->Unmap(valuesBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &valuesBuffer);


	
}



