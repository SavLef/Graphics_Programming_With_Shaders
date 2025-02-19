//Textures
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2 : register(t2);
Texture2D depthMapTexture3 : register(t3);

//Samplers
SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);
SamplerState shadowSampler2 : register(s2);
SamplerState shadowSampler3 : register(s3);

//LightBuffer - Accommodates three lights
cbuffer LightBuffer : register(b0)
{
	float4 ambient[3];
	float4 diffuse[3];
	float3 position;
	float padding3;
	float3 position2;
	float padding4;
	float3 position3;
	float padding5;
	float3 direction;
	float padding;
	float3 direction2;
	float padding2;
	float3 direction3;
	float padding6;
};

//Data that is taken from the Vertex Shader
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

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
	//Variable Declarations
	float depthValue[3];
	float lightDepthValue[3];

	//ShadowMapBias declaration
	float shadowMapBias = 0.005f;

	//Light Colours initialise
	float4 colour = float4(0.f, 0.f, 0.f, 1.f);
	float4 colour2 = float4(0.f, 0.f, 0.f, 1.f);
	float4 colour3 = float4(0.f, 0.f, 0.f, 1.f);

	//Sample the Texture for the Colour
	float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	//Create the lightVectors from the Light's position and the world position of the Vertex.
	float3 lightVector = normalize(input.wPosition - position2);

	//Return the length of the specified vector
	float D = length(lightVector);

	//Create the lightVectors from the second Light's position and the world position of the Vertex.
	float3 lightVector2 = normalize(input.wPosition - position3);

	//Return the length of the specified vector
	float D2 = length(lightVector2);

	//Normalize Vector
	lightVector /= D;

	//Normalize Vector
	lightVector2 /= D2;

	// Calculate the projected texture coordinates.
	float2 pTexCoord = input.lightViewPos.xy / input.lightViewPos.w;
	pTexCoord *= float2(0.5, -0.5);
	pTexCoord += float2(0.5f, 0.5f);

	// Calculate the projected texture coordinates for the second light.
	float2 pTexCoord2 = input.lightViewPos2.xy / input.lightViewPos2.w;
	pTexCoord2 *= float2(0.5, -0.5);
	pTexCoord2 += float2(0.5f, 0.5f);

	// Calculate the projected texture coordinates for the third light.
	float2 pTexCoord3 = input.lightViewPos3.xy / input.lightViewPos3.w;
	pTexCoord3 *= float2(0.5, -0.5);
	pTexCoord3 += float2(0.5f, 0.5f);

	// Determine if the projected coordinates for all three lights are in the 0 to 1 range.  If not don't do lighting.
	if ((pTexCoord.x < 0.f || pTexCoord.x > 1.f || pTexCoord.y < 0.f || pTexCoord.y > 1.f) && ((pTexCoord2.x < 0.f || pTexCoord2.x > 1.f || pTexCoord2.y < 0.f || pTexCoord2.y > 1.f)) && ((pTexCoord3.x < 0.f || pTexCoord3.x > 1.f || pTexCoord3.y < 0.f || pTexCoord3.y > 1.f)))
	{
		return textureColour * ((ambient[0] + ambient[1] + ambient[2]) / 3);
	}



	// Sample the shadow map (get depth of geometry) for each light.
	depthValue[0] = depthMapTexture.Sample(shadowSampler, pTexCoord).r;

	depthValue[1] = depthMapTexture2.Sample(shadowSampler2, pTexCoord2).r;

	depthValue[2] = depthMapTexture3.Sample(shadowSampler3, pTexCoord3).r;

	// Calculate the depth from the light.
	lightDepthValue[0] = input.lightViewPos.z / input.lightViewPos.w;
	lightDepthValue[0] -= shadowMapBias;

	// Calculate the depth from the second light.
	lightDepthValue[1] = input.lightViewPos2.z / input.lightViewPos2.w;
	lightDepthValue[1] -= shadowMapBias;

	// Calculate the depth from the third light.
	lightDepthValue[2] = input.lightViewPos3.z / input.lightViewPos3.w;
	lightDepthValue[2] -= shadowMapBias;

	//Attenuation and Angle for Spotlight Number 1.
	float attenuation = 1;
	float angle = dot(lightVector, normalize(direction2));

	//Attenuation and Angle for Spotlight Number 2.
	float attenuation2 = 0.1;
	float angle2 = dot(lightVector2, normalize(direction3));

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue[0] < depthValue[0])
	{
		colour = calculateLighting(-direction, input.normal, diffuse[0]);
	}

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue[1] < depthValue[1])
	{
	// Light only when the angle is above 0.8.
	if (angle > 0.8)
	{
		colour2 = calculateLighting(-lightVector, input.normal, diffuse[1]);
	}
	//If it's between 0.8, 0.65, smoothstep it to create smooth Shadows.
	else if (angle >= 0.65 && angle <= 0.8)
	{
		colour2 = calculateLighting(-lightVector, input.normal, diffuse[1] * smoothstep(0.65, 0.8, angle));
	}
	}

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue[2] < depthValue[2])
	{
		// Light only when the angle is above 0.9.
		if (angle2 > 0.9)
		{
			colour3 = calculateLighting(-lightVector2, input.normal, diffuse[2]);
		}
		//If it's between 0.8, 0.9, smoothstep it to create smooth Shadows.
		else if (angle2 >= 0.8 && angle2 <= 0.9)
		{
			colour3 = calculateLighting(-lightVector2, input.normal, diffuse[2] * smoothstep(0.65, 0.9, angle2));
		}
	}


	//Saturate the three different colours with their respective ambience
	  //colour = saturate(colour + ambient[0]);
	  //colour2 = saturate(colour2 + ambient[1]);
	  //colour3 = saturate(colour3 + ambient[2]);

	return  (colour + colour2+ colour3) * textureColour;
}