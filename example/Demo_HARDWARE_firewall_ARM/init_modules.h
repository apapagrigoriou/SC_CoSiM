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
// Create firewall as character device

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>   //Includes all functions for kernel like printk and their enum parameters
#include <linux/types.h>	//Defines the dev_t which contains both major and minor numbers
#include <linux/kdev_t.h>   //Obtain the Macros from which we obtain the major/Minor
#include <linux/fs.h>	   //Contains the function for registering the major & minor Numbers
#include <linux/device.h>   //Functions with which the driver populates the /sys using the Linux device model APIs decleared here
#include <linux/cdev.h>	 //cdev_init() and cdev_add()
#include <linux/ioport.h>   //allocation of memory regions request_mem_region()
#include <asm/uaccess.h>	//copy_from/to_user
#include <linux/slab.h>	 //kmalloc header
#include <asm/io.h>	     //io memory header
#include <linux/sched.h>	//add the delay

/* view registered character devices
 $ ls -l /dev/ | grep "^c" */

#define DRV_PAGE_SIZE 1024 
#define CMD_DELAY	 1 

// address map regions
#define BRAM1_DIRECT  0x40000000 // directly access BRAM 1
#define BRAM2_DIRECT  0x40001000 // directly access BRAM 2
#define BRAM3_DIRECT  0x40002000 // directly access BRAM 3
#define BRAM4_DIRECT  0x40003000 // directly access BRAM 4
#define FW_SETUP      0x50000000 // setup firewall rules, L/H Regs
#define FW_DEBUG      0x50001000 // debug firewall rules, L/H Regs
#define BRAM1         0x80000000 // access BRAM1 via Switch/Firewall port 0
#define BRAM2         0x80001000 // access BRAM2 via Switch/Firewall port 0
#define BRAM3         0x80002000 // access BRAM3 via Switch/Firewall port 0
#define BRAM4         0x80003000 // access BRAM4 via Switch/Firewall port 0

static int Device_Open = 0;
static int my_open(struct inode *i, struct file *f);
static int my_close(struct inode *i, struct file *f);

static dev_t first; // global variable for first device number
static struct class *cl; // global variable for device class
static struct cdev c_dev; // global variable for character device structure

volatile unsigned *iobram1_direct = NULL; // direct access to BRAM
volatile unsigned *iobram2_direct = NULL;
volatile unsigned *iobram3_direct = NULL;
volatile unsigned *iobram4_direct = NULL;

volatile unsigned *noc_setup_registers = NULL;
volatile unsigned *noc_monitor_registers = NULL;

volatile unsigned *iobram1 = NULL; // access to BRAM via switch (normal access)
volatile unsigned *iobram2 = NULL;
volatile unsigned *iobram3 = NULL;
volatile unsigned *iobram4 = NULL;

struct resource *rc_bram1 = NULL;   // direct access to BRAM
struct resource *rc_bram2 = NULL;   
struct resource *rc_bram3 = NULL;   
struct resource *rc_bram4 = NULL;   

struct resource *rc_fw_setup = NULL;
struct resource *rc_fw_debug = NULL;

struct resource *rc_bram1_direct = NULL; // direct access BRAM
struct resource *rc_bram2_direct = NULL;
struct resource *rc_bram3_direct = NULL;
struct resource *rc_bram4_direct = NULL;

//call the file_operations method
static struct file_operations pugs_fops;

static unsigned int init_stat_total1 = 0;
static unsigned int init_stat_total2 = 0;
static unsigned int init_stat_total3 = 0;
static unsigned int init_stat_total4 = 0;

static unsigned int init_stat_fifo1 = 0;
static unsigned int init_stat_fifo2 = 0;
static unsigned int init_stat_fifo3 = 0;
static unsigned int init_stat_fifo4 = 0;

static unsigned int init_stat_fw1 = 0;
static unsigned int init_stat_fw2 = 0;
static unsigned int init_stat_fw3 = 0;
static unsigned int init_stat_fw4 = 0;

