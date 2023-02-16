#include "Common.hlsl"

TextureCube SkyCubeTexture;

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float3 BitangentL : BITANGENT;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION0;
    float4 CurPosH : POSITION1;
    float4 PrevPosH : POSITION2;
};

struct PixelOut
{
    float4 Position             : SV_TARGET0;
    float4 Normal               : SV_TARGET1;
    float4 BaseColor            : SV_TARGET2;
    float4 MetallicRoughness    : SV_TARGET3;
    float4 Emissive             : SV_TARGET4;
    float2 Velocity             : SV_Target5;
};

VertexOut VS(VertexIn vertexIn)
{
    VertexOut vertexOut;

    float4 posW = float4(mul(gFarZ, vertexIn.PosL) + gEyePosW, 1.0f);
    float4 prevPosW = float4(mul(gFarZ, vertexIn.PosL) + gPrevPosW, 1.0f);

    vertexOut.PosH = mul(gJitterViewProj, posW);

    vertexOut.PosL = vertexIn.PosL;

    vertexOut.CurPosH = mul(gViewProj, posW).xyww;
    vertexOut.CurPosH.z = vertexOut.CurPosH.w * 1;
    vertexOut.PrevPosH = mul(gPrevViewProj, prevPosW);

    return vertexOut;
}

PixelOut PS(VertexOut pixelIn)
{
    PixelOut pixelOut;

    pixelOut.BaseColor = SkyCubeTexture.Sample(LinearWrapSampler, pixelIn.PosL);

    pixelOut.Position = float4(0.0f, 0.0f, 0.0f, 1.0f);
    pixelOut.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    pixelOut.MetallicRoughness = float4(0.0f, 0.0f, 0.0f, 0.0f);
    pixelOut.Emissive = float4(0.0f, 0.0f, 0.0f, 1.0f);

    float4 curPos = pixelIn.CurPosH;
    float4 prevPos = pixelIn.PrevPosH;
    curPos /= curPos.w;
    prevPos /= prevPos.w;

    curPos.xy = NDCToUV(curPos);
    prevPos.xy = NDCToUV(prevPos);

    pixelOut.Velocity = curPos.xy - prevPos.xy;

    return pixelOut;
}