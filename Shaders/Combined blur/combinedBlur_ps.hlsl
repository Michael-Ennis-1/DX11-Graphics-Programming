// Combined Blur pixel shader
// Calculates a basic gaussian blur, weighting the further away pixels less than the more central ones. Only blurs in the X and Y axis, for easier division

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

// Stores the screen's size 
cbuffer ScreenSizeBuffer : register(b0)
{
    float screenWidth;
    float screenHeight;
    float2 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float weight0, weight1, weight2, weight3, weight4;
    float4 colour;
    
    // Create the weights that each neighbor pixel will contribute to the blur. Overall, only 17 pixels affect the blur, 
    // but those pixels are enough to generate a decent gaussian blur.
    weight0 = 0.30;
    weight1 = 0.25 / 4;
    weight2 = 0.20 / 4;
    weight3 = 0.15 / 4;
    weight4 = 0.10 / 4;

	// Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Initializes the tex coord size, depending on the screen height and width
    float HtexelSize = 1.0f / screenWidth;
    float VtexelSize = 1.0f / screenHeight;
    
    // Samples the pixels going along the X (U) axis, taking all 8 of them plus the original texture colour
    colour += shaderTexture.Sample(SampleType, input.tex + float2(HtexelSize * -4.0f, 0.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(HtexelSize * -3.0f, 0.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(HtexelSize * -2.0f, 0.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(HtexelSize * -1.0f, 0.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(HtexelSize * 1.0f, 0.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(HtexelSize * 2.0f, 0.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(HtexelSize * 3.0f, 0.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(HtexelSize * 4.0f, 0.0f)) * weight4;
    
    // Samples the pixels going along the Y (V) axis, taking all 8 of them
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, VtexelSize * -4.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, VtexelSize * -3.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, VtexelSize * -2.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, VtexelSize * -1.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, VtexelSize * 1.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, VtexelSize * 2.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, VtexelSize * 3.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, VtexelSize * 4.0f)) * weight4;
    

	// Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}
