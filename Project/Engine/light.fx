#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

// ========================
// Directional Light Shader
// mesh : rect mesh
// blend state : One - One
// depth stencil state : NoTest, NoWrite
// g_tex_0 : Normal Target
// g_tex_1 : Position Target
// g_int_0 : Light Index

#define DYNAMIC_SHADOW_MAP      g_tex_3 // 동적 쉐도우 맵
#define LightCamViewProjMat     g_mat_0 // 광원 시점으로의 View * Proj 행렬
// ========================
struct VS_DIR_IN
{
    float3 vPos : POSITION;
};

struct VS_DIR_OUT
{
    float4 vPosition : SV_Position;
};

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
};

VS_DIR_OUT VS_DirLight(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;
    
    output.vPosition.xy = _in.vPos * 2.f;
    output.vPosition.zw = 1.f;
    
    return output;
}

PS_OUT PS_DirLight(VS_DIR_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
            
    float2 vScreenUV = _in.vPosition.xy / g_vResolution;
    
    float4 vViewNormal = g_tex_0.Sample(g_sam_0, vScreenUV);
    float3 vViewPosition = g_tex_1.Sample(g_sam_0, vScreenUV).xyz;
    
    if (vViewPosition.z == 0.f)
    {
        clip(-1);
    }     
    
    tLightColor lightColor = CalLight(g_int_0, vViewNormal.xyz, vViewPosition);    

    // 그림자 판정    
    // 빛이 없으면 그림자 처리를 하지 않는다.
    if (dot(lightColor.vDiff, lightColor.vDiff) != 0.f)
    {
        // 메인카메라 view 역행렬을 곱해서 월드좌표를 알아낸다.
        float4 vWorldPos = mul(float4(vViewPosition.xyz, 1.f), g_matViewInv); 
        
        //월드 좌표를 광원 시점으로 투영시킨 좌표 구하기
        float4 vShadowProj = mul(vWorldPos, LightCamViewProjMat); 
        //LightCamViewProjMat 광원 시점으로의 View * Proj 행렬

        // w 로 나눠서 실제 투영좌표 z 값을 구한다.(올바르게 비교하기 위해서)
        float fDepth = vShadowProj.z / vShadowProj.w;

        // 계산된 투영좌표를 UV 좌표로 변경
        float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
            , (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);

        if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f
            && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
        {
            //hadowMap 에 기록된 깊이값을 꺼내온다.
            float fShadowDepth = DYNAMIC_SHADOW_MAP.Sample(g_sam_0, vShadowUV).r;

            //추출한값과 투영시킨값을 비교를함 비교했는데 원래 기록되있는값이 더 작다
            //그말은 해당지금 비교한 지점이 광원시점에 기록된 물체의깊이보다 나를 이쪽으로 투영한게 멀다는건
            //나는 광원이 봤을대 물체뒤에있다는것 가려져있다는것

            // 그림자인 경우 빛을 약화시킨다.
            // 투영시킨 값이랑, 기록된 값이 같은경우는 그림자가 생기면 안되는데, 실수오차가 발생할 수 있기 때문에 보정값을 준다.
            if (fShadowDepth != 0.f && (fDepth > fShadowDepth + 0.00001f))
            {
                lightColor.vDiff *= 0.3f;
                lightColor.vSpec = (float4) 0.f;
            }
        }
    }


    // 그림자 판정    
    // 빛이 없으면 그림자 처리를 하지 않는다.
    if (dot(lightColor.vDiff, lightColor.vDiff) != 0.f) //성분끼리의 곱이 0이아니라는거니깐 빛이 있어야된다
    {
        float4 vWorldPos = mul(float4(vViewPosition.xyz, 1.f), g_matViewInv); // 메인카메라 view 역행렬을 곱해서 월드좌표를 알아낸다.
        //월드 좌표를 광원쪽으로 투영을 시킴 아까 세팅한 광원쪽으로 투영하기위한 뷰프로젝션 행렬을 미리 곱해서 메트릭스 0에 넣어놨음
        //#define LightCamViewProjMat     g_mat_0 // 광원 시점으로의 View * Proj 행렬
        //월드 좌표를 광원이 보고있는 시점으로 기준으로 보냄
        float4 vShadowProj = mul(vWorldPos, LightCamViewProjMat); // 광원 시점으로 투영시킨 좌표 구하기

        //투영행렬에서 나온 좌표는 뷰스페이스에서 z가 곱해진 형태로 나옴
        //실제 투영좌표 x y z 뷰스페이스 z가 곱해져 있고 뷰스페이스 z는 w에 있고 그래서 진짜 투영좌표계z를 구하기위해서
        // w 로 나눠서 실제 투영좌표 z 값을 구한다.(올바르게 비교하기 위해서)
        float fDepth = vShadowProj.z / vShadowProj.w; 
    
        // 계산된 투영좌표를 UV 좌표로 변경해서 ShadowMap 에 기록된 깊이값을 꺼내온다.
        float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
                           , (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);
       
        if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f
        && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
        {
            float fShadowDepth = DYNAMIC_SHADOW_MAP.Sample(g_sam_0, vShadowUV).r;
      
            //추출한값과 투영시킨값을 비교를함 비교했는데 원래 기록되있는값이 더 작다
            //그말은 해당지금 비교한 지점이 광원시점에 기록된 물체의깊이보다 나를 이쪽으로 투영한게 멀다는건
            //나는 광원이 봤을대 물체뒤에있다는것 가려져있다는것

            // 그림자인 경우 빛을 약화시킨다.
            // 투영시킨 값이랑, 기록된 값이 같은경우는 그림자가 생기면 안되는데, 실수오차가 발생할 수 있기 때문에 보정값을 준다.
            if (fShadowDepth != 0.f && (fDepth > fShadowDepth + 0.00001f))
            {
                lightColor.vDiff *= 0.3f;
                lightColor.vSpec = (float4) 0.f;
            }
        }
    }
    //문제점 광원시점 고해상도 멀리서 광원시점으로 하기때문에
    //그리고 직교투영으로 해야댐 광원쪽에서 일직선으로 바라보기때문에
    //원근투영으로 찍어버리면 그림자가 늘어지는데 태양이 가까워지면 그림자가 커짐 근데 애초에 태양은 가까워진다는 개념이없을정도로 멀리떨어진빛
    //따라서 직교로 원근감없게 찍어야댐 근데 이러면 직교범위안에 안들어오면 그림자맵에 안찍힘
    //따라서 그림자맵에 없는건 그림자가 안생김 그럼 카메라 직교투영 범위를 넓혀서 찍어야되는데 해상도가 엄청 높아야댐 이건 사실 불가능
    //그래서 지역별로 스태틱 물체들은 미리 찍어놔야댐 만약 그게 싫다 오래걸릴꺼같다 그럼 플레이어 중심으로 광원을 따라다니게 함
    //다만 이럴경우 멀리있는물체를 바라보면 그림자가 안뜸
    //그리고 뭉개지는게 싫다면 지금은 한번에 바로 계산을 하는데 이걸 2단계로 나눠서 
    //라이트는 라이트대로 뽑고 계산해서 처음부터 빛을 감쇄해서 넣어줄게 아니라 라이트타겟을 3장으로해서 그림자를 질거다 말거다 라는 정보만 기록한
    //타겟을 하나 더둠 그럼 그림자가 각진채로 나오는데 나중에 빛이 다있는거랑 합쳐서 그림자가 나오는데 바로 석지말고
    //패스작업을 하나더함 블러처리를해서 각져있는걸 그럼 그림자가 아닌부분과 그림자인부분은 칼같이 나눠져있는건 평균색상정도로하면
    //부드럽게 뭉그러짐
    
    output.vDiffuse = lightColor.vDiff + lightColor.vAmb;
    output.vSpecular = lightColor.vSpec;
    if (0.f != vViewNormal.a)
    {
        float4 vSpecCoeff = decode(vViewNormal.a);
        
        output.vSpecular *= vSpecCoeff;
    }
    return output;    
}


