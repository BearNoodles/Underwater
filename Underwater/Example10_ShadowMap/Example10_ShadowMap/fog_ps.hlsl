// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer UnderwaterBuffer : register(b0)
{
	float time;
	float3 padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float fogFactor : FOG;
};

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
	float4 fogColour;
	float4 finalColour;
	
	
	// Sample the texture pixel at this location.
	textureColour = texture0.Sample(sampler0, input.tex);
	
	// Set the colour of the fog to grey.
	fogColour = float4(0.5f, 0.5f, 0.5f, 1.0f);
	//The fog colour equation then does a linear interpolation between the texture colour and the fog colour based on the fog factor.
	
	// Calculate the final colour using the fog effect equation.
	finalColour = input.fogFactor * textureColour + (1.0 - input.fogFactor) * fogColour;
	
	return finalColour;

}



