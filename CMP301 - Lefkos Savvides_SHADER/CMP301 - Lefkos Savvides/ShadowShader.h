// Light shader.h
// Basic single light shader setup
#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_

#include "DXF.h"

using namespace std;
using namespace DirectX;


class ShadowShader : public BaseShader
{
private:

	//Matrix BufferType Initialization
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[3];
		XMMATRIX lightProjection[3];
	};

	//LightBufferType Initialization - 3 Lights
	struct LightBufferType
	{
		XMFLOAT4 ambient[3];
		XMFLOAT4 diffuse[3];
		XMFLOAT3 position;
		float padding3;
		XMFLOAT3 position2;
		float padding4;
		XMFLOAT3 position3;
		float padding5;
		XMFLOAT3 direction;
		float padding;
		XMFLOAT3 direction2;
		float padding2;
		XMFLOAT3 direction3;
		float padding6;
	};

public:

	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	//Pass in Texture, Depth Maps for each light and Lights 1, 2 and 3.
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* light, Light* light2, Light* light3);

private:
	void initShader(WCHAR*, WCHAR*);

private:

	//Buffers Initialization
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;

	//Sampler States Initialization
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11SamplerState* sampleStateShadow2;
	ID3D11SamplerState* sampleStateShadow3;

};

#endif