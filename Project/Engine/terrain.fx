#ifndef _TERRAIN
#define _TERRAIN

#include "value.fx"
#include "func.fx"

// ==============
// Terrain Shader
#define FaceX g_int_0
#define FaceZ g_int_1
// g_vec2_0 : HeightMap Resolution
#define CamPos g_vec4_0
// g_tex_0 : tile texture
// g_tex_1 : tile normal texture
// g_texcube_0 : Map

#define HEIGHT_MAP  g_tex_2
#define WEIGHT_MAP  g_tex_1
#define TILE_ARR    g_texarr_0
#define TILE_N_ARR  g_texarr_1
// =========================
struct VTX_IN
{
    float3 vPos : POSITION;        
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float3 vPos : POSITION; 
    float2 vUV : TEXCOORD;
};

VTX_OUT VS_Terrain(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPos = _in.vPos; 
    output.vUV = _in.vUV;
    
    return output;
}


// Hull Shader

// Patch Constant Func
struct PatchLevel
{
    float EdgeLevel[3] : SV_TessFactor;
    float InsideLevel : SV_InsideTessFactor;
};

PatchLevel PatchFunc(InputPatch<VTX_OUT, 3> _patch, uint _iPatchIdx : SV_PrimitiveID)
{
    PatchLevel level = (PatchLevel) 0.f;
    
    float3 vCamPos = CamPos;
    
    float3 vCenterPos = (_patch[0].vPos + _patch[1].vPos) / 2.f;
    float3 vSidePos = float3(vCenterPos.x - (vCenterPos.x - _patch[0].vPos.x), vCenterPos.y, vCenterPos.z);
    float3 vUpDownPos = float3(vCenterPos.x, vCenterPos.y, vCenterPos.z - (vCenterPos.z - _patch[1].vPos.z));
         
    vCenterPos = mul(float4(vCenterPos, 1.f), g_matWorld).xyz;
    vSidePos = mul(float4(vSidePos, 1.f), g_matWorld).xyz;
    vUpDownPos = mul(float4(vUpDownPos, 1.f), g_matWorld).xyz;
    
    float fCenter = CalTessLevel(vCamPos, vCenterPos, 1000.f, 4000.f, 4.f);
    float fSide = CalTessLevel(vCamPos, vSidePos, 1000.f, 4000.f, 4.f);
    float fUpDown = CalTessLevel(vCamPos, vUpDownPos, 1000.f, 4000.f, 4.f);
  
    level.EdgeLevel[0] = fUpDown;
    level.EdgeLevel[1] = fSide;
    level.EdgeLevel[2] = fCenter;
    level.InsideLevel = fCenter;
    
    return level;
}


struct HULL_OUT
{
    float3 vPos : POSITION; 
    float2 vUV : TEXCOORD;
};

[domain("tri")]
[partitioning("integer")]
[outputcontrolpoints(3)]
[maxtessfactor(64.f)]
[patchconstantfunc("PatchFunc")]
[outputtopology("triangle_cw")]
HULL_OUT HS_Terrain(InputPatch<VTX_OUT, 3> _patch, uint _iIdx : SV_OutputControlPointID, uint _iPatchIdx : SV_PrimitiveID)
{
    HULL_OUT output = (HULL_OUT) 0.f;
    
    output.vPos = _patch[_iIdx].vPos; 
    output.vUV = _patch[_iIdx].vUV;
    
    return output;
}


// --> Tessellator


struct DS_OUT
{
    float4 vPosition : SV_Position;    
    float3 vViewPos : POSITION;
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;    
    float2 vUV : TEXCOORD;
};

