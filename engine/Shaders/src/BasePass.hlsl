#include "Common.hlsl"

StructuredBuffer<InstanceData> gInstanceData : register(t0);

Texture2D BaseColorTexture  : register(t1);
Texture2D NormalTexture     : register(t2);
Texture2D MetallicTexture   : register(t3);
Texture2D RoughnessTexture  : register(t4);

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
    float4 Position             : SV_TARGET0;
    float4 Normal               : SV_TARGET1;
    float4 BaseColor            : SV_TARGET2;
    float4 MetallicRoughness    : SV_TARGET3;
    float4 Emissive             : SV_TARGET4;
    float2 Velocity             : SV_Target5;
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

    //vertexOut.NormalW = mul((float3x3)transpose(gInstanceData[instanceID].gInvWorld), vertexIn.NormalL);
    vertexOut.NormalW = mul((float3x3)gInstanceData[instanceID].gWorld, vertexIn.NormalL);

    vertexOut.TexC = vertexIn.TexC;
    //vertexOut.TangentW = mul((float3x3)transpose(gInstanceData[instanceID].gInvWorld), vertexIn.TangentL);
    vertexOut.TangentW = mul((float3x3)gInstanceData[instanceID].gWorld, vertexIn.TangentL);

    return vertexOut;
}

PixelOut PS(VertexOut pixelIn)
{
    PixelOut pixelOut;

    MaterialData material = cbMaterialData;

    float metallic = material.Metallic;
    float roughness = max(material.Roughness, 0.64f);
    
    pixelOut.Position = float4(pixelIn.PosW, 0.0f);

    if (material.HasAlbedoTexture == 1)
    {
        pixelOut.BaseColor = BaseColorTexture.Sample(AnisotropicWrapSampler, pixelIn.TexC);
        //pixelOut.BaseColor += material.AlbedoColor;
        //pixelOut.BaseColor *= 0.5f;
    }
    else
    {
        pixelOut.BaseColor = material.AlbedoColor;
    }

    if (material.HasNormalTexture == 1)
    {
		float4 NormalMapSample = NormalTexture.Sample(AnisotropicWrapSampler, pixelIn.TexC);
		float3 normal = NormalSampleToWorldSpace(NormalMapSample.rgb, pixelIn.NormalW, pixelIn.TangentW);
		pixelOut.Normal = float4(normalize(normal), 1.0f);
    }
    else
    {
        pixelOut.Normal = float4(normalize(pixelIn.NormalW), 1.0f);
    }

    if (material.HasMetallicTexture == 1)
    {
        metallic = MetallicTexture.Sample(AnisotropicWrapSampler, pixelIn.TexC).r;
    }

    if (material.HasRoughnessTexture == 1)
    {
        roughness = RoughnessTexture.Sample(AnisotropicWrapSampler, pixelIn.TexC).r;
    }

    pixelOut.MetallicRoughness = float4(metallic, roughness, 0.0f, 0.0f);

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
    
    pixelOut.Emissive = float4(material.Emissive, 1.0f);

    return pixelOut;
}