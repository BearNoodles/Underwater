
struct InputType
{
	float3 position : POSITION;
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

	// Pass the vertex position into the hull shader.
	output.position = input.position;

	output.tex = input.tex;

	return output;
}