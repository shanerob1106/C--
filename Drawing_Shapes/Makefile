TARGET = Drawing_Shapes
OBJS = main.o \
gfx.o \
player.o

CFLAGS =
CXXFLAGS = $(CFLAGS) -std=c++11 -fno-rtti 
ASFLAGS = $(CFLAGS)

LIBS += -lstdc++

# PSP stuff
BUILD_PRX = 1
PSP_FW_VERSION = 500
PSP_LARGE_MEMORY = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Drawing Shapes
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)

include $(PSPSDK)/lib/build.mak
