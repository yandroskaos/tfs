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

	inode = page->mapping->host;
	buffer = kmap(page);
	if (!buffer)
		return -ENOMEM;

	offset    = page_offset(page);
	size      = i_size_read(inode);
	fill_size = 0;
	ret       = 0;

	if (offset < size) {
		fill_size = (size - offset) > PAGE_SIZE ? PAGE_SIZE : (size - offset);

		sector = inode->i_ino + offset/XFS_SECTOR_SIZE;

		ret = tfs_dev_read(inode->i_sb, sector, buffer, fill_size);
		if (ret < 0) {
			SetPageError(page);
			fill_size = 0;
			ret = -EIO;
		}
	}

	if (fill_size < PAGE_SIZE)
		memset(buffer + fill_size, 0, PAGE_SIZE - fill_size);

	if (ret == 0)
		SetPageUptodate(page);

	flush_dcache_page(page);
	kunmap(page);
	unlock_page(page);
	
	return ret;
}

const struct file_operations tfs_file_ops = {
	.llseek     = generic_file_llseek,
	.mmap       = generic_file_mmap,
	.fsync      = generic_file_fsync,
	.read_iter  = generic_file_read_iter,
};

const struct address_space_operations tfs_as_ops = {
	.readpage    = tfs_readpage,
};
