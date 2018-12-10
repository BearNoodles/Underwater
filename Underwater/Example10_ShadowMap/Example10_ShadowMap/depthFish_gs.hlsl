
#define DIRCOUNT 2
#define POINTCOUNT 1

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

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};


[maxvertexcount(4)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
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


	for (int i = 0; i < 4; i++)
	{
		output.position = vertices[i];
		output.position = mul(output.position, worldMatrix);
		output.position = mul(output.position, viewMatrix);
		output.position = mul(output.position, projectionMatrix);


		output.depthPosition = output.position;

		triStream.Append(output);
	}

	triStream.RestartStrip();


}