#include "Common.hlsl"

StructuredBuffer<InstanceData> gInstanceData : register(t0);

Texture2D BaseColorTexture : register(t1);

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
    // H MVP后的位置  W世界变化后的位置
    float4 PosH     : SV_POSITION;
    float4 CurPosH  : POSITION0;
    float4 PrevPosH : POSITION1;
    float3 PosW     : POSITION2;
    float3 NormalW  : NORMAL;
    float2 TexC     : TEXCOORD;
    float3 TangentW : TANGENT;
};

struct PixelOut
{
    float4 Position     : SV_TARGET0;
    float4 Normal       : SV_TARGET1;
    float4 BaseColor    : SV_TARGET2;
    float4 Metallic     : SV_TARGET3;
    float2 Velocity     : SV_Target4;
};

VertexOut VS(VertexIn vertexIn, uint instanceID : SV_InstanceID)
{
    VertexOut vertexOut;

    // 转换到世界坐标
    float4 worldPos = mul(gInstanceData[instanceID].gWorld, float4(vertexIn.PosL, 1.0f));
    float4 preWorldPos = mul(gInstanceData[instanceID].gPreWorld, float4(vertexIn.PosL, 1.0f));
        
    vertexOut.PosW = worldPos.xyz;

    vertexOut.PosH = mul(gJitterViewProj, worldPos);
    
    vertexOut.CurPosH = mul(gViewProj, worldPos);
    vertexOut.PrevPosH = mul(gPrevViewProj, preWorldPos);

    vertexOut.NormalW = mul((float3x3)transpose(gInstanceData[instanceID].gInvWorld), vertexIn.NormalL);
    //vertexOut.NormalW = mul((float3x3)(gInstanceData[instanceID].gWorld), vertexIn.NormalL);
    vertexOut.TexC = vertexIn.TexC;
    vertexOut.TangentW = mul((float3x3)transpose(gInstanceData[instanceID].gInvWorld), vertexIn.TangentL);

    return vertexOut;
}

PixelOut PS(VertexOut pixelIn)
{
    PixelOut pixelOut;

    pixelOut.Position = float4(pixelIn.PosW, 1.0f);
    pixelOut.Normal = float4(pixelIn.NormalW, 1.0f);
    pixelOut.BaseColor = BaseColorTexture.Sample(AnisotropicWrapSampler, pixelIn.TexC);
    pixelOut.Metallic = 0.0f;

    if (pixelOut.BaseColor.a < 0.1f)
    {
        discard;
    }

    float4 curPos = pixelIn.CurPosH;
    float4 prevPos = pixelIn.PrevPosH;
    curPos /= curPos.w;
    prevPos /= prevPos.w;

    curPos.xy = NDCToUV(curPos);
    prevPos.xy = NDCToUV(prevPos);

    pixelOut.Velocity = curPos.xy - prevPos.xy;

    return pixelOut;
}