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
/* 
  Firewall module (driver)
   Kernel space firewall methods
  */

#define FW_KERN_LOGS
//#define FW_KERN_LOGS2

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>   // includes all functions for kernel like printk and their enum parameters
#include <linux/types.h>    // defines the dev_t which contains both major and minor numbers
#include <linux/kdev_t.h>   // obtain the Macros from which we obtain the major/Minor
#include <linux/fs.h>       // contains the function for registering the major & minor Numbers
#include <linux/device.h>   // APIs with which the driver populates the /sys using the Linux device model
#include <linux/cdev.h>     // cdev_init() and cdev_add()
#include <linux/ioport.h>   // allocation of memory regions request_mem_region()
#include <asm/uaccess.h>    // copy_from/to_user
#include <linux/slab.h>     // kmalloc header
#include <asm/io.h>         // io memory header
#include <linux/sched.h>    // add delay

#include "init_modules.h"
#include "ioctl_fw.h"
#include <linux/cred.h>

/*
//#include <linux/mutex.h>
//#include <asm/atomic.h>
//#include <linux/errno.h>
#include <linux/spinlock_types.h>
//DEFINE_MUTEX(my_mutex);
//struct mutex my_mutex;
spinlock_t mr_lock;
static int first_time=1;
static int count = 0;
*/

// Initialize register values
static int startingLowerReg = 5; // lower range register
static int startingHighReg = 13; // high range register
static int startingRuleReg = 21; // rule register
//my_access_ds *my_access_ds_ptr;
//EXPORT_SYMBOL(access_ds);
//struct access_ds *my_access_ds_ptr)

//access_ds_t *my_access_ds_ptr;
//EXPORT_SYMBOL(my_access_ds_ptr);


/* Function to setup a firewall rule 
   This function called from user space to define a 
   - simple firewall rule (e.g setup an input port,the absolute range(L_addr_reg- H_addr_reg etc) or 
   - complex firewall rule (e.g setup an input port, an outport, the relative range( L_addr_reg - H_addr_reg etc). 
    */
static void setupFW(struct fw_ds *my_fw_ds_ptr)
{
	int offset = 2;
	unsigned int inport = my_fw_ds_ptr->input_port;
	unsigned int L_addr_reg = my_fw_ds_ptr->L_addr_reg;
	unsigned int H_addr_reg = my_fw_ds_ptr->H_addr_reg;
	unsigned int Rule_reg = my_fw_ds_ptr->Rule_reg;
	int gid;

	gid = current_gid().val;
	if (gid == 0) {
#ifdef FW_KERN_LOGS
pr_info("setupFW:GO_ON  UID:%u GID:%u my_fw_ds_ptr->input_port: %d  my_fw_ds_ptr->L_addr_reg:0x%x  my_fw_ds_ptr->H_addr_reg:0x%x   my_fw_ds_ptr->Rule_reg:0x%x (w=0x90, r=0xA0, wr=0x80, none=0xB0)\n", current_uid().val, current_gid().val, my_fw_ds_ptr->input_port, my_fw_ds_ptr->L_addr_reg, my_fw_ds_ptr->H_addr_reg, my_fw_ds_ptr->Rule_reg);
#endif
		//write above data to kernel space .... Antonis QQQQQQQ
		iowrite32(0x00000000, noc_setup_registers + startingRuleReg + ((inport-1)*offset) ); //rule register
		iowrite32(L_addr_reg, noc_setup_registers + startingLowerReg + ((inport-1)*offset) ); //lower register
		iowrite32(H_addr_reg, noc_setup_registers + startingHighReg + ((inport-1)*offset) ); //high register
		iowrite32(Rule_reg, noc_setup_registers + startingRuleReg + ((inport-1)*offset) ); //rule register
	} else {
#ifdef FW_KERN_LOGS
pr_info("setupFW:STOP!  UID:%u GID:%u my_fw_ds_ptr->input_port: %d  my_fw_ds_ptr->L_addr_reg:0x%x  my_fw_ds_ptr->H_addr_reg:0x%x   my_fw_ds_ptr->Rule_reg:0x%x (w=0x90, r=0xA0, wr=0x80, none=0xB0)\n", current_uid().val, current_gid().val, my_fw_ds_ptr->input_port, my_fw_ds_ptr->L_addr_reg, my_fw_ds_ptr->H_addr_reg, my_fw_ds_ptr->Rule_reg);
pr_info("setupFW: gid:%d", (int)gid);
#endif
		pr_info("NON-ROOT TRIED TO PERFORM setupFW");
		BUG_ON((gid != 0));
	}
}

EXPORT_SYMBOL(setupFW); /* export static read / write function*/
/* Function to read firewall setup data.
   This function called from user space to return 
   - the simple firewall rule (e.g setup an input port,the absolute range(L_addr_reg- H_addr_reg etc) or 
   - or complex firewall rule (e.g setup an input port, an outport, the relative range( L_addr_reg - H_addr_reg etc). 
    */
