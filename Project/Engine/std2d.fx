#ifndef _STD2D
#define _STD2D

#include "value.fx"
#include "func.fx"

struct VTX_INPUT
{
    float3 vPos : POSITION; // Sementic
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;    
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;    
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    float2 vProjPos : POSITION;
    float3 vViewPos : POSITION1;
};

// ==========================
// Std2D Shader
// g_tex_0 : Output Texture
// ==========================

VTX_OUT VS_Std2D(VTX_INPUT _in)
{ 
    VTX_OUT output = (VTX_OUT) 0.f;                    
    
    float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);
        
    output.vPosition = vProjPos;
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    output.vProjPos = output.vPosition.xy;
    output.vViewPos = vViewPos.xyz;
        
    return output;
}

float4 PS_Std2D(VTX_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    if (g_Anim2DInfo[0].iAnimUse[0])
    {        
        float2 AnimUV = float2(g_Anim2DInfo[0].vLeftTop.x + g_Anim2DInfo[0].vStep.x * _in.vUV.x, g_Anim2DInfo[0].vLeftTop.y + g_Anim2DInfo[0].vStep.y * _in.vUV.y);
        vColor = g_animtex.Sample(g_sam_0, AnimUV);        
    }
    else
    {
        vColor = g_tex_0.Sample(g_sam_0, _in.vUV);        
    }    
     
    /*
    tLightColor LightColor = (tLightColor) 0.f;
        
    for (uint i = 0; i < iLight2DCount; ++i)
    {
        CalLight2D(LightColor, _in.vViewPos, i);        
    }
    
    vColor.rgb = (vColor.rgb * LightColor.vDiff.rgb) + (vColor.rgb * LightColor.vAmb.rgb);
        */
    return vColor;
}


float4 PS_SlashStd2D(VTX_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    if (g_Anim2DInfo[0].iAnimUse[0])
    {
        float2 AnimUV = float2(g_Anim2DInfo[0].vLeftTop.x + g_Anim2DInfo[0].vStep.x * _in.vUV.x, g_Anim2DInfo[0].vLeftTop.y + g_Anim2DInfo[0].vStep.y * _in.vUV.y);
        vColor = g_animtex.Sample(g_sam_0, AnimUV);
    }
    else
    {
        vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
     
    /*
    tLightColor LightColor = (tLightColor) 0.f;
        
    for (uint i = 0; i < iLight2DCount; ++i)
    {
        CalLight2D(LightColor, _in.vViewPos, i);        
    }
    
    vColor.rgb = (vColor.rgb * LightColor.vDiff.rgb) + (vColor.rgb * LightColor.vAmb.rgb);
        */
    return vColor * float4(g_float_0, g_float_1, g_float_2, 1.f);
}

// =========================
// Collider2D Shader
// g_int_0 : Collision check
// =========================
float4 VS_Collider2D(float3 _vPos : POSITION) : SV_Position
{
    float4 vPosition = (float4) 0.f;    
    vPosition = mul(float4(_vPos, 1.f), g_matWVP);
    
    return vPosition;
}

float4 PS_Collider2D(float4 _vScreenPos : SV_Position) : SV_Target
{
    if (g_int_0)
    {
        return float4(1.f, 0.f, 0.f, 1.f);
    }
    else
    {
        return float4(0.f, 1.f, 0.f, 1.f);
    }
}


// =========================
// Particle Rendering Shader
// mesh : point mesh
// g_tex_0 : particle texture
// =========================
struct VTX_PARTICLE_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;   
    uint iInstID : SV_InstanceID;
};

struct VTX_PARTICLE_OUT
{
    float4 vViewPos : POSITION;
    float2 vUV : TEXCOORD;
    float fInstID : FOG;
};

struct GS_PARTICLE_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float fInstID : FOG;
};

StructuredBuffer<tParticle> g_particelbuffer : register(t100);

VTX_PARTICLE_OUT VS_Particle(VTX_PARTICLE_IN _in)
{
    VTX_PARTICLE_OUT output = (VTX_PARTICLE_OUT) 0.f;

    float3 vWorldPos = _in.vLocalPos;
    
        
    vWorldPos += g_particelbuffer[_in.iInstID].vWorldPos;
   // float4 vViewPos = mul(float4(vWorldPos, 1.f), g_matView);
   
    
    output.vViewPos = mul(float4(vWorldPos, 1.f), g_matView);
   
    output.vUV = _in.vUV;
    output.fInstID = _in.iInstID;
    
    return output;
}

