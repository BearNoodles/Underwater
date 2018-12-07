// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
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
cbuffer WaveBuffer : register(b1)
{
	float time;
	float speed;
	float height;
	float frequency;
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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos0 : TEXCOORD1;
	float4 lightViewPos1 : TEXCOORD2;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 texPos;
	OutputType output;

	float3 normal;

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
	vertexPosition.y += sin(frequency * vertexPosition.x + time * speed) * (height / 50.0f);
	//vertexPosition.y += sin(frequency * vertexPosition.z + time * speed) * (height / 50.0f) + vertexPosition.y;
	//vertexPosition.y +=  20;

	//output.tex = texPos;
	output.tex = texPos;



	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos0 = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.lightViewPos0 = mul(output.lightViewPos0, lightViewMatrix[0]);
	output.lightViewPos0 = mul(output.lightViewPos0, lightProjectionMatrix[0]);

	output.lightViewPos1 = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix[1]);
	output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix[1]);


	float2 movingNormal;
	movingNormal.x = texPos.x + (time * speed / 100);
	movingNormal.y = texPos.y;// +(time * speed / 100);

	float4 normalColour;
	normalColour = texture2.SampleLevel(sampler1, movingNormal, 0);


	//Calculate normals from height map colour
	//TODO MAYBE USE THE HEIGHT TO MULTIPLY THESE? IDK
	//ALSO TODO, MOVE THE NORMALS WITH THE MOVING TEXTURE
	output.normal.x = -lerp(-1.0f, 1.0f, normalColour.x);
	output.normal.y = lerp(-1.0f, 1.0f, normalColour.z);
	output.normal.z = lerp(-1.0f, 1.0f, normalColour.y);


	normal.y = sin(frequency * normal.x + time * speed) * (height / 50.0f) + normal.y;
	output.normal = mul(output.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// Send the input colour into the pixel shader.
	//output.colour = patch[0].colour;
	return output;
}

