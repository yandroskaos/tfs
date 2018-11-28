#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

#include "tfs.h"

int tfs_dev_read(struct super_block *sb, unsigned long sector,
                 void *buffer, size_t buffer_size)
{
    int blocks;
    int last_read;
    struct buffer_head *bh;

    blocks    = buffer_size / XFS_SECTOR_SIZE;
    last_read = buffer_size % XFS_SECTOR_SIZE;
    
    for(; blocks; blocks--)
    {
		bh = sb_bread(sb, sector++);
		if (!bh)
			return -EIO;

		memcpy(buffer, bh->b_data, XFS_SECTOR_SIZE);
		brelse(bh);
		buffer += XFS_SECTOR_SIZE;
	}

    if(last_read)
    {
		bh = sb_bread(sb, sector++);
		if (!bh)
			return -EIO;

		memcpy(buffer, bh->b_data, last_read);
		brelse(bh);
    }

	return 0;
}