static void readFWRegs(struct fw_ds *my_fw_ds_ptr)
{
	int offset = 2;
	int inport = my_fw_ds_ptr->input_port;
	int gid;

	gid = current_gid().val;
	if (gid == 0) {
		my_fw_ds_ptr->L_addr_reg = 0x0;
		my_fw_ds_ptr->H_addr_reg = 0x0;
		my_fw_ds_ptr->Rule_reg = 0x0;
		//retrieve data from kernel space and eventually to user space 
		my_fw_ds_ptr->L_addr_reg = ioread32(noc_setup_registers + startingLowerReg + ((inport-1)*offset) );
		my_fw_ds_ptr->H_addr_reg = ioread32(noc_setup_registers + startingHighReg + ((inport-1)*offset) );
		my_fw_ds_ptr->Rule_reg = ioread32(noc_setup_registers + startingRuleReg + ((inport-1)*offset) );
#ifdef FW_KERN_LOGS
pr_info("readFWRegs:GO_ON  my_fw_ds_ptr->input_port: %d\n   my_fw_ds_ptr->L_addr_reg: 0x%x\n   my_fw_ds_ptr->H_addr_reg: 0x%x\n   my_fw_ds_ptr->Rule_reg: 0x%x\n",  my_fw_ds_ptr->input_port, my_fw_ds_ptr->L_addr_reg, my_fw_ds_ptr->H_addr_reg, my_fw_ds_ptr->Rule_reg);
#endif
	} else {
#ifdef FW_KERN_LOGS
pr_info("readFWRegs:STOP!  my_fw_ds_ptr->input_port: %d\n   my_fw_ds_ptr->L_addr_reg: 0x%x\n   my_fw_ds_ptr->H_addr_reg: 0x%x\n   my_fw_ds_ptr->Rule_reg: 0x%x\n",  my_fw_ds_ptr->input_port, my_fw_ds_ptr->L_addr_reg, my_fw_ds_ptr->H_addr_reg, my_fw_ds_ptr->Rule_reg);
pr_info("readFWRegs: gid:%d", (int)gid);
#endif
		pr_info("NON-ROOT TRIED TO PERFORM readFWRegs");
		BUG_ON((gid != 0));
	}
}

/* 
   Function to  validate a firewall rule
   Compare the initial data with these after call read method and return the outcome.
    */
static int checkFWRegs(struct fw_ds *my_fw_ds_ptr)
{
        struct fw_ds my_fw_ds_temp;
	int gid;
	int flag = 0;

	gid = current_gid().val;
	if (gid == 0) {
		my_fw_ds_temp.input_port = my_fw_ds_ptr->input_port;
		my_fw_ds_temp.L_addr_reg = my_fw_ds_ptr->L_addr_reg;
		my_fw_ds_temp.H_addr_reg = my_fw_ds_ptr->H_addr_reg;
		my_fw_ds_temp.Rule_reg = my_fw_ds_ptr->Rule_reg;
#ifdef FW_KERN_LOGS
pr_info("checkFWRegs:GO_ON (input_port:%d) given_values - L_addr_reg:%x H_addr_reg:%x Rule_reg:%x\n", my_fw_ds_temp.input_port, my_fw_ds_temp.L_addr_reg, my_fw_ds_temp.H_addr_reg, my_fw_ds_temp.Rule_reg);
#endif
		// check initial data with data from readFWRegs method
		readFWRegs(my_fw_ds_ptr);

#ifdef FW_KERN_LOGS
pr_info("checkFWRegs:GO_ON (input_port:%d)  actual_values - L_addr_reg:%x H_addr_reg:%x Rule_reg:%x\n", my_fw_ds_ptr->input_port, my_fw_ds_ptr->L_addr_reg, my_fw_ds_ptr->H_addr_reg, my_fw_ds_ptr->Rule_reg);
#endif
		if ((my_fw_ds_temp.L_addr_reg==my_fw_ds_ptr->L_addr_reg) 
		&&  (my_fw_ds_temp.H_addr_reg==my_fw_ds_ptr->H_addr_reg) 
		&&  (my_fw_ds_temp.Rule_reg==my_fw_ds_ptr->Rule_reg) ) {
        		flag = 1;
			return flag;
		}
        	else {
        		flag = -1;
			return flag;
		}
	} else {
#ifdef FW_KERN_LOGS

pr_info("checkFWRegs:STOP! (input_port:%d) given_values - L_addr_reg:%x H_addr_reg:%x Rule_reg:%x\n", my_fw_ds_temp.input_port, my_fw_ds_temp.L_addr_reg, my_fw_ds_temp.H_addr_reg, my_fw_ds_temp.Rule_reg);
pr_info("checkFWRegs: gid:%d", (int)gid);
#endif
		pr_info("NON-ROOT TRIED TO PERFORM checkFWRegs");
		BUG_ON((gid != 0));
	}
	return 0;
}

