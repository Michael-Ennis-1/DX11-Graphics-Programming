// Tessellation vertex shader
// Passes forward the vertex data to the hull shader, to be split appropriately

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;

	 // Pass the vertex position into the hull shader.
    output.position = input.position;
    
    // Pass the texture coordinates to the hull shader
    output.tex = input.tex;
    
    // Pass the normals to the hull shader
    output.normal = input.normal;
    
    return output;
}
