#ifndef DEFINE_PMFPLAY_HB
#define DEFINE_PMFPLAY_HB

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <psputility_avmodules.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <math.h>
#include "pmfplayer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

void play_pmf(const char *filepath);
void stop_pmf();

#endif
