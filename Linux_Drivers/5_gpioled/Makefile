KERNELDIR := /home/cl/Linux/I.MAX6ULL/linux/temp/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek

CURRENT_PATH := ${shell pwd}
obj-m :=gpioled.o

build : kernal_modules

kernal_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean