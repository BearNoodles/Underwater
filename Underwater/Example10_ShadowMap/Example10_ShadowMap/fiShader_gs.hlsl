
#define DIRCOUNT 2
#define POINTCOUNT 1

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[DIRCOUNT + POINTCOUNT];
	matrix lightProjectionMatrix[DIRCOUNT + POINTCOUNT];
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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos0 : TEXCOORD1;
	float4 lightViewPos1 : TEXCOORD2;
	float4 lightViewPos2 : TEXCOORD3;
	float3 worldPosition : TEXCOORD4;
};


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

	float2 texCoords[4];
	texCoords[0] = float2(1.0f, 0.0f);
	texCoords[1] = float2(0.0f, 0.0f);
	texCoords[2] = float2(1.0f, 1.0f);
	texCoords[3] = float2(0.0f, 1.0f);

	//Normal is always facing the player in the xz plane but never points up or down
	float3 normal = float3(cameraVec.x, 0, cameraVec.z);

	//Updated output position for each vertex and append these 
	for (int i = 0; i < 4; i++)
	{
		output.position = vertices[i];
		output.position = mul(output.position, worldMatrix);
		output.position = mul(output.position, viewMatrix);
		output.position = mul(output.position, projectionMatrix);

		output.tex = texCoords[i];
		output.normal = mul(normalize(normal), (float3x3) worldMatrix);
		output.normal = normalize(output.normal);

		// Calculate the position of the vertice as viewed by the light source.
		output.lightViewPos0 = mul(vertices[i], worldMatrix);
		output.lightViewPos0 = mul(output.lightViewPos0, lightViewMatrix[0]);
		output.lightViewPos0 = mul(output.lightViewPos0, lightProjectionMatrix[0]);

		output.lightViewPos1 = mul(vertices[i], worldMatrix);
		output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix[1]);
		output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix[1]);

		output.lightViewPos2 = mul(vertices[i], worldMatrix);
		output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix[2]);
		output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix[2]);


		output.worldPosition = mul(vertices[i], worldMatrix).xyz;

		triStream.Append(output);
	}

	triStream.RestartStrip();


}