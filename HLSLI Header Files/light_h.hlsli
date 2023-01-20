// Calculates directional light depending on the angle between the light's direction and the normal
float4 calculateDirectionalLighting(float3 lightDirection, float3 normal, float4 diffuse, float4 ambient)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return ambient + colour;
}

// Calculates point light by sending a light vector through the directional light, based on the pixel's world position and this light's position
float4 calculatePointLighting(float3 lightPosition, float3 worldPosition, float3 cameraPosition, float3 normal, float4 diffuse, float4 ambient, float dropoff, float specInt, float specExp)
{
    float4 colour;
    float3 lightVector, cameraVector;
    
    // Calculates the light vector and camera vector direction based on the world position, calculates directional lighting to give an appropriate colour
    lightVector = normalize(lightPosition - worldPosition);
    cameraVector = normalize(cameraPosition - worldPosition);
    colour = calculateDirectionalLighting(lightVector, normal, diffuse, ambient);
    
    // Attenuation Calculations
    float distanceToLight = length(lightPosition - worldPosition);
    float normalDistanceToLight = 1.0 - saturate(distanceToLight * dropoff);
    float attenuation = normalDistanceToLight;
    
    // Blinn specular calculations
    if (specInt > 0 && specExp > 0)
    {
        float3 mid = normalize(cameraVector + lightVector);
        float normViewAngle = saturate(dot(mid, normal));
        if (normViewAngle > 0.99f)
        {
            colour += diffuse * pow(normViewAngle, specExp) * specInt * distanceToLight;
        }
    }
    
    return colour * attenuation;
}

// Calculates spot lighting by creating an intensity based on the cutoff, and applies light colour only to pixels within the cutoff
float4 calculateSpotLighting(float3 lightPosition, float3 lightDirection, float3 worldPosition, float3 normal, float4 diffuse, float4 ambient, float cutoff)
{
    
    float4 colour;
    float3 lightVector;
    
    // Calculates the cutoff angle and halfs it, whilst making it into degrees instead of radians
    float intensity, finalCutoff = cutoff / (180 / 3.14) / 2;
    
    // Calculates the light vector direction based on the world position
    lightVector = normalize(lightPosition - worldPosition);
    
    // Calculates the intensity based on the lightVector, lightDirection and the cutoff value
    intensity = acos(dot(normalize(lightVector), normalize(lightDirection)));
    intensity = (finalCutoff - intensity) / finalCutoff;
    
    // Calculates directional lighting using the appropriate light vector to determine a colour value
    colour = calculateDirectionalLighting(lightVector, normal, diffuse, ambient);
    
    // Multiplies the colour by the intensity, meaning that if the cutoff is too large no value will show
    colour.x *= intensity;
    colour.y *= intensity;
    colour.z *= intensity;
    
    return colour;
}

// Calculates directional lighting, and calculates shadows simultaneously
float4 shadowCalculation(float3 lightDir, float4 lightDiff, float4 lightAmb, float3 lightNorm, float4 viewPos, Texture2D currentDepthMap, float bias, SamplerState shadowSampler)
{
    float4 tColour = { 0, 0, 0, 1 };
    
    // Caclulate the projected texture coordinates
    float2 projTex = viewPos.xy / viewPos.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    
    // Exits if the geometry is not in our shadow map
    if (projTex.x < 0.f || projTex.x > 1.f || projTex.y < 0.f || projTex.y > 1.f)
    {
        return float4(0, 0, 0, 1);
    }

    // Sample Shadow Map (get depth of geometry)
    float currentDepthValue = currentDepthMap.Sample(shadowSampler, projTex).r;
    
    // Calculate the depth from the view position of this light
    float lightDepthValue = viewPos.z / viewPos.w;
    lightDepthValue -= bias;
    
    // Only calculates lighting if closer than the shadowmap's depth value, otherwise in shadow
    if (lightDepthValue < currentDepthValue)
    {
        return tColour = calculateDirectionalLighting(-lightDir, lightNorm, lightDiff, lightAmb);
    }
    
    return tColour;
}

// Enhances the depth values given from the Camera's perspective through the near and far planes (0.1f and 200.0f respectively)
// Source:- https://learnopengl.com/depth-testing
float LinearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

// Calculates spot lighting by creating an intensity based on the cutoff, and applies light colour only to pixels within the cutoff, also calculates shadows
float4 spotlightShadowCalculation(float3 lightPosition, float3 lightDirection, float3 worldPosition, float4 viewPos, float3 normal, float4 diffuse, float4 ambient, float cutoff, Texture2D currentDepthMap, float bias, SamplerState shadowSampler, float2 uvTex)
{
    if (viewPos.x == 0 && viewPos.y == 0 && viewPos.z == 0)
    {
        return float4(1, 1, 0, 1);

    }
    
    float4 colour = { 0, 0, 0, 1 };
    float3 lightVector;
    float intensity, finalCutoff = cutoff / (180 / 3.14) / 2;
    
    // Calculates the light vector from the position of the spotlight to the texture coordinate
    lightVector = normalize(lightPosition - worldPosition);
    
    // Calculates the intensity of the spotlight, based on the final cutoff value
    intensity = acos(dot(normalize(lightVector), normalize(lightDirection)));
    intensity = (finalCutoff - intensity) / finalCutoff;
    
    // Set up the projected tex coords, for sampling the appropriate depth value
    float2 projTex = viewPos.xy / viewPos.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    
    // Exits if the geometry is not in our shadow map
    if (projTex.x < 0.f || projTex.x > 1.f || projTex.y < 0.f || projTex.y > 1.f)
    {
        return float4(0, 0, 0, 1);
    }
    
    // Sample Shadow Map (get depth of geometry) using LinearizeDepth to get an appropriate depth value
    float currentDepthValue = LinearizeDepth(currentDepthMap.Sample(shadowSampler, projTex).x, 0.1f, 200.0f) / 200;
    
    // Calculates the depth value from the light's view, and linearizes it to return an appropriate value
    float lightDepthValue = viewPos.z / viewPos.w;
    lightDepthValue = LinearizeDepth(lightDepthValue, 0.1f, 200.0f) / 200;
    lightDepthValue -= bias;
  
    // Calculates lighting only if not in shadow
    if (lightDepthValue < currentDepthValue)
    {
        colour = calculateDirectionalLighting(lightVector, normal, diffuse, ambient);
        colour.x *= intensity;
        colour.y *= intensity;
        colour.z *= intensity;
    }
    
    // If the intensity is less than or equal to 0, returns empty colour
    if (intensity <= 0.0f)
    {
        return float4(0, 0, 0, 1);
    }
    
    return colour;
}