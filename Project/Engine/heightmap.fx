#ifndef _HEIGHTMAP
#define _HEIGHTMAP

#include "value.fx"
#include "func.fx"

// =========================
// HeightMap Shader
// g_int_0 : Width
// g_int_1 : Height
// g_int_2 : Up / Down (1 / 0)
// g_float_0 : Scale Ratio
// =========================
RWTexture2D<float> g_HeightMap : register(u0);
Texture2D g_brushtex : register(t13);
StructuredBuffer<float4> g_PickPos : register(t14);


[numthreads(32, 32, 1)]
void CS_HeightMap(int3 _ThreadIdx : SV_DispatchThreadID)
{
    float UpDown = -1;
    if (g_int_2)
        UpDown = 1.f;
    
    int Scale = (int((float) g_int_0 * g_float_0));
    int2 center = int2((int) ((float) g_int_0 * g_PickPos[0].x), (int) ((float) g_int_1 * g_PickPos[0].y));
    
    if (center.x - Scale / 2 <= _ThreadIdx.x && _ThreadIdx.x <= center.x + Scale / 2
        && center.y - Scale / 2 <= _ThreadIdx.y && _ThreadIdx.y <= center.y + Scale / 2)
    {        
        float2 vLTPos = float2(center.x - Scale / 2, center.y - Scale / 2);
        float2 vBrushUV = (float2(_ThreadIdx.xy) - vLTPos) / (float) Scale;        
        g_HeightMap[_ThreadIdx.xy] += g_DT * 1.f * g_brushtex.SampleLevel(g_sam_0, vBrushUV, 0).a * UpDown;
        
        //float2 vDist = (float2) (center - _ThreadIdx.xy);
        //float fDist = length(vDist);
        //float fMax = ((float) Scale / 2.f);        
        //float fRatio = saturate(cos((fDist / fMax) * (3.1415926535 / 2.f)));
        //g_HeightMap[_ThreadIdx.xy] += g_DT * 1.f * fRatio;
    }
    
    

}

#endif