#Configurator for the CoSiM Platform
#Copyright (C) TEI Crete
#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#as published by the Free Software Foundation; either version 2
#of the License, or (at your option) any later version.
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

# Change these variables to match your installation

# Set to @ to enable quiet compilation & installation
Q = @

# Set location for the created SC_COSIM library
INST_LIB_PATH=/home/apapa/Workspace/CoSiM_lib
# Set location of SystemC Installation
SYSTEMC=/usr
# Set SystemC library architecture (linux or linux64)
TARGET_ARCH=linux64
# Enable Realtimify
REALTIME=no
# If Realtimify is enabled, set DELAY in number of units
RT_TIME=1
# If Realtimify is enabled, set unit type (SC_TIME)
RT_UNIT=SC_SEC

# Define communication interface, as follows: a) shm: means Shared Memory, posix: Posix Shared memory, tcp: TCP Interface
INT_TYPE=tcp
# For TCP interface, the listening port is fixed (the server listens to any interface address)
TCP_SERVER_PORT=1999
# For TCP interface, the default server address is localhost (used in the testbench)
TCP_TESTBENCH_SERVER_ADDRESS=localhost

#testbech parameters
# Set application username/group (OWNER)
APP_TESTBENCH_USERNAME=apapa
# Set to yes builds Only hardware testbenches (on ARM Zedboard with NoC Firewall installed,
# see https://github.com/angmouzakitis/student_xohw18-187
# To Buld the Kernel Module you must Manualy execute the Make using the Makefile 
# in example/Demo_HARDWARE_firewall_ARM/ folder
BUILD_HW_TESTBENCH_ARM=no
