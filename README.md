# tfs
Test Filesystem for Linux

Right now it is no more than a readonly filesystem for the filesystem shipped for [XkyOS](https://github.com/yandroskaos/XkyOS).

The layout of the filesystem is very simple as it was designed to be readonly and for minimal system use (mainly to load executables), and is as follows:

- **0**        => MBR
- **1**        => XFT (Directory of size 512 with 3 entries: Loader, Kernel Directory, XFS (Rest of Filesystem))
- **2 .. n**   => Loader
- **n+1 .. k** => Kernel directory + kernel binaries
- **k+1**      => XFS directory
- **k+2 ....** => Rest of data

A directory is size PAGE_SIZE always, and can have a maximum of 64 entries.
Each directory entry is 64 bytes long and has the following data:

- **used**         : indicates if the entry is being used
- **is_directory** : indicates if the entry points to a directory
- **items**        : indicates the number of used entries if entry points to a directory
- **name**         : name of the entry to a maximum of 52 usable characters preceded with the one byte size (as pascal strings)
- **lba**          : logical block address of the element
- **size**         : size (4096 for directories, whatever for files)

As is a readonly filesystem which is generated for system boot, all sector of a file happens to be contiguous.

# Test
To check, compile the module and do the following:
- losetup /dev/loopX /path/to/tfs/dat/tfs.dsk
- insmod tfs.ko
- mount -t tfs /dev/loopX /path/to/mount

where loopX is a usable loop device (you can find with losetup --find)

To unload:
- umount -t tfs /path/to/mount
- rmmod tfs.ko
- losetup -d /dev/loopX

