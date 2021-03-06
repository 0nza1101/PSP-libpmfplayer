/*
 *	PMF Player Module
 *	Copyright (c) 2006 by Sorin P. C. <magik@hypermagik.com>
 */
#include "pmfplayer.h"
#include <stdio.h>
#include "ctrl_video.h"

extern int stop;

int T_Reader(SceSize _args, void *_argp)
{
	ReaderThreadData* D = *((ReaderThreadData**)_argp);

	SceInt32 iTotalBytes	= 0;
	SceInt32 iFreePackets	= 0;
	SceInt32 iReadPackets	= 0;
	SceInt32 iPackets		= 0;

	for(;;)
	{
		iPackets = 0;

		if(D->m_Status == ReaderThreadData::READER_ABORT)
			break;

		iFreePackets = sceMpegRingbufferAvailableSize(D->m_Ringbuffer);

		if(iFreePackets > 0)
		{
			iReadPackets = iFreePackets;

			if(iTotalBytes < D->m_StreamSize)
			{
				if( iReadPackets > 32 )
					iReadPackets = 32;

				int iPacketsLeft = (D->m_StreamSize - iTotalBytes) / 2048;

				if( iPacketsLeft < iReadPackets )
					iReadPackets = iPacketsLeft;

				iPackets = sceMpegRingbufferPut(D->m_Ringbuffer, iReadPackets, iFreePackets);
				if(iPackets < 0)
				{
					sprintf(D->m_LastError, "sceMpegRingbufferPut() failed: 0x%08X", (int)iPackets);
					D->m_Status = ReaderThreadData::READER_ABORT;
					break;
				}
			}
		}

		sceKernelSignalSema(D->m_Semaphore, 1);

		if(iPackets > 0) iTotalBytes += iPackets * 2048;

		if(iTotalBytes >= D->m_StreamSize && D->m_Status != ReaderThreadData::READER_ABORT)
		{
			D->m_Status = ReaderThreadData::READER_EOF;
		}
        sceKernelDelayThread(100);

		stop_pmf();
		/*Human-Behind ** use to stop*/
		if(stop == 1)
		{
		break;
		}/*Human-Behind ** use to stop*/
	}

	sceKernelSignalSema(D->m_Semaphore, 1);

	sceKernelExitThread(0);

	return 0;
}

SceInt32 CPMFPlayer::InitReader()
{
	Reader.m_ThreadID = sceKernelCreateThread("reader_thread", T_Reader, 0x41, 0x10000, PSP_THREAD_ATTR_USER, NULL);
	if(Reader.m_ThreadID	< 0)
	{
		return -1;
	}

	Reader.m_Semaphore = sceKernelCreateSema("reader_sema", 0, 0, 1, NULL);
	if(Reader.m_Semaphore < 0)
	{
		sceKernelDeleteSema(Reader.m_Semaphore);
		return -1;
	}

	Reader.m_StreamSize			= m_MpegStreamSize;
	Reader.m_Ringbuffer			= &m_Ringbuffer;
	Reader.m_RingbufferPackets	= m_RingbufferPackets;
	Reader.m_Status				= 0;
	Reader.m_LastError			= m_LastError;

	return 0;
}

SceInt32 CPMFPlayer::ShutdownReader()
{
	sceKernelDeleteThread(Reader.m_ThreadID);

	sceKernelDeleteSema(Reader.m_Semaphore);

	return 0;
}
