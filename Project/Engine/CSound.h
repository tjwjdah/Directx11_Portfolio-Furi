#pragma once
#include "CRes.h"

#include "FMOD/fmod.h"
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_codec.h"

#ifdef _DEBUG
#pragma comment(lib, "FMOD/x64/fmodL64_vc.lib")
#else
#pragma comment(lib, "FMOD/x64/fmod64_vc.lib")
#endif


class CSound :
    public CRes
{
public:
    static FMOD::System* g_pFMOD;

private:
    FMOD::Sound* m_pSound;
    list<FMOD::Channel*>   m_listChannel;


public:
    // 0 (무한반복) 
    void Play(int _iRoopCount, float _Volume = 0.5f, bool _bOverlap = false);
    //void Play(int _iRoopCount, bool _bOverlap = false);
    void Stop();

private:
    void RemoveChannel(FMOD::Channel* _pTargetChannel);
    friend FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
        , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
        , void* commanddata1, void* commanddata2);

public:
    virtual void Save(const wstring& _strRelativePath) {}
    virtual void Load(const wstring& _strFilePath);

public:
    CSound();
    virtual ~CSound();
};