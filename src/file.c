#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

#include "tfs.h"

int tfs_readpage(struct file *file, struct page *page)
{
	struct inode *inode;
	void         *buffer;
	loff_t        offset;
	loff_t        size;
	unsigned long fill_size;
	unsigned long sector;
	int           ret;

	printk(KERN_ERR TFS "tfs_readpage: entry\n");

	inode = page->mapping->host;
	buffer = kmap(page);
	if (!buffer) {
		printk(KERN_ERR TFS "tfs_readpage: unable to map page\n");
		return -ENOMEM;
	}

	printk(KERN_ERR TFS "tfs_readpage: page mapped\n");

	offset    = page_offset(page);
	size      = i_size_read(inode);
	fill_size = 0;
	ret       = 0;

	printk(KERN_ERR TFS "tfs_readpage: offset[%lld] size[%lld]\n", offset, size);

	if (offset < size) {
		fill_size = (size - offset) > PAGE_SIZE ? PAGE_SIZE : (size - offset);

		printk(KERN_ERR TFS "tfs_readpage: fill_size[%ld]\n", fill_size);

		sector = inode->i_ino + offset/XFS_SECTOR_SIZE;

		printk(KERN_ERR TFS "tfs_readpage: sector[%ld]\n", sector);

		ret = tfs_dev_read(inode->i_sb, sector, buffer, fill_size);
		if (ret < 0) {
			printk(KERN_ERR TFS "tfs_readpage: error reading from device [%d]\n", ret);
			SetPageError(page);
			fill_size = 0;
			ret = -EIO;
		}

		printk(KERN_ERR TFS "tfs_readpage: success reading from device\n");
	}

	if (fill_size < PAGE_SIZE) {
		printk(KERN_ERR TFS "tfs_readpage: filling until PAGE_SIZE\n");
		memset(buffer + fill_size, 0, PAGE_SIZE - fill_size);
	}

	if (ret == 0) {
		printk(KERN_ERR TFS "tfs_readpage: updating page\n");
		SetPageUptodate(page);
	}

	flush_dcache_page(page);
	kunmap(page);
	unlock_page(page);
	
	printk(KERN_ERR TFS "tfs_readpage: exiting [%d]\n", ret);
	return ret;
}

struct file_operations tfs_file_ops = {
	.owner      = THIS_MODULE,
	.llseek     = generic_file_llseek,
	.mmap       = generic_file_mmap,
	.fsync      = generic_file_fsync,
	.read_iter  = generic_file_read_iter,
};

struct address_space_operations tfs_as_ops = {
	.readpage = tfs_readpage,
};
