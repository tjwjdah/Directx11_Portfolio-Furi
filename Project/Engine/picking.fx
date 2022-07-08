#ifndef _PICKING
#define _PICKING

#include "value.fx"
#include "func.fx"

// =========================
// Picking Shader

// g_int_0 : x face count
// g_int_1 : z face count
// g_vec4_0 : Ray Position
// g_vec4_1 : Ray Dir
// g_mat_0 : Terrain World Inv Matrix

// g_tex_0 : HeightMap Texture
// =========================

RWStructuredBuffer<float4> g_output : register(u0);

[numthreads(1024, 1, 1)]
void CS_Picking(int3 _ThreadIdx : SV_DispatchThreadID)
{
    if (!btex_0)
        return;
    
    uint iThreadIdx = _ThreadIdx.x;
    
    // 지형을 넘어서는 스레드는 리턴
    if (iThreadIdx > g_int_0 * 2 * g_int_1)
        return;
    
    // 스레드가 담당하는 지형의 행, 열 위치
    uint iRow = iThreadIdx / (uint) (g_int_0 * 2.f);
    uint iCol = iThreadIdx % (uint) (g_int_0 * 2.f);
    
    float3 vPos[3] = { (float3) 0.f, (float3) 0.f, (float3) 0.f };
    
    // 스레드가 담당하는 삼각형이 윗면인지 아랫면인지
    if (iCol % 2 == 0)
    {
        vPos[0] = float3((float) (iCol / 2) + 1.f, 0.f, (float) iRow);
        vPos[1] = float3((float) (iCol / 2), 0.f, (float) iRow);
        vPos[2] = float3((float) (iCol / 2), 0.f, (float) iRow + 1.f);
    }
    else
    {
        vPos[0] = float3((float) (iCol / 2), 0.f, (float) iRow + 1.f);
        vPos[1] = float3((float) (iCol / 2) + 1.f, 0.f, (float) iRow + 1.f);
        vPos[2] = float3((float) (iCol / 2) + 1.f, 0.f, (float) iRow);
    }
    
    // 로컬 정점의 높이를 높이맵에서 받아온다
    float2 vUV = float2(vPos[0].x / (float) g_int_0, 1.f - vPos[0].z / (float) g_int_1);
    vPos[0].y = g_tex_0.SampleLevel(g_sam_0, vUV, 0).r;
    
    vUV = float2(vPos[1].x / (float) g_int_0, 1.f - vPos[1].z / (float) g_int_1);
    vPos[1].y = g_tex_0.SampleLevel(g_sam_0, vUV, 0).r;
    
    vUV = float2(vPos[2].x / (float) g_int_0, 1.f - vPos[2].z / (float) g_int_1);
    vPos[2].y = g_tex_0.SampleLevel(g_sam_0, vUV, 0).r;
        
    
    // 최종 정점 3개와 Ray 와 충돌 검사를 진행
    float fResult = 0.f;
    if (IntersectsLay(vPos, g_vec4_0.xyz, g_vec4_1.xyz, fResult))
    {
        float3 vInter = GetIntercestsPos(vPos, g_vec4_0.xyz, g_vec4_1.xyz);
        g_output[0].xy = float2(vInter.x, vInter.z) / float2(g_int_0, g_int_1);
        g_output[0].y = 1.f - g_output[0].y;
        g_output[0].z = fResult;
    }
}

#endif