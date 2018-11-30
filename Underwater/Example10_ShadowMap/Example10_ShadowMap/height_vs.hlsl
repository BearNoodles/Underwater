#define DIRCOUNT 2

Texture2D texture1 : register(t0);
Texture2D texture2 : register(t1);
SamplerState sampler1 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[DIRCOUNT];
	matrix lightProjectionMatrix[DIRCOUNT];
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos0 : TEXCOORD1;
	float4 lightViewPos1 : TEXCOORD2;
};


OutputType main(InputType input)
{
    OutputType output;

	float4 textureColour;
	textureColour = texture1.SampleLevel(sampler1, input.tex, 0);

	input.position.y += textureColour.r * 40;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos0 = mul(input.position, worldMatrix);
	output.lightViewPos0 = mul(output.lightViewPos0, lightViewMatrix[0]);
	output.lightViewPos0 = mul(output.lightViewPos0, lightProjectionMatrix[0]);

	output.lightViewPos1 = mul(input.position, worldMatrix);
	output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix[1]);
	output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix[1]);

    output.tex = input.tex;

	float4 normalColour;
	normalColour = texture2.SampleLevel(sampler1, input.tex, 0);

	output.normal.x = -lerp(-1, 1, normalColour.x);
	output.normal.y = lerp(-1, 1, normalColour.z);
	output.normal.z = lerp(0, -1, normalColour.y);

    output.normal = mul(output.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

	return output;
}