#define DIRCOUNT 2

Texture2D texture1 : register(t0);
SamplerState sampler1 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[DIRCOUNT];
	matrix lightProjectionMatrix[DIRCOUNT];
};

cbuffer WaveBuffer : register(b1)
{
	float time;
	float frequency;
	float height;
	float speed;
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

	input.position.y = sin(frequency * input.position.x + time * speed) * height + input.position.y;
	//modify normals
	//input.normal.x = 1 - cos(input.position.x + time);
	//input.normal.y = abs(cos(input.position.x + time));
	input.normal.x = -cos(input.position.x + time * speed);
	input.normal.y = 1;

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

	output.tex.x = input.tex.x + (time * speed / 100);
	output.tex.y = input.tex.y;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}