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
#define FW_USER_LOGS
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "sc_kernel_mode.h"
//#include "ioctl_fw.h" // ioctl numbers and definitions
//#include "sc_kernel_structs.h"

void sc_user_setupSFW(unsigned int inport, unsigned long L_addr_reg, unsigned long H_addr_reg, unsigned int rule, unsigned int write_ops, unsigned int read_ops) { 
	int fd, rc;
	unsigned int Rule_reg = 0x0; 
	struct sc_fw_ds my_sc_fw_ds;		// firewall setup struct
	int ok_bram1 = 0, ok_bram2 = 0, ok_bram3 = 0, ok_bram4 = 0; 

    // checking parameters
    if (inport <= 0 || inport > 4){
		printf("sc_user_setupSFW: Invalid value for input port:%d - must be 1, 2, 3 or 4\n", inport);
        exit(1);
    }
	if (rule < 0 || rule > 1){
		printf("sc_user_setupSFW: Invalid value for rule:%d - must be 0 or 1.\n", rule);
		exit(1);
	}
	if (write_ops < 0 || write_ops > 1){
		printf("sc_user_setupSFW: Invalid value for (deny) write operation:%d - must be 0 or 1.\n", write_ops);
		exit(1);
	}
	if (read_ops < 0 || read_ops > 1){
		printf("sc_user_setupSFW: Invalid value for (deny) read operation:%d - must be 0 or 1.\n", read_ops);
		exit(1);
	}
	ok_bram1 = ((L_addr_reg >= 0x40000000 || L_addr_reg < 0x40001000) || (H_addr_reg >= 0x40000000 || H_addr_reg < 0x40001000));
	if (!ok_bram1) {
		ok_bram2 = ((L_addr_reg >= 0x40001000 || L_addr_reg < 0x40002000) || (H_addr_reg >= 0x40001000 || H_addr_reg < 0x40002000));
		if (!ok_bram2) {
			ok_bram3 = ((L_addr_reg >= 0x40002000 || L_addr_reg < 0x40003000) || (H_addr_reg >= 0x40002000 || H_addr_reg < 0x40003000));
			if (!ok_bram3) {
				ok_bram4 = ((L_addr_reg >= 0x40003000 || L_addr_reg < 0x40004000) || (H_addr_reg >= 0x40003000 || H_addr_reg < 0x40004000));
			}
		}
	}
	if (!ok_bram1 && !ok_bram2 && !ok_bram3 && !ok_bram4) {
		printf("sc_user_setupSFW: ERROR in offsets above \n");
		exit(1);
	}
	// prepare rule register for simple setup
	Rule_reg = rule << 7; // enable(1) or disable(0) rule
	Rule_reg += 0 << 6;   // set to 0 to operate in compatibility mode
	Rule_reg += write_ops << 5; //enable/disable rule for write operations
	Rule_reg += read_ops << 4; //enable/disable rule for read operations
	Rule_reg += 0x0; //unused

	// fill up setup data structure
	my_sc_fw_ds.input_port = inport;   
	my_sc_fw_ds.L_addr_reg = L_addr_reg;
	my_sc_fw_ds.H_addr_reg = H_addr_reg;
	my_sc_fw_ds.Rule_reg = Rule_reg;
	
	//call sc kernel / virtual kernel from sc_kernel_mode.h
	sc_setupFW(&my_sc_fw_ds);
}


