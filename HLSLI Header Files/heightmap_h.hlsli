// Returns a height value depending on the X value of the sampled tex coordinate's colour (e.g. the lighter the colour, the larger number it will return)
float GetHeight(float u, float v, Texture2D texture0, SamplerState sampler0)
{
    float4 colour = texture0.SampleLevel(sampler0, float2(u, v), 0);
    float height = colour.x;
    return height;
}

// Calculates the light normal by vertex
float3 CalculateVertexNormal(float u, float v, float meshSize, float heightMultiplier, Texture2D texture0, SamplerState sampler0)
{
    float northH, southH, eastH, westH;
    float3 northTangent, southTangent, eastTangent, westTangent;
    float3 Result;
    float3 origin = (0, GetHeight(u, v, texture0, sampler0) * heightMultiplier, 0);
	
    // Determines how far apart the Left, Right, Up and Down vertices are from the origin
    float uvInterval = 1 / meshSize;
	
    // Determines the height of the vertex to the Left, Right, Up and Down of the origin
    eastH = GetHeight(u + uvInterval, v, texture0, sampler0) * heightMultiplier;
    westH = GetHeight(u - uvInterval, v, texture0, sampler0) * heightMultiplier;
    northH = GetHeight(u, v + uvInterval, texture0, sampler0) * heightMultiplier;
    southH = GetHeight(u, v - uvInterval, texture0, sampler0) * heightMultiplier;
    
    // Determines the length of the vector, from the Origin to either the Left, Right, Up or Down vertex
    float length = uvInterval * 100;
	
    // Subtracts the original vector from the other vectors to generate a tangent
    eastTangent = float3(length, eastH, 0) - origin;
    westTangent = float3(-length, westH, 0) - origin;
    northTangent = float3(0, northH, length) - origin;
    southTangent = float3(0, southH, -length) - origin;
	
    // Crosses the four tangents to produce four differing results, and averages them to create a more accurate normal
    Result = float3(cross(northTangent, eastTangent) + cross(eastTangent, southTangent) + cross(southTangent, westTangent) + cross(westTangent, northTangent));
	
    Result /= 4;
	
    return normalize(Result);
}

float GetHeightPixel(float u, float v, Texture2D texture0, SamplerState sampler0)
{
    float4 colour = texture0.Sample(sampler0, float2(u, v));
    float height = colour.x;
    return height;
}


float3 CalculatePixelNormal(float u, float v, float heightWidth, float heightMultiplier, Texture2D texture0, SamplerState sampler0)
{
    float3 northTangent, eastTangent, southTangent, westTangent, Result;
    float northH, eastH, southH, westH;
    
    // Determines how far apart each pixel is (the width and height is the same, so the intervals for both U and V are exactly the same)
    float uvInterval = 1.0f / heightWidth;
    
     // Determines the height of the pixel to the Left, Right, Up and Down of the origin
    float3 origin = (0, GetHeightPixel(u, v, texture0, sampler0) * heightMultiplier, 0);
    eastH = GetHeightPixel(u + uvInterval, v, texture0, sampler0) * heightMultiplier;
    westH = GetHeightPixel(u - uvInterval, v, texture0, sampler0) * heightMultiplier;
    northH = GetHeightPixel(u, v + uvInterval, texture0, sampler0) * heightMultiplier;
    southH = GetHeightPixel(u, v - uvInterval, texture0, sampler0) * heightMultiplier;
    
    // Determines the length of the vector, from the Origin to either the Left, Right, Up or Down pixel
    float length = uvInterval * 100;
    
    // Subtracts the original pixel from the other pixels to generate a tangent
    eastTangent = float3(length, eastH, 0) - origin;
    westTangent = float3(-length, westH, 0) - origin;
    northTangent = float3(0, northH, length) - origin;
    southTangent = float3(0, southH, -length) - origin;
    
    // Crosses the four tangents to produce four differing results, and averages them to create a more accurate pixel normal
    Result = float3(cross(northTangent, eastTangent) + cross(eastTangent, southTangent) + cross(southTangent, westTangent) + cross(westTangent, northTangent));
    
    Result /= 4;
    
    return normalize(Result);
}


