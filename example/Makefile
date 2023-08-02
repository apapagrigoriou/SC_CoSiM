# Makefile
##############################################
# Automatically-generated file. Do not edit! #
##############################################
include ../config.mk

all:
	$(Q)echo "[INFO] Creating Firewall with Memory"
	$(MAKE) -C Firewall_with_mem
	$(Q)echo "[INFO] Creating Testbenches for Firewall"
	$(Q)$(MAKE) -C Firewall_with_mem/REGRESSION_TEST
	$(Q)echo "[INFO] Creating Firewall with Memory and Async Channel"
	$(Q)$(MAKE) -C Demo_SC_firewall
ifeq ($(strip $(BUILD_HW_TESTBENCH_ARM)), yes)
	$(Q)$(MAKE) -C Demo_HARDWARE_firewall_ARM -f Makefile.apps all	
endif

clean:
	$(Q)echo "[INFO] Cleaning Firewall with Memory"
	$(Q)$(MAKE) -C Firewall_with_mem clean
	$(Q)echo "[INFO] Cleaning Testbenches for Firewall"
	$(Q)$(MAKE) -C Firewall_with_mem/REGRESSION_TEST clean
	$(Q)echo "[INFO] Cleaning Firewall with Memory and Async Channel"
	$(Q)$(MAKE) -C Demo_SC_firewall clean
ifeq ($(strip $(BUILD_HW_TESTBENCH_ARM)), yes)
	$(Q)$(MAKE) -C Demo_HARDWARE_firewall_ARM -f Makefile.apps clean
endif
