// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
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
	float4 position : SV_POSITION;
	float4 colour : COLOR;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 tex;
	float3 normal;
	float4 lightVP;

	OutputType output;
	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	//TEX
	float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	tex = lerp(t1, t2, uvwCoord.x);

	//NORMAL
	float3 n1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
	float3 n2 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
	normal = lerp(n1, n2, uvwCoord.x);

	//LIGHT
	float4 lvp1 = lerp(patch[0].lightViewPos, patch[1].lightViewPos, uvwCoord.y);
	float4 lvp2 = lerp(patch[3].lightViewPos, patch[2].lightViewPos, uvwCoord.y);
	lightVP = lerp(lvp1, lvp2, uvwCoord.x);

	float4 heightmap = texture0.SampleLevel(sampler0, tex, 0);
	vertexPosition.z += -heightmap * 35;


	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.colour = patch[0].colour;
	output.tex = tex;

	output.normal = mul(normal, worldMatrix);
	output.normal = normalize(output.normal);
	// Send the input colour into the pixel shader.

	
	
	output.lightViewPos = lightVP;
	

	return output ;
}