/* Function to write directly to BRAM 
    */
static int accessBram(struct direct_access_ds *my_direct_access_ds_ptr)
{
	unsigned int test_data;
	unsigned int op_code = my_direct_access_ds_ptr->op_code;
	unsigned int bram_no = my_direct_access_ds_ptr->bram_no;
	unsigned int addr_reg = my_direct_access_ds_ptr->addr;
	unsigned int data = my_direct_access_ds_ptr->data;
	int gid;

	gid = current_gid().val;
	if ((gid != 0) &&  ((op_code == 0x0) || (op_code == 0x1))) { // only root group can perform direct read/write
#ifdef FW_KERN_LOGS
pr_info("accessBram: gid:%d", (int)gid);
#endif
		pr_info("accessBram: NON-ROOT TRIED TO PERFORM direct access (r/w)\n");
		BUG_ON((gid != 0) &&  (op_code == 0x0));
	}

	if (op_code == 0x0){ // write data in kernel space
		switch(bram_no) {
	  	case 1:
             		iowrite32(data,iobram1_direct + addr_reg);
#ifdef FW_KERN_LOGS
pr_info("accessBram: W - BRAM1[%x]<-%x\n", addr_reg, data);
test_data = ioread32(iobram1_direct + addr_reg);
//no need to reverse bits
//print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // reverse bits
//print_data = (print_data) >> 16; //write address offset
//pr_info("accessBram: W - BRAM1[%x]<-%x (test_data:%x)\n", addr_reg, print_data, test_data);
pr_info("accessBram: R - BRAM1[%x]<-%x (for checking with W) \n", addr_reg, test_data);
#endif
             		return 0; 

	  	case 2:	
             		iowrite32(data,iobram2_direct + addr_reg);
#ifdef FW_KERN_LOGS
pr_info("accessBram: W - BRAM2[%x]<-%x\n", addr_reg, data);
test_data = ioread32(iobram2_direct + addr_reg);
//no need to reverse bits
//print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // reverse bits
//print_data = (print_data) >> 16; //write address offset
//pr_info("accessBram: W - BRAM2[%x]<-%x (test_data:%x)\n", addr_reg, print_data, test_data);
pr_info("accessBram: R - BRAM2[%x]<-%x (for checking with W) \n", addr_reg, test_data);
#endif
             		return 0;

	   	case 3:
             		iowrite32(data,iobram3_direct + addr_reg);
#ifdef FW_KERN_LOGS
pr_info("accessBram: W - BRAM3[%x]<-%x\n", addr_reg, data);
test_data = ioread32(iobram3_direct + addr_reg);
//no need to reverse bits
//print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // reverse bits
//print_data = (print_data) >> 16; //write address offset
//pr_info("accessBram: W - BRAM3[%x]<-%x (test_data:%x)\n", addr_reg, print_data, test_data);
pr_info("accessBram: R - BRAM3[%x]<-%x (for checking with W) \n", addr_reg, test_data);
#endif
	     		return 0;

	  	case 4:
             		iowrite32(data,iobram4_direct + addr_reg);
#ifdef FW_KERN_LOGS
pr_info("accessBram: W - BRAM4[%x]<-%x\n", addr_reg, data);
test_data = ioread32(iobram4_direct + addr_reg);
//no need to reverse bits
//print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // reverse bits
//print_data = (print_data) >> 16; //write address offset
//pr_info("accessBram: W - BRAM4[%x]<-%x (test_data:%x)\n", addr_reg, print_data, test_data);
pr_info("accessBram: R - BRAM4[%x]<-%x (for checking with W) \n", addr_reg, test_data);
#endif
	     		return 0;

		default:
			return -1; // maybe already checked in regard to ioread/iowrite !
		} 
	} else { // read data in kernel space
		switch(bram_no) {
	  	case 1:
	     		test_data = ioread32(iobram1_direct + addr_reg);
#ifdef FW_KERN_LOGS
//print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // reverse bits
//print_data = (print_data) >> 16; //write address offset
//pr_info("accessBram: R - BRAM1[%x]->%x (test_data:%x)\n", addr_reg, print_data, test_data);
//my_direct_access_data_ptr->data = print_data;
pr_info("accessBram: R - BRAM1[%x]->%x \n", addr_reg, test_data);
#endif
	     		my_direct_access_ds_ptr->data = test_data;
	     		return 0; 

	  	case 2:	
	     		test_data = ioread32(iobram2_direct + addr_reg);
#ifdef FW_KERN_LOGS
//print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // reverse bits
//print_data = (print_data) >> 16; //write address offset
//pr_info(KERN_INFO "accessBram: R - BRAM2[%x]->%x (test_data:%x)\n", addr_reg, print_data, test_data);
//my_direct_access_data_ptr->data = print_data;
pr_info(KERN_INFO "accessBram: R - BRAM2[%x]->%x \n", addr_reg, test_data);
#endif
	     		my_direct_access_ds_ptr->data = test_data;
             		return 0;

	   	case 3:
	     		test_data = ioread32(iobram3_direct + addr_reg);
#ifdef FW_KERN_LOGS
//print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // reverse bits
//print_data = (print_data) >> 16; //write address offset
//pr_info(KERN_INFO "accessBram: R - BRAM3[%x]->%x (test_data:%x)\n", addr_reg, print_data, test_data);
//my_direct_access_data_ptr->data = print_data;
pr_info(KERN_INFO "accessBram: R - BRAM3[%x]->%x \n", addr_reg, test_data);
#endif
	     		my_direct_access_ds_ptr->data = test_data;
	     		return 0;

	  	case 4:
	     		test_data = ioread32(iobram4_direct + addr_reg);
#ifdef FW_KERN_LOGS
//print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // reverse bits
//print_data = (print_data) >> 16; //write address offset
//pr_info(KERN_INFO "accessBram: R - BRAM4[%x]->%x (test_data:%x)\n", addr_reg, print_data, test_data);
//my_direct_access_data_ptr->data = print_data;
pr_info(KERN_INFO "accessBram: R - BRAM4[%x]->%x \n", addr_reg, test_data);
#endif
	     		my_direct_access_ds_ptr->data = test_data;
	     		return 0;

		default:
			return -1; // maybe already checked in regard to ioread/iowrite!
		} 
	}
}
EXPORT_SYMBOL(accessBram); /* export static read / write function*/

