#ifndef _STD3D
#define _STD3D

#include "value.fx"
#include "func.fx"


struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    
    float3 vVeiwNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
};

// =========================
// Std3D Shader
// g_tex_0 : Output Texture
// g_texcube_0 : Enviroment Map
// =========================


//StructuredBuffer<tSectorInfo> g_SectorInfo : register(t75);


VTX_OUT VS_Std3D(VTX_IN _in)
{

    //상태행렬의 4행에는 이동데이터 네번째에 1이 있으면 
    //상태행렬안에서 이동 영향을 받겠다라는것 0이면 크기 회전만 받고 이동은 무시   
    //포지션은 위치를 받아야되니 1를 해야되고 노말 탄젠 바이노말은 방향정보이기때문에 0
    //XMVector3TransformNormal 는 0으로 넣어주는거고 XMVector3TransformCoord 는1
    VTX_OUT output = (VTX_OUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    // 
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    //이동은 무시됐더라도 크기는 아직 영향을 받았기때문에 노말라이즈 해줘야지 깔끔해짐
    output.vVeiwNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;
   
    return output;
}

float4 PS_Std3D(VTX_OUT _in) : SV_Target
{
    // Albedo Texture Sampling
    float4 vObjectColor = float4(1.f, 0.f, 1.f, 1.f);
    if (btex_0)
    {
        vObjectColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
        
    // Normal Mapping
    float3 vViewNormal = _in.vVeiwNormal;
    if (btex_1)
    {
        float3x3 matTBN =
        {
            _in.vViewTangent
            , _in.vViewBinormal
            , _in.vVeiwNormal
        };
        
        float3 vTangentNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        vTangentNormal = (vTangentNormal - 0.5f) * 2.f;
        vViewNormal = normalize(mul(vTangentNormal, matTBN));
    }
    
    tLightColor lightcolor = (tLightColor) 0.f;
    
    // 
    for (int i = 0; i < iLight3DCount; ++i)
    {
        tLightColor color = CalLight(i, vViewNormal, _in.vViewPos);
        lightcolor.vDiff += color.vDiff;
        lightcolor.vSpec += color.vSpec;
        lightcolor.vAmb += color.vAmb;
    }
    
    //
    if (btexcube_0)
    {
        float3 vEye = normalize(_in.vViewPos);
        float3 vWorldEye = normalize(mul(float4(vEye, 0.f), g_matViewInv).xyz);
        float3 vWorldNormal = normalize(mul(float4(_in.vVeiwNormal, 0.f), g_matViewInv).xyz);
        float3 vWorldEvyRelfect = reflect(vWorldEye, vWorldNormal);
        float4 vReflectColor = g_texcube_0.Sample(g_sam_0, vWorldEvyRelfect);
        
        vObjectColor = lerp(vObjectColor, vReflectColor, 0.9f);
    }
    
    
    float4 vColor = lightcolor.vDiff * vObjectColor
                    + lightcolor.vSpec        
                    + lightcolor.vAmb * vObjectColor;
    
   // 
    
    if (g_int_0 == 0) //
    {
        if (_in.vUV.x > g_float_0)
        {
            vColor = (float4(0.7f, 0.1f, 0.1f, 1.f));
        }
        else
        {
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        }
    
    }
    if (g_int_1 == 1) //
    {
        if (_in.vUV.x > g_float_0)
        {
            vColor = (float4(0.f, 0.1f, 0.1f, 1.f));
        }
        else
        {
            vColor = (float4(0.7f, 0.1f, 0.1f, 1.f));
            //vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        }
    }
    
    return vColor;
}

float4 PS_Std3D_Effect(VTX_OUT _in) : SV_Target //추가
{
     // Albedo Texture Sampling
    float4 vObjectColor = float4(1.f, 0.6f, 0.7f, 0.6f);
    if (btex_0)
    {
        vObjectColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
        
    // Normal Mapping
    float3 vViewNormal = _in.vVeiwNormal;
    if (btex_1)
    {
        float3x3 matTBN =
        {
            _in.vViewTangent
            , _in.vViewBinormal
            , _in.vVeiwNormal
        };
        
        float3 vTangentNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        vTangentNormal = (vTangentNormal - 0.5f) * 2.f;
        vViewNormal = normalize(mul(vTangentNormal, matTBN));
    }
    
    tLightColor lightcolor = (tLightColor) 0.f;
    
    // 광원에 의한 최종 라이팅 누적
    for (int i = 0; i < iLight3DCount; ++i)
    {
        tLightColor color = CalLight(i, vViewNormal, _in.vViewPos);
        lightcolor.vDiff += color.vDiff;
        lightcolor.vSpec += color.vSpec;
        lightcolor.vAmb += color.vAmb;
    }
    
    // 환경맵핑
    if (btexcube_0)
    {
        float3 vEye = normalize(_in.vViewPos);
        float3 vWorldEye = normalize(mul(float4(vEye, 0.f), g_matViewInv).xyz);
        float3 vWorldNormal = normalize(mul(float4(_in.vVeiwNormal, 0.f), g_matViewInv).xyz);
        float3 vWorldEvyRelfect = reflect(vWorldEye, vWorldNormal);
        float4 vReflectColor = g_texcube_0.Sample(g_sam_0, vWorldEvyRelfect);
        
        vObjectColor = lerp(vObjectColor, vReflectColor, 0.9f);
    }
    
    
    float4 vColor = lightcolor.vDiff * vObjectColor
                    + lightcolor.vSpec        
                    + lightcolor.vAmb * vObjectColor;
    
     // 부채꼴 공격 이펙
    float4 vWorldPos = mul(float4(_in.vViewPos, 1.f), g_matViewInv);
    float3 vLocalPos = mul(vWorldPos, g_matWorldInv).xyz;
    
    //1.57이 세타, float(1,0,0)이 방향벡터, vLocalPos는 지금 시작 좌표는 0,0으로 전제해서 나중에는 시작좌표랑 빼줘서 넣어줘야함
    float DotDir = dot(float3(0, 0, 1), normalize(vLocalPos));
    float Angle;
    if (DotDir > 0)
        Angle = acos(DotDir);
    else
        Angle = 6.28 - acos(DotDir);
    
    /*
    if (g_SectorInfo[0].fAngle < 6.28318530718f)
    {
        vColor = float4(1.f, 0.f, 0.f, 0.35f); // (22.1.3)
      
        if ((g_SectorInfo[0].fAngle / 2) < Angle)
        {
            clip(-1);
        }
        else if ((vLocalPos.x * vLocalPos.x) + (vLocalPos.z * vLocalPos.z) <= g_SectorInfo[0].fDistance)
        {
            clip(-1);
        }
    }
    else
    {
        vColor = float4(1.f, 0.f, 0.f, 0.35f); // (22.1.3)
      
        if ((vLocalPos.x * vLocalPos.x) + (vLocalPos.z * vLocalPos.z) <= g_SectorInfo[0].fDistance)
        {
            clip(-1);
        }
    }
    */
    
    return vColor;
}


// =========================
// Std3D Deferred Shader
// g_tex_0 : Output Texture
// g_texcube_0 : Enviroment Map
// =========================
VTX_OUT VS_Std3D_Deferred(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    if (iAnim3D)
    {
        Skinning(_in.vPos, _in.vTangent
        , _in.vBinormal, _in.vNormal
        , _in.vWeights, _in.vIndices, 0);
    }
        
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    // 로컬 스페이스의 위치 및 법선 벡터를 ViewSpace 까지 변환 후 픽셀 쉐이더로 전달
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    output.vVeiwNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;
   
    return output;
}

struct VTX_IN_INST
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
    
    // Instancing 당 데이터
    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    int iRowIdx : ROWINDEX; // Animaion 행렬 행

    uint iInstanceID : SV_InstanceID;
};

VTX_OUT VS_Std3D_Deferred_Inst(VTX_IN_INST _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    if (iAnim3D)
    {
        Skinning(_in.vPos, _in.vTangent
        , _in.vBinormal, _in.vNormal
        , _in.vWeights, _in.vIndices, _in.iRowIdx);
    }
    
    output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.vUV = _in.vUV;
    
    // 로컬 스페이스의 위치 및 법선 벡터를 ViewSpace 까지 변환 후 픽셀 쉐이더로 전달
    output.vViewPos = mul(float4(_in.vPos, 1.f), _in.matWV).xyz;
    
    output.vVeiwNormal = normalize(mul(float4(_in.vNormal, 0.f), _in.matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), _in.matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), _in.matWV)).xyz;
   
    return output;
}




struct PS_OUT
{
    float4 vDiff : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vDiffLight : SV_Target3;
};

PS_OUT PS_Std3D_Deferred(VTX_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // Albedo Texture Sampling
    float4 vObjectColor = float4(1.f, 1.f, 1.f, 1.f);
    
    
    // colider ~
    if (g_int_0 == 1)
    {
        vObjectColor = (float4(1.0f, 0.1f, 0.1f, 1.f));
    }
    else
    {
        vObjectColor = (float4(0.1f, 1.1f, 0.1f, 1.f));
    }
    // collider ~
    
    // hit ~
    if (g_int_3 == 1)
    {
        vObjectColor = (float4(1.0f, 0.1f, 0.1f, 1.f));
    }
    else
    {
        if (btex_0)
        {
            vObjectColor = g_tex_0.Sample(g_sam_0, _in.vUV);
            
            //vObjectColor *= g_vDiff;

        }
    }
        
    // Normal Mapping
    float3 vViewNormal = _in.vVeiwNormal;
    if (btex_1)
    {
        float3x3 matTBN =
        {
            _in.vViewTangent
            , _in.vViewBinormal
            , _in.vVeiwNormal
        };
        
        //float3 vTangentNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        float3 vTangentNormal;
        vTangentNormal.xy = g_tex_1.Sample(g_sam_0, _in.vUV).wy * 2 - 1;
        vTangentNormal.z = sqrt(1 - vTangentNormal.x * vTangentNormal.x - vTangentNormal.y * vTangentNormal.y);
        //vTangentNormal = (vTangentNormal - 0.5f) * 2.f;
        vViewNormal = normalize(mul(vTangentNormal, matTBN));
        //vViewNormal = vTangentNormal;

    }
       
    
    // 환경맵핑
    if (btexcube_0)
    {
        float3 vEye = normalize(_in.vViewPos);
        float3 vWorldEye = normalize(mul(float4(vEye, 0.f), g_matViewInv).xyz);
        float3 vWorldNormal = normalize(mul(float4(_in.vVeiwNormal, 0.f), g_matViewInv).xyz);
        float3 vWorldEvyRelfect = reflect(vWorldEye, vWorldNormal);
        float4 vReflectColor = g_texcube_0.Sample(g_sam_0, vWorldEvyRelfect);
        
        vObjectColor = lerp(vObjectColor, vReflectColor, 0.9f);
    }
    
    output.vDiff = vObjectColor;
    output.vNormal.xyz = vViewNormal;
    output.vPosition.xyz = _in.vViewPos;
    
    // 반사광 맵이 있다면
    if (btex_2)
    {
        float4 vMtrlCoeff = g_tex_2.Sample(g_sam_0, _in.vUV);
        output.vNormal.a = encode(vMtrlCoeff);
    }
    
    // 발광 맵이 있다면
    if (btex_3)
    {
        float4 vEmissive = g_tex_3.Sample(g_sam_0, _in.vUV);
        vEmissive *= float4(0.1f, 0.7f,1.0f, 1.0);
        output.vDiffLight.xyz = vEmissive.xyz * 1.5f;
     

    }
    
    return output;
}





// =========================
// SkyBox Shader
// mesh : sphere, cube
// g_int_0 : type
// g_tex_0 : skybox texture
// g_texcube_0 : skybox array texture
// ========================
struct VTX_SKY_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_SKY_OUT
{
    float4 vPosition : SV_Position;
    float3 vUVDir : NORMAL;
    float2 vUV : TEXCOORD;
};

VTX_SKY_OUT VS_SkyBox(VTX_SKY_IN _in)
{
    VTX_SKY_OUT output = (VTX_SKY_OUT) 0.f;
    
    float4 vViewPos = mul(float4(_in.vPos, 0.f), g_matView);
    float4 vProjPos = mul(float4(vViewPos.xyz, 1.f), g_matProj);
    vProjPos.z = vProjPos.w;
    
    output.vPosition = vProjPos;
    output.vUV = _in.vUV;
    
    output.vUVDir = normalize(_in.vPos);
    
    return output;
}

float4 PS_SkyBox(VTX_SKY_OUT _in) : SV_Target
{
    if (1 == g_int_0)
    {
        return g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    else
    {
        return g_texcube_0.Sample(g_sam_0, _in.vUVDir);
    }
}

#endif
