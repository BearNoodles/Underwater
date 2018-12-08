
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
	float3 cameraPos;
	float padding;
}
cbuffer WaveBuffer : register(b2)
{
	float time;
	float speed;
	float height;
	float frequency;
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
	float speed = 5;
	float3 wPosition = mul(input[0].position, worldMatrix);
	float3 cameraVec = cameraPos - wPosition;
	cameraVec = normalize(cameraVec);
	float3 upVec = float3(0.0f, 1.0f, 0.0f);
	float3 rightVec = normalize(cross(cameraVec, upVec));

	float4 up = float4(upVec, 0.0f);
	float4 right = float4(rightVec, 0.0f);

	OutputType output;

	float fishSize = 0.2f;
	float4 vertices[4];
	vertices[0] = input[0].position + (right * fishSize) + (up * fishSize);
	vertices[1] = input[0].position - (right * fishSize) + (up * fishSize);
	vertices[2] = input[0].position + (right * fishSize) - (up * fishSize);
	vertices[3] = input[0].position - (right * fishSize) - (up * fishSize);

	float2 texCoords[4];
	texCoords[0] = float2(1.0f, 0.0f);
	texCoords[1] = float2(0.0f, 0.0f);
	texCoords[2] = float2(1.0f, 1.0f);
	texCoords[3] = float2(.0f, 1.0f);

	//vertices[0] = input[0].position + (right);
	//vertices[1] = input[0].position + (up);
	//vertices[2] = input[0].position;
	//vertices[3] = input[0].position - (right);

	for (int i = 0; i < 4; i++)
	{
		output.position = vertices[i];
		output.position = mul(output.position, worldMatrix);
		output.position = mul(output.position, viewMatrix);
		output.position = mul(output.position, projectionMatrix);
		//output.colour = input[0].colour;
		output.tex = texCoords[i];
		output.normal = mul(input[0].normal, (float3x3) worldMatrix);
		output.normal = normalize(output.normal);
		triStream.Append(output);
	}

	triStream.RestartStrip();
}