#ifndef _TESS
#define _TESS

#include "value.fx"

struct VTX_IN
{
    float3 vPos : POSITION;
};

struct VTX_OUT
{
    float3 vPos : POSITION;
};

VTX_OUT VS_Tess(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPos = _in.vPos;
    
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

    level.EdgeLevel[0] = 1.f;
    level.EdgeLevel[1] = 1.f;
    level.EdgeLevel[2] = 1.f;
    
    level.InsideLevel = 1.f;
    
    return level;
}


struct HULL_OUT
{
    float3 vPos : POSITION;
};

[domain("tri")]
[partitioning("integer")]
[outputcontrolpoints(3)]
[maxtessfactor(64.f)]
[patchconstantfunc("PatchFunc")]
[outputtopology("triangle_cw")]
HULL_OUT HS_Tess(InputPatch<VTX_OUT, 3> _patch, uint _iIdx : SV_OutputControlPointID, uint _iPatchIdx : SV_PrimitiveID)
{
    HULL_OUT output = (HULL_OUT) 0.f;
    
    output.vPos = _patch[_iIdx].vPos;

    return output;
}


// --> Tessellator


struct DS_OUT
{
    float4 vPosition : SV_Position;
};

// Domain Shader
[domain("tri")]
DS_OUT DS_Tess(const OutputPatch<HULL_OUT, 3> _patch, float3 _vLocation : SV_DomainLocation, PatchLevel _patchlevel)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    float3 vLocalPos = _patch[0].vPos * _vLocation[0] + _patch[1].vPos * _vLocation[1] + _patch[2].vPos * _vLocation[2];
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    
    return output;
}

// Rasterizer


float4 PS_Tess(DS_OUT _in) : SV_Target
{
    return float4(1.f, 0.f, 1.f, 1.f);
}


#endif