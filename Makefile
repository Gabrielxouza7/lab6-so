obj-m += poker_driver.o

# Aponta para a árvore do kernel que compilamos nos labs anteriores
KDIR := /home/gabriel/kernel/linux

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
