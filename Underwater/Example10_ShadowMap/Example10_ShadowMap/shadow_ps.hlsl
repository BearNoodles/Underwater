Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture0 : register(t1);
Texture2D depthMapTexture1 : register(t2);
Texture2D depthMapTexture2 : register(t3);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);
//SamplerState shadowSampler2 : register(s2);

#define DIRCOUNT 2
#define SPOTCOUNT 1

cbuffer DirLightBuffer : register(b0)
{
	float4 dAmbient[DIRCOUNT];
	float4 dDiffuse[DIRCOUNT];
	float4 dDirection[DIRCOUNT];
};

cbuffer SpotLightBuffer : register(b1)
{
	float4 sAmbient[SPOTCOUNT];
	float4 sDiffuse[SPOTCOUNT];
	float4 sPosition[SPOTCOUNT];
	float4 sDirection[SPOTCOUNT];
	float4 sAttenuation[SPOTCOUNT];
	float4 sCone[SPOTCOUNT];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos0 : TEXCOORD1;
	float4 lightViewPos1 : TEXCOORD2;
	float4 lightViewPos2 : TEXCOORD3;
	float3 worldPosition : TEXCOORD4;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float calculateAttenuation(float constant, float lin, float quadratic, float3 lightPos, float3 worldPos)
{
	float dist = distance(lightPos, worldPos);
	return 1 / (constant + (lin * dist) + (quadratic * pow(dist, 2)));
}

float4 calculateSpot(float3 position, float3 worldPos, float4 spotDirection, float spotCone, float4 spotDiffuse, float range, float3 normal)
{
	//initialise colour as black for pixels not in spotlight cone
	float4 colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//calculate light vector here without normalising to get distance between spotlight and pixel
	float3 lightVec = position - worldPos;
	float d = length(lightVec);

	//return black if too far
	if (d > range)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	//normalise light vector
	lightVec /= d;

	//set colour to spotlight diffuse and calculate spotlight cone
	colour += spotDiffuse;
	colour *= pow(max(dot(-lightVec, spotDirection), 0.0f), spotCone);

	return saturate(colour);
}

float4 main(InputType input) : SV_TARGET
{
	float depthValue0;
	float depthValue1;
	float depthValue2;
	float lightDepthValue0;
	float lightDepthValue1;
	float lightDepthValue2;
	float shadowMapBias = 0.006f;
	float4 colour0 = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 colour1 = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 colour2 = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 colour;
	float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	float sConstantFactor[SPOTCOUNT] = { sAttenuation[0].x };
	float sLinearFactor[SPOTCOUNT] = { sAttenuation[0].y };
	float sQuadraticFactor[SPOTCOUNT] = { sAttenuation[0].z };
	
	// Calculate the projected texture coordinates.
	float2 pTexCoord0 = input.lightViewPos0.xy / input.lightViewPos0.w;
	pTexCoord0 *= float2(0.5f, -0.5f);
	pTexCoord0 += float2(0.5f, 0.5f);

	float2 pTexCoord1 = input.lightViewPos1.xy / input.lightViewPos1.w;
	pTexCoord1 *= float2(0.5f, -0.5f);
	pTexCoord1 += float2(0.5f, 0.5f);

	float2 pTexCoord2 = input.lightViewPos2.xy / input.lightViewPos2.w;
	pTexCoord2 *= float2(0.5f, -0.5f);
	pTexCoord2 += float2(0.5f, 0.5f);
	
	// Determine if the projected coordinates are in the 0 to 1 range.  If not don't do lighting.
	if ((pTexCoord0.x < 0.0f || pTexCoord0.x > 1.0f || pTexCoord0.y < 0.0f || pTexCoord0.y > 1.0f) 
		&& (pTexCoord1.x < 0.0f || pTexCoord1.x > 1.0f || pTexCoord1.y < 0.0f || pTexCoord1.y > 1.0f)
		&& (pTexCoord2.x < 0.0f || pTexCoord2.x > 1.0f || pTexCoord2.y < 0.0f || pTexCoord2.y > 1.0f))
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
		colour0 = calculateLighting(-dDirection[0], input.normal, dDiffuse[0]);
	}

	colour0 = saturate(colour0 + dAmbient[0]);
	
	
	// Sample the shadow map (get depth of geometry)
	depthValue1 = depthMapTexture1.Sample(shadowSampler, pTexCoord1).r;
	// Calculate the depth from the light.
	lightDepthValue1 = input.lightViewPos1.z / input.lightViewPos1.w;
	lightDepthValue1 -= shadowMapBias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue1 < depthValue1)
	{
		colour1 = calculateLighting(-dDirection[1], input.normal, dDiffuse[1]);
	}

	colour1 = saturate(colour1 + dAmbient[1]);

	// Sample the shadow map (get depth of geometry)
	depthValue2 = depthMapTexture2.Sample(shadowSampler, pTexCoord2).r;
	// Calculate the depth from the light.
	lightDepthValue2 = input.lightViewPos2.z / input.lightViewPos2.w;
	lightDepthValue2 -= shadowMapBias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue2 < depthValue2)
	{
		float sAtten[SPOTCOUNT] = { calculateAttenuation(sConstantFactor[0], sLinearFactor[0], sQuadraticFactor[0], sPosition[0], input.worldPosition) };
		colour2 = sAmbient[0] + calculateSpot(sPosition[0], input.worldPosition, sDirection[0], sCone[0], sDiffuse[0], sAttenuation[0].w, input.normal) * sAtten[0];
	}

	//colour2 = saturate(colour2);

	colour = saturate(colour0 + colour1 + colour2);

    return saturate(colour * textureColour);
}