obj-m += poker_driver.o

KDIR := /home/gabriel/kernel/linux

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
