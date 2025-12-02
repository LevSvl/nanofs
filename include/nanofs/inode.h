#ifndef NANOFS_INODE_H
#define NANOFS_INODE_H

#include <nanofs/info.h>

uint8_t fs_low_level_storage fs_inode_alloc();

void fs_low_level_storage fs_inode_free(uint8_t inum);

inode_t * fs_low_level_storage fs_low_level_inode fs_get_inode_by_inum(uint8_t inum);

int fs_low_level_storage fs_store_inode_by_inum(inode_t *ip, uint8_t inum);

inode_t * fs_low_level_storage fs_find_inode_in_dir(inode_t *ip_parent, char *fname);

int fs_low_level_storage fs_add_inode_in_dir(inode_t *ip_parent, char *fname, uint8_t inum_child);

inode_t * fs_low_level_inode fs_get_inode(char *path);

inode_t *fs_create_inode(char *path, uint8_t type);

int fs_trunc_inode(inode_t *ip);

int fs_low_level_storage fs_write_to_inode(inode_t *ip, uint8_t off, 
                            uint8_t size, char * buf);

int fs_low_level_storage fs_read_from_inode(inode_t *ip, uint8_t off, 
                            uint8_t size, char * buf);

int inode_init_root(uint8_t root_inum);

#endif // NANOFS_INODE_H
