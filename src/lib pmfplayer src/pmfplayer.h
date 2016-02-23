/*
 *	PMF Player Module
 *	Copyright (c) 2006 by Sorin P. C. <magik@hypermagik.com>
 *	Modified by Human-Behind
 */
#ifndef __PMFPLAYER_H__
#define __PMFPLAYER_H__

#include <pspkernel.h>
#include "pspmpeg.h"
#include <psptypes.h>
#include <pspctrl.h>

#include "decoder.h"
#include "reader.h"
#include "audio.h"
#include "video.h"

class CPMFPlayer
{

public:

	CPMFPlayer(void);
	~CPMFPlayer(void);

	char* GetLastError();
	SceInt32 Initialize(SceInt32 nPackets = 0x3C0);
	SceInt32 Load(const char* pFileName);
	SceInt32 Play();
	SceVoid Shutdown();
	void play_pmf(const char *pathfile);

private:

	char m_LastError[256];

	SceCtrlData padBis;

	SceInt32 m_PmfScaling;
	SceInt32 m_MovieWidth;
	SceInt32 m_MovieHeight;
	SceInt32 m_AudioStreamExist;
	SceUID 	m_FileHandle;
	SceInt32 m_MpegStreamOffset;
	SceInt32 m_MpegStreamSize;
	
	SceMpeg m_Mpeg;
	SceInt32 m_MpegMemSize;
	ScePVoid m_MpegMemData;

	SceInt32 m_RingbufferPackets;
	SceInt32 m_RingbufferSize;
	ScePVoid m_RingbufferData;
	SceMpegRingbuffer m_Ringbuffer;

	SceMpegStream* m_MpegStreamAVC;
	ScePVoid m_pEsBufferAVC;
	SceMpegAu m_MpegAuAVC;

	SceMpegStream* m_MpegStreamAtrac;
	ScePVoid m_pEsBufferAtrac;
	SceMpegAu m_MpegAuAtrac;

	SceInt32 m_MpegAtracEsSize;
	SceInt32 m_MpegAtracOutSize;

	SceInt32 m_iLastTimeStamp;

        SceInt32 ParseHeader();
	SceInt32 InitAudio();
	SceInt32 ShutdownAudio();
	SceInt32 InitDecoder();
	SceInt32 ShutdownDecoder();
	SceInt32 InitVideo();
	SceInt32 ShutdownVideo();
	SceInt32 InitReader();
	SceInt32 ShutdownReader();

	DecoderThreadData Decoder;

	ReaderThreadData Reader;

	VideoThreadData Video;

	AudioThreadData Audio;

};

#define SWAPINT(x) (((x)<<24) | (((uint)(x)) >> 24) | (((x) & 0x0000FF00) << 8) | (((x) & 0x00FF0000) >> 8))

#endif
