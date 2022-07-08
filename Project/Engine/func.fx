#ifndef _FUNC
#define _FUNC
#include "value.fx"
#include "struct.fx"

static float gaussian5x5[25] =
{
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
};

float gaussian5x5Sample(in int2 _uv, in Texture2D _tex)
{
    float4 fOut = (float4) 0.f;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            fOut += _tex[_uv + int2(i - 2, j - 2)] * gaussian5x5[i * 5 + j];
        }
    }
    return fOut.x;
}


// 0 ~ 1 사이 랜덤
float Rand(in float _fKey)
{
    // 랜덤 생성 위치, 방향               
    float2 vUV = float2(_fKey + g_AccTime, g_AccTime);
    vUV.y += sin(vUV.x * 2 * 3.141592);
            
    if (vUV.x > 0)
        vUV.x = frac(vUV.x);
    else
        vUV.x = 1.f - abs(vUV.x);
            
    if (vUV.y > 0)
        vUV.y = frac(vUV.y);
    else
        vUV.y = 1.f - abs(vUV.y);
            
    vUV = vUV * g_vNoiseResolution;
    
    return gaussian5x5Sample(vUV, g_noise);
}


// _lightcol : 빛을 누적받는 인자
// _vViewPos : 픽셀의 뷰스페이스 기준 좌표
// _iIdx : 광원 인덱스
void CalLight2D(inout tLightColor _lightcol, float3 _vViewPos, uint _iIdx)
{
    float fRatio = 1.f;
    
    if (1 == g_Light2DInfo[_iIdx].eType)
    {
        float4 vLightWorldPos = float4(g_Light2DInfo[_iIdx].vWorldPos.xyz, 1.f);
        float3 vViewLightPos = mul(vLightWorldPos, g_matView);
        
        float2 vDiff = vViewLightPos.xy - _vViewPos.xy;
        fRatio = saturate(1.f - length(vDiff) / g_Light2DInfo[_iIdx].fRange);
    }
    else
    {
        
    }
    
    _lightcol.vAmb += g_Light2DInfo[_iIdx].color.vAmb * fRatio;
}


