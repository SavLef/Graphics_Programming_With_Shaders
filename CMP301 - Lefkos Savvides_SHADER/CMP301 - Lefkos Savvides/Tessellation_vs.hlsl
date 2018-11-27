// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

struct InputType
{
	float3 position : POSITION;
	float4 colour : COLOR;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;

};

struct OutputType
{
	float3 position : POSITION;
	float4 colour : COLOR;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
};

OutputType main(InputType input)
{
	OutputType output;

	// Pass the vertex position into the hull shader.
	output.position = input.position;
	output.colour = float4(1.0, 0.0, 0.0, 1.0);
	output.tex = input.tex;
	// Pass the input color into the hull shader.
	output.normal = input.normal;
	output.lightViewPos = input.lightViewPos;

	return output;
}
