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
	//float4 colourOut = colour;
	//
	//colourOut.r = (colour.r * 0.393) + (colour.g * 0.769) + (colour.b * 0.189);
	//colourOut.g = (colour.r * 0.349) + (colour.g * 0.686) + (colour.b * 0.168);
	//colourOut.b = (colour.r * 0.272) + (colour.g * 0.534) + (colour.b * 0.131);

	// Determine the floating point size of a texel for a screen with this specific width.
	//float texelWidth = 100.0f / screenWidth;
	//float texelHeight = 1.0f / screenHeight;

	colour = texture0.Sample(sampler0, input.tex + float2((sin(0.1f * input.position.y + (time * 5))) *  0.001f, 0.0f));// (sin(0.05f * input.position.x + (time * 10))) *  0.001f));
	//colour = texture0.Sample(sampler0, input.tex + float2(sin(input.position.y) * texelWidth / 50, 0.0f));

	return colour;

}



