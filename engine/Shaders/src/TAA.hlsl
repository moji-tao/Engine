#include "Common.hlsl"

Texture2D ColorBuffer;
Texture2D PrevColorTexture;
Texture2D VelocityBuffer;
Texture2D DepthTexture;

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

// Clips towards aabb center
// Ref: "Temporal Reprojection Anti-Aliasing in INSIDE"
float3 ClipAABB(float3 AabbMin, float3 AabbMax, float3 Point)
{
	float3 Center = 0.5 * (AabbMax + AabbMin);
	float3 Extend = 0.5 * (AabbMax - AabbMin);

	float3 Dist = Point - Center;
	float3 DistUnit = Dist.xyz / Extend;
	DistUnit = abs(DistUnit);
	float MaxDistUnit = max(DistUnit.x, max(DistUnit.y, DistUnit.z));

	if (MaxDistUnit > 1.0)
		return Center + Dist / MaxDistUnit;
	else
		return Point; // point inside aabb
}

float4 PS(VertexOut pixelIn) : SV_TARGET
{
    float4 color;

    float depth = DepthTexture.SampleLevel(PointClampSampler, pixelIn.TexC, 0).r;
    
    float4 curPos = UVToNDC(pixelIn.TexC, depth);
    float4 prevPosN = mul(gPrevViewProj, mul(gInvViewProj, curPos));
    prevPosN /= prevPosN.w;

    float2 uvOffset;
    
    // Velocity = NDCToUV(curPos) - NDCToUV(prevPos)
    // uv = NDCToUV(curPos) - Velocity
    float2 Velocity = VelocityBuffer.SampleLevel(PointClampSampler, pixelIn.TexC, 0).rg;

    float3 curColor = ColorBuffer.SampleLevel(PointClampSampler, pixelIn.TexC, 0).rgb;
    float3 prevColor = PrevColorTexture.SampleLevel(PointClampSampler, pixelIn.TexC - Velocity, 0).rgb;


    uint sampleCount = 9;
    float3 moment1 = 0.0f;
    float3 moment2 = 0.0f;
    int x, y;
    for (y = -1; y <= 1; ++y)
    {
        for (x = -1; x <= 1; ++x)
        {
            float2 sampleOffset = float2(x, y) * gInvRenderTargetSize;
            float2 sampleUV = pixelIn.TexC + sampleOffset;
            sampleUV = saturate(sampleUV);

            float3 c = ColorBuffer.Sample(PointClampSampler, sampleUV).rgb;
            c = max(c, 0.0f);

            moment1 += c;
            moment2 += c * c;
        }
    }

    float3 mean = moment1 / sampleCount;
    float3 sigma = sqrt(moment2 / sampleCount - mean * mean);
    float3 mmin = mean - sigma;
    float3 mmax = mean + sigma;

    prevColor = ClipAABB(mmin, mmax, prevColor);

	const float alpha = 0.1f;
    color = float4(curColor * alpha + prevColor * (1.0f - alpha), 1.0f);

    return color;
}