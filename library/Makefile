# Makefile
################
# Do not edit! #
################

include ../config.mk

library:
	$(Q)echo "[INFO] Building Helper"
	$(Q)$(MAKE) -C Helper library
	$(Q)echo "[INFO] Building  Realtimify"
	$(Q)$(MAKE) -C Realtimify library
	$(Q)echo "[INFO] Building  Basic Asynchronous Channel"
	$(Q)$(MAKE) -C Async_channel library
	$(Q)echo "[INFO] Building  SC Cosim Async Channel"
	$(Q)$(MAKE) -C Async_sc_cosim_int library

install:
	$(Q)echo "[INFO] Installing Helper"
	$(Q)$(MAKE) -C Helper install
	$(Q)echo "[INFO] Installing  Realtimify"
	$(Q)$(MAKE) -C Realtimify install
	$(Q)echo "[INFO] Installing  Basic Asynchronous Channel"
	$(Q)$(MAKE) -C Async_channel install
	$(Q)echo "[INFO] Installing  SC Cosim Async Channel"
	$(Q)$(MAKE) -C Async_sc_cosim_int install

all:
	$(Q)echo "[INFO] Working on Helper"
	$(Q)$(MAKE) -C Helper all
	$(Q)echo "[INFO] Creating Testbench for Realtimify"
	$(Q)$(MAKE) -C Realtimify/REGRESSION_TEST all
	$(Q)echo "[INFO] Working on Basic Asynchronous Channel"
	$(Q)$(MAKE) -C Async_channel/REGRESSION_TEST all
	$(Q)echo "[INFO] Working on SC Cosim Async Channel"
	$(Q)$(MAKE) -C Async_sc_cosim_int all
	$(Q)echo "[INFO] Creating Testbenches for Async SC Cosim Channel"
	$(Q)$(MAKE) -C Async_sc_cosim_int/REGRESSION_TEST all


clean:
	$(Q)echo "[INFO] Cleaning Helper"
	$(MAKE) -C Helper clean
	$(Q)echo "[INFO] Cleaning Realtimify"
	$(MAKE) -C Realtimify clean
	$(Q)echo "[INFO] Cleaning Testbench for Realtimify"
	$(Q)$(MAKE) -C Realtimify/REGRESSION_TEST clean
	$(Q)echo "[INFO] Cleaning Basic Asynchronous Channel"
	$(MAKE) -C Async_channel clean
	$(Q)echo "[INFO] Cleaning Basic Asynchronous Channel"
	$(Q)$(MAKE) -C Async_channel/REGRESSION_TEST clean
	$(Q)echo "[INFO] Cleaning SC Cosim Async Channel"
	$(MAKE) -C Async_sc_cosim_int clean
	$(Q)echo "[INFO] Cleaning Testbenches for Async SC Cosim Channel"
	$(Q)$(MAKE) -C Async_sc_cosim_int/REGRESSION_TEST clean
