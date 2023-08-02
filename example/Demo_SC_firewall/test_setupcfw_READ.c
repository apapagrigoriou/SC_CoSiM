/*
CoSiM Demo Using the Firewall Module as DuT 
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
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h> 
#include <unistd.h> //new processes 
#include <sys/wait.h> 
#include <inttypes.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/mman.h>
#include <stdarg.h>
#ifdef _SHM
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#endif
#ifdef _POSIX
#include <sys/mman.h>
#include <semaphore.h>
#endif
#ifdef _TCP
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#endif

#include "sc_user_mode.h"
// this is pass rule, w -> w will pass
enum ruletype {w = 0x90, r = 0xA0, wr = 0x80, none = 0xB0};
enum ruletype pattern[4];
enum ruletype patt;

const char *getPatternName(enum ruletype pattern){
	switch(pattern){
		case w: return "w";
		case r: return "r";
		case wr: return "wr";
		case none: return "none";
	}
	return "none";
}

// check write test
void check_write(unsigned int input_port, enum ruletype pattern,  unsigned int actual_value, unsigned int write_value) {
switch(pattern){
        case w:
                if(write_value == actual_value)
		   printf("_Input_Port:%d  Accept:W  -> Rule 'W' PASS (write_value, actual_value):(%x :%x) \n", input_port, write_value, actual_value);
		else
		   printf("_Input_Port:%d  Accept:W  -> Rule 'W' ________________FAIL (write_value, actual_value):(%x %x)\n\n", input_port, write_value, actual_value);

		break;
        case r:
                if(write_value != actual_value)
		   printf("_Input_Port:%d  Accept:R  -> Rule 'W' PASS (write_value, actual_value):(%x :%x) \n", input_port, write_value, actual_value);
		else
		   printf("_Input_Port:%d  Accept:R  -> Rule 'W' ________________FAIL (write_value, actual_value):(%x %x)\n\n", input_port, write_value, actual_value);

		break;
        case wr:
                if(write_value == actual_value)
		   printf("_Input_Port:%d  Accept:WR -> Rule 'W' PASS (write_value, actual_value):(%x :%x) \n", input_port, write_value, actual_value);
		else
		   printf("_Input_Port:%d  Accept:WR -> Rule 'W' ________________FAIL (write_value, actual_value):(%x %x)\n\n", input_port, write_value, actual_value);

		break;
        case none: // deny all
                if(write_value != actual_value)
		   printf("_Input_Port:%d  Accept:-  -> Rule 'W' PASS (write_value, actual_value):(%x :%x) \n", input_port, write_value, actual_value);
		else
		   printf("_Input_Port:%d  Accept:-  -> Rule 'W' ________________FAIL (write_value, actual_value):(%x %x)\n\n", input_port, write_value, actual_value);

		break;
//	default: printf("not special");
//	 break;
    }

}

void check_read(unsigned int input_port, enum ruletype pattern, unsigned int actual_value, unsigned int read_value) {
switch(pattern){
        case w:
		if(read_value != actual_value)
		   printf("_Input_Port:%d  Accept:W  -> Rule 'R' PASS (read_value, actual_value):(%x %x) \n", input_port, read_value, actual_value);
		else
		   printf("_Input_Port:%d  Accept:W  -> Rule 'R' ________________FAIL (read_value, actual_value):(%x %x) \n\n", input_port, read_value, actual_value);
		break;
        case r:
		if(read_value == actual_value)
		   printf("_Input_Port:%d  Accept:R  -> Rule 'R' PASS (read_value, actual_value):(%x %x) \n", input_port, read_value, actual_value);
		else
	           printf("_Input_Port:%d  Accept:R  -> Rule 'R' ________________FAIL (read_value, actual_value):(%x %x) \n\n", input_port, read_value, actual_value);
		break;
        case wr:
		if(read_value == actual_value)
		   printf("_Input_Port:%d  Accept:WR -> Rule 'R' PASS (read_value, actual_value):(%x %x) \n", input_port, read_value, actual_value);
		else
	           printf("_Input_Port:%d  Accept:WR -> Rule 'R' ________________FAIL (read_value, actual_value):(%x %x) \n\n", input_port, read_value, actual_value);
		break;
        case none: // deny all
		if(read_value != actual_value)
		   printf("_Input_Port:%d  Accept:-  -> Rule 'R' PASS (read_value, actual_value):(%x %x) \n", input_port, read_value, actual_value);
		else
	           printf("_Input_Port:%d  Accept:-  -> Rule 'W' ________________FAIL (read_value, actual_value):(%x %x) \n\n", input_port, read_value, actual_value);
		break;
//	default: printf("not special");
//	 break;
    }

}

// This function calls dynamically arguments with ... 
int doSetUp(unsigned int inport, unsigned int L_addr_reg, unsigned int H_addr_reg, unsigned int myrule, unsigned int read_ops, unsigned int write_ops, unsigned int outport){
    pid_t myid1;
    // timing variables
    myid1=fork();
    if(myid1 > 0){
        wait(0);
	}else if(myid1==0){
//		printf("doSetUp_setupCFW: inport:%u, L/H_addr_reg:(%x %x) => Rule_reg:%x W/R:(%u %u) outport:%u \n", inport, L_addr_reg, H_addr_reg, myrule, write_ops, read_ops, outport);
		sc_user_setupCFW(inport, L_addr_reg, H_addr_reg, myrule, read_ops, write_ops, outport);
	    exit(0);
	}
	return 0;
}

int doFWReadWrite(unsigned int op_code, unsigned int* data, unsigned int inport, unsigned int outport, unsigned int addr_reg){
    pid_t myid1;
    myid1=fork();
    if(myid1 > 0){
        wait(0);
	}else if(myid1==0){
		sc_user_accessBramFW(op_code, inport, outport, addr_reg, data);
		exit(0);
	}
	return 0;
}


int main(int argc, char *argv[])
{
unsigned int rule;
int i, j, k, l = 0;
unsigned int addr_reg = 0x00000000;
unsigned int wBit, rBit = 0;

unsigned int default_written_value1 = (unsigned int) 0; // written directly to BRAM
unsigned int read_value1  = (unsigned int) 10000; // written via FW
unsigned int default_written_value2 = (unsigned int) 1024; // written directly to BRAM
unsigned int read_value2  = (unsigned int) 10000; // written via FW
unsigned int default_written_value3 = (unsigned int) 2048; // written directly to BRAM
unsigned int read_value3  = (unsigned int) 10000; // written via FW
unsigned int default_written_value4 = (unsigned int) 3072; // written directly to BRAM
unsigned int read_value4  = (unsigned int) 10000; // written via FW
unsigned int sc_default_written_value1 = (unsigned int) 0; // written directly to BRAM
unsigned int sc_read_value1  = (unsigned int) 10000; // written via FW
unsigned int sc_default_written_value2 = (unsigned int) 1024; // written directly to BRAM
unsigned int sc_read_value2  = (unsigned int) 10000; // written via FW
unsigned int sc_default_written_value3 = (unsigned int) 2048; // written directly to BRAM
unsigned int sc_read_value3  = (unsigned int) 10000; // written via FW
unsigned int sc_default_written_value4 = (unsigned int) 3072; // written directly to BRAM
unsigned int sc_read_value4  = (unsigned int) 10000; // written via FW

pattern[3] = w;
pattern[2] = r;
pattern[1] = wr;
pattern[0] = none;

for (i=0; i<4; i++){  
  for (j=0; j<4; j++){
    for (k=0; k<4; k++){
      for (l=0; l<4; l++){
		printf("\n##################################################\n");
		printf("_###### PORT 1 ## PORT 2 ## PORT 3 ## PORT 4 ######\n");
		printf("_######   %s    ##   %s    ##   %s    ##   %s    ######\n", getPatternName(pattern[i]), getPatternName(pattern[j]), getPatternName(pattern[k]), getPatternName(pattern[l]));
		// test 1 - complex setup with wr accept
		// inport, Low range, High range, rule, wbit, rbit, outport
		if (default_written_value1 >= 9)
			default_written_value1 = (unsigned int) 0;
		default_written_value1 += (unsigned int) 1;
		//printf("written_value:%d \n",default_written_value1);
		read_value1   = (unsigned int) 65535;
		//sc_values
		sc_default_written_value1 = default_written_value1;
		sc_read_value1 = read_value1;
		
		// note with accessBram the test fails (no  guarrantee when it will be executed, ok only if second follows and used)
        //printf("Simulator\n");
        sc_user_accessBramFW(0x0, 1, 1, addr_reg, &sc_default_written_value1); // write 00000000 
        //printf("HW\n");
		//rc1 = accessBramFW(0x0, 1, 1, addr_reg, &default_written_value1); // write 00000000

		//Port 1: Get write and read bit from rule for this port and setup FW on this port
		rule = pattern [i];
		wBit = (rule & (1 << 5)) >> 5;
		rBit = (rule & (1 << 4)) >> 4;
        //printf("Simulator\n");
		doSetUp(1, 0x00000000, 0x00000100, 1, wBit, rBit, 1);
        //printf("HW\n");
      	//setupCFW(1, 0x00000000, 0x00000100, 1, wBit, rBit, 1);

		// test 1.a
        //printf("Simulator : test 1.a\n");
        sc_user_accessBramFW(0x1, 1, 1, addr_reg, &sc_read_value1);
		check_read(1, rule, sc_default_written_value1, sc_read_value1);
        //printf("HW : test 1.a\n");
		//rc2 = accessBramFW(0x1, 1, 1, addr_reg, &read_value1);
		//printf("default_written_value1:%d, read_value1:%d \n", default_written_value1, read_value1); fflush(stdout);
		//check_read(1, rule, default_written_value1, read_value1);

		// reset for new round
        //printf("Simulator\n");
		doSetUp(1, 0x00000000, 0x00000100, 1, 0, 0, 1);
        //printf("HW\n");
      	//setupCFW(1, 0x00000000, 0x00000100, 1, 0, 0, 1); 

		//test 2 - complex setup with wr accept
		// inport, Low range, High range, rule, wbit, rbit, outport
		default_written_value2 += (unsigned int) 1;
		read_value2   = (unsigned int) 65535;
		//sc_values
		sc_default_written_value2 = default_written_value2;
		sc_read_value2 = read_value2;

		// note with accessBram the test fails (no  guarrantee when it will be executed, ok only if second follows and used)
        //printf("Simulator\n");
        sc_user_accessBramFW(0x0, 2, 2, addr_reg, &sc_default_written_value2); // write 00000000 
        //printf("HW\n");
        //rc1 = accessBramFW(0x0, 2, 2, addr_reg, &default_written_value2); // write 00000000

		//Port 2: Get write and read bit from rule for this port and setup FW on this port
		rule = pattern [j];
		wBit = (rule & (1 << 5)) >> 5;
		rBit = (rule & (1 << 4)) >> 4;
        //printf("Simulator\n");
		doSetUp(2, 0x00000000, 0x00000100, 1, wBit, rBit, 2);
        //printf("HW\n");
   		//setupCFW(2, 0x00000000, 0x00000100, 1, wBit, rBit, 2);

		// test 1.b
        //printf("Simulator test 1.b\n");
        sc_user_accessBramFW(0x1, 2, 2, addr_reg, &sc_read_value2);
		check_read(2, rule, sc_default_written_value2, sc_read_value2);
        //printf("HW test 1.b\n");
		//rc2 = accessBramFW(0x1, 2, 2, addr_reg, &read_value2);
		//printf("default_written_value2:%d, read_value2:%d \n", default_written_value2, read_value2); fflush(stdout);
		//check_read(2, rule, default_written_value2, read_value2);

		// reset for new round
        //printf("Simulator\n");
		doSetUp(2, 0x00000000, 0x00000100, 1, 0, 0, 2);
        //printf("HW\n");
   		//setupCFW(2, 0x00000000, 0x00000100, 1, 0, 0, 2);

		//test 3 - complex setup with wr accept
		// inport, Low range, High range, rule, wbit, rbit, outport
		default_written_value3 += (unsigned int) 1;
		read_value3   = (unsigned int) 65535;
		//sc_values
		sc_default_written_value3 = default_written_value3;
		sc_read_value3 = read_value3;

		// note with accessBram the test fails (no  guarrantee when it will be executed, ok only if second follows and used)
        //printf("Simulator\n");
        sc_user_accessBramFW(0x0, 3, 3, addr_reg, &sc_default_written_value3); // write 00000000 
        //printf("HW\n");
        //rc1 = accessBramFW(0x0, 3, 3, addr_reg, &default_written_value3); // write 00000000

		//Port 3: Get write and read bit from rule for this port and setup FW on this port
		rule = pattern [k];
		wBit = (rule & (1 << 5)) >> 5;
		rBit = (rule & (1 << 4)) >> 4;
        //printf("Simulator\n");
		doSetUp(3, 0x00000000, 0x00000100, 1, wBit, rBit, 3);
        //printf("HW\n");
   		//setupCFW(3, 0x00000000, 0x00000100, 1, wBit, rBit, 3);

		// test 1.c
        //printf("Simulator test 1.c\n");
        sc_user_accessBramFW(0x1, 3, 3, addr_reg, &sc_read_value3);
		check_read(3, rule, sc_default_written_value3, sc_read_value3);
        //printf("HW\n");
		//rc2 = accessBramFW(0x1, 3, 3, addr_reg, &read_value3);
		//printf("default_written_value3:%d, read_value3:%d \n", default_written_value3, read_value3); fflush(stdout);
		//check_read(3, rule, default_written_value3, read_value3);

		// reset for new round
        //printf("Simulator\n");
		doSetUp(3, 0x00000000, 0x00000100, 1, 0, 0, 3);
        //printf("HW\n");
      	//setupCFW(3, 0x00000000, 0x00000100, 1, 0, 0, 3);

		//test 4 - complex setup with wr accept
		// inport, Low range, High range, rule, wbit, rbit, outport
		default_written_value4 += (unsigned int) 1;
		read_value4   = (unsigned int) 65535;
		//sc_values
		sc_default_written_value4 = default_written_value4;
		sc_read_value4 = read_value4;

		// note with accessBram the test fails (no  guarrantee when it will be executed, ok only if second follows and used)
        //printf("Simulator\n");
        sc_user_accessBramFW(0x0, 4, 4, addr_reg, &sc_default_written_value4); // write 00000000 
        //printf("HW\n");
        //rc1 = accessBramFW(0x0, 4, 4, addr_reg, &default_written_value4); // write 00000000

		//Port 4: Get write and read bit from rule for this port and setup FW on this port
		rule = pattern [l];
		wBit = (rule & (1 << 5)) >> 5;
		rBit = (rule & (1 << 4)) >> 4;
        //printf("Simulator\n");
		doSetUp(4, 0x00000000, 0x00000100, 1, wBit, rBit, 4);
        //printf("HW\n");
   		//setupCFW(4, 0x00000000, 0x00000100, 1, wBit, rBit, 4);

		// test 1.d
        //printf("Simulator test 1.d\n");
        sc_user_accessBramFW(0x1, 4, 4, addr_reg, &sc_read_value4);
		check_read(4, rule, sc_default_written_value4, sc_read_value4);
        //printf("HW\n");
		//rc2 = accessBramFW(0x1, 4, 4, addr_reg, &read_value4);
		//printf("default_written_value4:%d, read_value4:%d \n", default_written_value4, read_value4); fflush(stdout);
		//check_read(4, rule, default_written_value4, read_value4);

		// reset for new round
		// reset for new round
        //printf("Simulator\n");
		doSetUp(4, 0x00000000, 0x00000100, 1, 0, 0, 4);
        //printf("HW\n");
      	//setupCFW(4, 0x00000000, 0x00000100, 1, 0, 0, 4);

		printf("_##################################################\n\n");


	}
	}
	}
	}
    exit(0);
}
