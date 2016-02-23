#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputility_avmodules.h>

#include <pmfplayer/ctrl_video.h> //The pmf player library.


PSP_MODULE_INFO("PMF PLAYER SAMPLE", 0, 0, 0);
PSP_HEAP_SIZE_KB(2048);

int main (void)
{
    play_pmf("./data/INTRO.pmf");//To play a .pmf, locate the directory of the .pmf file.
    stop_pmf();//This function stop the video lecture by pressing whatever button.

    sceKernelExitGame();//Exit the Game.
    return 0;
}
