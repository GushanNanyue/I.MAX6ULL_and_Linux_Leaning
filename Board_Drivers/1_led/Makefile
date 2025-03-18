objs =  led.o 

led.bin : $(objs)

#编译代码
	arm-linux-gnueabihf-ld -Ttext 0X87800000 $^ -o led.elf
	arm-linux-gnueabihf-objcopy -O binary -S led.elf $@
	arm-linux-gnueabihf-objdump -D -m arm led.elf > led.dis

#为代码加入IVT BOOT DCD数据头 header文件在官方imx文件中提前即可
	cat header led.bin >led.imx

#制作SD卡烧录文件	
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin led.imx > led.img

#删除平时不需要的文件
	rm -rf *.o *.bin *.dis *.elf  *.img


%.o : %*.S
	arm-linux-gnueabihf-gcc -Wall -nostdlib -c $< -o $@ 

%.o : %*.c
	arm-linux-gnueabihf-gcc -Wall -nostdlib -c $< -o $@ 

clean:
	rm -rf *.o *.bin *.dis *.elf *.imx *.img

run:
	sudo -i $(PWD)/uuu $(PWD)/led.imx 
	