//called when a process calls the module
static int my_open(struct inode *i, struct file *f) {
	//	printk(KERN_INFO "my_open(%p)\n", f);
	if (Device_Open)
	    return -EBUSY;
	Device_Open++;
	try_module_get(THIS_MODULE);
	return 0;
}
//called when a process calls to close the module
static int my_close(struct inode *i, struct file *f) {
	//	printk(KERN_INFO "my_close(%p,%p)\n", i, f);
	Device_Open--;
	module_put(THIS_MODULE);
	return 0;
}

static int __init mfd_init(void) /* Constructor */ {
	unsigned int BRAM_REGION_SIZE;
	BRAM_REGION_SIZE = 4 * DRV_PAGE_SIZE;
 
        printk(KERN_INFO "NoC - Firewall Module registered");
	if (alloc_chrdev_region(&first, 0, 1, "region_sw_firewall") < 0) {
	    return -1;
	}
	if ((cl = class_create(THIS_MODULE, "class_sw_firewall")) == NULL) {
	    unregister_chrdev_region(first, 1);
	    return -1;
	}
	if (device_create(cl, NULL, first, NULL, "sw_firewall") == NULL) {
	    class_destroy(cl);
	    unregister_chrdev_region(first, 1);
	    return -1;
	}
	cdev_init(&c_dev, &pugs_fops);
	if (cdev_add(&c_dev, first, 1) == -1) {
	    device_destroy(cl, first);
	    class_destroy(cl);
	    unregister_chrdev_region(first, 1);
	    return -1;
	}
	printk(KERN_INFO "Inserted switch firewall::<Major, Minor>: <%d, %d>\n", MAJOR(first), MINOR(first));

	// obtain virtual address to access physical memory
        rc_bram1_direct = request_mem_region(BRAM1_DIRECT, BRAM_REGION_SIZE, "BRAM1_DIRECT");
        rc_bram2_direct = request_mem_region(BRAM2_DIRECT, BRAM_REGION_SIZE, "BRAM2_DIRECT");
        rc_bram3_direct = request_mem_region(BRAM3_DIRECT, BRAM_REGION_SIZE, "BRAM3_DIRECT");
        rc_bram4_direct = request_mem_region(BRAM4_DIRECT, BRAM_REGION_SIZE, "BRAM4_DIRECT");

	rc_fw_setup = request_mem_region(FW_SETUP, DRV_PAGE_SIZE, "setup_fw_registers");
	rc_fw_debug = request_mem_region(FW_DEBUG, DRV_PAGE_SIZE, "debug_fw_registers");

        rc_bram1 = request_mem_region(BRAM1, DRV_PAGE_SIZE, "BRAM1");
        rc_bram2 = request_mem_region(BRAM2, DRV_PAGE_SIZE, "BRAM2");
        rc_bram4 = request_mem_region(BRAM3, DRV_PAGE_SIZE, "BRAM3");
        rc_bram4 = request_mem_region(BRAM4, DRV_PAGE_SIZE, "BRAM4");

/*
	if (rc_bram1_direct == NULL) {
		printk(KERN_INFO "Unable to create mem region BRAM1_DIRECT - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "BRAM1_DIRECT ok ");

	if (rc_bram2_direct == NULL) {
		printk(KERN_INFO "Unable to create mem region BRAM2_DIRECT - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "BRAM2_DIRECT ok ");

	if (rc_bram3_direct == NULL) {
		printk(KERN_INFO "Unable to create mem region BRAM3_DIRECT - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "BRAM3_DIRECT ok ");

	if (rc_bram4_direct == NULL) {
		printk(KERN_INFO "Unable to create mem region BRAM4_DIRECT - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "BRAM4_DIRECT ok ");


	if (rc_fw_setup == NULL) {
		printk(KERN_INFO "Unable to create mem region FW_SETUP - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "FW_SETUP ok ");

	if (rc_fw_debug == NULL) {
		printk(KERN_INFO "Unable to create mem region FW_DEBUG - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "FW_DEBUG ok ");


	if (rc_bram1 == NULL) {
		printk(KERN_INFO "Unable to create mem region BRAM1 - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "BRAM1 ok ");

	if (rc_bram2 == NULL) {
		printk(KERN_INFO "Unable to create mem region BRAM2 - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "BRAM2 ok ");

	if (rc_bram3 == NULL) {
		printk(KERN_INFO "Unable to create mem region BRAM3 - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "BRAM3 ok ");

	if (rc_bram4 == NULL) {
		printk(KERN_INFO "Unable to create mem region BRAM4 - possibly occupied by other driver");
                return -1;
        } else
		printk(KERN_INFO "BRAM4 ok ");
*/

	printk(KERN_INFO "ok with request_mem_region");

	iobram1_direct = ioremap(BRAM1_DIRECT, BRAM_REGION_SIZE);
	iobram2_direct = ioremap(BRAM2_DIRECT, BRAM_REGION_SIZE);
	iobram3_direct = ioremap(BRAM3_DIRECT, BRAM_REGION_SIZE);
	iobram4_direct = ioremap(BRAM4_DIRECT, BRAM_REGION_SIZE);

	noc_setup_registers = ioremap(FW_SETUP, DRV_PAGE_SIZE);
	noc_monitor_registers = ioremap(FW_DEBUG, DRV_PAGE_SIZE);

	iobram1 = ioremap(BRAM1, DRV_PAGE_SIZE);
	iobram2 = ioremap(BRAM2, DRV_PAGE_SIZE);
	iobram3 = ioremap(BRAM3, DRV_PAGE_SIZE);
	iobram4 = ioremap(BRAM4, DRV_PAGE_SIZE);

	// access statistics (reset them on new insmod since hw does not reset them)
	init_stat_total1 = ioread32(iobram1 + 9);
	init_stat_total2 = ioread32(iobram2 + 9);
	init_stat_total3 = ioread32(iobram3 + 9);
	init_stat_total4 = ioread32(iobram4 + 9);

	init_stat_fifo1 = ioread32(iobram1 + 11);
	init_stat_fifo2 = ioread32(iobram2 + 11);
	init_stat_fifo3 = ioread32(iobram3 + 11);
	init_stat_fifo4 = ioread32(iobram4 + 11);

	init_stat_fw1 = ioread32(iobram1 + 13);
	init_stat_fw2 = ioread32(iobram2 + 13);
	init_stat_fw3 = ioread32(iobram3 + 13);
	init_stat_fw4 = ioread32(iobram4 + 13);
  
  return 0;
}

