# SC_CoSiM

Description
-----------
SC_CoSiM is a platform which focuses on validating a full system, by performing near real-time, command-to-command co-simulation of the hardware DUT implemented on FPGA as a full system prototype (with CPU, on-chip interconnect, memory, application, drivers, and OS, typically Linux) against its equivalent cycle-approximate system-level model of the DUT. Our work presented at DVCon-Europe 2018 extended the technology demonstrator from David C Black(https://github.com/dcblack/technology_demonstrator).
This project was moved from sourseforge. For more information and history about it please visit https://sourceforge.net/projects/sc-cosim/.
Our platform is based on systemC v 2.3 which can be downloaded from: https://github.com/systemc/systemc-2.3
To install systemC on zedboard or other ARMv7 architecture you must enable the pthreads support in systemC (--enable-pthreads) in the configure of systemC

This package specifies an experimental framework (called co-simulation platform) for co-validation and co-design. The proposed framework is based on a bidirectional asynchronous channel between a SystemC device model and a physical, full system prototype of the same device on FPGA. The asynchronous channel uses TCP sockets when the
SystemC model is on a different host, and shared memory when the model is on the same host.
Synchronization is maintained between the two sides by aligning the SystemC side with the real-time clock (using Realtimify). The full system is used to co-simulate a complete firewall application, consisting of a NoC firewall module (the DUT), drivers, OS, and application software running on an ARM v7 Zedboard. The asynchronous channel has good performance, especially for shared memory communication, with an overhead in the ms range. Overall simulation speed is sufficient such that real-time performance characteristics can be verified.

Documentation & Reference
-------------------------
For more information, please refer to the presentation and pdf (preprint) file in docs/

Installation Notes
-------------------

1) Modify the config.mk file according to your installation
The variables are:

Q                            : Possible values are "@" or none. This variable enables/disables display of compilation details
INST_LIB_PATH                : Absolute path indicating the location of the SC_COSIM library to be built
SYSTEMC                      : Absolute path indicating the location of the SystemC library
TARGET_ARCH                  : Possible values are "linux64" and "linux" depending on the linux system architecture (64- or 32-bit)
REALTIME                     : Possible values are "yes" or "no". This variable enables support for near real-time simulation via Realtimify
RT_TIME                      : If Realtimify is enabled, this is the number of SC_TIME units used by Realtimify module 
RT_UNIT                      : If Realtimify is enabled, this describes the unit type (SC_TIME)
INT_TYPE                     : This defines the communication interface: a) shm: Shared Memory, posix: Posix Shared memory, tcp: TCP
TCP_SERVER_PORT              : For TCP interface, this is the listening port
TCP_TESTBENCH_SERVER_ADDRESS : For TCP interface, this is the server address (used in the testbench)
APP_TESTBENCH_USERNAME       : This is the username of the system user (used in the firewall)
BUILD_HW_TESTBENCH_ARM       : Possible values are "yes" or "no". This option builds hardware testbenches on ARM Zedboard with NoC Firewall
			       (for the NoC Firewall, boot.bin, dtb, driver and file sys, see https://github.com/angmouzakitis/student_xohw18-187)
KERNELDIR                    : Absolute path indicating the sources of the kernel for building the hardware NoC Firewall module

Installing the Library and Examples
-----------------------------------
After defining configuration in config.mk, install the library by running from the same dir 
   make &&  make install
To Build the Exaples & regression tests afterwards the instalation of the library run:
    make all

Each interface type (tcp/shm/posix) creates an appropriate testbench application in the example folder

Execution Notes (for testing only the SystemC simulator)
---------------------------------------------------------
To start yp the simulator, open a terminal, change directory to the sources dir, and run the demo, i.e.
cd example/Demo_SC_firewall
./demo_sc_firewall.exe

Then open another terminal, change directory to the sources dir, and run the appropriate test
	cd example/Demo_SC_firewall
[A]   ./test_read.exe
	To test the read functionality of the firewall DUT model (testbench 1)
[B]   ./test_write.exe
	To test the write functionality of the firewall DUT Model (testbench 2)
[C]   ./test_app.exe
	To run a real appplication that uses the DUT model (testbench 3, e-Health)

To run the same tests on the real hardware enable the option for ARM build in the config.mk,
change directory to the sources dir, and run 
cd example/Demo_HARDWARE_firewall_ARM
and the one of the following to run the appropriate test
[A]   ./test_read.exe
	To test the read functionality of the Hardeare firewall DUT (testbench 1) 
[B]   ./test_write.exe
	To test the write functionality of the Hardeare firewall DUT (testbench 2)
[C]   ./test_app.exe
	To run a real appplication that uses the Hardeare firewall DUT (testbench 3, e-Health)
	
To run a test with another communication interface type you have to rebuild the library & testbenches 
with the apropriate option set in the config.mk file, and make clean before you rebuild everything

PS: [!!! Sometimes it will be better to restart the simulator after each test!!!]


Enjoy :-)

License
-------
GPLv2

