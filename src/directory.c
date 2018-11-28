#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

#include "tfs.h"

int tfs_iterate(struct file *filp, struct dir_context *ctx)
{
	struct super_block *sb        = 0;
	struct xfs_entry   *directory = 0;
	struct xfs_entry   *iter      = 0;
	int                 nentries  = 0;
	int                 ret       = 0;
	int                 i         = 0;
	
	sb        = filp->f_inode->i_sb;
	directory = (struct xfs_entry *)kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!directory)
		return -ENOMEM;

	ret = tfs_dev_read(sb, filp->f_inode->i_ino, (void*)directory, PAGE_SIZE);
	if(ret)
	{
		kfree(directory);
		return ret;
	}

	iter = directory;
	while(iter->used)
	{
		iter++;
		nentries++;
	}
	
	if (ctx->pos >= nentries) {
		kfree(directory);
		return 0;
	}

	iter = directory + ctx->pos;
	for (i = ctx->pos; i < nentries; ++i) {
		dir_emit(ctx, iter->name, strlen(iter->name), iter->lba, 
			iter->is_directory ? DT_DIR : DT_REG);
		ctx->pos++;
		iter++;
	}
	kfree(directory);
	return 0;
}

const struct file_operations tfs_dir_ops = {
	.read		  = generic_read_dir,
	.iterate_shared = tfs_iterate,
	.llseek	  = generic_file_llseek,
};