// Lighting 연산 함수
tLightColor CalLight(int _LightIdx, float3 _vViewNormal, float3 _vViewPos)
{
    tLightColor outcolor = (tLightColor) 0.f;
    
    float fDiffusePow = 1.f;
    
    float fSpecPow = 1.f;
    float fDistRatio = 1.f; // 광원과의 거리에 따른 비율
    
    
    // 광원 타입 체크
    // Directional Light
    if (0 == g_Light3DInfo[_LightIdx].eType)
    {
        // 빛의 방향
        float3 vViewLightDir = normalize(mul(float4(g_Light3DInfo[_LightIdx].vDir.xyz, 0.f), g_matView));
        
        // Diffuse 세기
        // 광원의 방향과 표면의 법선벡터를 내적
        fDiffusePow = saturate(dot(-vViewLightDir, _vViewNormal));
        // saturate값범위 0~1사이

        //디퓨즈 난반사광 물체표면에 빛이 부딧혔을대 산란되는 빛 그걸 눈이 보고 물체의 색을 인식
        //ambient  환경광 빛이 반사되기때문에 빛이 안닿는부분에도 최소한 표시
        //specular 반사광 빛을 보고있는 기준으로 색이 나와야되는데 이렇게 반사된빛이 보고잇는 카메라로 들어가는 경우가있음
        //그런 부분은 유독 더 밝아보임
        
        // 반사광 세기    
        // 반사벡터와 시선벡터를 구해서 내적   
        float3 vReflect = normalize(vViewLightDir + 2 * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        //반사광은↘→↗이런식으로 반사되기때문에 빛방향에 노말벡터와 빛반대방향으로 내적한 크기를 2번 곱해서 더하면 반사벡터가 나온다
        //하지만 결국 이러한 반사광이 내눈에 들어오는지 알아야댐
        float3 vEye = normalize(_vViewPos);
        fSpecPow = saturate(dot(vReflect, -vEye));
        //반사광 쌔기도 램버트코사인법칙써서 구할수있음
        fSpecPow = pow(fSpecPow, 10);
        
    }
    
    // Point Light
    else if (1 == g_Light3DInfo[_LightIdx].eType)
    {        
        // viewspace 광원 위치와 표면의 거리에 따른 강도 비율 구하기
        float3 vViewLightPos = mul(float4(g_Light3DInfo[_LightIdx].vWorldPos.xyz, 1.f), g_matView).xyz;
        fDistRatio = 1.f - saturate(length(_vViewPos - vViewLightPos) / g_Light3DInfo[_LightIdx].fRange);
        
        float3 vViewLightDir = normalize(_vViewPos - vViewLightPos);
        
        // Diffuse 세기
        // 광원의 방향과 표면의 법선벡터를 내적
        fDiffusePow = saturate(dot(-vViewLightDir, _vViewNormal));
                       
        // 반사광 세기    
        // 반사벡터와 시선벡터를 구해서 내적
        float3 vReflect = normalize(vViewLightDir + 2 * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        float3 vEye = normalize(_vViewPos);
        fSpecPow = saturate(dot(vReflect, -vEye));
        fSpecPow = pow(fSpecPow, 20);
    }
    
    // Spot Light
    else
    {
        
    }
        
    
    outcolor.vDiff = fDiffusePow * g_Light3DInfo[_LightIdx].color.vDiff * fDistRatio;
    outcolor.vSpec = fSpecPow * g_Light3DInfo[_LightIdx].color.vSpec * fDistRatio;
    outcolor.vAmb = g_Light3DInfo[_LightIdx].color.vAmb;
    
    return outcolor;
}

int IntersectsLay(float3 _vertices[3], float3 _vStart, float3 _vDir, out float _fResult)
{
    float3 edge[2] = { (float3) 0.f, (float3) 0.f };
    edge[0] = _vertices[1].xyz - _vertices[0].xyz;
    edge[1] = _vertices[2].xyz - _vertices[0].xyz;

    float3 normal = normalize(cross(edge[0], edge[1]));
    float b = dot(normal, _vDir);

    float3 w0 = _vStart - _vertices[0].xyz;
    float a = -dot(normal, w0);
    float t = a / b;

    _fResult = t;

    float3 p = _vStart + t * _vDir;
    float uu, uv, vv, wu, wv, inverseD;
    uu = dot(edge[0], edge[0]);
    uv = dot(edge[0], edge[1]);
    vv = dot(edge[1], edge[1]);

    float3 w = p - _vertices[0].xyz;
    wu = dot(w, edge[0]);
    wv = dot(w, edge[1]);
    inverseD = uv * uv - uu * vv;
    inverseD = 1.0f / inverseD;

    float u = (uv * wv - vv * wu) * inverseD;
    if (u < 0.0f || u > 1.0f)
    {
        return 0;
    }

    float v = (uv * wu - uu * wv) * inverseD;
    if (v < 0.0f || (u + v) > 1.0f)
    {
        return 0;
    }
        
    return 1;
}


float3 GetIntercestsPos(float3 _vertices[3], float3 _vStart, float3 _vDir)
{
    // 평면의 방정식
    float3 vFlow1 = _vertices[1] - _vertices[0];
    float3 vFlow2 = _vertices[2] - _vertices[0];

    // 평면의 법선벡터
    float3 vNormal = normalize(cross(vFlow1, vFlow2));
    float fD = -dot(_vertices[0], vNormal);

    // 직선의 방정식
    float fT = (-dot(vNormal, _vStart) - fD) / dot(_vDir, vNormal);
    
    // 교점
    return float3(_vDir.x * fT + _vStart.x, _vDir.y * fT + _vStart.y, _vDir.z * fT + _vStart.z);
}


// 거리에 따른 테셀레이션 분할레벨 지정
float CalTessLevel(in float3 _vWorldCamPos, float3 _vPatchPos, float _fMin, float _fMax, float _fMaxLv)
{
    float fLen = length(_vPatchPos - _vWorldCamPos);
            
    float fLevel = (_fMaxLv - 1.f) * ((1.f - saturate((fLen - _fMin) / (_fMax - _fMin))));
    if (fLevel == _fMaxLv - 1.f)
    {
        fLevel += 1.f;
    }
        
    fLevel = pow(2, fLevel);
    
    return fLevel;
}






matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{   
    return g_arrFinalMat[(iBoneCount * _iRowIdx) + _iBoneIdx];
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
                        , inout float4 _vWeight, inout float4 _vIndices
                        , int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;
    
    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vTangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
        info.vBinormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
        info.vNormal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
    }
    
    _vPos = info.vPos;
    _vTangent = normalize(info.vTangent);
    _vBinormal = normalize(info.vBinormal);
    _vNormal = normalize(info.vNormal);
}


void Skinning(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
    }
    
    _vPos = info.vPos;
}


float encode(float4 _value)
{
    int rgba = (int(_value.x * 255.f) << 24) + (int(_value.y * 255.f) << 16) + (int(_value.z * 255.f) << 8) + int(_value.w * 255.f);
    return asfloat(rgba);
}

float4 decode(float _value)
{
    int rgba = asint(_value);
    
    float r = (float) (rgba >> 24) / 255.f;
    float g = (float) ((rgba & 0x00ff0000) >> 16) / 255.f;
    float b = (float) ((rgba & 0x0000ff00) >> 8) / 255.f;
    float a = (float) (rgba & 0x000000ff) / 255.f;
    
    return float4(r, g, b, a);
}
#endif
