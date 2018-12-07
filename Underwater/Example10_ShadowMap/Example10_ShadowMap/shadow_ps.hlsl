Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture0 : register(t1);
Texture2D depthMapTexture1 : register(t2);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);
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
	float4 lightViewPos0 : TEXCOORD1;
	float4 lightViewPos1 : TEXCOORD2;
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
	float depthValue0;
	float depthValue1;
	float lightDepthValue0;
	float lightDepthValue1;
	float shadowMapBias = 0.006f;
	float4 colour0 = float4(0.f, 0.f, 0.f, 1.f);
	float4 colour1 = float4(0.f, 0.f, 0.f, 1.f);
	float4 colour;
	float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
	
	// Calculate the projected texture coordinates.
	float2 pTexCoord0 = input.lightViewPos0.xy / input.lightViewPos0.w;
	pTexCoord0 *= float2(0.5, -0.5);
	pTexCoord0 += float2(0.5f, 0.5f);

	float2 pTexCoord1 = input.lightViewPos1.xy / input.lightViewPos1.w;
	pTexCoord1 *= float2(0.5, -0.5);
	pTexCoord1 += float2(0.5f, 0.5f);
	
	// Determine if the projected coordinates are in the 0 to 1 range.  If not don't do lighting.
	if ((pTexCoord0.x < 0.f || pTexCoord0.x > 1.f || pTexCoord0.y < 0.f || pTexCoord0.y > 1.f) && (pTexCoord1.x < 0.f || pTexCoord1.x > 1.f || pTexCoord1.y < 0.f || pTexCoord1.y > 1.f))
	{
		return textureColour;
	}
	
	// Sample the shadow map (get depth of geometry)
	depthValue0 = depthMapTexture0.Sample(shadowSampler, pTexCoord0).r;
	// Calculate the depth from the light.
	lightDepthValue0 = input.lightViewPos0.z / input.lightViewPos0.w;
	lightDepthValue0 -= shadowMapBias;
	
	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue0 < depthValue0)
	{
		colour0 = calculateLighting(-direction[0], input.normal, diffuse[0]);
	}

	colour0 = saturate(colour0 + ambient[0]);
	
	
	// Sample the shadow map (get depth of geometry)
	depthValue1 = depthMapTexture1.Sample(shadowSampler, pTexCoord1).r;
	// Calculate the depth from the light.
	lightDepthValue1 = input.lightViewPos1.z / input.lightViewPos1.w;
	lightDepthValue1 -= shadowMapBias;
	
	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue1 < depthValue1)
	{
		colour1 = calculateLighting(-direction[1], input.normal, diffuse[1]);
	}
	
	colour1 = saturate(colour1 + ambient[1]);

	colour = saturate(colour0 + colour1);

    return saturate(colour * textureColour);
}