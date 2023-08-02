#module Name
MODULE = demo_sc_firewall

#Library sources
#use the "NONE" option if this Application has no Library
LIBSRCS = NONE

# Extra header file for the library to be instaled
#use the "NONE" option if no extra is requered
EXTRA_H = NONE

#Test Application sources
#use the "NONE" option if this library has no Test Binary
APPSRCS = receiver.cpp \
	  main.cpp

#######################
# DO NOT touch below #
#######################
#include config file
include ../../config.mk

ifeq ($(strip $(INT_TYPE)), tcp)
INT_TYPE_CFLAGS = -D_TCP -D_SERVER_PORT=$(TCP_SERVER_PORT) -D_SERVER_ADDR=\""$(TCP_TESTBENCH_SERVER_ADDRESS)"\"
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
EXTRA_LIBS = -lhelper -lfirewall_with_mem -lasync_sc_cosim_int -lasync_channel $(RT_LIBS) -lpthread $(INT_TYPE_LIBS)

# Library Install location
LIB_PATH=$(INST_LIB_PATH)

INCDIR = -I. -I.. -I$(SYSTEMC)/include -I$(LIB_PATH)/include -I../Firewall_with_mem
LIBDIR = -L. -L.. -L$(SYSTEMC)/lib-$(TARGET_ARCH) -L$(LIB_PATH)/lib -L../Firewall_with_mem
LIBS = -L.. $(EXTRA_LIBS) -lsystemc -lm
LDFLAGS = -g

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -std=c++11 -Wreorder -DSC_INCLUDE_DYNAMIC_PROCESSES 
CFLAGS = -g -Wall $(RT_CFLAGS) $(INT_TYPE_CFLAGS) -D_USERNAME=\""$(APP_TESTBENCH_USERNAME)"\" -DSHA3_USE_KECCAK
#CFLAGS = -g -Wall $(RT_CFLAGS) $(INT_TYPE_CFLAGS) -D_USERNAME=\""$(APP_TESTBENCH_USERNAME)"\" -DSHA3_USE_KECCAK


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
	$(Q)$(CC) $(INCDIR) $(CCFLAGS) -c $< -o $@

ifneq ($(LIBSRCS),NONE)
$(MODULE): $(LIBOBJS)
	@echo "[INFO] Creating Library"
	$(Q)ar rcs $(LIBRARY) $(LIBOBJS)
endif

ifneq ($(APPSRCS),NONE)
$(EXE): $(APPOBJS) 
	@echo "[INFO] Creating Application $*"
	$(Q)$(CC) $(CFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(APPOBJS) $(LIBS) 2>&1 | c++filt
tesbench:
	@echo "[INFO] Creating FW Read Testbench Application"
	$(Q)$(C) $(CFLAGS) $(INCDIR) $(LIBDIR) test_setupcfw_READ.c -o test_read.exe -pthread -lpthread $(INT_TYPE_LIBS)
	@echo "[INFO] Creating FW Write Testbench Application"
	$(Q)$(C) $(CFLAGS) $(INCDIR) $(LIBDIR) test_setupcfw_WRITE.c -o test_write.exe -pthread -lpthread $(INT_TYPE_LIBS)
	@echo "[INFO] Creating FW Testbench for Real Application"
	$(Q)$(C) $(CFLAGS) $(INCDIR) $(LIBDIR) test_real_application.c -o test_app.exe -pthread -lpthread $(INT_TYPE_LIBS)
endif
clean:
	@echo "[INFO] Cleaning files"
	$(Q)rm -f $(APPOBJS) $(LIBOBJS) $(LIBRARY) $(EXE) test*.exe tesbench *~ core

ifneq ($(APPSRCS),NONE)
ifneq ($(LIBSRCS),NONE)
all: $(MODULE) $(EXE) tesbench
else
all: $(EXE) tesbench
endif
else
all: $(MODULE) 
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

