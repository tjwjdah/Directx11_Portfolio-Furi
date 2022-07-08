#ifndef _PARTICLEUPDATE
#define _PARTICLEUPDATE

#include "value.fx"
#include "func.fx"

// =========================
// Particle Update Shader
// g_int_0 : max particle count
// g_float_0 : min life time
// g_float_1 : max life time
// g_float_2 : min speed
// g_float_3 : max speed
// g_vec4_0 : world position
// g_vec4_1 : create range
// g_particlebuffer : particle data array

// =========================
RWStructuredBuffer<tParticle> g_particlebuffer : register(u0);
RWStructuredBuffer<tParticleShared> g_shared : register(u1);

[numthreads(1024, 1, 1)]
void CS_ParticleUpdate(int3 _ThreadIdx : SV_DispatchThreadID)
{
    if (g_int_0 <= _ThreadIdx.x)
        return;
    g_particlebuffer[_ThreadIdx.x].iPadding[0] = g_int_1;

    // particle update
    if (0 == g_particlebuffer[_ThreadIdx.x].iAlive)
    {
        // 2. 스레드 동기화
        // - atomic function
        int iOrigin = g_shared[0].iAliveCount;
        int iExchange = 0;
        
        while (0 < iOrigin)
        {
            int iInput = iOrigin - 1;
            //락을 걸고 얼라이브 카운터랑 오리진이랑 동일할 경우에만 통과
            InterlockedCompareExchange(g_shared[0].iAliveCount, iOrigin, iInput, iExchange);
            //iAliveCount의 값과 iOrigin 값을 비교하여 만약 같으면 iInput를 iAliveCount에 저장한다.
            //iExchange에 iAliveCount원래값이 들어간다.

            // 성공
            if (iOrigin == iExchange)
            {
                // 파티클 활성화       
                g_particlebuffer[_ThreadIdx.x].iAlive = 1;
                break;
            }
            
            iOrigin = iInput;
        }
        
        if (0 == g_particlebuffer[_ThreadIdx.x].iAlive)
            return;
        
        // 활성화 된 파티클을 랜덤한 좌표, 방향으로 초기화한다.
        float fKey = _ThreadIdx.x / (float) g_int_0; // 각 스레드가 겹치지 않는 키값을 얻어내기 위해서       
        float3 vRand = float3(Rand(fKey), Rand(fKey * 2.f), Rand(fKey * 3.f)); // 3번의 랜덤을 구함(키값 중복을 피하기 위해서 배수함)
        
        int RainEnable = g_shared[0].iPadding[0]; //(22.1.3)
        float3 RainPos = g_particlebuffer[_ThreadIdx.x].vWorldPos;
        if (2 == RainEnable)//(22.1.3)
        {
            float3 vPositionRange = (vRand - 0.005f) * g_vec4_1.xyz;
            
            g_particlebuffer[_ThreadIdx.x].vWorldPos = vPositionRange + g_vec4_0.xyz;
            g_particlebuffer[_ThreadIdx.x].vWorldDir = normalize(vPositionRange);
            g_particlebuffer[_ThreadIdx.x].m_fSpeed = g_float_2 + (g_float_3 - g_float_2) * vRand.x;
            g_particlebuffer[_ThreadIdx.x].m_fCurTime = 0.f;
            g_particlebuffer[_ThreadIdx.x].m_fMaxLife = g_float_0 + (g_float_1 - g_float_0) * vRand.y;

        }
        else if (3 == RainEnable)//(22.1.3)
        {
            float3 vPositionRange = (vRand - 0.5f) * g_vec4_1.xyz;
            float3 vPositionRange2 = (vRand - 0.005f) * g_vec4_1.y;
        
            g_particlebuffer[_ThreadIdx.x].vWorldPos = vPositionRange + g_vec4_0.xyz;
            
            g_particlebuffer[_ThreadIdx.x].vWorldDir.y = -1;
            g_particlebuffer[_ThreadIdx.x].vWorldDir.x = normalize(vPositionRange);
        
            g_particlebuffer[_ThreadIdx.x].m_fSpeed = g_float_2 + (g_float_3 - g_float_2) * vRand.x;
        
            g_particlebuffer[_ThreadIdx.x].m_fCurTime = 0.f;
            g_particlebuffer[_ThreadIdx.x].m_fMaxLife = g_float_0 + (g_float_1 - g_float_0) * vRand.y;
        }
        else
        {
            // vRand 의 각 성분값이 0 ~ 1 이므로, -0.5 ~ 0.5 범위로 확장 후 범위를 곱한다.
            float3 vPositionRange = (vRand - 0.5f) * g_vec4_1.xyz;
            
        
            g_particlebuffer[_ThreadIdx.x].vWorldPos = vPositionRange + g_vec4_0.xyz;
            g_particlebuffer[_ThreadIdx.x].vWorldDir = normalize(vPositionRange);
        
            g_particlebuffer[_ThreadIdx.x].m_fSpeed = g_float_2 + (g_float_3 - g_float_2) * vRand.x;
        
            g_particlebuffer[_ThreadIdx.x].m_fCurTime = 0.f;
            g_particlebuffer[_ThreadIdx.x].m_fMaxLife = g_float_0 + (g_float_1 - g_float_0) * vRand.y;
        }
        

    }
    else
    {
        // 파티클 업데이트        
        g_particlebuffer[_ThreadIdx.x].m_fCurTime += g_DT;
        if (g_particlebuffer[_ThreadIdx.x].m_fCurTime >= g_particlebuffer[_ThreadIdx.x].m_fMaxLife)
        {
            g_particlebuffer[_ThreadIdx.x].iAlive = 0;
            return;
        }
        if (g_int_3)
            g_particlebuffer[_ThreadIdx.x].vWorldPos += g_particlebuffer[_ThreadIdx.x].vWorldDir * g_particlebuffer[_ThreadIdx.x].m_fSpeed * g_DT;
        else
            g_particlebuffer[_ThreadIdx.x].vWorldPos += -g_particlebuffer[_ThreadIdx.x].vWorldDir * g_particlebuffer[_ThreadIdx.x].m_fSpeed * g_DT;
    }
}











#endif