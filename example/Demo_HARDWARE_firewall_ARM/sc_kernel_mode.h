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
#ifndef SC_KERNEL_MODE_H
#define SC_KERNEL_MODE_H
#define CMD_DELAY 1
//#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>   // includes all functions for kernel like printk and their enum parameters
#include <linux/types.h>    // defines the dev_t which contains both major and minor numbers
#include <linux/kdev_t.h>   // obtain the Macros from which we obtain the major/Minor
#include <linux/fs.h>       // contains the function for registering the major & minor Numbers
#include <linux/sched.h>    // add delay

#include "sc_kernel_structs.h" //sc_fw_ds, sc_access_ds structs
#include "async_packet2.h"     //async_packet struct

uint32_t iobram1 = 0x80000000; // access BRAM1 via Switch/Firewall port 0
uint32_t iobram2 = 0x80001000; // access BRAM2 via Switch/Firewall port 0
uint32_t iobram3 = 0x80002000; // access BRAM3 via Switch/Firewall port 0
uint32_t iobram4 = 0x80003000; // access BRAM4 via Switch/Firewall port 0
uint32_t iobram1_direct = 0x40000000; // direct access BRAM1
uint32_t iobram2_direct = 0x40001000; // direct access BRAM2
uint32_t iobram3_direct = 0x40002000; // direct access BRAM3
uint32_t iobram4_direct = 0x40003000; // direct access BRAM4
uint32_t noc_setup_registers = 0x50000000;

//#include <linux/cred.h>

static int first_time=1;
static int count = 0;

// Initialize register values
static int startingLowerReg = 5*4; // lower range register
static int startingHighReg = 13*4; // high range register
static int startingRuleReg = 21*4; // rule register
static int startingStatisticsTotal = 9*4; // Total Packets Stats Register
static int startingStatisticsFifo = 11*4; // FIFO Packets Stats Register
static int startingStatisticsFw = 13*4;  //FW Packets Stats Register

static int id_counter = 0;

sem_t *sharedsemA, *sharedsemB;  //shared semaphore to real and virtual driver

const char *semnameA = "sharedsemA";
const char *semnameB = "sharedsemB";

struct async_packet *shptr; //name of coSimulation data struct

struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec - start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

void sc_iowrite32(uint32_t data, uint32_t address, double *totaldelay, double *in_channel_delay, double *out_channel_delay){
//        printf("** IOWRITE:: 0x%x : 0x%x\n", address, data); 

	struct timespec diff_struct;
	//REQUEST
	shptr->id = id_counter++;
        if (data == 0x80000000){
            shptr->command = (uint8_t) 9;
        }else{
            shptr->command = (uint8_t) ASYNC_WRITE;
        }
	shptr->status = (uint8_t) ASYNC_INCOMPLETE_RESPONSE;
	shptr->address = address;
	shptr->systemc_start_time = 0;
	shptr->data = data;

//	printf("sc_iowrite32: prepared shptr for virtualClient \n");
    shptr->status = (uint8_t) ASYNC_INCOMPLETE_RESPONSE;
    clock_gettime(CLOCK_REALTIME, &(shptr->sender_out));
	//post sharedsemA
	sem_post(sharedsemA);

        if (data == 0x80000000){
            return;
        }
//	printf("sc_iowrite32: waiting for sharedsemB post from virtualClient \n");
	
	//RESPONCE
	//wait sharedsemB
	sem_wait(sharedsemB);
    clock_gettime(CLOCK_REALTIME, &(shptr->sender_in));

//	printf("sc_iowrite32: received *response* from virtualClient \n");
	
	if(shptr->status != (uint8_t) ASYNC_OK_RESPONSE){
		printf("sc_iowrite32: Status error!\n");	
	}else {
		diff_struct = diff(shptr->sender_out, shptr->channel_in);
		printf("sc_iowrite32:  inChannelDelay: %.0lf nsec\n", (double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec));
		(*in_channel_delay) = (double)(double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec);
		diff_struct = diff(shptr->sc_in, shptr->sc_out);
		printf("sc_iowrite32:  SystemCDelay: %.0lf nsec\n", (double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec));
		diff_struct = diff(shptr->channel_out, shptr->sender_in);
		printf("sc_iowrite32:  outChannelDelay: %.0lf nsec\n", (double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec));
		(*out_channel_delay) = ((double)((shptr->channel_in.tv_sec-shptr->sender_out.tv_sec)*1.0e-9 + (shptr->channel_in.tv_nsec-shptr->sender_out.tv_nsec)) + (double)((shptr->sender_in.tv_sec-shptr->channel_out.tv_sec)*1.0e-9 + (shptr->sender_in.tv_nsec-shptr->channel_out.tv_nsec))) / 2.0;
		diff_struct = diff(shptr->sender_out, shptr->sender_in);
		printf("sc_iowrite32:  TotalDelay: %.0lf nsec\n", (double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec));
		(*totaldelay) = (double)(double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec);
	}
}


