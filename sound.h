// Class to encapsulate the sound, includs the
// initialisation and loading of sounds

#pragma once
#include <stdio.h>

#include <xtl.h>
#include <dmusici.h>

// Class def
class CSound
{
protected:
	IDirectMusicSegment8*     m_pSoundSegment;

public:
	void Create(char* filename);  //e.g. "D:\\chomp.wav" 

	void playsound();

	void Release();

protected:
	static IDirectMusicLoader8*      m_pLoader;
	static IDirectMusicPerformance8* m_pPerformance;

	void LoadSound(char* filename);

	void SetupSound();
};

// How this works:
// 1) Initialize DirectX (the sounds are part DirectX, it's not just visuals)
// 2) Create as many CSound objects as you like
// 3) During initialization, call the Create functions with filepath to load the WAV
// 4) Call Play(), or Stop(), or Pause() as you like.


