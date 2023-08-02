#module Name
MODULE = sc_listener

#Library sources
#use the "NONE" option if this Application has no Library to build
LIBSRCS = NONE

# Extra header file for the library to be instaled
#use the "NONE" option if no extra is requered
EXTRA_H = NONE

#Test Application sources
#use the "NONE" option if this library has no Test Binary
APPSRCS = sc_listener.cpp \
	  sc_main_with_listener.cpp

#######################
# DO NOT touch below #
#######################

#include config file
include ../../../config.mk

ifeq ($(strip $(INT_TYPE)), tcp)
INT_TYPE_CFLAGS = -D_TCP -D_SERVER_PORT=$(TCP_SERVER_PORT)
endif

ifeq ($(strip $(INT_TYPE)), shm) 
INT_TYPE_LIBS = -lrt
INT_TYPE_CFLAGS = -D_SHM
endif

ifeq ($(strip $(INT_TYPE)), posix) 
INT_TYPE_CFLAGS = -D_POSIX
INT_TYPE_LIBS = -lrt
endif

ifeq ($(strip $(REALTIME)), yes) 
RT_LIBS = -lrealtimify
RT_CFLAGS = -D_REALTIME -D_RT_TIME=$(RT_TIME) -D_RT_UNIT=$(RT_UNIT)
endif
	  
# Extra required libraries for Application
EXTRA_LIBS = -lhelper -lasync_sc_cosim_int -lasync_channel $(RT_LIBS) -lpthread $(INT_TYPE_LIBS)
#-lfirewall_with_mem 

# Library Install location
LIB_PATH=$(INST_LIB_PATH)

INCDIR = -I. -I.. -I$(SYSTEMC)/include -I$(LIB_PATH)/include
LIBDIR = -L. -L.. -L$(SYSTEMC)/lib-$(TARGET_ARCH) -L$(LIB_PATH)/lib
LIBS = $(EXTRA_LIBS) -lsystemc -lm -lpthread
LDFLAGS = -g

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -std=c++11 -Wreorder -DSC_INCLUDE_DYNAMIC_PROCESSES 
CFLAGS = -g -Wall $(RT_CFLAGS) $(INT_TYPE_CFLAGS)

# compiler
CC = c++
C = gcc

#Test Binary name
LIBRARY = lib$(MODULE).a
EXE = $(MODULE).exe

# compile flags
LIBOBJS = $(LIBSRCS:.cpp=.o)
APPOBJS = $(APPSRCS:.cpp=.o)

.SUFFIXES: .cpp .o

default: all

.cpp.o:
	@echo "[INFO] Compiling $*.cpp"
	$(Q)$(CC) $(INCDIR) $(CCFLAGS) $(CFLAGS) -c $< -o $@

ifneq ($(LIBSRCS),NONE)
$(MODULE): $(LIBOBJS)
	@echo "[INFO] Creating Library"
	$(Q)ar rcs $(LIBRARY) $(LIBOBJS)
endif

ifneq ($(APPSRCS),NONE)
$(EXE): $(APPOBJS) 
	@echo "[INFO] Creating Channel Testbench "
	$(Q)$(CC) $(CCFLAGS) $(CFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(APPOBJS) $(LIBS) 2>&1 | c++filt
tesbench:
	@echo "[INFO] Creating Client Application"
	$(Q)$(C) $(CFLAGS) $(INCDIR) $(LIBDIR) testbench_with_main.c -o testbench_with_main.exe -pthread -lpthread $(INT_TYPE_LIBS)
endif
clean:
	@echo "[INFO] Cleaning files"
	$(Q)rm -f $(APPOBJS) $(LIBOBJS) $(LIBRARY) $(EXE) *.exe *~ core

ifneq ($(APPSRCS),NONE)
ifneq ($(LIBSRCS),NONE)
all: $(MODULE) install $(EXE) tesbench
else
all: $(EXE) tesbench
endif
else
all: $(MODULE) install
endif

#install only exists for library 
ifneq ($(LIBSRCS),NONE)
install: $(MODULE)
	@echo "[INFO] Instaling Library"
	test -d $(LIB_PATH)/include/ || mkdir -p $(LIB_PATH)/include/
	test -d $(LIB_PATH)/lib/ || mkdir -p $(LIB_PATH)/lib/
	install $(MODULE).h $(LIB_PATH)/include/
ifneq ($(EXTRA_H),NONE)
	install $(EXTRA_H) $(LIB_PATH)/include/
endif
	install $(LIBRARY) $(LIB_PATH)/lib/

.PHONY: install
endif

