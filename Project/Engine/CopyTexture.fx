#ifndef _COPYTEX
#define _COPYTEX

#include "value.fx"

RWTexture2D<float4> g_DestTex : register(u0);
Texture2D g_SrcTex : register(t13);

// Compute Shader 에서 Thread ID 구하기
// SV_GroupID           : 호출된 스레드가 속한 그룹의 3차원 인덱스 좌표
// SV_GroupThreadID     : 그룹 내에서 호출된 스레드의 3차원 인덱스 좌표
// SV_GroupIndex        : 그룹 내에서 호출된 스레드의 1차원 인덱스 좌표(3차원으로 구성된 그룹 스레드를 1행으로 배치한 기준)
// SV_DispatchTreadID   : 호출된 스레드를 모든 그룹을 기준으로 했을때의 3차원 인덱스 좌표

// g_vec2_0 : 복사를 받는 원본 텍스쳐의 해상도
[numthreads(32, 32, 1)]
void CS_CopyTex(uint3 _iThreadIdx : SV_DispatchThreadID)
{
    if (_iThreadIdx.x >= g_vec2_0.x || _iThreadIdx.y >= g_vec2_0.y)
        return;
    
    if(_iThreadIdx.y == 127)
        g_DestTex[_iThreadIdx.xy] = float4(1.f, 0.f, 0.f, 1.f);

}

#endif