/* Function to write or read data to a block (specific range).   
   This function called from user space to write (op_code 0) data to kernel space or read (op_code 1) data from kernel
   space.
    */
static int accessBramFW(struct access_ds *my_access_ds_ptr)
{
	unsigned int write_value = 0x0; //hold validation data (check with ioread32 method) 
	volatile unsigned int test_data = 0;
	unsigned int print_data;
        unsigned int op_code = my_access_ds_ptr->op_code;
	unsigned int input_port = my_access_ds_ptr->input_port;
        unsigned int output_port = my_access_ds_ptr->output_port;
        unsigned int addr_reg = my_access_ds_ptr->addr;
        unsigned int data = my_access_ds_ptr->data;
	int gid;

#ifdef FW_KERN_LOGS2
        int startingStatisticsFifo = 11; // Fifo dropped - last bits from register(2C from eg 0x8000002C) in Dec are 44/4=>11
	int startingStatisticsFw = 13; // Fw dropped
	unsigned int Fifo_dropped = 0;
	unsigned int Fw_dropped = 0;
#endif

	gid = current_gid().val;
	if ((gid != 0) &&  (op_code == 0x0)) { // only root group can perform write via firewall
#ifdef FW_KERN_LOGS
pr_info("accessBramFW: gid:%d", (int)gid);
#endif
		pr_info("accessBramFW: NON-ROOT TRIED TO PERFORM write access via FW \n");
		BUG_ON((gid != 0) &&  (op_code == 0x0));
	}

	if (op_code == 0x0){ // write option


/*
	if (first_time) {
		spin_lock_init(&mr_lock);
		first_time=0;
	}

//atomic_read(&my_mutex.count));
//if (!mutex_trylock(&my_mutex)) {
//mutex_lock(&my_mutex);

spin_lock(&mr_lock);
//spin_lock_irqsave(&spinlock, flags);
count++;
pr_info("MODULE count:%d:\n", count);

*/

		write_value = (addr_reg) << 24; // write address offset
		write_value += ((input_port - 1) << 18); // write source port
		write_value += ((output_port - 1) << 16); // write destination port
		write_value += data; // write command payload
		switch (input_port) {
		// + 5 (below) comes from address of write packet register for each switch port (last two bits)
		// This in decimal (20/4) corresponds to converting to bytes instead of words
		  case 1:
                    iowrite32(write_value, (iobram1 + 5));
                    break;
                  case 2:
                    iowrite32(write_value, (iobram2 + 5));
                    break;
                  case 3:
                    iowrite32(write_value, (iobram3 + 5));
                    break;
                  case 4:
                    iowrite32(write_value, (iobram4 + 5));
                    break;
		}

#ifdef FW_KERN_LOGS
pr_info("accessBramFW: W - BRAM%d[%x]<-%x input_port:%d\n", output_port, addr_reg, data, input_port);
#endif

#ifdef FW_KERN_LOGS2
Fw_dropped = 0x0;
Fifo_dropped = 0x0;
//read statistics  per port
if(input_port == 1){
Fifo_dropped = ioread32(iobram1 + startingStatisticsFifo) - init_stat_fifo1;
Fw_dropped = ioread32(iobram1 + startingStatisticsFw) - init_stat_fw1;
}else if(input_port == 2){
Fifo_dropped = ioread32(iobram2 + startingStatisticsFifo) - init_stat_fifo2;
Fw_dropped = ioread32(iobram2 + startingStatisticsFw) - init_stat_fw2;
}else if(input_port == 3){
Fifo_dropped = ioread32(iobram3 + startingStatisticsFifo) - init_stat_fifo3;
Fw_dropped = ioread32(iobram3 + startingStatisticsFw) - init_stat_fw3;
}else if(input_port == 4){
Fifo_dropped = ioread32(iobram4 + startingStatisticsFifo) - init_stat_fifo4;
Fw_dropped = ioread32(iobram4 + startingStatisticsFw) - init_stat_fw4;
}
pr_info("accessBramFW: W - readStatsPerPort: Fifo_dropped[%d]:%d Fw_dropped[%d]:%d \n",  input_port, Fifo_dropped, input_port, Fw_dropped);
#endif

/*
//mutex_unlock(&my_mutex);
//pr_info("MODULE mutex end count=%d:\n", atomic_read(&my_mutex.count));
spin_unlock(&mr_lock);
//spin_unlock_irqrestore(&spinlock, flags);
*/

	     	return 0;

		
	} else { // read option

#ifdef FW_KERN_LOGS
pr_info("accessBramFW: TRY_R - BRAM%d[%x]->(data TBD) input_port:%d\n", output_port, addr_reg, input_port);
#endif

		write_value = (addr_reg) << 24; // write address offset
          	write_value += (0x1 << 20); // write the read opcode
          	write_value += ((input_port - 1) << 18); // write the source port
          	write_value += ((output_port - 1) << 16); // write the destination port
          	write_value += 0x0; // no  payload for read
		switch (input_port) {
                  case 1:
                    	iowrite32(write_value, (iobram1 + 5));
                  	break;
                  case 2:
                    	iowrite32(write_value, (iobram2 + 5));
                  	break;
                  case 3:
                    	iowrite32(write_value, (iobram3 + 5));
                  	break;
                  case 4:
                    	iowrite32(write_value, (iobram4 + 5));
                  	break;
		}

		test_data = 0x80000000;
		while (test_data == 0x80000000) {
			switch (input_port) {
			// + 7 (below) comes from address of read response register for each switch port
				case 1:
					test_data = ioread32((iobram1 + 7));
					break;
				case 2:
					test_data = ioread32((iobram2 + 7));
					break;
				case 3:
					test_data = ioread32((iobram3 + 7));
					break;
				case 4:
					test_data = ioread32((iobram4 + 7));
					break;
			}
			set_current_state(TASK_INTERRUPTIBLE); // scheduler won't run same process again for CMD_DELAY
			schedule_timeout(CMD_DELAY);
		}
/*
pr_info("BELOW TO BE ERASED \n");
test_data = ioread32((iobram1 + 7));
print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // chop last 16 bits for final 16-bit value
print_data = (print_data) >> 16; //write address offset
pr_info("accessBramFW: R - BRAM%d[%x]->%x input_port:%d\n", output_port, addr_reg, test_data, input_port);
pr_info("accessBramFW: R - BRAM%d[%x]->%x (final 16-bit) input_port:%d \n", output_port, addr_reg, print_data, input_port);
test_data = ioread32((iobram2 + 7));
print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // chop last 16 bits for final 16-bit value
print_data = (print_data) >> 16; //write address offset
pr_info("accessBramFW: R - BRAM%d[%x]->%x input_port:%d\n", output_port, addr_reg, test_data, input_port);
pr_info("accessBramFW: R - BRAM%d[%x]->%x (final 16-bit) input_port:%d \n", output_port, addr_reg, print_data, input_port);
test_data = ioread32((iobram3 + 7));
print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // chop last 16 bits for final 16-bit value
print_data = (print_data) >> 16; //write address offset
pr_info("accessBramFW: R - BRAM%d[%x]->%x input_port:%d\n", output_port, addr_reg, test_data, input_port);
pr_info("accessBramFW: R - BRAM%d[%x]->%x (final 16-bit) input_port:%d \n", output_port, addr_reg, print_data, input_port);
test_data = ioread32((iobram4 + 7));
print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // chop last 16 bits for final 16-bit value
print_data = (print_data) >> 16; //write address offset
pr_info("accessBramFW: R - BRAM%d[%x]->%x input_port:%d\n", output_port, addr_reg, test_data, input_port);
pr_info("accessBramFW: R - BRAM%d[%x]->%x (final 16-bit) input_port:%d \n", output_port, addr_reg, print_data, input_port);
pr_info("ABOVE TO BE ERASED \n");
*/

		print_data = (unsigned int) ((test_data << 16) | (test_data >> 16)); // chop last 16 bits for final 16-bit value
		print_data = (print_data) >> 16; //write address offset

#ifdef FW_KERN_LOGS
pr_info("accessBramFW: R - BRAM%d[%x]->%x input_port:%d\n", output_port, addr_reg, test_data, input_port);
pr_info("accessBramFW: R - BRAM%d[%x]->%x (final 16-bit) input_port:%d \n", output_port, addr_reg, print_data, input_port);
#endif

		//save print_data to pass this value back to user_mode
		my_access_ds_ptr->data = print_data;

/*
#ifdef FW_KERN_LOGS2
Fw_dropped = 0x0;
Fifo_dropped = 0x0;
//read statistics  per port
if(input_port == 1){
Fifo_dropped = ioread32(iobram1 + startingStatisticsFifo) - init_stat_fifo1;
Fw_dropped = ioread32(iobram1 + startingStatisticsFw) - init_stat_fw1;
}else if(input_port == 2){
Fifo_dropped = ioread32(iobram2 + startingStatisticsFifo) - init_stat_fifo2;
Fw_dropped = ioread32(iobram2 + startingStatisticsFw) - init_stat_fw2;
}else if(input_port == 3){
Fifo_dropped = ioread32(iobram3 + startingStatisticsFifo) - init_stat_fifo3;
Fw_dropped = ioread32(iobram3 + startingStatisticsFw) - init_stat_fw3;
}else if(input_port == 4){
Fifo_dropped = ioread32(iobram4 + startingStatisticsFifo) - init_stat_fifo4;
Fw_dropped = ioread32(iobram4 + startingStatisticsFw) - init_stat_fw4;
}
pr_info("accessBramFW: W - readStatsPerPort: Fifo_dropped[%d]:%d Fw_dropped[%d]:%d \n",  input_port, Fifo_dropped, input_port, Fw_dropped);
#endif
*/
		return 0;

	}

}
EXPORT_SYMBOL(accessBramFW); /* export static read / write function*/

