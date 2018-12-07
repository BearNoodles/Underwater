
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
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
};

[maxvertexcount(4)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
	OutputType output;

	// Move the vertex away from the point position
	//1
	output.position = input[0].position + float4(0.2f, 0.2f, 0.0f, 0.0f);
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	//output.colour = input[0].colour;
	output.tex = input[0].tex;
	output.normal = mul(input[0].normal, (float3x3) worldMatrix);
	output.normal = normalize(output.normal);
	triStream.Append(output);

	// Move the vertex away from the point position
	//2
	output.position = input[0].position + float4(-0.2f, 0.2f, 0.0f, 0.0f);
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	//output.colour = input[0].colour;
	output.tex = input[0].tex;
	output.normal = mul(input[0].normal, (float3x3) worldMatrix);
	output.normal = normalize(output.normal);
	triStream.Append(output);

	// Move the vertex away from the point position
	//3
	output.position = input[0].position + float4(0.2f, -0.2f, 0.0f, 0.0f);
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	//output.colour = input[0].colour;
	output.tex = input[0].tex;
	output.normal = mul(input[0].normal, (float3x3) worldMatrix);
	output.normal = normalize(output.normal);
	triStream.Append(output);

	// Move the vertex away from the point position
	//4
	output.position = input[0].position + float4(-0.2f, -0.2f, 0.0f, 0.0f);
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	//output.colour = input[0].colour;
	output.tex = input[0].tex;
	output.normal = mul(input[0].normal, (float3x3) worldMatrix);
	output.normal = normalize(output.normal);
	triStream.Append(output);


	triStream.RestartStrip();
}