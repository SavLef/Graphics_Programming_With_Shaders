#pragma once

#include "DXF.h"

using namespace DirectX;

class TexturedQuad : public BaseMesh
{
public:
	TexturedQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~TexturedQuad();

	virtual void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
protected:
	void initBuffers(ID3D11Device* device);
};