uint32_t sc_ioread32(uint32_t address, double *totaldelay, double *in_channel_delay, double *out_channel_delay){
	uint32_t data;
	struct timespec diff_struct;
         // timing variables
//    printf("** IOREAD:: 0x%x\n", address); 

	//REQUEST
	shptr->id = id_counter++;
	shptr->command = (uint8_t) ASYNC_READ;
	shptr->status = (uint8_t) ASYNC_INCOMPLETE_RESPONSE;
	shptr->address = address;
	shptr->systemc_start_time = 0;
	shptr->data = data;

//	printf("sc_ioread32: prepared shptr for virtualClient \n");

    shptr->status = (uint8_t) ASYNC_INCOMPLETE_RESPONSE;
    clock_gettime(CLOCK_REALTIME, &(shptr->sender_out));
	
	//post sharedsemA
	sem_post(sharedsemA);

//	printf("sc_ioread32: waiting for sharedsemB post from virtualClient \n");

	//RESPONCE
	//wait sharedsemB
	sem_wait(sharedsemB);
    clock_gettime(CLOCK_REALTIME, &(shptr->sender_in));
	
//	printf("sc_ioread32: received *response* from virtualClient \n");

	if(shptr->status != (uint8_t) ASYNC_OK_RESPONSE ){
		data = -1;
		printf("sc_ioread32: Status error!\n");
	}else{
		data = shptr->data;
		diff_struct = diff(shptr->sender_out, shptr->channel_in);
		printf("sc_ioread32:  inChannelDelay: %.0lf nsec\n", (double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec));
		(*in_channel_delay) = (double)(double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec);
		diff_struct = diff(shptr->sc_in, shptr->sc_out);
		printf("sc_ioread32:  SystemCDelay: %.0lf nsec\n", (double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec));
		diff_struct = diff(shptr->channel_out, shptr->sender_in);
		printf("sc_ioread32:  outChannelDelay: %.0lf nsec\n", (double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec));
		(*out_channel_delay) = ((double)((shptr->channel_in.tv_sec-shptr->sender_out.tv_sec)*1.0e-9 + (shptr->channel_in.tv_nsec-shptr->sender_out.tv_nsec)) + (double)((shptr->sender_in.tv_sec-shptr->channel_out.tv_sec)*1.0e-9 + (shptr->sender_in.tv_nsec-shptr->channel_out.tv_nsec))) / 2.0;
		diff_struct = diff(shptr->sender_out, shptr->sender_in);
		printf("sc_ioread32:  TotalDelay: %.0lf nsec\n", (double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec));
		(*totaldelay) = (double)(double)(diff_struct.tv_sec*1.0e-9 + diff_struct.tv_nsec);
	}
	
	return data;
}


