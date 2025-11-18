#include "sound.h"

// Static class members. Only need one each, so these are static
// and therefore all sounds share one instance

IDirectMusicLoader8*		CSound::m_pLoader		= NULL;
IDirectMusicPerformance8*	CSound::m_pPerformance	= NULL;

// Initialize the sound, all the do-it-once-at-start things
void CSound::Create(char* filename) //e.g. "D:\\chomp.wav"
{
	
	if( (m_pLoader == NULL) && (m_pPerformance == NULL))
	{
		SetupSound();
	}

	LoadSound(filename);
}

// Load the wav into the sound container
void CSound::LoadSound(char* filename) //e.g. "D:\\chomp.wav"
{	
	m_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8,
                                  filename, (VOID**)&m_pSoundSegment);
}

// Using the segment loaded in LoadSound(), we can now play it here
// Can also change/add parameters from here to loop or do more complex things
// See: DirectMusic documentation, if you can find it online
void CSound::playsound()
{
	m_pPerformance->PlaySegmentEx( m_pSoundSegment, NULL, NULL, DMUS_SEGF_SECONDARY, 
		                                   0, NULL, NULL, NULL );
}

// Requires knowing how DirectMusic works...very complex system that was
// sliced up and only a pinch was implemented in the XDK
void CSound::SetupSound()
{
	// Initialize DMusic
    IDirectMusicHeap* pNormalHeap;
    DirectMusicCreateDefaultHeap( &pNormalHeap );

    IDirectMusicHeap* pPhysicalHeap;
    DirectMusicCreateDefaultPhysicalHeap( &pPhysicalHeap );

    DirectMusicInitializeEx( pNormalHeap, pPhysicalHeap, &DirectMusicDefaultFactory );

    // Why do we release these already??
    pNormalHeap->Release();
    pPhysicalHeap->Release();
	
    // Create loader object
    DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL,
                               IID_IDirectMusicLoader8, (VOID**)&m_pLoader );
    // Create performance object
    DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
                               IID_IDirectMusicPerformance8, (VOID**)&m_pPerformance );
    // Init the performance object; manually updated, 64 channels, 128 voices
    m_pPerformance->InitAudioX( DMUS_INITAUDIO_NOTHREADS, 64, 128, 0 );
}

void CSound::Release()
{
	m_pSoundSegment->Release();
	
}

