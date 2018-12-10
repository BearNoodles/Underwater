

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
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