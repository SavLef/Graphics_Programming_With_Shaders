// Tessellation Hull Shader
// Prepares control points for tessellation


struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
	float4 lightViewPos2 : TEXCOORD2;
	float4 lightViewPos3 : TEXCOORD2;
	float4 wPosition:POSITION;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
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

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;


	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = 10;// tessellationFactor;
	output.edges[1] = 10;//tessellationFactor;
	output.edges[2] = 10;
	output.edges[3] = 10;// tessellationFactor;

						// Set the tessellation factor for tessallating inside the triangle.
	output.inside[0] = 10;
	output.inside[1] = 10;// tessellationFactor;

	return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;
	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;
	// Set the input colour as the output colour.
	//output.colour = patch[pointId].colour;
	output.tex = patch[pointId].tex;
	output.normal = patch[pointId].normal;
	output.lightViewPos = patch[pointId].lightViewPos;
	output.lightViewPos2 = patch[pointId].lightViewPos2;
	output.lightViewPos3 = patch[pointId].lightViewPos3;
	output.wPosition = patch[pointId].wPosition;

	return output;
}