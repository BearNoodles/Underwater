//Numbers of each type of light
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

//Outputs 2 triangles in a stream so max vertex count is 4
[maxvertexcount(4)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
	//calculate the world position here to use for a point to camera vector
	float3 wPosition = mul(input[0].position, worldMatrix);
	float3 cameraVec = cameraPos - wPosition;
	cameraVec = normalize(cameraVec);

	//Up is always constant and right vector is the cross of the camera vector and up
	float3 upVec = float3(0.0f, 1.0f, 0.0f);
	float3 rightVec = normalize(cross(cameraVec, upVec));

	//Turn these into float4s to match the screen position output type
	float4 up = float4(upVec, 0.0f);
	float4 right = float4(rightVec, 0.0f);

	OutputType output;

	//height and width of each quad to be created
	float fishSize = 0.2f;

	//Positon vertices using the right and up vector so that they always face the camera in the xz plane
	float4 vertices[4];
	vertices[0] = input[0].position + (right * fishSize) + (up * fishSize);
	vertices[1] = input[0].position - (right * fishSize) + (up * fishSize);
	vertices[2] = input[0].position + (right * fishSize) - (up * fishSize);
	vertices[3] = input[0].position - (right * fishSize) - (up * fishSize);

	//Updated output position for each vertex and append these 
	for (int i = 0; i < 4; i++)
	{
		output.position = vertices[i];
		output.position = mul(output.position, worldMatrix);
		output.position = mul(output.position, viewMatrix);
		output.position = mul(output.position, projectionMatrix);

		// Store the position value in a second input value for depth value calculations.
		output.depthPosition = output.position;

		triStream.Append(output);
	}

	triStream.RestartStrip();


}