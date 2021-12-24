KDIR ?= /home/speedy1/work/git_repos/ti-linux-kernel-dev/KERNEL

default:
	$(MAKE) ARCH=arm -C $(KDIR) M=$$PWD

clean:
	rm -f *.o *.a lepton.ko Module.symvers modules.order
