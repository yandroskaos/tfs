obj-m := tfs.o
tfs-objs := super.o inode.o file.o directory.o block.o
CFLAGS_super.o := -DDEBUG
CFLAGS_inode.o := -DDEBUG
CFLAGS_file.o := -DDEBUG
CFLAGS_directory.o := -DDEBUG
CFLAGS_block.o := -DDEBUG

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
