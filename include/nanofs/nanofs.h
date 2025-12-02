#ifndef NANOFS_NANOFS_H
#define NANOFS_NANOFS_H

#include "types.h"
#include "math.h"
#include "time.h"


/* ======================================================================== */
/* Filesystem layout                                                        */
/* ======================================================================== */

#include <nanofs/layout.h>

extern superblock_t * sb;
extern inode_t *current_dir;
extern uintptr_t fs_base;


/* ======================================================================== */
/* Access to files                                                          */
/* ======================================================================== */

#define FS_O_RDONLY  0x000
#define FS_O_WRONLY  0x001
#define FS_O_RDWR    0x002
#define FS_O_CREATE  0x200
#define FS_O_TRUNC   0x400


/* ======================================================================== */
/* Filesystem interface                                                     */
/* ======================================================================== */

int fs_init();
int fs_open(char * path, int flags);
int fs_read(int fd, void *buf, uint8_t size);
int fs_write(int fd, void *buf, uint8_t size);
int fs_close(int fd);

#include <nanofs/info.h>
#include <nanofs/block.h>
#include <nanofs/inmem.h>
#include <nanofs/inode.h>

/*
 * Not for directly usage
 */
extern struct inmem_block *__inmem_sb;

static inline void read_sb()
{
    __inmem_sb = inmem_read_block(0);
    if (__inmem_sb == 0) {
        printf("Failed to read superblock");
        while (1) ;
    }

    sb = (superblock_t *)__inmem_sb->inmem_addr;
}

static inline void write_sb()
{
    inmem_block_free(__inmem_sb, 1);
    read_sb();
}

#endif /* NANOFS_NANOFS_H */
