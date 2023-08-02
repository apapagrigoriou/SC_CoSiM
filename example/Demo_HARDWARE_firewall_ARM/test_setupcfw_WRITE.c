/*
CoSiM Demo Using the Firewall Hardware as DuT 
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
#include "user_mode.h"

#define BRAM_NO 1 // FIXED THROUGHOUT THE TEST

// none -> wbit=1, rbit=1 (deny all, i.e. none passes)

// this is pass rule, w -> w will pass
enum rule {w = 0x90, r = 0xA0, wr = 0x80, none = 0xB0};
enum rule pattern[4], patt;


const char *getPatternName(enum rule pattern){
	switch(pattern){
		case w: return "w";
		case r: return "r";
		case wr: return "wr";
		case none: return "none";
	}
}


// check write test
void check_write(unsigned int input_port, enum rule pattern, 
                 unsigned int actual_value, unsigned int write_value) {
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


void check_read(unsigned int input_port, enum rule pattern, 
                 unsigned int actual_value, unsigned int read_value) {
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

int main(int argc, char *argv[])
{
unsigned int rule;
int i, j, k, l, m = 0;
unsigned int wBit, rBit = 0;
volatile int readVal = 0, writeVal = 0;
volatile int rc1 = 0, rc2 = 0, rc3 = 0;
volatile int rc4 = 0, rc5 = 0, rc6 = 0;

unsigned int default_written_value = (unsigned int) 0; // written directly to BRAM
unsigned int write_value   = (unsigned int) 1; // written via FW
unsigned int actual_value   = (unsigned int) 65535;
unsigned int read_value = (unsigned int) 65534;

int count = 0;

pattern[3] = w;
pattern[2] = r;
pattern[1] = wr;
pattern[0] = none;

  for (i=0; i<4; i++){  
    for (j=0; j<4; j++){
      for (k=0; k<4; k++){
        for (l=0; l<4; l++){
        //getchar();


       //pattern[i] = pattern[j] = pattern[k] = pattern[l] = 0xB0; // w = 0x90, r = 0xA0, wr = 0x80, none = 0xB0
       //pattern[i] = pattern[j] = pattern[k] = pattern[l] = 0x80; // w = 0x90, r = 0xA0, wr = 0x80, none = 0xB0

	printf("\n##################################################\n");
	printf("_###### PORT 1 ## PORT 2 ## PORT 3 ## PORT 4 ######\n");
	printf("_######   %s    ##   %s    ##   %s    ##   %s    ######\n", getPatternName(pattern[i]), getPatternName(pattern[j]), getPatternName(pattern[k]), getPatternName(pattern[l]));

	//Port 1: Get write and read bit from rule for this port and setup FW on this port
	rule = pattern[i]; //pattern for all
	wBit = (rule & (1 << 5)) >> 5;
	rBit = (rule & (1 << 4)) >> 4;
/*
if ((wBit==0) && (rBit==0))
setupCFW(1, 0x00000000, 0x00000100, 0, wBit, rBit, 1); // also check [0x0, 0x80] and [0x80, 0x100] -> ECG scenario
else
*/
	setupCFW(1, 0x00000000, 0x00000100, 1, wBit, rBit, BRAM_NO); // also check [0x0, 0x80] and [0x80, 0x100] -> ECG scenario
//sleep(0.999);

	//Port 2: Get write and read bit from rule for this port and setup FW on this port
	rule = pattern[j]; //pattern for all
	wBit = (rule & (1 << 5)) >> 5;
	rBit = (rule & (1 << 4)) >> 4;
/*
if ((wBit==0) && (rBit==0))
setupCFW(2, 0x00000000, 0x00000100, 0, wBit, rBit, 2); // also check [0x0, 0x80] and [0x80, 0x100] -> ECG scenario
else
*/
	setupCFW(2, 0x00000000, 0x00000100, 1, wBit, rBit, BRAM_NO); // also check [0x0, 0x80] and [0x80, 0x100] -> ECG scenario
//sleep(0.999);

	//Port 3: Get write and read bit from rule for this port and setup FW on this port
	rule = pattern[k]; //pattern for all
	wBit = (rule & (1 << 5)) >> 5;
	rBit = (rule & (1 << 4)) >> 4;
/*
if ((wBit==0) && (rBit==0))
setupCFW(3, 0x00000000, 0x00000100, 0, wBit, rBit, 3); // also check [0x0, 0x80] and [0x80, 0x100] -> ECG scenario
else
*/
	setupCFW(3, 0x00000000, 0x00000100, 1, wBit, rBit, BRAM_NO); // also check [0x0, 0x80] and [0x80, 0x100] -> ECG scenario

//sleep(0.999);
	//Port 4: Get write and read bit from rule for this port and setup FW on this port
	rule = pattern[l]; //pattern for all
	wBit = (rule & (1 << 5)) >> 5;
	rBit = (rule & (1 << 4)) >> 4;
/*
if ((wBit==0) && (rBit==0))
setupCFW(4, 0x00000000, 0x00000100, 0, wBit, rBit, 4); // also check [0x0, 0x80] and [0x80, 0x100] -> ECG scenario
else
*/
	setupCFW(4, 0x00000000, 0x00000100, 1, wBit, rBit, BRAM_NO); // also check [0x0, 0x80] and [0x80, 0x100] -> ECG scenario
//sleep(0.999);


	//system("sleep 0.2;");

	//change output port also?
	for(m=1; m<5; m++){

	        if (m==1)
	        patt = pattern [i];
	        else if (m==2)
	        patt = pattern [j];
	        else if (m==3)
	        patt = pattern [k];
	        else
	        patt = pattern [l];

                // WRITE TEST FOR BRAM1
                rc1 = accessBram(0x0, BRAM_NO, 0x00000000, &default_written_value); // write 00000000
                //if (rc1 != 0)
                //  exit(1);

		// practically_not_needed: make sure accessBram write was executed before proceeding to write via FW
		do {
                	rc1 = accessBram(0x1, BRAM_NO, 0x00000000, &read_value); // read value
			count++;
			//printf("***********************Multiple calls to accessBram -  count:%d \n", count);
		} while (read_value != default_written_value);

		rc2 = accessBramFW(0x0, m, BRAM_NO, 0x00000000, &write_value); // initially 0;
                //if (rc2 != 0)
                //  exit(1);

                if (rc2 == 0) // practically_not_needed: block execution of accessBram read until accessBramFW executed
                rc3 = accessBram(0x1, BRAM_NO, 0x00000000, &actual_value); // intially 65535

printf("______________write_value:%d actual_value:%d \n", write_value, actual_value);

                //if (rc3 != 0)
                //  exit(1);
		check_write(m, patt, actual_value, write_value);
		actual_value   = (unsigned int) 65535;
                write_value = write_value + (unsigned int) 1;
//printf("______________write_value:%d\n", write_value);

		//if (m==4)
		//	system("sleep 10000");

	}
	//system("rmmod kernel_mode.ko;");
	//system("insmod kernel_mode.ko;");
	printf("_##################################################\n\n");


       }
     }
   }
 }
        exit(0);
}
