// texture vertex shader
// Basic shader for rendering textured geometry
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

//Height Mapping Matrix Buffer, accommodates 3 lights.
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[3];
	matrix lightProjectionMatrix[3];
};

//Takes in data from the mesh.
struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

};

//Sends the data to the pixel shader.
struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
	float4 lightViewPos2 : TEXDOORD2;
	float4 lightViewPos3 : TEXDOORD3;
	float4 wPosition:POSITION;
	
};



OutputType main(InputType input)
{
	OutputType output;


	//Height Map
	float4 heightmap = texture0.SampleLevel(sampler0, input.tex, 0);
	input.position.y += heightmap * 1.2;


	output.wPosition = mul(input.position, worldMatrix);
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	   
	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos = mul(input.position, worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix[0]);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix[0]);

	// Calculate the position of the vertice as viewed by the second light source.
	output.lightViewPos2 = mul(input.position, worldMatrix);
	output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix[1]);
	output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix[1]);

	// Calculate the position of the vertice as viewed by the third light source.
	output.lightViewPos3 = mul(input.position, worldMatrix);
	output.lightViewPos3 = mul(output.lightViewPos3, lightViewMatrix[2]);
	output.lightViewPos3 = mul(output.lightViewPos3, lightProjectionMatrix[2]);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

	return output;
}