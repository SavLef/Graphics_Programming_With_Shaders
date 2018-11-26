// Tessellation pixel shader
// Output colour passed to stage.
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
   return texture0.Sample(sampler0, input.tex);
}