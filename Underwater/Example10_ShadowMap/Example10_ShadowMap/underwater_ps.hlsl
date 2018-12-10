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
};

float4 main(InputType input) : SV_TARGET
{
	//float4 colour = texture0.Sample(sampler0, input.tex);
	float4 colour;

	//Sample a pixel a set distance from the current pixel and set the colour to that.  Distance is based on a sine wave of the y position
	colour = texture0.Sample(sampler0, input.tex + float2((cos(0.1f * input.position.y + (time * 7))) *  0.002f, 0.0f));

	return colour;

}



