#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class SepiaShader : public BaseShader
{
public:
	SepiaShader(ID3D11Device* device, HWND hwnd);
	~SepiaShader();

	//Pass in Texture
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture);

private:
	void initShader(WCHAR*, WCHAR*);

private:

	//Buffer Initialization
	ID3D11Buffer * matrixBuffer;

	//SamplerState Initialization
	ID3D11SamplerState* sampleState;
};