// Domain Shader
[domain("tri")]
DS_OUT DS_Terrain(const OutputPatch<HULL_OUT, 3> _patch, float3 _vLocation : SV_DomainLocation, PatchLevel _patchlevel)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    // 정점의 UV
    output.vUV = _patch[0].vUV * _vLocation[0] + _patch[1].vUV * _vLocation[1] + _patch[2].vUV * _vLocation[2];
    float2 vFullUV = float2(output.vUV.x / (float) FaceX, output.vUV.y / (float) FaceZ);
    
    // Local Space Position
    float3 vLocalPos = _patch[0].vPos * _vLocation[0] + _patch[1].vPos * _vLocation[1] + _patch[2].vPos * _vLocation[2];   
    
    // 높이맵의 높이값을 로컬 Y 좌표로 설정한다.
    float2 vUVStep = float2(1.f / g_vec2_0.x, 1.f / g_vec2_0.y);
    float3 vLocalStep = float3(FaceX / g_vec2_0.x, 0.f, FaceZ / g_vec2_0.y);
    vLocalPos.y = HEIGHT_MAP.SampleLevel(g_sam_0, vFullUV, 0).r;
    
    
    float3 vLeftPos = float3(vLocalPos.x - vLocalStep.x
                            , HEIGHT_MAP.SampleLevel(g_sam_0, float2(vFullUV.x - vUVStep.x, vFullUV.y), 0).r
                            , vLocalPos.z);
    
    float3 vRightPos = float3(vLocalPos.x + vLocalStep.x
                            , HEIGHT_MAP.SampleLevel(g_sam_0, float2(vFullUV.x + vUVStep.x, vFullUV.y), 0).r
                            , vLocalPos.z);
    
    float3 vUpPos = float3( vLocalPos.x
                          , HEIGHT_MAP.SampleLevel(g_sam_0, float2(vFullUV.x, vFullUV.y - vUVStep.y), 0).r
                          , vLocalPos.z + vLocalStep.z);
    
    float3 vDownPos = float3(vLocalPos.x
                          , HEIGHT_MAP.SampleLevel(g_sam_0, float2(vFullUV.x, vFullUV.y + vUVStep.y), 0).r
                          , vLocalPos.z - vLocalStep.z);
    
    
    float3 vTangent = normalize(vRightPos - vLeftPos);
    float3 vBinormal = normalize(vDownPos - vUpPos);
    float3 vNormal = normalize(cross(vTangent, vBinormal));       
    
    
    output.vViewTangent = normalize(mul(float4(vTangent, 0.f), g_matWV));
    output.vViewNormal = normalize(mul(float4(vNormal, 0.f), g_matWV));    
    output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), g_matWV));
        
    
    output.vViewPos = mul(float4(vLocalPos, 1.f), g_matWV).xyz;
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    
    
    return output;    
}

// Rasterizer


struct PS_OUT
{
    float4 vDiff : SV_Target;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
};

PS_OUT PS_Terrain(DS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // Albedo Texture Sampling
    float4 vObjectColor = (float4) 0.f;
        
    float2 vFullUV = _in.vUV / float2(FaceX, FaceZ);
        
    float4 vWeight = WEIGHT_MAP.Sample(g_sam_0, vFullUV);
    for (int i = 0; i < 4; ++i)
    {
        vObjectColor += TILE_ARR.Sample(g_sam_0, float3(_in.vUV, (float) i)) * vWeight[i];
    }
            
    // Normal Mapping
    float3 vViewNormal = _in.vViewNormal;
   
    if (btexarr_1)
    {
        vViewNormal = (float3) 0.f;
        
        float3x3 matTBN =
        {
            _in.vViewTangent
          , _in.vViewBinormal
          , _in.vViewNormal
        };
    
        float3 vTangentNormal = (float3) 0.f;
        for (i = 0; i < 4; ++i)
        {
            if (vWeight[i] == 0.f)
                continue;
            
            vTangentNormal = TILE_N_ARR.Sample(g_sam_0, float3(_in.vUV, (float) i)).xyz;
            vTangentNormal = (vTangentNormal - 0.5f) * 2.f;
            vViewNormal += normalize(mul(vTangentNormal, matTBN)) * vWeight[i];
        }
        vViewNormal = normalize(vViewNormal);
    }
           
    // 환경맵핑
    if (btexcube_0)
    {
        float3 vEye = normalize(_in.vViewPos);
        float3 vWorldEye = normalize(mul(float4(vEye, 0.f), g_matViewInv).xyz);
        float3 vWorldNormal = normalize(mul(float4(_in.vViewNormal, 0.f), g_matViewInv).xyz);
        float3 vWorldEvyRelfect = reflect(vWorldEye, vWorldNormal);
        float4 vReflectColor = g_texcube_0.Sample(g_sam_0, vWorldEvyRelfect);
        
        vObjectColor = lerp(vObjectColor, vReflectColor, 0.9f);
    }
    
    output.vDiff = vObjectColor;
    output.vNormal.xyz = vViewNormal;
    output.vPosition.xyz = _in.vViewPos;
    
    return output;
}

#endif