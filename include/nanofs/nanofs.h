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
int fs_read(int fd, void *buf, uint32_t size);
int fs_write(int fd, void *buf, uint32_t size);
int fs_close(int fd);

#include <nanofs/info.h>
#include <nanofs/block.h>
#include <nanofs/inmem.h>
#include <nanofs/inode.h>

#endif /* NANOFS_NANOFS_H */
