// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.



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

OutputType main(InputType input)
{
	OutputType output;

	// Pass the vertex position into the hull shader.
	output.position = input.position;
	//output.colour = float4(1.0, 0.0, 0.0, 1.0);
	output.tex = input.tex;
	// Pass the input color into the hull shader.
	output.normal = input.normal;
	output.lightViewPos = input.lightViewPos;

	output.lightViewPos2 = input.lightViewPos2;

	output.lightViewPos3 = input.lightViewPos3;

	output.wPosition = input.wPosition;

	return output;
}
