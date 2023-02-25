#include "Common.hlsl"

StructuredBuffer<InstanceData> gInstanceData;
StructuredBuffer<float4x4> gBoneTransforms;

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float3 BitangentL : BITANGENT;
    float2 TexC : TEXCOORD;
    uint4  BoneIndices : BONEINDICES; 
    float4 BoneWeights : BONEWEIGHTS; 
    uint BoneNum : BONENUM; 
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
};

VertexOut VS(VertexIn vertexIn, uint instanceID : SV_InstanceID)
{
    VertexOut vertexOut;

    float4 PosW = float4(0.0f, 0.0f, 0.0f, 0.0f);

    if (gInstanceData[instanceID].gBoneSum == 0)
    {
        // 转换到世界坐标
        PosW = mul(gInstanceData[instanceID].gWorld, float4(vertexIn.PosL, 1.0f));
    }
    else
    {
        for (uint i = 0; i < vertexIn.BoneNum; ++i)
        {
            PosW += vertexIn.BoneWeights[i] * mul(gBoneTransforms[vertexIn.BoneIndices[i]], float4(vertexIn.PosL, 1.0f));
        }

        PosW = mul(gInstanceData[instanceID].gWorld, PosW);
    }

    vertexOut.PosH = mul(gJitterViewProj, PosW);

    return vertexOut;
}

void PS(VertexOut pixelIn)
{

}