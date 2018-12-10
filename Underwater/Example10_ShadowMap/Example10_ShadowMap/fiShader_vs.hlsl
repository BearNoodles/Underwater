struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

InputType main(InputType input)
{
	return input;
}