obj-m += kernel_mode.o
KERNELDIR = /root/linux-2015_R1/
MODULEPATH=$(PWD)

all:
	make -C $(CFLAGS) $(KERNELDIR) M=$(MODULEPATH) modules

clean:
	@echo "[INFO] Clean kernel module"
	make -C $(KERNELDIR) M=$(MODULEPATH) clean

