//Inputs
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

//Outputs to Hull Shader
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

	// Pass the vertex position into the Hull shader.
	output.position = input.position;

	// Pass the tex coordinates to the Hull shader.
	output.tex = input.tex;

	// Pass the normal coordinates into the Hull shader.
	output.normal = input.normal;

	//Declare the data to be sent to the pixel shader regarding the lightViewPositions for each of the three lights from the input values.
	output.lightViewPos = input.lightViewPos;
	output.lightViewPos2 = input.lightViewPos2;
	output.lightViewPos3 = input.lightViewPos3;

	//Send the world Position for the vertex to the Pixel Shader.
	output.wPosition = input.wPosition;

	return output;
}
