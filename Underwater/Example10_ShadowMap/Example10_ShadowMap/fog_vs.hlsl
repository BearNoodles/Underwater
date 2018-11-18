// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 cameraPosition;
	float fogStart;
	float fogEnd;
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
	float fogFactor : FOG;
};

OutputType main(InputType input)
{
	OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
	//We first calculate the Z coordinate of the vertex in view space.We then use that with the fog end and start position in the fog factor equation to produce a fog factor that we send into the pixel shader.

	// Calculate the camera position.
	float3 cameraPos = mul(input.position, worldMatrix);
	cameraPos = mul(cameraPosition, viewMatrix);

	// Calculate linear fog.    
	output.fogFactor = saturate((fogEnd - cameraPos.z) / (fogEnd - fogStart));

	return output;

	return output;
}