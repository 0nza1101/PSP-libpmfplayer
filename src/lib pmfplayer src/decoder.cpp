/*
 *	PMF Player Module
 *	Copyright (c) 2006 by Sorin P. C. <magik@hypermagik.com>
 */
#include "pmfplayer.h"
#include <psppower.h>

#include <stdio.h>
#include "ctrl_video.h"

extern int stop;

SceInt32 IsRingbufferFull(ReaderThreadData* D)
{
	if(D->m_Status == ReaderThreadData::READER_EOF)
		return 1;

	if(sceMpegRingbufferAvailableSize(D->m_Ringbuffer) > 0)
		return 0;

	return 1;
}


int T_Decoder(SceSize _args, void *_argp)
{
	int retVal;

	int iInitAudio = 1;
	SceInt32 iVideoStatus = 0;

	SceInt32 unknown = 0;

	int iThreadsRunning = 0;

	SceInt32 m_iAudioCurrentTimeStamp = 0;
	SceInt32 m_iVideoCurrentTimeStamp = 0;
	SceInt32 m_iVideoLastTimeStamp = 0;

	DecoderThreadData* D = *((DecoderThreadData**)_argp);

	for(;;)
	{
		sceKernelDelayThread(1);

		scePowerTick(0);

		if( iThreadsRunning == 0 &&
			IsRingbufferFull(D->Reader) &&
			D->Video->m_iNumBuffers == D->Video->m_iFullBuffers && 
			D->Audio->m_iNumBuffers == D->Audio->m_iFullBuffers )
		{
			iThreadsRunning = 1;
			sceKernelSignalSema(D->Video->m_SemaphoreStart, 1);
			sceKernelSignalSema(D->Audio->m_SemaphoreStart, 1);
		}

		if(D->Reader->m_Status == ReaderThreadData::READER_ABORT)
		{
			break;
		}
		else if(D->Reader->m_Status == ReaderThreadData::READER_EOF)
		{
			retVal = sceMpegRingbufferAvailableSize(D->Reader->m_Ringbuffer);

			if(retVal == D->Reader->m_RingbufferPackets)
				break;
		}

		if(!IsRingbufferFull(D->Reader))
		{
			sceKernelWaitSema(D->Reader->m_Semaphore, 1, 0);

			if(D->Reader->m_Status == ReaderThreadData::READER_ABORT)
				break;
		}

		if(D->Audio->m_iFullBuffers < D->Audio->m_iNumBuffers)
		{
			retVal = sceMpegGetAtracAu(&D->m_Mpeg, D->m_MpegStreamAtrac, D->m_MpegAuAtrac, &unknown);
			if(retVal != 0)
			{
				if(!IsRingbufferFull(D->Reader))
				{
					sceKernelWaitSema(D->Reader->m_Semaphore, 1, 0);

					if(D->Reader->m_Status == ReaderThreadData::READER_ABORT)
						break;
				}
			}
			else
			{
				if(m_iAudioCurrentTimeStamp >= D->m_iLastTimeStamp - D->m_iVideoFrameDuration)
					break;

				retVal = sceMpegAtracDecode(&D->m_Mpeg, D->m_MpegAuAtrac, D->Audio->m_pAudioBuffer[D->Audio->m_iDecodeBuffer], iInitAudio);
				if(retVal != 0)
				{
					sprintf(D->m_LastError, "sceMpegAtracDecode() failed: 0x%08X", retVal);
					break;
				}

				if(D->m_MpegAuAtrac->iTimestamp < 0)
					m_iAudioCurrentTimeStamp += D->m_iAudioFrameDuration;
				else
					m_iAudioCurrentTimeStamp = D->m_MpegAuAtrac->iTimestamp;

				if(m_iAudioCurrentTimeStamp <= 0x15F90 /* video start ts */ - D->m_iAudioFrameDuration)
					iInitAudio = 1;

				D->Audio->m_iBufferTimeStamp[D->Audio->m_iDecodeBuffer] = m_iAudioCurrentTimeStamp;

				if(iInitAudio == 0)
				{
					sceKernelWaitSema(D->Audio->m_SemaphoreLock, 1, 0);

					D->Audio->m_iFullBuffers++;

					sceKernelSignalSema(D->Audio->m_SemaphoreLock, 1);

					D->Audio->m_iDecodeBuffer = (D->Audio->m_iDecodeBuffer + 1) % D->Audio->m_iNumBuffers;
				}

				iInitAudio = 0;
			}
		}

		if(!IsRingbufferFull(D->Reader))
		{
			sceKernelWaitSema(D->Reader->m_Semaphore, 1, 0);

			if(D->Reader->m_Status == ReaderThreadData::READER_ABORT)
				break;
		}

		if(D->Video->m_iFullBuffers < D->Video->m_iNumBuffers)
		{
			retVal = sceMpegGetAvcAu(&D->m_Mpeg, D->m_MpegStreamAVC, D->m_MpegAuAVC, &unknown);
			if((SceUInt32)retVal == 0x80618001)
			{
				if(!IsRingbufferFull(D->Reader))
				{
					sceKernelWaitSema(D->Reader->m_Semaphore, 1, 0);

					if(D->Reader->m_Status == ReaderThreadData::READER_ABORT)
						break;
				}
			}
			else if(retVal != 0)
			{
				sprintf(D->m_LastError, "sceMpegGetAvcAu() failed: 0x%08X", retVal);
				break;
			}
			else
			{
				if(m_iVideoCurrentTimeStamp >= D->m_iLastTimeStamp - D->m_iVideoFrameDuration)
					break;

				retVal = sceMpegAvcDecode(&D->m_Mpeg, D->m_MpegAuAVC, BUFFER_WIDTH, &D->Video->m_pVideoBuffer[D->Video->m_iPlayBuffer], &iVideoStatus);
				if(retVal != 0)
				{
					sprintf(D->m_LastError, "sceMpegAvcDecode() failed: 0x%08X", retVal);
					break;
				}

				if(D->m_MpegAuAVC->iTimestamp < 0)
					m_iVideoCurrentTimeStamp += 0x0BBC;
				else
					m_iVideoCurrentTimeStamp = D->m_MpegAuAVC->iTimestamp;

				if(iVideoStatus == 1)
				{
					D->Video->m_iBufferTimeStamp[D->Video->m_iPlayBuffer] = m_iVideoLastTimeStamp;

					sceKernelWaitSema(D->Video->m_SemaphoreLock, 1, 0);

					D->Video->m_iFullBuffers++;

					sceKernelSignalSema(D->Video->m_SemaphoreLock, 1);
				}

				m_iVideoLastTimeStamp = m_iVideoCurrentTimeStamp;
			}
		}

		if(!IsRingbufferFull(D->Reader))
		{
			sceKernelWaitSema(D->Reader->m_Semaphore, 1, 0);

			if(D->Reader->m_Status == ReaderThreadData::READER_ABORT)
				break;
		}
        sceKernelDelayThread(100);

		stop_pmf();
		/*Human-Behind ** use to stop*/
		if(stop == 1)
		{
		break;
		}/*Human-Behind ** use to stop*/
	}

	sceKernelSignalSema(D->Audio->m_SemaphoreStart, 1);
	sceKernelSignalSema(D->Video->m_SemaphoreStart, 1);

	D->Reader->m_Status = ReaderThreadData::READER_ABORT;

	D->Audio->m_iAbort = 1;

	while(D->Video->m_iFullBuffers > 0)
	{

		sceKernelWaitSema(D->Video->m_SemaphoreWait, 1, 0);
		sceKernelSignalSema(D->Video->m_SemaphoreLock, 1);
        sceKernelDelayThread(100);

		stop_pmf();
		/*Human-Behind ** use to stop*/
		if(stop == 1)
		{
		break;
		}/*Human-Behind ** use to stop*/
	}

	sceMpegAvcDecodeStop(&D->m_Mpeg, BUFFER_WIDTH, D->Video->m_pVideoBuffer, &iVideoStatus);

	if(iVideoStatus > 0)
	{
		sceKernelWaitSema(D->Video->m_SemaphoreLock, 1, 0);

		D->Video->m_iFullBuffers++;

		sceKernelSignalSema(D->Video->m_SemaphoreLock, 1);
	}

	D->Video->m_iAbort = 1;

	sceMpegFlushAllStream(&D->m_Mpeg);

	sceKernelExitThread(0);

	return 0;
}