/* Function to read/show statistics per port (e.g read total packets passed from port) 
   This function called from user space to read specific statistics per port, i.e.
   total packets passed, dropped packets (due to fifo full error) and dropped packets (due to firewall rule) per port. 
    */
static void readStatsPerPort(struct stat_ds *my_stat_ds_ptr)
{
        int startingStatisticsTotal = 9; // last bits from register(24 from eg 0x80000024) in Dec are 36/4=>9
        int startingStatisticsFifo = 11; // last bits from register(2C from eg 0x8000002C) in Dec are 44/4=>11
        int startingStatisticsFw = 13;
	int inport = my_stat_ds_ptr->port;
	int gid;

	gid = current_gid().val;
	if ((gid != 0)) { // only root group can access statistics
#ifdef FW_KERN_LOGS
pr_info("readStatsPerPort: gid:%d", (int)gid);
#endif
		pr_info("NON-ROOT TRIED TO PERFORM readStatsPerPort");
		BUG_ON(gid != 0);
	}

	my_stat_ds_ptr-> Total_passed = 0x0;
	my_stat_ds_ptr-> Fifo_dropped = 0x0;
	my_stat_ds_ptr-> Fw_dropped = 0x0;
	//read statistics  per port
	if(inport == 1){
          my_stat_ds_ptr-> Total_passed = ioread32(iobram1 + startingStatisticsTotal) - init_stat_total1;
	  my_stat_ds_ptr-> Fifo_dropped = ioread32(iobram1 + startingStatisticsFifo) - init_stat_fifo1;
	  my_stat_ds_ptr-> Fw_dropped = ioread32(iobram1 + startingStatisticsFw) - init_stat_fw1;
	}else if(inport == 2){
	  my_stat_ds_ptr-> Total_passed = ioread32(iobram2 + startingStatisticsTotal) - init_stat_total2;
          my_stat_ds_ptr-> Fifo_dropped = ioread32(iobram2 + startingStatisticsFifo) - init_stat_fifo2;
          my_stat_ds_ptr-> Fw_dropped = ioread32(iobram2 + startingStatisticsFw) - init_stat_fw2;
	}else if(inport == 3){
          my_stat_ds_ptr-> Total_passed = ioread32(iobram3 + startingStatisticsTotal) - init_stat_total3;
          my_stat_ds_ptr-> Fifo_dropped = ioread32(iobram3 + startingStatisticsFifo) - init_stat_fifo3;
          my_stat_ds_ptr-> Fw_dropped = ioread32(iobram3 + startingStatisticsFw) - init_stat_fw3;
        }else if(inport == 4){
          my_stat_ds_ptr-> Total_passed = ioread32(iobram4 + startingStatisticsTotal) - init_stat_total4;
          my_stat_ds_ptr-> Fifo_dropped = ioread32(iobram4 + startingStatisticsFifo) - init_stat_fifo4;
          my_stat_ds_ptr-> Fw_dropped = ioread32(iobram4 + startingStatisticsFw) - init_stat_fw4;
        }

#ifdef FW_KERN_LOGS
  pr_info("readStatsPerPort: (input_port:%d) Total_passed:%d - Fifo_dropped:%d Fw_dropped:%d\n",  my_stat_ds_ptr->port, my_stat_ds_ptr->Total_passed, my_stat_ds_ptr->Fifo_dropped, my_stat_ds_ptr->Fw_dropped);
#endif
}

