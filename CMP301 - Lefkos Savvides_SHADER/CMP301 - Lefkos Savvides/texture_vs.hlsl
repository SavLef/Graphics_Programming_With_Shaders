//Matrix Buffer
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//Input data from mesh.
struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

//Output to Pixel Shader.
struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
	OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Store the normal coordinates for the pixel shader.
    output.normal = input.normal;

	return output;
}