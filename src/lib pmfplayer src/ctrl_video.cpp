#include "ctrl_video.h"

int stop;
int appuis_touche = 0;

void play_pmf(const char *filepath)
{
sceKernelDelayThread(500000);
//Chargement des  moduleS
sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
sceUtilityLoadAvModule(PSP_AV_MODULE_MPEGBASE);
CPMFPlayer *PmfPlayer = new CPMFPlayer();
PmfPlayer->play_pmf(filepath);
delete PmfPlayer;
sceUtilityUnloadAvModule(PSP_AV_MODULE_MPEGBASE);
sceUtilityUnloadAvModule(PSP_AV_MODULE_AVCODEC);
}

void stop_pmf()
{
	SceCtrlData padBis;
	sceCtrlPeekBufferPositive(&padBis, 1);
	if((padBis.Buttons) && (appuis_touche >= 100))
	{
	stop = 1;
	appuis_touche = 0;
	sceKernelDelayThread(500000);
	}
	if(appuis_touche < 200) appuis_touche++;

}
