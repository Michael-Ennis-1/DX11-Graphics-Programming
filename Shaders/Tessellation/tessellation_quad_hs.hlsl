// Tessellation Hull Shader
// Prepares control points for tessellation by partioning them appropriately with the inside and outside factor

// Stores the inside and outside factors, for determining how the tessellator will partion the quad
cbuffer TessBuffer : register(b0)
{
    float insideFactor;
    float outsideFactor;
    float2 padding;
};

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

// Set the InputPatch to 4, too allow for Quad Tessellation
ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    // Set the tessellation factors for the four edges of the quad
	output.edges[0] = insideFactor; 
    output.edges[1] = insideFactor;
	output.edges[2] = insideFactor;
    output.edges[3] = insideFactor;

    // Set the tessellation factor for tessallating inside the quad
	output.inside[0] = outsideFactor; 
    output.inside[1] = outsideFactor;

    // The tessellation factors determine how the Tessellator will cut the quad into smaller pieces
    return output;
}


// Set the domain to "Quad", the output control points to 4 and the InputPatch to 4 to work with the quad
[domain("quad")]
[partitioning("integer")] 
[outputtopology("triangle_ccw")] // Direction in which the quads' vertices are combined into triangles
[outputcontrolpoints(4)] // Number of times the hull shader executes
[patchconstantfunc("PatchConstantFunction")]
[maxtessfactor(64.0f)] // Maximum factor at which the tessellator can tessellate, otherwise the program crashes
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Sets the tex position for this control point
    output.tex = patch[pointId].tex;
    
    // Sets the normal for this control point
    output.normal = patch[pointId].normal;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    return output;
}