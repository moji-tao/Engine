
cbuffer cbBlurSetting
{
    int gBlurRadius;
    float w0;
    float w1;
    float w2;
    float w3;
    float w4;
    float w5;
    float w6;
    float w7;
    float w8;
    float w9;
    float w10;
};

Texture2D InputTexture;
RWTexture2D<float4> OutputTexture;

#define N 256
#define MAX_BLUR_RADIUS 5
groupshared float4 gCache[N + MAX_BLUR_RADIUS * 2];

[numthreads(N, 1, 1)]
void HorzCS(int3 groupThreadID : SV_GroupThreadID, int3 dispatchThread : SV_DispatchThreadID)
{
	float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };

    if (groupThreadID.x < gBlurRadius)
    {
        gCache[groupThreadID.x] = InputTexture[int2(max(dispatchThread.x - gBlurRadius, 0), dispatchThread.y)];
    }
    else if (groupThreadID.x >= N - gBlurRadius)
    {
        gCache[groupThreadID.x + 2 * gBlurRadius] = InputTexture[int2(min(dispatchThread.x + gBlurRadius, InputTexture.Length.x - 1), dispatchThread.y)];
    }

    gCache[groupThreadID.x + gBlurRadius] = InputTexture[min(dispatchThread.xy, InputTexture.Length.xy - 1)];

    GroupMemoryBarrierWithGroupSync();

    float4 blurColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = -gBlurRadius; i < gBlurRadius; ++i)
    {
        blurColor += weights[i + gBlurRadius] * gCache[gBlurRadius + i + groupThreadID.x];
    }

    OutputTexture[dispatchThread.xy] = blurColor;
}

[numthreads(1, N, 1)]
void VertCS(int3 groupThreadID : SV_GroupThreadID, int3 dispatchThread : SV_DispatchThreadID)
{
	float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };

    if (groupThreadID.y < gBlurRadius)
    {
        gCache[groupThreadID.y] = InputTexture[int2(dispatchThread.x, max(dispatchThread.y - gBlurRadius, 0))];
    }
    else if (groupThreadID.y >= N - gBlurRadius)
    {
        gCache[groupThreadID.y + 2 * gBlurRadius] = InputTexture[int2(dispatchThread.x, min(dispatchThread.y + gBlurRadius, InputTexture.Length.y - 1))];
    }
    
    gCache[groupThreadID.y + gBlurRadius] = InputTexture[min(dispatchThread.xy, InputTexture.Length.xy - 1)];
    
    GroupMemoryBarrierWithGroupSync();

    float4 blurColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = -gBlurRadius; i < gBlurRadius; ++i)
    {
        blurColor += weights[i + gBlurRadius] * gCache[gBlurRadius + i + groupThreadID.y];
    }

    OutputTexture[dispatchThread.xy] = blurColor;
}