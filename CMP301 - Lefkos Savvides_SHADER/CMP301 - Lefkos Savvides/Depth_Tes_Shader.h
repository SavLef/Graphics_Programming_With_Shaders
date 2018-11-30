#pragma once// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class Depth_Tes_Shader : public BaseShader
{
private:
	//Matrix BufferType Initiation
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	//Tessellation BufferType Initiation
	struct TessellationBufferType
	{
		float tessellationFactor;
		XMFLOAT3 padding;
	};

	//Timer BufferType Initiation (For DeltaTime)
	struct TimeBufferType
	{
		float dtime;
		XMFLOAT3 padding;
	};


public:

	Depth_Tes_Shader(ID3D11Device* device, HWND hwnd);
	~Depth_Tes_Shader();

	//Pass in texture and Delta_Time
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float dtimed);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	//Buffer Initializations
	ID3D11Buffer * matrixBuffer;
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;

	//Sample State Initializations
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;

};
