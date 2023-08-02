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

#define BRAM_NO 1 // FIXED THROUGHOUT THE TEST

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
int i, j, k, l, m = 0;
unsigned int wBit, rBit = 0;

unsigned int default_written_value = (unsigned int) 0; // written directly to BRAM
unsigned int write_value   = (unsigned int) 1; // written via FW
unsigned int actual_value   = (unsigned int) 65535;

pattern[3] = w;
pattern[2] = r;
pattern[1] = wr;
pattern[0] = none;
  for (i=0; i<4; i++){  
    for (j=0; j<4; j++){
      for (k=0; k<4; k++){
        for (l=0; l<4; l++){
       
        printf("\n##################################################\n");
        printf("###############     CASE       ###################\n");
	printf("##################################################\n");
	printf("###### PORT 1 ## PORT 2 ## PORT 3 ## PORT 4 #######\n");
	printf("######  %s    ##  %s    ##   %s   ##   %s   #######\n", getPatternName(pattern[i]), getPatternName(pattern[j]), getPatternName(pattern[k]), getPatternName(pattern[l]));
        printf("##################################################\n");
//        printf("Press Any Key to Continue...");
//        char c = getchar();
        
	//Port 1: Get write and read bit from rule for this port and setup FW on this port
	rule = pattern[i]; //pattern for all
	wBit = (rule & (1 << 5)) >> 5;
	rBit = (rule & (1 << 4)) >> 4;
        printf("*** Setting up the Rules...\n");
//        printf("Setting Rule %s for port 1 on Simulator\n", getPatternName(pattern[i]));
	doSetUp(1, 0x00000000, 0x00000100, 1, wBit, rBit, BRAM_NO);
        
	//Port 2: Get write and read bit from rule for this port and setup FW on this port
	rule = pattern[j]; //pattern for all
	wBit = (rule & (1 << 5)) >> 5;
	rBit = (rule & (1 << 4)) >> 4;
//        printf("Setting Rule %s for port 2 on Simulator\n", getPatternName(pattern[j]));
	doSetUp(2, 0x00000000, 0x00000100, 1, wBit, rBit, BRAM_NO);
        
	//Port 3: Get write and read bit from rule for this port and setup FW on this port
	rule = pattern[k]; //pattern for all
	wBit = (rule & (1 << 5)) >> 5;
	rBit = (rule & (1 << 4)) >> 4;
//        printf("Setting Rule %s for port 3 on Simulator\n", getPatternName(pattern[k]));
	doSetUp(3, 0x00000000, 0x00000100, 1, wBit, rBit, BRAM_NO);
        
    //Port 4: Get write and read bit from rule for this port and setup FW on this port
	rule = pattern[l]; //pattern for all
	wBit = (rule & (1 << 5)) >> 5;
	rBit = (rule & (1 << 4)) >> 4;
//        printf("Setting Rule %s for port 4 on Simulator\n", getPatternName(pattern[l]));
	doSetUp(4, 0x00000000, 0x00000100, 1, wBit, rBit, BRAM_NO);
        
	//change output port also?
        printf("*** Executing The Write Tests...\n");
	for(m=1; m<5; m++){
		if (m==1)
	        patt = pattern [i];
	    else if (m==2)
	        patt = pattern [j];
	    else if (m==3)
	        patt = pattern [k];
	    else
	        patt = pattern [l];
        sc_user_accessBram(0x0, BRAM_NO, 0x00000000, &default_written_value);
        write_value += 1;
        doFWReadWrite(0x0, &write_value, m, BRAM_NO,  0x00000000);                
        sc_user_accessBram(0x1, BRAM_NO, 0x00000000, &actual_value);
        check_write(m, patt, actual_value, write_value);
        actual_value   = (unsigned int) 65535;
        if (write_value > 15) {
			write_value = 1;
        }
     }
}
}
}
}
return(0);	
}
