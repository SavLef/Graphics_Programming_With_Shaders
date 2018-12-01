// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

//Matrix BufferType
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//Timer BufferType (For DeltaTime)
cbuffer TimerBuffer : register(b1)
{
	float dtime;
	float3 padding;
}

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

//Inputs from Hull Shader Stage
struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD1;
	float3 normal : NORMAL;
	float3 depthPosition : TEXCOORD0;
};

//Outputs to Pixel Shader
struct OutputType
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 depthPosition : TEXCOORD0;
};

//Sets the domain to quad - mesh we will be tessellating
[domain("quad")]

//Set to 4 control Points
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	//Variable declarations for final values of inputs to output to Pixel Shader.

	float3 vertexPosition;
	float2 tex;
	float3 normal;
	float4 lightVP;
	float4 depthpos;

	//Linear interpolation of vertex positions.
	OutputType output;
	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	//Linear interpolation of normals.
	float3 n1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
	float3 n2 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
	normal = lerp(n1, n2, uvwCoord.x);

	//Linear interpolation of texes.
	float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	tex = lerp(t1, t2, uvwCoord.x);

	//Height Map application as the Tessellated object in our scene so the shadow is animated.
	float4 heightmap = texture0.SampleLevel(sampler0, tex, 0);
	vertexPosition.z += -heightmap * 20;
	vertexPosition.z = sin(vertexPosition.z + dtime * 40/*speec*/) * 10;

	//Normal Adjustments
	normal.z += -heightmap * 20;
	normal.z = sin(vertexPosition.z + dtime * 40/*speec*/) * 10;

	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the new vertex normals against the world, view, and projection matrices.
	output.normal = mul(float4(normal, 1.0f), worldMatrix);
	output.normal = mul(output.normal, viewMatrix);
	output.normal = mul(output.normal, projectionMatrix);

	//Normalize the normal
	output.normal = normalize(output.normal);

	//Set the output depthPosition the same as the output position, not needing to re-do calculations.
	output.depthPosition = output.position;


	return output;
}