[maxvertexcount(6)]
void GS_Particle(point VTX_PARTICLE_OUT _in[1], inout TriangleStream<GS_PARTICLE_OUT> _stream)
{
    GS_PARTICLE_OUT arrVTX[4] = { (GS_PARTICLE_OUT) 0.f, (GS_PARTICLE_OUT) 0.f, (GS_PARTICLE_OUT) 0.f, (GS_PARTICLE_OUT)0.f };

    // 인스턴스 아이디로, 파티클 버퍼의 활성유무 체크
    uint instID = (uint) _in[0].fInstID;    
    if (0 == g_particelbuffer[instID].iAlive)
    {
        // 비활성 파티클인 경우 폐기
        return;
    }
    
    // 0 -- 1
    // | \  |
    // 3 -- 2
    float fRatio = g_particelbuffer[instID].m_fCurTime / g_particelbuffer[instID].m_fMaxLife;    
    float fScale = (g_vec4_1 - g_vec4_0) * fRatio + g_vec4_0;   
    
        
    arrVTX[0].vPosition = _in[0].vViewPos + float4(-fScale / 2.f, fScale / 2, 0.f, 0.f);
    arrVTX[0].vUV = float2(0.f, 0.f);
    arrVTX[0].fInstID = (float)instID;
    
    arrVTX[1].vPosition = _in[0].vViewPos + float4(fScale / 2.f, fScale / 2, 0.f, 0.f);
    arrVTX[1].vUV = float2(1.f, 0.f);
    arrVTX[1].fInstID = (float) instID;
    
    arrVTX[2].vPosition = _in[0].vViewPos + float4(fScale / 2.f, -fScale / 2, 0.f, 0.f);
    arrVTX[2].vUV = float2(1.f, 1.f);
    arrVTX[2].fInstID = (float) instID;
    
    arrVTX[3].vPosition = _in[0].vViewPos + float4(-fScale / 2.f, -fScale / 2, 0.f, 0.f);
    arrVTX[3].vUV = float2(0.f, 1.f);
    arrVTX[3].fInstID = (float) instID;
    

    for (int i = 0; i < 4; ++i)
    {
        arrVTX[i].vPosition = mul(arrVTX[i].vPosition, g_matProj);
    }
    
    _stream.Append(arrVTX[0]);
    _stream.Append(arrVTX[2]);
    _stream.Append(arrVTX[3]);
    _stream.RestartStrip();
    
    _stream.Append(arrVTX[2]);
    _stream.Append(arrVTX[0]);
    _stream.Append(arrVTX[1]);
    _stream.RestartStrip();
}


float4 PS_Particle(GS_PARTICLE_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    uint iInst = (uint) _in.fInstID;
    
    float fRatio = g_particelbuffer[iInst].m_fCurTime / g_particelbuffer[iInst].m_fMaxLife;
    vColor = (g_vec4_3 - g_vec4_2) * fRatio + g_vec4_2;
    
    if (g_particelbuffer[iInst].iPadding[0] == 0)
        return g_tex_0.Sample(g_sam_0, _in.vUV) * vColor;
    else
    {
        uint a = iInst % g_int_2;
        float2 FrmUV = float2(g_vec2_0.x + g_vec2_1.x * a, g_vec2_0.y);
        float2 AnimUV = float2(FrmUV.x + g_vec2_1.x * _in.vUV.x, FrmUV.y + g_vec2_1.y * _in.vUV.y);
        
        
       /*
        const float Deg2Rad = (3.14159265359f * 2.0) / 360.0f; //1도의 라디안값을 구한다 
       
        float rotationRadians = 90.f * Deg2Rad; //원하는 각도(디그리)의 라디안 값을 구한다 
        float s = sin(rotationRadians);
        float c = cos(rotationRadians);
        float2x2 rotationMatrix = float2x2(c, -s, s, c); //회전 2차원 행렬을 만든다 
        AnimUV -= 0.5f; //중심축을 이동시켜 가운데로 만들고 
        AnimUV = mul(rotationMatrix, AnimUV); //회전한 다음에 
        AnimUV += 0.5f; //다시 중심축 이동시켜 제자리로 만든다 
        */
        vColor = g_tex_0.Sample(g_sam_0, AnimUV) * vColor;
        //vColor.a = 1.f;
        return vColor;
    }
}

float4 PS_DegreesParticle(GS_PARTICLE_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    uint iInst = (uint) _in.fInstID;
    
    float fRatio = g_particelbuffer[iInst].m_fCurTime / g_particelbuffer[iInst].m_fMaxLife;
    vColor = (g_vec4_3 - g_vec4_2) * fRatio + g_vec4_2;
    float2 Uv = _in.vUV;
    const float Deg2Rad = (3.14159265359f * 2.0) / 360.0f; //1도의 라디안값을 구한다 
    float3 Dir = g_particelbuffer[_in.fInstID].vWorldDir;
   
    float degreesa = atan2(-Dir.z, -Dir.x); //   atan2(Dir.z, Dir.x);
    float rotationRadians = (degrees(degreesa)) * Deg2Rad; //원하는 각도(디그리)의 라디안 값을 구한다 
    float s = sin(rotationRadians);
    float c = cos(rotationRadians);
    float2x2 rotationMatrix = float2x2(c, -s, s, c); //회전 2차원 행렬을 만든다 
    _in.vUV -= 0.5f; //중심축을 이동시켜 가운데로 만들고 
    _in.vUV = mul(rotationMatrix, _in.vUV); //회전한 다음에 
    _in.vUV += 0.5f; //다시 중심축 이동시켜 제자리로 만든다 
    
    return g_tex_0.Sample(g_sam_0, _in.vUV) * vColor;
}

#endif

