

struct InputType
{
	float4 position : POSITION;
};

struct OutputType
{
	float3 position : POSITION;
};

OutputType main(InputType input)
{
	OutputType output;

	output.position = input.position;

	return output;
}