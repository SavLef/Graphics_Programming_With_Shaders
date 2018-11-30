//Matrix Buffer
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//Inputs from Quad information
struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD1;
	float3 normal : NORMAL;
};

//Outputs to Hull Shader
struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD1;
	float3 normal : NORMAL;
	float3 depthPosition : TEXCOORD0;
};

OutputType main(InputType input)
{
	OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
	// Calculate the normals of the vertex against the world, view, and projection matrices.
	output.normal = mul(input.normal, worldMatrix);
	output.normal = mul(output.normal, viewMatrix);
	output.normal = mul(output.normal, projectionMatrix);

	//Normalise the normal
	output.normal = normalize(output.normal);

	//Sets the tex output to the same value as the input.
	output.tex = input.tex;

	//Sets the depthPosition output to the same value as the output position.
	output.depthPosition= output.position;

	return output;
}