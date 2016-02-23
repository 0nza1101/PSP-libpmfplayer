TARGET = PMFPSP
OBJS = main.o

CFLAGS = -O3 -G4 -Wall -DPSPFW3xx
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lpmfplayer -lpspmpeg -lpspdebug -lpspsdk -lc -lstdc++ -lpspgum -lpspgu -lpsppower -lpspaudiolib -lpspaudio -lpsphprm -lpsputility -lz -lm -lfreetype
BUILD_PRX = 0

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = pmf player sample

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
