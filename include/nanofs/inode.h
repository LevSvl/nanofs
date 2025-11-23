#ifndef NANOFS_INODE_H
#define NANOFS_INODE_H

#include <nanofs/info.h>

int fs_low_level_storage fs_inode_alloc();

void fs_low_level_storage fs_inode_free(int inum);

inode_t * fs_low_level_storage fs_low_level_inode fs_get_inode_by_inum(int inum);

int fs_low_level_storage fs_store_inode_by_inum(inode_t *ip, int inum);

inode_t * fs_low_level_storage fs_find_inode_in_dir(inode_t *ip_parent, char *fname);

int fs_low_level_storage fs_add_inode_in_dir(inode_t *ip_parent, char *fname, int inum_child);

inode_t * fs_low_level_inode fs_get_inode(char *path);

inode_t *fs_create_inode(char *path, int type);

int fs_trunc_inode(inode_t *ip);

int fs_low_level_storage fs_write_to_inode(inode_t *ip, uint32_t off, 
                            uint32_t size, char * buf);

int fs_low_level_storage fs_read_from_inode(inode_t *ip, uint32_t off, 
                            uint32_t size, char * buf);

int inode_init_root(int root_inum);

superblock_t *inode_init_sb(int sb_num);

#endif // NANOFS_INODE_H
