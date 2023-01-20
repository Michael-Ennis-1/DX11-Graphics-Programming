// Tessellation pixel shader
// After the domain shader has determined the normals and vertices, calculates the lighting and the shadows based on the shadow maps and potentially calculates per pixel normals if
// the user desires.

#include "light_h.hlsli"
#include "heightmap_h.hlsli"

Texture2D heightMapTexture : register(t0);
Texture2D shadowMap1 : register(t1);
Texture2D shadowMap2 : register(t2);

SamplerState sampler0 : register(s0);

// Stores Directional, Point and Spot light Attributes
cbuffer LightBuffer : register(b0)
{
    float4 ambientColour1;
    float4 diffuseColour1;
    float3 lightDirection1;
    float active1;
    
    float4 ambientColour2;
    float4 diffuseColour2;
    float3 lightPosition2;
    float active2;
    float dropoff2;
    float bumpMapping;
    float specIntensity;
    float specExponent;
    float3 camPos;
    float padding;
    
    float4 ambientColour3;
    float4 diffuseColour3;
    float3 lightPosition3;
    float active3;
    float3 lightDirection3;
    float cutoff;
    
};

// Stores tesselattion factors, for use when bump-mapping
cbuffer TessBuffer : register(b1)
{
    float insideFactor;
    float outsideFactor;
    float2 padding2;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos1 : TEXCOORD2;
    float4 lightViewPos2 : TEXCOORD3;
};

float4 main(InputType input) : SV_TARGET
{
    // Stores the texture colour, final colour, the individual Light's colours and the state of each light
    float4 textureColour;
    float4 totalColour = { 0, 0, 0, 1 };
    float4 lightColour[3];
    float activeStates[3] = { active1, active2, active3};

	// Samples the texture
    textureColour = heightMapTexture.Sample(sampler0, input.tex);
    
    // If using Per-Pixel normals, change the normal to be used in lighting calculations
    if (bumpMapping)
    {
        input.normal = CalculatePixelNormal(input.tex.x, input.tex.y, 2048, 30, heightMapTexture, sampler0);
    }
    
    // Calculates shadows for the Directional Light, and also calculates lighting
    lightColour[0] = shadowCalculation(lightDirection1, diffuseColour1, ambientColour1, input.normal, input.lightViewPos1, shadowMap1, 0.005f, sampler0);
    
    // Calculates lighting for the point light, as well as applying specular and attenuation values to affect those attributes
    lightColour[1] = calculatePointLighting(lightPosition2, input.worldPosition, camPos, input.normal, diffuseColour2, ambientColour2, dropoff2, specIntensity, specExponent);
    
    // Calcualtes shadows for the Spot Light, and also calculates lighting
    lightColour[2] = spotlightShadowCalculation(lightPosition3, -lightDirection3, input.worldPosition, input.lightViewPos2, input.normal, diffuseColour3, ambientColour3, cutoff, shadowMap2, 0.005f, sampler0, input.tex);
    
     // Spot light checks allow for other lights to function, so the cutoff feature doesn't apply to every light in the scene
    if (lightDirection3.x == 0 && lightDirection3.y == 0 && lightDirection3.z == 0)
    {
        lightColour[2] = float4(0, 0, 0, 1);
    }
    if (lightColour[2].x < 0 || lightColour[2].y < 0 || lightColour[2].z < 0)
    {
        lightColour[2] = float4(0, 0, 0, 1);
    }
	
    // Checks if any of the lights are active, and if so, adds the colour to total colour variable
    for (int i = 0; i < 3; i++)
    {
        if (activeStates[i])
        {
            totalColour += lightColour[i];
        }
    }
    
    
    // Returns the final colour value times the texture colour
    return totalColour * textureColour;
    
    // Returns the normal colour values, for testing purposes
    //return float4(input.normal.x, input.normal.y, input.normal.z, 1);
}