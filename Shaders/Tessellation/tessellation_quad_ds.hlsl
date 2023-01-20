// Tessellation domain shader
// After tessellation the domain shader processes all the vertices to create new ones based on the partitioning of the old ones
#include "heightmap_h.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// Stores world, view and projection matrix data
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    
    matrix lightViewMatrix1;
    matrix lightProjectionMatrix1;
    
    matrix lightViewMatrix2;
    matrix lightProjectionMatrix2;
};

// Stores the inside and outside factors, for determining how the tessellator will partion the quad
cbuffer TessBuffer : register(b1)
{
    float insideFactor;
    float outsideFactor;
    float2 padding;
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
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos1 : TEXCOORD2;
    float4 lightViewPos2 : TEXCOORD3;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    OutputType output;
 
    // Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
    
    // Determine the new vertex position at this partition by interpolating using the uvwCoord.y
    float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    // Also interpolate between the texture coords, to find the appropriate texture coord at this partition (slice)
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    float2 texResult = lerp(t1, t2, uvwCoord.x);
    
    // Determine the height at this partition's vertex position
    vertexPosition.y = GetHeight(texResult.x, texResult.y, texture0, sampler0) * 30;
    
    // Determine the worldPosition by multiplying the new vertex position through only the world matrix
    output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix).xyz;
    
    // Calculate the new vertex normal based on the new partitioned vertex, instead of the old factor
    output.normal = CalculateVertexNormal(texResult.x, texResult.y, 100 * insideFactor, 30, texture0, sampler0);
		    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Calculate the position of the new vertex against the world matrix as well as the directional light's orthographic and view matrix
    output.lightViewPos1 = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix1);
    output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix1);
    
    // Calculate the position of the new vertex against the world matrix as well as the spot light's projection and view matrix
    output.lightViewPos2 = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix2);
    output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix2);
    
    output.tex = texResult;

    return output;
}