PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR = $(shell psp-config --psp-prefix)

CFLAGS = -O2 -G0 -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

OBJS = decoder.o reader.o video.o audio.o pmfplayer.o ctrl_video.o
TARGET_LIB = libpmfplayer.a

LIBS = -lpspdebug -lpspsdk -lc -lstdc++ -lpspgum -lpspgu -lpsppower -lpspaudiolib -lpspaudio -lpsphprm -lpsputility -lz -lm -lfreetype libmpeg/libpspmpeg.a

include $(PSPSDK)/lib/build.mak