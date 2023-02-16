#include "Common.hlsl"

Texture2D ColorTexture;

struct VertexIn
{
    float3 PosL : POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float2 TexC  : TEXCOORD;
};

VertexOut VS(VertexIn vertexIn)
{
    VertexOut vertexOut;
    vertexOut.PosH = float4(vertexIn.PosL, 1.0f);
    vertexOut.TexC = vertexIn.TexC;

    return vertexOut;
}

float3 ACESToneMapping(float3 Color, float AdaptedLum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	Color *= AdaptedLum;
	return (Color * (A * Color + B)) / (Color * (C * Color + D) + E);
}

float4 PS(VertexOut pixelIn) : SV_TARGET
{
    float3 color = ColorTexture.Sample(PointClampSampler, pixelIn.TexC).rgb;

    color = ACESToneMapping(color, 1.0f); 

	float Gamma = 2.2f;
    float3 CorrectColor = pow(abs(color), 1.0f / Gamma);	
	
	return float4(CorrectColor, 1.0f);
}