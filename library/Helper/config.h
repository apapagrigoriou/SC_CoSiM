/*
CoSiM Helper Functions/Classes
Copyright (C) TEI Crete

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// C++ Interface: config
//
// Description: Common configuration parameters of the HSoC library
//
// Note: In a new HSoC library version, more system configuration parameters will be
//       shifted from the modules to config.h to simplify design space exploration
//
//
// Authors: HSoC Team (C) - 29 August 2013
// Initial version: Antonis Papagrigoriou
// Changes: Polydoros Petrakis, Miltos Grammatikakis
//
// Copyright: See LICENSE file that comes with this distribution
//
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#ifndef CONFIG_H_
#define CONFIG_H_

//Packet Structure Size in Bits - 
#define PACKET_DATA_WIDTH 128

//Fifo Size in Packets
#define FIFO_SIZE 10000

//Network-related
#define DEVICE_ADDR_WIDTH 8 // device node address (node id)

//Memory - default values in bits
#define MEMORY_SIZE 256 // number of memory words (if not redefined in the constructor)
#define MEMORY_DATA_WIDTH 64 // width of a memory word
#define MEMORY_ADDR_WIDTH 64 //memory address

//Memory - init/read/write operation execution time
#define MEM_INIT_OP_TIME 120 // time to initialize memory
#define MEM_READ_OP_TIME 10 // time to read from memory
#define MEM_WRITE_OP_TIME 10 // time to write to memory

// Memory Controller - default buffer Size at ts network interface
#define PCKT_BUFF_SIZE 10

//Router - No. of VCs
#define NUM_OF_VCS  6

// Amba Bus Parameters (default values - note: other values have not been checked!)
typedef bool signal_t; //Signal state type
typedef sc_dt::uint64 data_t; // 64-bit Bus data type
typedef unsigned int data_t32; // 32-bit Bus data type (used only in DMA registers).
static const unsigned int BUSWIDTH = sizeof(data_t) * 8; // 64-bit Bus width
static const unsigned int BURSTLENGTH = 2U; // 128 bit (2 words) 

#endif /*CONFIG_H_*/