/* Function to read/show statistics for all ports totally (e.g read total packets passed from all ports) 
   This function called from user space to read specific statistics for all ports, i.e.
   total packets passed, dropped packets (due to fifo full error) and dropped packets (due to firewall rule) for all port. 
    */
static void readStatsPerAllPorts(struct stat_ds *my_stat_ds_ptr)
{
	int gid;

        int startingStatisticsTotal = 9; // last bits from register(24 from eg 0x80000024) in Dec are 36/4=>9
        int startingStatisticsFifo = 11; // last bits from register(2C from eg 0x8000002C) in Dec are 44/4=>11
        int startingStatisticsFw = 13;

	int totalPort1 = 0;
	int totalPort2 = 0;
	int totalPort3 = 0;
	int totalPort4 = 0;
	int fifoPort1 = 0;
	int fifoPort2 = 0;
	int fifoPort3 = 0;
	int fifoPort4 = 0;
	int fwPort1 = 0;
	int fwPort2 = 0;
	int fwPort3 = 0;
	int fwPort4 = 0;

        my_stat_ds_ptr-> Total_passed = 0x0;
        my_stat_ds_ptr-> Fifo_dropped = 0x0;
        my_stat_ds_ptr-> Fw_dropped = 0x0;

	gid = current_gid().val;
	if ((gid != 0)) { // only root group can access statistics
#ifdef FW_KERN_LOGS
pr_info("readStatsPerAllPorts: gid:%d", (int)gid);
#endif
		pr_info("NON-ROOT TRIED TO PERFORM readStatsPerPort");
		BUG_ON(gid != 0);
	}

        //read statistics per all ports (totally)
        totalPort1 = ioread32(iobram1 + startingStatisticsTotal) - init_stat_total1;
        fifoPort1 = ioread32(iobram1 + startingStatisticsFifo) - init_stat_fifo1;
        fwPort1 = ioread32(iobram1 + startingStatisticsFw) - init_stat_fw1;

	totalPort2 = ioread32(iobram2 + startingStatisticsTotal) - init_stat_total2;
        fifoPort2 = ioread32(iobram2 + startingStatisticsFifo) - init_stat_fifo2;
        fwPort2 = ioread32(iobram2 + startingStatisticsFw) - init_stat_fw2;    

	totalPort3 = ioread32(iobram3 + startingStatisticsTotal) - init_stat_total3;
        fifoPort3 = ioread32(iobram3 + startingStatisticsFifo) - init_stat_fifo3;
        fwPort3 = ioread32(iobram3 + startingStatisticsFw) - init_stat_fw3; 

	totalPort4 = ioread32(iobram4 + startingStatisticsTotal) - init_stat_total4;
        fifoPort4 = ioread32(iobram4 + startingStatisticsFifo) - init_stat_fifo4;
        fwPort4 = ioread32(iobram4 + startingStatisticsFw) - init_stat_fw4; 

	my_stat_ds_ptr->Total_passed = totalPort1 + totalPort2 + totalPort3 + totalPort4;
	my_stat_ds_ptr->Fifo_dropped = fifoPort1 + fifoPort2 + fifoPort3 + fifoPort4;
	my_stat_ds_ptr->Fw_dropped = fwPort1 + fwPort2 + fwPort3 + fwPort4;

#ifdef FW_KERN_LOGS
pr_info("readStatsPerAllPorts: Total_passed:%d - Fifo_dropped:%d Fw_dropped:%d\n",  my_stat_ds_ptr->Total_passed, my_stat_ds_ptr->Fifo_dropped, my_stat_ds_ptr->Fw_dropped);
#endif

}

