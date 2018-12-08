Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture1 : register(t1);
Texture2D depthMapTexture2 : register(t2);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler1 : register(s1);
//SamplerState shadowSampler2 : register(s2);

#define DIRCOUNT 2

cbuffer LightBuffer : register(b0)
{
	float4 ambient[DIRCOUNT];
	float4 diffuse[DIRCOUNT];
	float4 direction[DIRCOUNT];
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos1 : TEXCOORD1;
	float4 lightViewPos2 : TEXCOORD2;
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
	float depthValue1;
	float depthValue2;
	float lightDepthValue1;
	float lightDepthValue2;
	float shadowMapBias = 0.006f;
	float4 colour1 = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 colour2 = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 colour;
	float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	
	// Calculate the projected texture coordinates.
	float2 pTexCoord1 = input.lightViewPos1.xy / input.lightViewPos1.w;
	pTexCoord1 *= float2(0.5f, -0.5f);
	pTexCoord1 += float2(0.5f, 0.5f);
	
	float2 pTexCoord2 = input.lightViewPos2.xy / input.lightViewPos2.w;
	pTexCoord2 *= float2(0.5f, -0.5f);
	pTexCoord2 += float2(0.5f, 0.5f);

	// Determine if the projected coordinates are in the 0 to 1 range.  If not don't do lighting.
	if ((pTexCoord1.x < 0.0f || pTexCoord1.x > 1.0f || pTexCoord1.y < 0.0f || pTexCoord1.y > 1.0f) && (pTexCoord2.x < 0.0f || pTexCoord2.x > 1.0f || pTexCoord2.y < 0.0f || pTexCoord2.y > 1.0f))
	{
		return textureColour;
	}
	
	// Sample the shadow map (get depth of geometry)
	depthValue1 = depthMapTexture1.Sample(shadowSampler1, pTexCoord1).r;
	// Calculate the depth from the light.
	lightDepthValue1 = input.lightViewPos1.z / input.lightViewPos1.w;
	lightDepthValue1 -= shadowMapBias;
	
	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue1 < depthValue1)
	{
		colour1 = calculateLighting(-direction[0], input.normal, diffuse[0]);
	}
	
	colour1 = saturate(colour1 + ambient[0]);
	
	
	// Sample the shadow map (get depth of geometry)
	depthValue2 = depthMapTexture2.Sample(shadowSampler1, pTexCoord2).r;
	// Calculate the depth from the light.
	lightDepthValue2 = input.lightViewPos2.z / input.lightViewPos2.w;
	lightDepthValue2 -= shadowMapBias;
	
	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue2 < depthValue2)
	{
		colour2 = calculateLighting(-direction[1], input.normal, diffuse[1]);
	}
	
	//colour2 = saturate(colour2 + ambient[1]);
	
	colour = saturate(colour1 + colour2);
	
	return saturate(colour * textureColour);
}