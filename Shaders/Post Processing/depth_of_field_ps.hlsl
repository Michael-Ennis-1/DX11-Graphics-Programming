// Depth of Field Pixel Shader
// Samples the depth map from the Camera's perspective, then changes the values to more usable ones, and lerps between the blurred and normal texture depending on the difference in
// depth from the centre pixel to the current one. Also can apply a cutoff for more interesting results
#include "light_h.hlsli"

// Texture and sampler registers
Texture2D normalTexture : register(t0);
Texture2D blurTexture : register(t1);
Texture2D depthTexture : register(t2);

SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

cbuffer ActiveBuffer : register(b0)
{
    float percent;
    float cutoff;
    float weight;
    float active;
};

float4 main(InputType input) : SV_TARGET
{
    // Samples the scene and blur texture, as well as sampling the depth (and generating a more appropriate value) from both the centre of the screen and the current pixel
    // Would normally divide by the Far variable (200.0f), however decided to divide by 75 to get more distinct values
    float4 textureColour = normalTexture.Sample(Sampler0, input.tex);
    float4 blurColour = blurTexture.Sample(Sampler0, input.tex);
    float depth = LinearizeDepth(depthTexture.Sample(Sampler0, input.tex).x, 0.1f, 200.0f) / 75;
    float centreDepth = LinearizeDepth(depthTexture.Sample(Sampler0, float2(0.5f, 0.5f)).x, 0.1f, 200.0f) / 75;
    
    // Only runs if depth of field is allowed, otherwise returns just the normal texture colour
    if (active)
    {
        float4 finalColour = { 0, 0, 0, 1 };
        float lerpValue;
        
        // Finds the absolute value (between 0 and 1) from the distance in depth values between the centre pixel and the current one
        lerpValue = abs(centreDepth - depth);
        
        // If the value is above the cutoff, blurs entirely. Allows for a more noticeable Depth of Field view
        if (lerpValue > cutoff)
        {
            finalColour = blurColour;
        }
        else
        {
            // Multiplies the lerp value by a weighting value, to allow for more control
            finalColour = lerp(textureColour, blurColour, lerpValue * weight);
        }
       
        return finalColour;
    }
    else
    {
        return textureColour;
        
        // Returns the depth value of the Camera Depth Texture, for testing purposes
        //return float4(depth, depth, depth, 1);
    }
}