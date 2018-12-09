#define DIRCOUNT 2
#define SPOTCOUNT 1

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[DIRCOUNT + SPOTCOUNT];
	matrix lightProjectionMatrix[DIRCOUNT + SPOTCOUNT];
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
    float4 lightViewPos2 : TEXCOORD3;
	float3 worldPosition : TEXCOORD4;
};


OutputType main(InputType input)
{
    OutputType output;

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

	output.lightViewPos2 = mul(input.position, worldMatrix);
	output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix[2]);
	output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix[2]);

    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

	output.worldPosition = mul(input.position, worldMatrix).xyz;

	return output;
}