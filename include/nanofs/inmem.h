#ifndef INMEM_H
#define NANOFS_INMEM_H

#include <nanofs/info.h>

/* ======================================================================== */
/* Inmemory config                                                          */
/* ======================================================================== */

#define INMEM_BLOCKS_NUM    10
#define INMEM_INODES_NUM    10

enum state {UNUSED, BUSY};

struct inmem_block {
    enum state state;
    char inmem_addr[BSIZE];
    uint32_t dev_block_num;
};

struct inmem_inode {
    enum state state;
    inode_t inode;
};


/* ======================================================================== */
/* Inmemory interface                                                       */
/* ======================================================================== */

struct inmem_block * fs_low_level_storage inmem_read_block(int block_num);

void fs_low_level_storage
inmem_block_free(struct inmem_block *inmem_block, int is_dirty);

inode_t *fs_low_level_inode inmem_lock_inode();

void fs_low_level_inode inmem_free_inode(inode_t *ip);

void inmem_init(void);

#endif // NANOFS_INMEM_H