void init_virtual_kernel(){
	//shared memory variables & semaphores
    int shmid;
    key_t key;

	if(shptr==NULL){
	
                if ((key = ftok("/tmp/hello.txt", 'R')) == -1) /*Here the file must exist */ 
                { 
                    printf("Key File /tmp/hello.txt doesnot exists %s\n", strerror(errno));
                    exit(-1);
                }
		shmid = shmget(key, sizeof(struct async_packet), IPC_CREAT | 0666);
/*
                if(shmid < 0)
			printf("shmid now created\n");
		else
			printf("smid is %d\n", shmid);
*/
		shptr=(struct async_packet *)shmat(shmid,0,0);
	
	}
	if(sharedsemA==NULL){
		sharedsemA = sem_open(semnameA, O_EXCL);
	}
	if(sharedsemB==NULL){
		sharedsemB = sem_open(semnameB, O_EXCL);
	}	
		
}

/* Function to setup a firewall rule 
   This function called from user space to define a 
   - simple firewall rule (e.g setup an input port,the absolute range(L_addr_reg- H_addr_reg etc) or 
   - complex firewall rule (e.g setup an input port, an outport, the relative range( L_addr_reg - H_addr_reg etc). 
    */
void sc_setupFW(struct sc_fw_ds *my_sc_fw_ds_ptr)			
{
	int offset = 2*4;
	unsigned int inport = my_sc_fw_ds_ptr->input_port;
	unsigned int L_addr_reg = my_sc_fw_ds_ptr->L_addr_reg;
	unsigned int H_addr_reg = my_sc_fw_ds_ptr->H_addr_reg;
	unsigned int Rule_reg = my_sc_fw_ds_ptr->Rule_reg;
	double p_totaldelay, p_in_channel_delay, p_out_channel_delay;
	double totaldelay = 0.0;
	double in_channel_delay = 0.0, out_channel_delay = 0.0;
	//int gid;

	//gid = current_gid().val;
	//if (gid == 0) {

	init_virtual_kernel();
	
	id_counter = 1;
	//write above data to kernel space .... Antonis QQQQQQQ
//	printf("sc_setupFW: sending 1.iowrite32 to virtualClient \n");
	sc_iowrite32(0x00000000, noc_setup_registers + startingRuleReg + ((inport-1)*offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay ); //rule register
	totaldelay += p_totaldelay;
	in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
//	printf("sc_setupFW: sending 2.iowrite32 to virtualClient \n");
	sc_iowrite32(L_addr_reg, noc_setup_registers + startingLowerReg + ((inport-1)*offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay ); //lower register
	totaldelay += p_totaldelay;
	in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
//	printf("sc_setupFW: sending 3.iowrite32 to virtualClient \n");
	sc_iowrite32(H_addr_reg, noc_setup_registers + startingHighReg + ((inport-1)*offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay ); //high register
	totaldelay += p_totaldelay;
	in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
//	printf("sc_setupFW: sending 4.iowrite32 to virtualClient \n");
	sc_iowrite32(Rule_reg, noc_setup_registers + startingRuleReg + ((inport-1)*offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay ); //rule register
	totaldelay += p_totaldelay;
	in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
	printf("_STATS_ [sc_setupFW] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
	//} else {
	//	pr_info("NON-ROOT TRIED TO PERFORM setupFW");
	//	BUG_ON((gid != 0));
	//}
}

//EXPORT_SYMBOL(setupFW); /* export static read / write function*/
/* Function to read firewall setup data.
   This function called from user space to return 
   - the simple firewall rule (e.g setup an input port,the absolute range(L_addr_reg- H_addr_reg etc) or 
   - or complex firewall rule (e.g setup an input port, an outport, the relative range( L_addr_reg - H_addr_reg etc). 
    */
static void sc_readFWRegs(struct sc_fw_ds *my_sc_fw_ds_ptr)
{
	int offset = 2*4;
	int inport = my_sc_fw_ds_ptr->input_port;
	
	double p_totaldelay, p_in_channel_delay, p_out_channel_delay;
	double totaldelay = 0.0;
	double in_channel_delay = 0.0, out_channel_delay = 0.0;

	//gid = current_gid().val;
	//if (gid == 0) {
	my_sc_fw_ds_ptr->L_addr_reg = 0x0;
	my_sc_fw_ds_ptr->H_addr_reg = 0x0;
	my_sc_fw_ds_ptr->Rule_reg = 0x0;
	//retrieve data from kernel space and eventually to user space 
	my_sc_fw_ds_ptr->L_addr_reg = sc_ioread32(noc_setup_registers + startingLowerReg + ((inport-1)*offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
	totaldelay += p_totaldelay;
	in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
	my_sc_fw_ds_ptr->H_addr_reg = sc_ioread32(noc_setup_registers + startingHighReg + ((inport-1)*offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
	totaldelay += p_totaldelay;
	in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
	my_sc_fw_ds_ptr->Rule_reg = sc_ioread32(noc_setup_registers + startingRuleReg + ((inport-1)*offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
	totaldelay += p_totaldelay;
	in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;

	//} else {
	//	pr_info("NON-ROOT TRIED TO PERFORM readFWRegs");
	//	BUG_ON((gid != 0));
	//}
	printf("_STATS_ [sc_readFWRegs] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
}


/* Function to write or read data to a block (specific range).   
   This function called from user space to write (op_code 0) data to kernel space or read (op_code 1) data from kernel
   space.
    */
static int sc_accessBramFW(struct sc_access_ds *my_sc_access_ds_ptr)
{
	unsigned int write_value = 0x0; //hold validation data (check with sc_ioread32 method) 
	volatile unsigned int test_data = 0;
	unsigned int print_data;
    unsigned int op_code = my_sc_access_ds_ptr->op_code;
	unsigned int input_port = my_sc_access_ds_ptr->input_port;
	unsigned int output_port = my_sc_access_ds_ptr->output_port;
	unsigned int addr_reg = my_sc_access_ds_ptr->addr;
	unsigned int data = my_sc_access_ds_ptr->data;
	double p_totaldelay, p_in_channel_delay, p_out_channel_delay;
	double totaldelay = 0.0;
	double in_channel_delay = 0.0, out_channel_delay = 0.0;

	init_virtual_kernel();

	if (op_code == 0x0){ // write option
        int offset = 5*4;
		write_value = (addr_reg) << 24; // write address offset
		write_value += ((input_port - 1) << 18); // write source port
		write_value += ((output_port - 1) << 16); // write destination port
		write_value += data; // write command payload
		switch (input_port) {
		// + 5 (below) comes from address of write packet register for each switch port (last two bits)
		// This in decimal (20/4) corresponds to converting to bytes instead of words
	   case 1:
                    sc_iowrite32(write_value, (iobram1 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
                    break;
       case 2:
                    sc_iowrite32(write_value, (iobram2 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
                    break;
       case 3:
                    sc_iowrite32(write_value, (iobram3 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
                    break;
       case 4:
                    sc_iowrite32(write_value, (iobram4 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
                    break;
		}
		printf("_STATS_ [sc_accessBramFW] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
     	return 0;

		
	} else { // read option
                int offset = 5*4;

		write_value = (addr_reg) << 24; // write address offset
		write_value += (0x1 << 20); // write the read opcode
		write_value += ((input_port - 1) << 18); // write the source port
		write_value += ((output_port - 1) << 16); // write the destination port
		write_value += 0x0; // no  payload for read
		switch (input_port) {
		  case 1:
                    sc_iowrite32(write_value, (iobram1 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
                    break;
                  case 2:
                    sc_iowrite32(write_value, (iobram2 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
                    break;
                  case 3:
                    sc_iowrite32(write_value, (iobram3 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
                    break;
                  case 4:
                    sc_iowrite32(write_value, (iobram4 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
                    break;
		}

		test_data = 0x80000000;
        offset = 7*4;
		while (test_data == 0x80000000) {
			switch (input_port) {
			// + 7 (below) comes from address of read response register for each switch port
				case 1:
					test_data = sc_ioread32((iobram1 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
					break;
				case 2:
					test_data = sc_ioread32((iobram2 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
					break;
				case 3:
					test_data = sc_ioread32((iobram3 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
					break;
				case 4:
					test_data = sc_ioread32((iobram4 + offset), &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
					totaldelay += p_totaldelay;
					in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
					break;
			}
			//set_current_state(TASK_INTERRUPTIBLE); // scheduler won't run same process again for CMD_DELAY
			//schedule_timeout(CMD_DELAY);
		}

		print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // chop last 16 bits for final 16-bit value
		print_data = (print_data) >> 16; //write address offset
		//save print_data to pass this value back to user_mode
		my_sc_access_ds_ptr->data = print_data;

		printf("_STATS_ [sc_accessBramFW] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
		return 0;

	}

}

static int sc_accessBram(struct direct_access_ds *my_direct_access_ds_ptr)
{
	unsigned int test_data;
	unsigned int op_code = my_direct_access_ds_ptr->op_code;
	unsigned int bram_no = my_direct_access_ds_ptr->bram_no;
	unsigned int addr_reg = my_direct_access_ds_ptr->addr * 4; //the ofset set in bytes not unsigned int
	unsigned int data = my_direct_access_ds_ptr->data;
	double p_totaldelay, p_in_channel_delay, p_out_channel_delay;
	double totaldelay = 0.0;
	double in_channel_delay = 0.0, out_channel_delay = 0.0;

	init_virtual_kernel();

	if (op_code == 0x0){ // write data in sc kernel space
		switch(bram_no) {
	  	case 1:
			sc_iowrite32(data, iobram1_direct + addr_reg, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
			totaldelay += p_totaldelay;
			in_channel_delay += p_in_channel_delay;
			out_channel_delay += p_out_channel_delay;
//			printf("accessBram: W - BRAM1[%x]<-%x\n", addr_reg, data);
//			test_data = sc_ioread32(iobram1_direct + addr_reg);
//			printf("accessBram: R - BRAM1[%x]<-%x (for checking with W) \n", addr_reg, test_data);
			printf("_STATS_ [sc_accessBram] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
       		return 0; 
	  	case 2:	
            sc_iowrite32(data, iobram2_direct + addr_reg, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
			totaldelay += p_totaldelay;
			in_channel_delay += p_in_channel_delay;
			out_channel_delay += p_out_channel_delay;
//			printf("accessBram: W - BRAM2[%x]<-%x\n", addr_reg, data);
//			test_data = sc_ioread32(iobram2_direct + addr_reg);
//			printf("accessBram: R - BRAM2[%x]<-%x (for checking with W) \n", addr_reg, test_data);
			printf("_STATS_ [sc_accessBram] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
       		return 0;
	   	case 3:
            sc_iowrite32(data,iobram3_direct + addr_reg, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
			totaldelay += p_totaldelay;
			in_channel_delay += p_in_channel_delay;
			out_channel_delay += p_out_channel_delay;
//			 printf("accessBram: W - BRAM3[%x]<-%x\n", addr_reg, data);
//			 test_data = sc_ioread32(iobram3_direct + addr_reg);
//			 printf("accessBram: R - BRAM3[%x]<-%x (for checking with W) \n", addr_reg, test_data);
			printf("_STATS_ [sc_accessBram] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
			return 0;
	  	case 4:
            sc_iowrite32(data,iobram4_direct + addr_reg, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
			totaldelay += p_totaldelay;
			in_channel_delay += p_in_channel_delay;
			out_channel_delay += p_out_channel_delay;
//			 printf("accessBram: W - BRAM4[%x]<-%x\n", addr_reg, data);
//			 test_data = sc_ioread32(iobram4_direct + addr_reg);
//			 printf("accessBram: R - BRAM4[%x]<-%x (for checking with W) \n", addr_reg, test_data);
			printf("_STATS_ [sc_accessBram] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
			return 0;
		default:
			 return -1; // maybe already checked in regard to ioread/iowrite !
		} 
	} else { // read data in sc kernel space
		switch(bram_no) {
	  	case 1:
            test_data = sc_ioread32(iobram1_direct + addr_reg, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
			totaldelay += p_totaldelay;
			in_channel_delay += p_in_channel_delay;
			out_channel_delay += p_out_channel_delay;
//           printf("accessBram: R - BRAM1[%x]->%x \n", addr_reg, test_data);
            my_direct_access_ds_ptr->data = test_data;
			printf("_STATS_ [sc_accessBram] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
            return 0; 
	  	case 2:	
            test_data = sc_ioread32(iobram2_direct + addr_reg, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
			totaldelay += p_totaldelay;
			in_channel_delay += p_in_channel_delay;
			out_channel_delay += p_out_channel_delay;
//           printf("accessBram: R - BRAM2[%x]->%x \n", addr_reg, test_data);
            my_direct_access_ds_ptr->data = test_data;
			printf("_STATS_ [sc_accessBram] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
			return 0;
	   	case 3:
            test_data = sc_ioread32(iobram3_direct + addr_reg, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
			totaldelay += p_totaldelay;
			in_channel_delay += p_in_channel_delay;
			out_channel_delay += p_out_channel_delay;
//           printf("accessBram: R - BRAM3[%x]->%x \n", addr_reg, test_data);
            my_direct_access_ds_ptr->data = test_data;
			printf("_STATS_ [sc_accessBram] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
			return 0;
	  	case 4:
            test_data = sc_ioread32(iobram4_direct + addr_reg, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
			totaldelay += p_totaldelay;
			in_channel_delay += p_in_channel_delay;
			out_channel_delay += p_out_channel_delay;
//           printf("accessBram: R - BRAM4[%x]->%x \n", addr_reg, test_data);
            my_direct_access_ds_ptr->data = test_data;
			printf("_STATS_ [sc_accessBram] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
     		return 0;
		default:
			return -1; // maybe already checked in regard to ioread/iowrite!
		} 
	}
}

static void sc_readStatsPerPort(struct sc_stat_ds *my_stat_ds_ptr)
{
	int inport = my_stat_ds_ptr->port;
	my_stat_ds_ptr->Total_passed = 0x0;
	my_stat_ds_ptr-> Fifo_dropped = 0x0;
	my_stat_ds_ptr-> Fw_dropped = 0x0;
	double p_totaldelay, p_in_channel_delay, p_out_channel_delay;
	double totaldelay = 0.0;
	double in_channel_delay = 0.0, out_channel_delay = 0.0;
	//read statistics  per port
	if(inport == 1){
		my_stat_ds_ptr-> Total_passed = sc_ioread32(iobram1 + startingStatisticsTotal, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
		my_stat_ds_ptr-> Fifo_dropped = sc_ioread32(iobram1 + startingStatisticsFifo, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
		my_stat_ds_ptr-> Fw_dropped = sc_ioread32(iobram1 + startingStatisticsFw, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
	}else if(inport == 2){
		my_stat_ds_ptr-> Total_passed = sc_ioread32(iobram2 + startingStatisticsTotal, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
		my_stat_ds_ptr-> Fifo_dropped = sc_ioread32(iobram2 + startingStatisticsFifo, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
		my_stat_ds_ptr-> Fw_dropped = sc_ioread32(iobram2 + startingStatisticsFw, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
	}else if(inport == 3){
		my_stat_ds_ptr-> Total_passed = sc_ioread32(iobram3 + startingStatisticsTotal, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
		my_stat_ds_ptr-> Fifo_dropped = sc_ioread32(iobram3 + startingStatisticsFifo, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
		my_stat_ds_ptr-> Fw_dropped = sc_ioread32(iobram3 + startingStatisticsFw, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
	}else if(inport == 4){
		my_stat_ds_ptr-> Total_passed = sc_ioread32(iobram4 + startingStatisticsTotal, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
		my_stat_ds_ptr-> Fifo_dropped = sc_ioread32(iobram4 + startingStatisticsFifo, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
		my_stat_ds_ptr-> Fw_dropped = sc_ioread32(iobram4 + startingStatisticsFw, &p_totaldelay, &p_in_channel_delay, &p_out_channel_delay );
		totaldelay += p_totaldelay;
		in_channel_delay += p_in_channel_delay; out_channel_delay += p_out_channel_delay;
	}
	printf("_STATS_ [sc_readStatsPerPort] total delay:%f, in_channel_delay:%f, out_channel_delay:%f\n", totaldelay, in_channel_delay, out_channel_delay);
}

#endif //SC_KERNEL_MODE_H