/*
  This function called by user to setup, read, write brams or stats
  The kernel or driver device allows the userspace to send commands
  */
static long my_unlocked_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	struct fw_ds my_fw_ds;
	struct stat_ds my_stat_ds;
	struct access_ds my_access_ds;
	struct direct_access_ds my_direct_access_ds;
	
	int size, rc, rc1, rc2;
	void __user *ioargp = (void __user *)arg;

	if (_IOC_TYPE(cmd) != MYIOC_TYPE) {
#ifdef FW_KERN_LOGS
pr_info("Got invalid case, CMD=%d\n", cmd);
#endif
		return -EINVAL;
	}

	size = _IOC_SIZE(cmd);
	
	switch (cmd) {
		case IOCTL_SETUP_FW_PORT:
#ifdef FW_KERN_LOGS
pr_info("\n\nCALL IOCTL_SETUP_FW_PORT\n");
#endif
		   rc = copy_from_user(&my_fw_ds, ioargp, size);
		   setupFW(&my_fw_ds);
#ifdef FW_KERN_LOGS
pr_info("EXTT IOCTL_SETUP_FW_PORT\n\n");
#endif
		   return rc;


		case IOCTL_CHECK_FW_PORT:
#ifdef FW_KERN_LOGS
pr_info("\n\nCALL IOCTL_CHECK_FW_PORT\n");
#endif
		   rc = copy_from_user(&my_fw_ds, ioargp, size);
		   rc = checkFWRegs(&my_fw_ds);
		   rc = copy_to_user(ioargp, &my_fw_ds, size);
#ifdef FW_KERN_LOGS
pr_info("EXIT IOCTL_CHECK_FW_PORT\n\n");
#endif
		   return rc; // return code 1 -> check ok


		case IOCTL_DIRECT_ACCESS_BRAM:
#ifdef FW_KERN_LOGS
pr_info("\n\nCALL IOCTL_DIRECT_ACCESS_BRAM\n");
#endif
                  rc1 = copy_from_user(&my_direct_access_ds, ioargp, size);
                  accessBram(&my_direct_access_ds);
                  rc2 = copy_to_user(ioargp, &my_direct_access_ds, size);
#ifdef FW_KERN_LOGS
pr_info("EXIT IOCTL_DIRECT_ACCESS_BRAM\n\n");
#endif
		  //return (rc1 || rc2); //return ((rc1 == 0) && (rc2 ==0));
		  return rc2; //return ((rc1 == 0) && (rc2 ==0));


		case IOCTL_ACCESS_BRAM:
#ifdef FW_KERN_LOGS
pr_info("\n\nCALL IOCTL_ACCESS_BRAM\n\n");
#endif
                  rc1 = copy_from_user(&my_access_ds, ioargp, size);
                  accessBramFW(&my_access_ds);

                  rc2 = copy_to_user(ioargp, &my_access_ds, size);
#ifdef FW_KERN_LOGS
pr_info("EXIT IOCTL_ACCESS_RAM\n\n");
#endif
		  return rc2; //return ((rc1 == 0) && (rc2 ==0));


		case IOCTL_READ_FW_PORT:
#ifdef FW_KERN_LOGS
pr_info("\n\nCALL IOCTL_READ_FW_PORT\n");
#endif
		  rc = copy_from_user(&my_fw_ds, ioargp, size);
		  readFWRegs(&my_fw_ds);
		  rc = copy_to_user(ioargp, &my_fw_ds, size);
#ifdef FW_KERN_LOGS
pr_info("EXIT IOCTL_READ_FW_PORT\n\n");
#endif
		  return rc;


		case IOCTL_READ_STATS:
#ifdef FW_KERN_LOGS
pr_info("\n\nCALL IOCTL_READ_STATS\n");
#endif
		  rc = copy_from_user(&my_stat_ds, ioargp, size);
                  readStatsPerPort(&my_stat_ds);
                  rc = copy_to_user(ioargp, &my_stat_ds, size);
#ifdef FW_KERN_LOGS
pr_info("EXIT IOCTL_READ_STATS\n\n");
#endif
		  return rc;


		case IOCTL_READ_STATS_ALL:
#ifdef FW_KERN_LOGS
pr_info("\n\nCALL IOCTL_READ_STATS_ALL\n");
#endif
                  rc = copy_from_user(&my_stat_ds, ioargp, size);
		  readStatsPerAllPorts(&my_stat_ds);
		  rc = copy_to_user(ioargp, &my_stat_ds, size);
#ifdef FW_KERN_LOGS
pr_info("EXIT IOCTL_READ_STATS_ALL\n\n");
#endif
                  return rc;

		default:
#ifdef FW_KERN_LOGS
pr_info("Got invalid case, CMD=%d\n", cmd);
#endif
                  return -EINVAL;
	}
}


static struct file_operations pugs_fops = // file ops struct
{
	.owner = THIS_MODULE,
 	// no .read = my_read or .write = my_write,
	.unlocked_ioctl = my_unlocked_ioctl,
	.open = my_open,
	.release = my_close
};

module_init(mfd_init);
module_exit(mfd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEI of Crete <dreams@cs.teicrete.gr>");
MODULE_DESCRIPTION("Dreams Access BRAM memory module");
