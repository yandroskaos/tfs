#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

#include "tfs.h"

struct dentry *tfs_lookup(struct inode *parent_inode,
			  struct dentry *child_dentry, unsigned int flags)
{
	struct super_block *sb        = 0;
	struct xfs_entry   *directory = 0;
	struct xfs_entry   *iter      = 0;
	int                 nentries  = 0;
	int                 ret       = 0;
	int                 i         = 0; 
	struct inode       *inode     = 0;

	printk(KERN_ERR TFS "tfs_lookup: entry\n");
	
	sb        = parent_inode->i_sb;
	directory = (struct xfs_entry *)kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!directory) {
		printk(KERN_ERR TFS "tfs_lookup: cannot alloc page for directory\n");
		return ERR_PTR(-ENOMEM);
	}

	printk(KERN_ERR TFS "tfs_lookup: page for directory allocated\n");

	ret = tfs_dev_read(sb, parent_inode->i_ino, (void*)directory, PAGE_SIZE);
	if(ret) {
		printk(KERN_ERR TFS "tfs_lookup: error reading from device inode number[%ld] error[%d]\n", parent_inode->i_ino, ret);
		kfree(directory);
		return ERR_PTR(ret);
	}

	printk(KERN_ERR TFS "tfs_lookup: read a page from device inode number[%ld]\n", parent_inode->i_ino);
	
	iter = directory;
	while(iter->used) {
		iter++;
		nentries++;
	}

	printk(KERN_ERR TFS "tfs_lookup: directory nentries [%d]\n", nentries);

	printk(KERN_ERR TFS "tfs_lookup: searching for [%s]\n", child_dentry->d_name.name);

	iter = directory;
	for (i = 0; i < nentries; ++i, ++iter) {
		size_t length = min(strlen(child_dentry->d_name.name), strlen(iter->name));

		printk(KERN_ERR TFS "tfs_lookup: entry [%s] length selected [%ld]\n", iter->name, length);

		if (strncmp(child_dentry->d_name.name, iter->name, length) != 0)
			continue;

		printk(KERN_ERR TFS "tfs_lookup: match\n");

		inode = iget_locked(sb, iter->lba);
		if (!inode) {
			printk(KERN_ERR TFS "tfs_lookup: iget_locked returned NULL\n");
			kfree(directory);
			return ERR_PTR(-EFAULT);
		}

		if (inode->i_state & I_NEW) {
			inode_init_owner(inode, parent_inode, 0);

			inode->i_ino  = iter->lba;
			inode->i_mode = iter->is_directory ? TFS_MODE_DIRECTORY : TFS_MODE_FILE;
			inode->i_size = iter->is_directory ? iter->items : iter->size;
			//set_nlink(inode, ?);
			//i_uid_write(inode, ?);
			//i_gid_write(inode, ?);
			inode->i_atime = inode->i_ctime = inode->i_mtime = current_time(inode);
			inode->i_op = &tfs_inode_ops;
			if (iter->is_directory)	{
				inode->i_fop = &tfs_dir_ops;
			} else {
				inode->i_fop            = &tfs_file_ops;
				inode->i_mapping->a_ops = &tfs_as_ops;
			}
			insert_inode_hash(inode);
			unlock_new_inode(inode);

			printk(KERN_ERR TFS "tfs_lookup: inode set up\n");
		}
		break;
	}

	kfree(directory);
	d_add(child_dentry, inode);

	printk(KERN_ERR TFS "tfs_lookup: exiting\n");
	return NULL;
}

struct inode_operations tfs_inode_ops = {
	.lookup = tfs_lookup,
};