void sc_user_setupCFW(unsigned int inport, unsigned int L_addr_reg, unsigned int H_addr_reg, unsigned int rule, unsigned int write_ops, unsigned int read_ops, unsigned int outport) {
	unsigned int Rule_reg = 0x0; 
	struct sc_fw_ds my_sc_fw_ds;		// firewall setup struct

//	printf("setupCFW: inport:%u, L/H_addr_reg:(%x %x) => Rule_reg:%x W/R:(%u %u) outport:%u \n", inport, L_addr_reg, H_addr_reg, rule, write_ops, read_ops, outport);

	// checking parameters
	if (inport <= 0 || inport > 4){
		printf("sc_user_setupCFW: Invalid value for input port:%d - must be 1, 2, 3 or 4\n", inport);
		exit(1);
	}
	if (outport <= 0 || outport > 4){
	  	printf("sc_user_setupCFW: Invalid value for output port:%d - must be 1, 2, 3, or 4.\n", outport);
		exit(1);
	}
	if (rule < 0 || rule > 1){
	  	printf("sc_user_setupCFW: Invalid value for rule:%d - must be 0 or 1.\n", rule);
		exit(1);
	}
	if (write_ops < 0 || write_ops > 1){
	  	printf("sc_user_setupCFW: Invalid value for (deny) write operation:%d - must be 0 or 1.\n", write_ops);
		exit(1);
	}
	if (read_ops < 0 || read_ops > 1){
	  	printf("sc_user_setupCFW: Invalid value for (deny) read operation:%d - must be 0 or 1.\n", read_ops);
		exit(1);
	}
	if((L_addr_reg >= H_addr_reg) || (L_addr_reg < 0x00000000 || L_addr_reg >= 0x00001000)
           || (H_addr_reg < 0x00000000 || H_addr_reg > 0x00001000)){
		printf("sc_user_setupCFW: Invalid range [%x %x] - must be in [0x00000000, 0x00001000].\n", L_addr_reg, H_addr_reg);
		exit(1);
	}

	// prepare rule register for complex setup
	Rule_reg = rule << 7; //enable or disable rule
	Rule_reg += 1 << 6;   //Value set to 1 to operate in NoC mode
	Rule_reg += write_ops << 5; //enable or disable rule for write operations
	Rule_reg += read_ops << 4; //enable or disable rule for read operations

    // last part setups output port
	if(outport == 4)
		Rule_reg += 1 << 3; //Value set to 1 to enable rule for output port 4 (BRAM 1)
        else 
	  	Rule_reg += 0 << 3;

	if(outport == 3)
        	  Rule_reg += 1 << 2; //Value set to 1 to enable rule for output port 3 (BRAM 2)
        else
          	Rule_reg += 0 << 2;

	if(outport == 2)
          	Rule_reg += 1 << 1; //Value set to 1 to enable rule for output port 2 (BRAM 3)
        else
          	Rule_reg += 0 << 1;

	if(outport == 1)
          	Rule_reg += 1; //Value set to 1 to enable rule for output port 1 (BRAM 4)
        else
          	Rule_reg += 0;

	// fill up setup data structure
	my_sc_fw_ds.input_port = inport;   
	my_sc_fw_ds.L_addr_reg = L_addr_reg;
	my_sc_fw_ds.H_addr_reg = H_addr_reg;
	my_sc_fw_ds.Rule_reg = Rule_reg;
	my_sc_fw_ds.output_port = outport;
	
	//call sc kernel / virtual kernel from sc_kernel_mode.h
	sc_setupFW(&my_sc_fw_ds);
	
}

void sc_user_accessBramFW(unsigned int op_code, unsigned int inport, unsigned int outport, 
							unsigned int addr_reg, unsigned int *data){
    struct sc_access_ds my_sc_access_ds;

	if (inport <= 0 || inport > 4){
		printf("sc_user_accessBramFW: Invalid value for input port:%d - must be 1, 2, 3 or 4 \n", inport);
		exit(1);
	}
	if (outport <= 0 || outport > 4){
		printf("sc_user_accessBramFW: Invalid value for output port.\n");
		exit(1);
	}
	if (op_code < 0 || op_code > 1){
		printf("sc_user_accessBramFW: Invalid value for operation code.\n");
		exit(1);
	}

    my_sc_access_ds.input_port = inport;
    my_sc_access_ds.output_port = outport;
    my_sc_access_ds.addr = addr_reg;
	my_sc_access_ds.op_code = op_code;

	if (op_code == 0x1) // read option
		my_sc_access_ds.data = 0; // always initialize data before read operation
    else
		my_sc_access_ds.data = *data;

	//call sc kernel / virtual kernel
	sc_accessBramFW(&my_sc_access_ds);
	 

	if (op_code == 0x1) // read option
		*data = my_sc_access_ds.data;
		
}

/* Function for direct read access to BRAM
   This function must be called from admin
*/
void sc_user_accessBram(unsigned int op_code, unsigned int bram_no, unsigned int addr_reg, unsigned int *data){
	int fd, rc;
	struct direct_access_ds my_direct_access_ds;

	// checking parameters
	if (bram_no <= 0 || bram_no > 4){
		printf("accessBram: Invalid value for output port.\n");
		exit(1);
	}
	if (op_code < 0 || op_code > 1){
		printf("accessBram: Invalid value for operation code.\n");
		exit(1);
	}

	// fill up the data structure
    my_direct_access_ds.bram_no = bram_no;
    my_direct_access_ds.addr = addr_reg;
	my_direct_access_ds.op_code = op_code;
	
	if (op_code == 0x1) // read option
		my_direct_access_ds.data = 0; // always initialize data before read operation
    else
		my_direct_access_ds.data = *data; 

	//call sc kernel / virtual kernel
	sc_accessBram(&my_direct_access_ds);

	if (op_code == 0x1) // read option
		*data = my_direct_access_ds.data;

}