static void __exit mfd_exit(void) { // destructor
unsigned int BRAM_REGION_SIZE; // access BRAM
BRAM_REGION_SIZE = 4 * DRV_PAGE_SIZE; // access BRAM

  iounmap(iobram1_direct);
  iounmap(iobram2_direct);
  iounmap(iobram3_direct);
  iounmap(iobram4_direct);

  iounmap(noc_setup_registers);
  iounmap(noc_monitor_registers);

  iounmap(iobram1);
  iounmap(iobram2);
  iounmap(iobram3);
  iounmap(iobram4);

  release_mem_region(BRAM1_DIRECT, BRAM_REGION_SIZE);
  release_mem_region(BRAM2_DIRECT, BRAM_REGION_SIZE);
  release_mem_region(BRAM3_DIRECT, BRAM_REGION_SIZE);
  release_mem_region(BRAM4_DIRECT, BRAM_REGION_SIZE);

  release_mem_region(FW_SETUP, DRV_PAGE_SIZE);
  release_mem_region(FW_DEBUG, DRV_PAGE_SIZE);

  release_mem_region(BRAM1, DRV_PAGE_SIZE);
  release_mem_region(BRAM2, DRV_PAGE_SIZE);
  release_mem_region(BRAM3, DRV_PAGE_SIZE);
  release_mem_region(BRAM4, DRV_PAGE_SIZE);

  cdev_del(&c_dev);
  device_destroy(cl, first);
  class_destroy(cl);
  unregister_chrdev_region(first, 1);
  printk(KERN_INFO "Switch Firewall Module::unregistered\n");
}
