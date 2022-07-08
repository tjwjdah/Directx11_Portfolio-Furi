#ifndef _STRUCT
#define _STRUCT

struct tParticle
{
    float3 vWorldPos;
    float3 vWorldDir;

    float m_fCurTime;
    float m_fMaxLife;
    float m_fSpeed;
    
    int iAlive;
    int iPadding[2];
};

struct tParticleShared
{
    int iAliveCount;
    int iPadding[3];
};

struct tLightColor
{
    float4 vDiff; // Diffuse (³­¹Ý»ç±¤ ¼¼±â)	
    float4 vSpec; // Specular ¹Ý»ç±¤ ¼¼±â
    float4 vAmb; // Ambient (È¯°æ±¤)
};

struct tLightInfo
{
    tLightColor color; // ±¤¿ø »ö»ó

    float4 vDir; // ±¤¿øÀÇ ¹æÇâ
    float4 vWorldPos; // ±¤¿øÀÇ ¿ùµå À§Ä¡
    
    uint  eType; // ±¤¿ø Å¸ÀÔ		
    float fRange; // ±¤¿øÀÇ ¹üÀ§
    float fAngle; // ±¤¿ø °¢µµ
	
    int iPadding;
};

struct tAnim2D
{
    float2  vLeftTop;
    float2  vStep;
    int     iAnimUse[4];
};


struct tSkinningInfo
{
    float3 vPos;
    float3 vTangent;
    float3 vBinormal;
    float3 vNormal;
};
struct tSectorInfo
{
    float fAngle;
    float fDistance;
    int   iPadding[2];
};
#endif
