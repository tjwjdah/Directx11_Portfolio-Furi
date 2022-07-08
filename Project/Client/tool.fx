#ifndef _TOOL
#define _TOOL

#include "value.fx"


// =============================
// Textrue DownScale Copy Shader
// mesh : rect mesh
// g_tex_0 : origin texture
// =============================

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VTX_OUT VS_CopyTex(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.vPosition.xy = _in.vPos * 2.f;
    output.vPosition.z = 0.5f; output.vPosition.w = 1.f;    
    output.vUV = _in.vUV;
    
    return output;
}

struct PS_OUT
{
    float4 vTarget0 : SV_Target0;
    //float4 vTarget1 : SV_Target1;
    //float4 vTarget2 : SV_Target2;
    //float4 vTarget3 : SV_Target3;
    //float4 vTarget4 : SV_Target4;
    //float4 vTarget5 : SV_Target5;
    //float4 vTarget6 : SV_Target6;
    //float4 vTarget7 : SV_Target7;
};

PS_OUT PS_CopyTex(VTX_OUT _in)
{    
    PS_OUT output = (PS_OUT) 0.f;
        
    output.vTarget0.xyz = g_tex_0.Sample(g_sam_0, _in.vUV).xyz;
    //output.vTarget1.xyz = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
    //output.vTarget2.xyz = g_tex_2.Sample(g_sam_0, _in.vUV).xyz;
    //output.vTarget3.xyz = g_tex_3.Sample(g_sam_0, _in.vUV).xyz;
    //output.vTarget4.xyz = g_tex_4.Sample(g_sam_0, _in.vUV).xyz;
    //output.vTarget5.xyz = g_tex_5.Sample(g_sam_0, _in.vUV).xyz;
    //output.vTarget6.xyz = g_tex_6.Sample(g_sam_0, _in.vUV).xyz;
    //output.vTarget7.xyz = g_tex_7.Sample(g_sam_0, _in.vUV).xyz;
        
    output.vTarget0.a = 1.f;
    //output.vTarget1.a = 1.f;
    //output.vTarget2.a = 1.f;
    //output.vTarget3.a = 1.f;
    //output.vTarget4.a = 1.f;
    //output.vTarget5.a = 1.f;
    //output.vTarget6.a = 1.f;
    //output.vTarget7.a = 1.f;    
    
    return output;
}


// ======================
// Grid Shader
// AlphaBlend : true
// g_int_0 : 간격 단계
// g_float_0 : 카메라 높이에 따른 그리드의 투명도
// g_float_1 : 선의 두께
// g_vec4_0  : 카메라 위치
// g_vec4_1  : Grid 색상
// g_tex_0 : Position Target
// ======================
struct VTX_GRID_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_GRID_OUT
{
    float4 vPosition : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
    float3 vViewPos : POSITION1;
};

VTX_GRID_OUT VS_Grid(VTX_GRID_IN _in)
{
    VTX_GRID_OUT output = (VTX_GRID_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vUV = _in.vUV;

    return output;
}

float4 PS_Grid(VTX_GRID_OUT _in) : SV_Target
{
    float2 vFullUV = float2(_in.vPosition.x / g_vResolution.x, _in.vPosition.y / g_vResolution.y);
    float3 vTargetViewPos = g_tex_0.Sample(g_sam_0, vFullUV).xyz;
    
    float fLen = length(vTargetViewPos);
    float fMax = fLen * 0.01f; // 거리값의 10%를 최대보간값으로 본다.    
    
    float fDepthRatio = 1.f;
    if (vTargetViewPos.z != 0.f)
    {
        fDepthRatio = 1.f - saturate((length(_in.vViewPos) - length(vTargetViewPos)) / fMax);
    }
    
    
    // 현재 월드 위치값이 카메라로부터 멀면    
    float fDist = abs(distance(_in.vWorldPos, g_vec4_0.xyz));
    float fThickness = g_float_1 * (fDist / abs(g_vec4_0.y)) * 1.2f;

    // 현재보다 다음 단계 간격을 잡아낸다.
    int iStep = 1;
    for (int i = 0; i < g_int_0 + 1; ++i)
    {
        iStep *= 10;
    }

    if (fDist > 200.f && fDist > abs(g_vec4_0.y) * 5.f)
    {
        // 시야 멀어서 안보임
        clip(-1);
    }

    float fX = abs(_in.vWorldPos.x - (float) ((int) _in.vWorldPos.x / iStep) * iStep);
    float fZ = abs(_in.vWorldPos.z - (float) ((int) _in.vWorldPos.z / iStep) * iStep);

    if (fThickness / 2.f < fX)
        fX = (float) iStep - fX;
    if (fThickness / 2.f < fZ)
        fZ = (float) iStep - fZ;
    
    float fXRatio = 0.f;
    float fZRatio = 0.f;
    float fRatio = 0.f;

    if (fX < fThickness / 2.f || fZ < fThickness / 2.f)
    {
        fXRatio = ((fThickness / 2.f) - abs(fX)) / (fThickness / 2.f);
        fZRatio = ((fThickness / 2.f) - abs(fZ)) / (fThickness / 2.f);

        fRatio = 0.f;
        if (fXRatio > fZRatio)
            fRatio = fXRatio;
        else
            fRatio = fZRatio;

        return float4(g_vec4_1.rgb, fRatio * fDepthRatio);
    }
    
    // 현재 단계 라인
    iStep = 1;
    for (i = 0; i < g_int_0; ++i)
    {
        iStep *= 10;
    }

    fX = abs(_in.vWorldPos.x - (float) ((int) _in.vWorldPos.x / iStep) * iStep);
    fZ = abs(_in.vWorldPos.z - (float) ((int) _in.vWorldPos.z / iStep) * iStep);

    if (fThickness / 2.f < fX)
        fX = (float) iStep - fX;
    if (fThickness / 2.f < fZ)
        fZ = (float) iStep - fZ;
    
    fXRatio = 0.f;
    fZRatio = 0.f;
    fRatio = 0.f;

    if (fX < fThickness / 2.f || fZ < fThickness / 2.f)
    {
        fXRatio = ((fThickness / 2.f) - abs(fX)) / (fThickness / 2.f);
        fZRatio = ((fThickness / 2.f) - abs(fZ)) / (fThickness / 2.f);

        fRatio = 0.f;
        if (fXRatio > fZRatio)
            fRatio = fXRatio;
        else
            fRatio = fZRatio;

        return float4(g_vec4_1.rgb, fRatio * g_float_0 * fDepthRatio);
    }
    
    clip(-1);
    return float4(0.f, 0.f, 0.f, 0.f);
}

#endif