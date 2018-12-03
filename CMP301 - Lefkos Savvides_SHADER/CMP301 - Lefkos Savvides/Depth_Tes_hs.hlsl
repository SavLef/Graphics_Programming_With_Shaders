// Tessellation Hull Shader
// Prepares control points for tessellation

//Inputs from Vertex Shader

cbuffer ValuesBuffer : register(b0)
{
	int4 eg;
	int4 insd;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD1;
	float3 normal : NORMAL;
	float3 depthPosition : TEXCOORD0;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

//Outputs to Domain Shader
struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD1;
	float3 normal : NORMAL;
	float3 depthPosition : TEXCOORD0;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	
	// Set the tessellation factors for the four edges of the quad.
	output.edges[0] = eg.x;// 1st Edge
	output.edges[1] = eg.y;// 2nd Edge
	output.edges[2] = eg.z;// 3rd Edge
	output.edges[3] = eg.w;// 4th Edge

						   // Set the tessellation factor for tessallating inside the quad.
	output.inside[0] = insd.x;// tessellationFactor;
	output.inside[1] = insd.y;// tessellationFactor;

	return output;
}

//Sets the domain to quad
[domain("quad")]
//Works wth integers
[partitioning("integer")]
[outputtopology("triangle_ccw")]
//Control points to output
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;
	// Sets the position for this control point as the output position.
	output.position = patch[pointId].position;
	// Sets the tex for this control point as the output tex.
	output.tex = patch[pointId].tex;
	// Sets the normal for this control point as the output normal.
	output.normal = patch[pointId].normal;
	// Sets the depthPosition for this control point as the output depthPosition.
	output.depthPosition = patch[pointId].depthPosition;

	return output;
}