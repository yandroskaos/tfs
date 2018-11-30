#ifndef __TFS_H__
#define __TFS_H__

#define XFS_SECTOR_SIZE 512
#define XFT_LBA		    1
#define XFT_SECTORS	    1
#define XFT_SIZE	    (XFS_SECTOR_SIZE * XFT_SECTORS)

#define XFS_DIRECTORY_SECTORS	8
#define XFS_DIRECTORY_SIZE		(XFS_SECTOR_SIZE * XFS_DIRECTORY_SECTORS)
#define XFS_ENTRY_NAME_SIZE		52

struct xfs_entry
{
	u8	used;
	u8	is_directory;
	u8	items;
	u8	name[XFS_ENTRY_NAME_SIZE];
	__le32	lba;
	__le32	size;
};

#define TFS "[tfs]"

extern const struct file_system_type         tfs_type;
extern const struct file_operations          tfs_file_ops;
extern const struct file_operations          tfs_dir_ops;
extern const struct inode_operations         tfs_inode_ops;
extern const struct super_operations         tfs_super_ops;
extern const struct address_space_operations tfs_as_ops;

#define TFS_MODE_DIRECTORY	(S_IFDIR | 0644)
#define TFS_MODE_FILE		(S_IFREG | 0644)

int tfs_dev_read(struct super_block *sb, unsigned long sector,
                 void *buffer, size_t buffer_size);

#endif
