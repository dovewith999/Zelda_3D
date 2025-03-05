#pragma once
#include "Base.h"

#include "fmod.hpp"
#include "fmod_codec.h"
#include "fmod_dsp.h"
#include "fmod_errors.h"
#include "fmod_output.h"
#include "fmod_memoryinfo.h"
#include "io.h"
#include "playsoundapi.h"

BEGIN(Engine)
class CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
private:
	CSound_Manager();
	virtual ~CSound_Manager() override = default;

public:
	void Initialize_Manager();

public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	std::map<TCHAR*, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	//FMOD_SYSTEM* m_pSystem;
	FMOD::System* m_pSystem = nullptr;

public:
	virtual void Free() override;
};
END

