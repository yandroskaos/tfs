#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

#include "tfs.h"

int tfs_fill_super(struct super_block *sb, void *data, int silent)
{
	int                 ret = -EPERM;
	struct xfs_entry   *xft = 0;
	struct inode       *root_inode = 0;

	printk(KERN_ERR TFS "tfs_fill_super: entry\n");

	ret = sb_set_blocksize(sb, XFS_SECTOR_SIZE);
	if(!ret) {
		printk(KERN_ERR TFS "tfs_fill_super: cannot set block size in device\n");
		return ret;
	}

	printk(KERN_ERR TFS "tfs_fill_super: block size set\n");

	//
	// Read block 1 (0 is for booting)
	//
	xft = (struct xfs_entry *)kmalloc(XFS_SECTOR_SIZE, GFP_KERNEL);
	if (!xft)
		return -ENOMEM;

	printk(KERN_ERR TFS "tfs_fill_super: xft allocated\n");

	ret = tfs_dev_read(sb, 1, (void*)xft, XFS_SECTOR_SIZE);
	if(ret)
		goto release;

	printk(KERN_ERR TFS "tfs_fill_super: xft read\n");
	
	//
	// Fill vfs super block
	//
	sb->s_magic    = 0x54465353; //'TFSS'
	sb->s_fs_info  = xft;
	sb->s_maxbytes = 4*1024*1024;
	sb->s_flags   |= SB_RDONLY | SB_NOATIME;
	//sb->s_op       = &tfs_super_ops;

	printk(KERN_ERR TFS "tfs_fill_super: vfs superblock init\n");

	//
	// Create vfs root inode
	//
	root_inode = new_inode(sb);
	if (!root_inode) {
		ret = -ENOMEM;
		goto release;
	}

	printk(KERN_ERR TFS "tfs_fill_super: root inode created\n");

	inode_init_owner(root_inode, NULL, S_IFDIR |
			 S_IRUSR | S_IXUSR |
			 S_IRGRP | S_IXGRP |
			 S_IROTH | S_IXOTH);
	root_inode->i_sb    = sb;
	root_inode->i_ino   = 1;
	root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime =
		current_time(root_inode);
	root_inode->i_mode = TFS_MODE_DIRECTORY;
	root_inode->i_size = 3;
	inc_nlink(root_inode);
	root_inode->i_op  = &tfs_inode_ops;
	root_inode->i_fop = &tfs_dir_ops;

	printk(KERN_ERR TFS "tfs_fill_super: root inode init\n");

	//
	// Make dentry
	//
	sb->s_root = d_make_root(root_inode);
	if (!sb->s_root) {
		ret = -ENOMEM;
		goto release;
	}

	printk(KERN_ERR TFS "tfs_fill_super: root dentry created\n");

	printk(KERN_ERR TFS "tfs_fill_super: exit success\n");
	return 0;

release:
	sb->s_fs_info = 0;
	if(xft)
		kfree(xft);
	printk(KERN_ERR TFS "tfs_fill_super: exit failure [%d]\n", ret);
	return ret;
}

struct dentry *tfs_mount(struct file_system_type *fs_type, int flags,
						 const char *dev_name, void *data)
{
	struct dentry *ret;

	ret = mount_bdev(fs_type, flags, dev_name, data, tfs_fill_super);

	if (IS_ERR(ret))
		printk(KERN_ERR TFS "Error mounting on [%s]\n", dev_name);
	else
		printk(KERN_INFO TFS "Succesfully mounted on [%s]\n", dev_name);

	return ret;
}

void tfs_unmount(struct super_block *sb)
{
	kill_block_super(sb);

	printk(KERN_INFO TFS "Unmount succesful.\n");
}

struct file_system_type tfs_type = {
	.owner   = THIS_MODULE,
	.name    = "tfs",
	.mount   = tfs_mount,
	.kill_sb = tfs_unmount,
	.fs_flags = FS_REQUIRES_DEV
};

int tfs_init(void)
{
	int ret;

	ret = register_filesystem(&tfs_type);
	
	if (ret == 0)
		printk(KERN_INFO TFS "Sucessfully registered\n");
	else
		printk(KERN_ERR TFS "Failed to register. Error: [%d]\n", ret);

	return ret;
}

void tfs_exit(void)
{
	int ret;

	ret = unregister_filesystem(&tfs_type);

	if (ret == 0)
		printk(KERN_INFO TFS "Sucessfully unregistered\n");
	else
		printk(KERN_ERR TFS "Failed to unregister. Error: [%d]\n", ret);
}

module_init(tfs_init);
module_exit(tfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pablo Bravo");
MODULE_DESCRIPTION("Test Filesystem.");
MODULE_VERSION("0.01");
