#include "types.h"
#include "math.h"
#include "console.h"
#include "string.h"

#include <nanofs/nanofs.h>

struct inmem_block inmem_blocks[INMEM_BLOCKS_NUM];
struct inmem_inode inmem_inodes[INMEM_INODES_NUM];

/* ======================================================================== */
/* Support of inmem operations                                              */
/* ======================================================================== */

struct inmem_block * fs_low_level_storage
inmem_read_block(uint8_t block_num)
{
    for (int i = 0; i < INMEM_BLOCKS_NUM; i++) {
        if (inmem_blocks[i].state == UNUSED) {
            inmem_blocks[i].state = BUSY;
            inmem_blocks[i].dev_block_num = block_num;
            
            block_read(inmem_blocks[i].inmem_addr, block_num);

            return &inmem_blocks[i];
        }
    }
    return 0;
}

void fs_low_level_storage
inmem_block_free(struct inmem_block *inmem_block, int is_dirty)
{
    if (is_dirty)
        block_write(inmem_blocks->inmem_addr, inmem_blocks->dev_block_num);
    inmem_blocks->state = UNUSED;
}

inode_t * fs_low_level_inode inmem_lock_inode()
{
    for (int i = 0; i < INMEM_INODES_NUM; i++)
        if (inmem_inodes[i].state == UNUSED) {
            inmem_inodes[i].state = BUSY;
            return &inmem_inodes[i].inode;
        }
    return 0;
}

void fs_low_level_inode inmem_free_inode(inode_t *ip)
{
    for (int i = 0; i < INMEM_INODES_NUM; i++)
        if (&inmem_inodes[i].inode == ip)
            inmem_inodes[i].state = BUSY;
}

void inmem_init()
{
    /* init inmemory inodes and blocks */
    for (int i = 0; i < INMEM_BLOCKS_NUM; i++) {
        inmem_blocks[i].state = UNUSED;
    }
    for (int i = 0; i < INMEM_INODES_NUM; i++) {
        inmem_inodes[i].state = UNUSED;
    }
}
