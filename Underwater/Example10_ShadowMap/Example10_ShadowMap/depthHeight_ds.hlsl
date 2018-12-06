// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
#define DIRCOUNT 2

Texture2D texture1 : register(t0);
SamplerState sampler1 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};
cbuffer WaveBuffer : register(b1)
{
	float time;
	float speed;
	float height;
	float padding;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 texPos;
	OutputType output;

	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	texPos = lerp(t1, t2, uvwCoord.x);

	float2 movingTex;
	movingTex.x = texPos.x + (time * speed / 100);
	movingTex.y = texPos.y;// +(time * speed / 100);

	float4 textureColour;
	textureColour = texture1.SampleLevel(sampler1, movingTex, 0);

	vertexPosition.y += textureColour.r * height;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);


	// Store the position value in a second input value for depth value calculations.
	output.depthPosition = output.position;

	return output;
}

