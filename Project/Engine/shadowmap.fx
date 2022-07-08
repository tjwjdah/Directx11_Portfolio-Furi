#ifndef _SHADOWMAP
#define _SHADOWMAP

#include "value.fx"
#include "func.fx"

// =================
// Shadow Map Shader
// =================
struct VS_ShadowIn
{
    float3 vPos : POSITION;
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_ShadowOut
{
    float4 vPos : SV_Position;
    float4 vProj : POSITION;
};

VS_ShadowOut VS_ShadowMap(VS_ShadowIn _in)
{
    VS_ShadowOut output = (VS_ShadowOut) 0.f;
    
    if (iAnim3D)
    {
        Skinning(_in.vPos, _in.vWeights, _in.vIndices, 0);
    }
    
    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vProj = output.vPos;

    return output;
}

struct VS_ShadowIn_Inst
{
    float3 vPos : POSITION;
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
    
    // Instancing 당 데이터
    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    int iRowIdx : ROWINDEX; // Animaion 행렬 행

    uint iInstanceID : SV_InstanceID;
};

VS_ShadowOut VS_ShadowMap_Inst(VS_ShadowIn_Inst _in)
{
    VS_ShadowOut output = (VS_ShadowOut) 0.f;
    
    if (iAnim3D)
    {
        Skinning(_in.vPos, _in.vWeights, _in.vIndices, _in.iRowIdx);
    }
    
    output.vPos = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.vProj = output.vPos;

    return output;
}

float4 PS_ShadowMap(VS_ShadowOut _input) : SV_Target
{
    return float4(_input.vProj.z / _input.vProj.w, 0.f, 0.f, 0.f);
}

#endif