#include "LaserBeam.h"



TexturedQuad::TexturedQuad(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	initBuffers(device);
}


TexturedQuad::~TexturedQuad()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build shape and fill buffers.
void TexturedQuad::initBuffers(ID3D11Device* device)
{
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//Give the desired values for the mesh.
	vertexCount = 4;
	indexCount = 4;

	VertexType* vertices = new VertexType[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	// Load the vertex array with data for the first Vertex.
	vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[0].normal = XMFLOAT3(-1.0f, 1.0f, 0.0f);

	// Load the vertex array with data for the second Vertex.
	vertices[1].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // bottom left.
	vertices[1].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[1].normal = XMFLOAT3(-1.0f, 1.0f, 0.0f);

	// Load the vertex array with data for the third Vertex.
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // bottom right.
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].normal = XMFLOAT3(1.0f, -1.0f, 0.0f);

	// Load the vertex array with data for the fourth Vertex.
	vertices[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // top right.
	vertices[3].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[3].normal = XMFLOAT3(1.0f, 1.0f, 0.0f);

	// Load the index array with data.
	indices[0] = 0;  // Top left
	indices[1] = 1;  // Bottom left.
	indices[2] = 2;  // Bottom right.
	indices[3] = 3;  // Top left.


	//Set up a Vertex Buffer Description.
	D3D11_BUFFER_DESC vertexBufferDesc = { sizeof(VertexType) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	//Pass Vertex Data to be send to the Buffer.
	vertexData = { vertices, 0 , 0 };
	//Send the Vertex Data to the Buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	//Set up an Index Buffer Description.
	D3D11_BUFFER_DESC indexBufferDesc = { sizeof(unsigned long) * indexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	//Pass Index Data to be send to the Buffer.
	indexData = { indices, 0, 0 };
	//Send the Index Data to the Buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

// Send Geometry data to the GPU
void TexturedQuad::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}

