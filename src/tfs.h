#ifndef __TFS_H__
#define __TFS_H__

#define XFS_SECTOR_SIZE 512
#define XFT_LBA		    1
#define XFT_SECTORS	    1
#define XFT_SIZE	    (XFS_SECTOR_SIZE * XFT_SECTORS)

#define XFS_DIRECTORY_SECTORS	8
#define XFS_DIRECTORY_SIZE	(XFS_SECTOR_SIZE * XFS_DIRECTORY_SECTORS)
#define XFS_ENTRY_NAME_SIZE	52

struct xfs_string
{
	u8 size;
	u8 text[XFS_ENTRY_NAME_SIZE];
};

struct xfs_entry
{
	u8                used;
	u8                is_directory;
	u8                items;
	struct xfs_string name;
	__le32            lba;
	__le32            size;
};

#define TFS "[tfs]"

extern struct file_system_type         tfs_type;
extern struct file_operations          tfs_file_ops;
extern struct file_operations          tfs_dir_ops;
extern struct inode_operations         tfs_inode_ops;
extern struct super_operations         tfs_super_ops;
extern struct address_space_operations tfs_as_ops;

#define TFS_MODE_DIRECTORY	(S_IFDIR | 0644)
#define TFS_MODE_FILE		(S_IFREG | 0644)

int tfs_dev_read(struct super_block *sb, unsigned long sector,
                 void *buffer, size_t buffer_size);

#endif
