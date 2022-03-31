include makefile_define.mak

EXTERNAL_DIR = _external

TARGET = UmdImageCreator
SRCS = main.cpp \
       convert.cpp \
       execScsiCmdforFileSystem.cpp \
	   get.cpp \
	   output.cpp \
	   outputScsiCmdLogforCD.cpp \
	   testcode.cpp \

OBJS := $(SRCS:%.cpp=%.o) $(EXTERNAL_DIR)/pspUmdMan_driver.o

INCDIR =
CFLAGS = -O2 -G0 -Wall -Wextra 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -e _module_start
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =

ifeq ($(PSP_FW_VERSION),150)
  .PHONY : pbp
     pbp : SCEkxploit
  .PHONY : rebuild_pbp
     rebuild_pbp : clean SCEkxploit
else
  .PHONY : pbp
    pbp : all
  .PHONY : rebuild_pbp
    rebuild_pbp : rebuild
endif

.PHONY : lib
   lib : all

.PHONY : rebuild_lib
   rebuild_lib : rebuild

PSPSDK=$(shell psp-config --pspsdk-path)

ifeq ($(BUILD_PBP),1)
  EXTRA_TARGETS = EBOOT.PBP
  PSP_EBOOT_TITLE = UmdImageCreator
  LIBS = -lpspgum -lpspgu -lpsppower -lpspumd
  include $(PSPSDK)/lib/build.mak
else
   BUILD_PRX = 1
#  USE_KERNEL_LIBC = 1
#  USE_KERNEL_LIBS = 1
  LIBS = -lpspgu -lpsppower -lpspumd
  include $(PSPSDK)/lib/build_prx.mak
endif
