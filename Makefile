# Makefile
##############################################
# Automatically-generated file. Do not edit! #
##############################################

include ./config.mk

install:
	$(Q)echo "[INFO] Installing Library"
	$(Q)$(MAKE) -C library install

library:
	$(Q)echo "[INFO] Building Library"
	$(Q)$(MAKE) -C library library

all:
	$(Q)echo "[INFO] Working on Library"
	$(Q)$(MAKE) -C library all
	$(Q)echo "[INFO] Working on Example"
	$(Q)$(MAKE) -C example all

clean:
	$(Q)echo "[INFO] Cleaning Library"
	$(Q)$(MAKE) -C library clean
	$(Q)echo "[INFO] Cleaning Example"
	$(Q)$(MAKE) -C example clean