// ========================
// Point Light Shader
// mesh : sphere mesh
// blend state : One - One
// depth stencil state : NoTest, NoWrite
// g_tex_0 : Normal Target
// g_tex_1 : Position Target
// g_int_0 : Light Index
// ========================
VS_DIR_OUT VS_PointLight(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    return output;
}

PS_OUT PS_PointLight(VS_DIR_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
            
    float2 vScreenUV = _in.vPosition.xy / g_vResolution;
    
    float3 vViewNormal = g_tex_0.Sample(g_sam_0, vScreenUV).xyz;
    float3 vViewPosition = g_tex_1.Sample(g_sam_0, vScreenUV).xyz;
    
    if (vViewPosition.z == 0.f)
    {
        //output.vDiffuse = float4(1.f, 0.f, 0.f, 1.f);
        //return output;
        clip(-1);
    }    
    
    // deferred 때 그려진 물체의 viewPos 를 가져와서, 볼륨메쉬(Sphere) 의 Local 까지 따라서 이동했을때,
    // Local 공간(Sphere 의) 에서 원점까지 거리가 1 이내라면(구의 반지름 내 범위) sphere 안에 있던 좌표였다.
    float4 vWorldPos = mul(float4(vViewPosition, 1.f), g_matViewInv);
    float3 vLocalPos = mul(vWorldPos, g_matWorldInv).xyz;
    if (length(vLocalPos) >  1.f)
    {
        //output.vDiffuse = float4(0.f, 0.f, 1.f, 1.f);        
        clip(-1);
    }       
    
    //output.vDiffuse = float4(0.f, 1.f, 0.f, 1.f);
    
    tLightColor lightColor = CalLight(g_int_0, vViewNormal, vViewPosition);
    output.vDiffuse = lightColor.vDiff + lightColor.vAmb;
    output.vSpecular = lightColor.vSpec;
    
    return output;
}


// ==================
// Light Merge Shader
// mesh : rect
// g_tex_0 : Diffuse Target
// g_tex_1 : Diffuse Light Target
// g_tex_2 : Specular Light Target
// ==================
VS_DIR_OUT VS_LightMerge(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;
    
    output.vPosition.xy = _in.vPos * 2.f;
    output.vPosition.zw = 1.f;
    
    return output;
}

float4 PS_LightMerge(VS_DIR_OUT _in) : SV_Target
{   
    float2 vScreenUV = _in.vPosition.xy / g_vResolution;
    
    float4 vDiffuseColor = g_tex_0.Sample(g_sam_0, vScreenUV);
    float4 vDiffuseLight = g_tex_1.Sample(g_sam_0, vScreenUV);
    float4 vSpeculLight = g_tex_2.Sample(g_sam_0, vScreenUV);
    
    float4 vColor = vDiffuseColor * vDiffuseLight + vSpeculLight;
    vColor.a = 1.f;
    
    return vColor;
}


#endif