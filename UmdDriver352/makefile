TARGET = pspumdman352
OBJS = main.o sceUmdMan_driver352.o exports352.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -Wextra
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

#PSP_FW_VERSION = 661
#PSP_FW_VERSION = 150

BUILD_PRX = 1
PRX_EXPORTS = exports352.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LIBDIR =
LDFLAGS = 
LIBS = 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

