

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
};

OutputType main(InputType input)
{
	OutputType output;

	output.position = input.position;

	// Pass the input color into the hull shader.
	//output.colour = float4(1.0, 1.0, 1.0, 1.0);

	output.tex = input.tex;

	return output;
}