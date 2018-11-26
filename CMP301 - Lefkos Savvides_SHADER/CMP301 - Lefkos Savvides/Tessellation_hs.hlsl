// Tessellation Hull Shader
// Prepares control points for tessellation
struct InputType
{
	float3 position : POSITION;
	float4 colour : COLOR;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float3 position : POSITION;
	float4 colour : COLOR;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;


	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = 3;// tessellationFactor;
	output.edges[1] = 3;//tessellationFactor;
	output.edges[2] = 3;
	output.edges[3] = 3;// tessellationFactor;

						// Set the tessellation factor for tessallating inside the triangle.
	output.inside[0] = 3;
	output.inside[1] =3;// tessellationFactor;

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
	output.colour = patch[pointId].colour;
	output.tex = patch[pointId].tex;

	return output;
}