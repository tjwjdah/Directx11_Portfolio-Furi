#ifndef _WEIGHTMAP
#define _WEIGHTMAP

#include "value.fx"

// =======================
// CS_Weight
// WeightTex : 출력 텍스쳐 (weight texture)
// g_arrtex_0 : Brush Array Texture

// g_int_0 : Tile Index (가중치를 올릴 타일)
// g_int_1 : Brush Index
// g_float_0 : 브러쉬 크기
// g_vec2_0 : 가중치 텍스쳐 해상도
// =======================
RWTexture2D<float4> WeightTex : register(u0);
StructuredBuffer<float4> g_PickPos : register(t14);

[numthreads(32, 32, 1)] // 1024
void CS_WeightMap(int3 _iThreadID : SV_DispatchThreadID)
{
    float2 vBrushPos = g_PickPos[0].xy;
    float2 vLTPos = vBrushPos - (g_float_0 / 2.f); // Brush 좌상단 위치    
    float2 vThreadPos = float2((float) _iThreadID.x / (float) g_vec2_0.x, (float) _iThreadID.y / (float) g_vec2_0.y); // 스레드의 위치값
    
    float2 vBrushUV = (vThreadPos - vLTPos) / g_float_0; // 브러쉬 추출 UV 값
    
    if (0.f <= vBrushUV.x && vBrushUV.x <= 1.f
        && 0.f <= vBrushUV.y && vBrushUV.y <= 1.f)
    {
        float fRatio = saturate(cos((distance(vBrushPos, vThreadPos) / (g_float_0 / 2.f)) * (3.141592 / 2.f)));
        
        float vAlpha = g_texarr_0.SampleLevel(g_sam_0, float3(vBrushUV, g_int_1), 0).a;
        if (vAlpha != 0.f)
        {
            float fAdd = g_DT * fRatio * vAlpha * 4.f;
            float4 vOrigin = WeightTex[_iThreadID.xy];
            
            if (g_int_0 == 0)
            {
                vOrigin[0] += fAdd;
                if (vOrigin[0] > 1.f)
                    vOrigin[0] = 1.f;
                
                float fOver = vOrigin[0] + vOrigin[1] + vOrigin[2] + vOrigin[3] - 1.f;
                if (fOver < 0.f)
                    fOver = 0.f;
                
                float fRest = vOrigin[1] + vOrigin[2] + vOrigin[3];
                if (fRest == 0.f)
                    fRest = 1.f;
                
                vOrigin[1] -= fOver * (vOrigin[1] / fRest);
                vOrigin[2] -= fOver * (vOrigin[2] / fRest);
                vOrigin[3] -= fOver * (vOrigin[3] / fRest);
                
                WeightTex[_iThreadID.xy] = vOrigin;
            }
            else if (g_int_0 == 1)
            {
                vOrigin[1] += fAdd;
                if (vOrigin[1] > 1.f)
                    vOrigin[1] = 1.f;
                
                float fOver = vOrigin[0] + vOrigin[1] + vOrigin[2] + vOrigin[3] - 1.f;
                if (fOver < 0.f)
                    fOver = 0.f;
                
                float fRest = vOrigin[0] + vOrigin[2] + vOrigin[3];
                if (fRest == 0.f)
                    fRest = 1.f;
                
                vOrigin[0] -= fOver * (vOrigin[0] / fRest);
                vOrigin[2] -= fOver * (vOrigin[2] / fRest);
                vOrigin[3] -= fOver * (vOrigin[3] / fRest);
                
                WeightTex[_iThreadID.xy] = vOrigin;
            }
            else if (g_int_0 == 2)
            {
                vOrigin[2] += fAdd;
                if (vOrigin[2] > 1.f)
                    vOrigin[2] = 1.f;
                
                float fOver = vOrigin[0] + vOrigin[1] + vOrigin[2] + vOrigin[3] - 1.f;
                if (fOver < 0.f)
                    fOver = 0.f;
                
                float fRest = vOrigin[0] + vOrigin[1] + vOrigin[3];
                if (fRest == 0.f)
                    fRest = 1.f;
                
                vOrigin[0] -= fOver * (vOrigin[0] / fRest);
                vOrigin[1] -= fOver * (vOrigin[1] / fRest);
                vOrigin[3] -= fOver * (vOrigin[3] / fRest);
                
                WeightTex[_iThreadID.xy] = vOrigin;
            }
            else if (g_int_0 == 3)
            {
                vOrigin[3] += fAdd;
                if (vOrigin[3] > 1.f)
                    vOrigin[3] = 1.f;
                
                float fOver = vOrigin[0] + vOrigin[1] + vOrigin[2] + vOrigin[3] - 1.f;
                if (fOver < 0.f)
                    fOver = 0.f;
                
                float fRest = vOrigin[0] + vOrigin[1] + vOrigin[2];
                if (fRest == 0.f)
                    fRest = 1.f;
                
                vOrigin[0] -= fOver * (vOrigin[0] / fRest);
                vOrigin[1] -= fOver * (vOrigin[1] / fRest);
                vOrigin[2] -= fOver * (vOrigin[2] / fRest);
                
                WeightTex[_iThreadID.xy] = vOrigin;
            }
            
            //vOrigin.a = 1.f;
            //WeightTex[_iThreadID.xy] = vOrigin;
        }
    }
   
}


#endif