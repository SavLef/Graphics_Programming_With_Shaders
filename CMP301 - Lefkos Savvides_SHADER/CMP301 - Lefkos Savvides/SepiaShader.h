#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class SepiaShader : public BaseShader
{
public:
	SepiaShader(ID3D11Device* device, HWND hwnd);
	~SepiaShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
};