SceInt32 CPMFPlayer::InitDecoder()
{
	Decoder.m_ThreadID	= sceKernelCreateThread("decoder_thread", T_Decoder, 0x10, 0x10000, PSP_THREAD_ATTR_USER, NULL);

	if(Decoder.m_ThreadID < 0)
	{
		sprintf(m_LastError, "sceKernelCreateThread() failed: 0x%08X", (int)Decoder.m_ThreadID);
		return -1;
	}

	Decoder.Reader				= &Reader;
	Decoder.Video				= &Video;
	Decoder.Audio				= &Audio;
	Decoder.m_Mpeg				= m_Mpeg;
	Decoder.m_MpegStreamAVC		= m_MpegStreamAVC;
	Decoder.m_MpegAuAVC			= &m_MpegAuAVC;
	Decoder.m_MpegStreamAtrac	= m_MpegStreamAtrac;
	Decoder.m_MpegAuAtrac		= &m_MpegAuAtrac;
	Decoder.m_LastError			= m_LastError;

	Decoder.m_iAudioFrameDuration = 4180; // ??
	Decoder.m_iVideoFrameDuration = (int)(90000 / 29.97);
	Decoder.m_iLastTimeStamp      = m_iLastTimeStamp;

	return 0;
}

SceInt32 CPMFPlayer::ShutdownDecoder()
{
	sceKernelDeleteThread(Decoder.m_ThreadID);

	return 0;
}
