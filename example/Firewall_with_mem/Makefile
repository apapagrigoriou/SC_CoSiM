#module Name
MODULE = firewall_with_mem

#Library sources
#use the "NONE" option if this Application has no Library
LIBSRCS = firewall_with_mem.cpp 

# Extra header file for the library to be instaled
#use the "NONE" option if no extra is requered
EXTRA_H = NONE

#Test Application sources
#use the "NONE" option if this library has no Test Binary
APPSRCS =  NONE
#	sender.cpp \
#	receiver.cpp \
#	main.cpp

# Extra required libraries for Application
EXTRA_LIBS = -lhelper -pthread

#######################
# DO NOT touch below #
#######################
#include config file
include ../../config.mk

# Library Install location
LIB_PATH=$(INST_LIB_PATH)

INCDIR = -I. -I.. -I$(SYSTEMC)/include -I$(LIB_PATH)/include
LIBDIR = -L. -L.. -L$(SYSTEMC)/lib-$(TARGET_ARCH) -L$(LIB_PATH)/lib
LIBS = -L.. -l$(MODULE) $(EXTRA_LIBS) -lsystemc -lm
LDFLAGS = -g

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS =  -g -Wall -std=c++11 -Wc++0x-compat -Wno-reorder -DSC_INCLUDE_DYNAMIC_PROCESSES

# compiler
CC = c++

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
	$(Q)$(CC) $(CCFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(APPOBJS) $(LIBS) 2>&1 | c++filt
endif
clean:
	@echo "[INFO] Cleaning files"
	$(Q)rm -f $(APPOBJS) $(LIBOBJS) $(LIBRARY) $(EXE) *~ core

ifneq ($(APPSRCS),NONE)
ifneq ($(LIBSRCS),NONE)
all: $(MODULE) $(EXE)
else
all: $(EXE)
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

