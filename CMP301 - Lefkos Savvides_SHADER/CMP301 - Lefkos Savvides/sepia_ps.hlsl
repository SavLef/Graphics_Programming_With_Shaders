// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

//Takes in information from the Vertex Shader
struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{

	//Sample the Texture and use it to extract the values.
	float4 colour = texture0.Sample(Sampler0, input.tex);

	float4 SepiaColor = colour;

	//Sepia Values (Mathematics found online)
	//SepiaColor.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
	//SepiaColor.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);
	//SepiaColor.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);

	//Altered Values - More Winter look
	colour.r = (colour.r * 0.393) + (colour.g * 0.769) + (colour.b * 0.189);
	colour.g = (colour.r * 0.349) + (colour.g * 0.686) + (colour.b * 0.168);
	colour.b = (colour.r * 0.272) + (colour.g * 0.534) + (colour.b * 0.131);


	return colour;


}