/* Function to map group names (equivalently clinics, and indirectly doctors) to NoC input ports for access via FW
   This function must be called from admin
*/
void sc_setGidPerNoCInport(char *group_name, unsigned int inport){
	gid_t admin_group, gid;
	unsigned int data;

	// Set parameter restrictions
	if(inport <= 0 || inport > USER_NOC_INPUT_PORTS){
		printf("setGidPerNoCInport: Invalid number for NoC input port.\n");
		exit(1);
	}

	// only if user is root, then save in a table the matching pair (inport, group-id)
	if(is_admin()){ // maybe also check valid_group(group_name)
		// set group id to input ports.
		admin_group = groupIdFromName("root");
		gid = groupIdFromName(group_name); //get group id from group name
		if((int)gid < 0){
			printf("setGidPerNoCInport: Invalid group.\n");
			exit(1);
		}
		data = (unsigned int) gid;
		sc_user_accessBramFW(0x0, ADMIN_INPUT_PORT, ADMIN_OUTPUT_PORT, OFFSET_INPUT_PORTS_BRAM4 + ((inport-1)*0x4), &data);
	} else {
		printf("SetupGidPerNoCInputPort: You are not authorized as system admin.\n");
	}
}

/* Function to map group names (equivalently clinics, and indirectly doctors) to NoC output ports (BRAMS) for access via FW
   This function must be called from admin
   */
void sc_setGidPerNoCOutport(char *group_name, int outport){
	gid_t admin_group, gid;
	unsigned int data;
	// Set parameter restrictions
	if(outport <= 0 || outport > USER_NOC_OUTPUT_PORTS){
		printf("setGidPerNoCOutport: Invalid number for NoC outport port.\n");
		return; 
	}
	//only if the user is root, then save in a table the matching pair (outport, group-id)
	if(is_admin()){
		//Set group id to output ports.
        admin_group = groupIdFromName("root");
        gid = groupIdFromName(group_name); //get group id from group name
		if((int)gid < 0){
			printf("setGidPerNoCOutport: Invalid group.\n");
			exit(1);
		}
        data = (unsigned int) gid;
		sc_user_accessBramFW(0x0, ADMIN_INPUT_PORT, ADMIN_OUTPUT_PORT, OFFSET_INPUT_PORTS_BRAM4 + ((outport-1)*0x4), &data);
	} else {
		printf("setGidPerNoCOutport: You are not authorized as system admin.\n");
	}
}

/* Function for ioctl call to read the total packets passed per port
   This function must be called from admin
*/
unsigned int sc_readStatsTotalPerPort(unsigned int inport){
	struct sc_stat_ds my_stat_ds;
	// checking parameters
	if (inport <= 0 || inport > 4){
		printf("sc_readStatsTotalPerPort: Invalid value for input port:%d - must be 1, 2, 3 or 4\n", inport);
		exit(1);
	}
    // fill up the data structure
	my_stat_ds.port = inport;
	my_stat_ds.Total_passed = 0x0;	
	sc_readStatsPerPort(&my_stat_ds);
	return my_stat_ds.Total_passed;
}

/* Function for ioctl call to read the dropped packets from port(due to Fifo full error)
   This function must be called from admin
*/
unsigned int sc_readStatsFifoPerPort(unsigned int inport)
{
	struct sc_stat_ds my_stat_ds;
	// checking parameters
	if (inport <= 0 || inport > 4){
		printf("sc_readStatsFifoPerPort: Invalid value for input port:%d - must be 1, 2, 3 or 4\n", inport);
		exit(1);
	}
	// fill up the data structure
	my_stat_ds.port = inport;
	my_stat_ds.Fifo_dropped = 0x0;
	sc_readStatsPerPort(&my_stat_ds);
	return my_stat_ds.Fifo_dropped;
}

/* Function for ioctl call to read the droped packets from port (due to firewall rule)
   This function must be called from admin
*/
unsigned int sc_readStatsFwPerPort(unsigned int inport) {
	struct sc_stat_ds my_stat_ds;
	// checking parameters
	if (inport <= 0 || inport > 4){
		printf("sc_readStatsFwPerPort: Invalid value for input port:%d - must be 1, 2, 3 or 4\n", inport);
		exit(1);
	}
    // set input port - fill empty data structure
	my_stat_ds.port = inport;
	my_stat_ds.Fw_dropped = 0x0;
	sc_readStatsPerPort(&my_stat_ds);
    return my_stat_ds.Fw_dropped;
}
