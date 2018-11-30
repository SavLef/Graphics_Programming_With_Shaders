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
	matrix lightViewMatrix[3];
	matrix lightProjectionMatrix[3];
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

cbuffer TimerBuffer : register(b1)
{
	float dtime;
	float3 padding;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
	float4 lightViewPos2 : TEXCOORD2;
	float4 lightViewPos3 : TEXCOORD3;
	float4 wPosition:POSITION;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
	float4 lightViewPos2 : TEXCOORD2;
	float4 lightViewPos3 : TEXCOORD3;
	float4 wPosition:POSITION;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 tex;
	float3 normal;
	float4 lightVP;
	float4 lightVP2;
	float4 lightVP3;

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

	//LIGHT
	float4 lvp1_2 = lerp(patch[0].lightViewPos2, patch[1].lightViewPos2, uvwCoord.y);
	float4 lvp2_2 = lerp(patch[3].lightViewPos2, patch[2].lightViewPos2, uvwCoord.y);
	lightVP2 = lerp(lvp1_2, lvp2_2, uvwCoord.x);

	//LIGHT
	float4 lvp1_3 = lerp(patch[0].lightViewPos3, patch[1].lightViewPos3, uvwCoord.y);
	float4 lvp2_3 = lerp(patch[3].lightViewPos3, patch[2].lightViewPos3, uvwCoord.y);
	lightVP3 = lerp(lvp1_3, lvp2_3, uvwCoord.x);

	float4 heightmap = texture0.SampleLevel(sampler0, tex, 0);
	vertexPosition.z += -heightmap * 20;
	vertexPosition.z = sin(vertexPosition.z + dtime * 40/*speec*/)*10;

	normal.z += -heightmap * 20;
	normal.z = sin(vertexPosition.z + dtime * 40/*speec*/) * 10;


	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.wPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);

	output.normal = mul(float4(normal, 1.0f), worldMatrix);
	output.normal = mul(output.normal, viewMatrix);
	output.normal = mul(output.normal, projectionMatrix);

	// Send values to pixel shader.
	//output.colour = patch[0].colour;
	output.tex = tex;

	//output.normal = mul(normal, worldMatrix);
	output.normal = normalize(output.normal);


	
	output.lightViewPos = lightVP;

	output.lightViewPos2 = lightVP2;

	output.lightViewPos3 = lightVP3;


	

	return output ;
}
