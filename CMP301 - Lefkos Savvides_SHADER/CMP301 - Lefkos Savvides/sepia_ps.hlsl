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
	//Apply the tan Function into the tex position to achieve the screen line effect.
	input.tex = input.tex + (tan(input.tex * 10) * 0.001);
	//Sample the Texture and use it to extract the values.
	float4 colour = texture0.Sample(Sampler0, input.tex);

	//Calculate Colour to achieve the unique Black and White look
	colour.rgb = (colour.r + colour.g + colour.b) / 3.0f;
	if (colour.r<0.2 || colour.r>0.9)
	{
		colour.r = 0.2; 
	}
	else
	{
		colour.r = 1.0f;
	}
	if (colour.g<0.2 || colour.g>0.9)
	{
		colour.g = 0.2;
	}
	else 
	{
		colour.g = 1.0f;
	}
	if (colour.b<0.2 || colour.b>0.9)
	{
		colour.b = 0.2;
	}
	else
	{
		colour.b = 1.0f;
	}
	


	return colour;


}