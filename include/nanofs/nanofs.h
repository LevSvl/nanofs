#ifndef NANOFS_NANOFS_H
#define NANOFS_NANOFS_H

#include "types.h"
#include "math.h"
#include "time.h"


/* ======================================================================== */
/* Image config                                                             */
/* ======================================================================== */

#define TARGET_IMAGE_NAME   "nanofs.img"
#define IMG(base, x)  (base + x)


/* ======================================================================== */
/* Filesystem config                                                        */
/* ======================================================================== */

#define FS_TOTAL_SIZE   (16)
#define FS_MAGIC        (0x4eb14eb1U)
#define FS_BLOCK_SIZE   (64)

#define BSIZE     (FS_BLOCK_SIZE)
#define BNUM      FS_TOTAL_SIZE
#define SECSIZE   512
#define SECNUM    (udiv4((BNUM*BSIZE),SECSIZE))
#define MAXFNAME  8
#define NDIRECT   4
#define ROOTINO   2
#define MAX_FILES_OPENED    4

/* ======================================================================== */
/* Filesystem descriptors                                                   */
/* ======================================================================== */

typedef struct superblock {
  uint32_t magic;               // Magic bytes
  uint32_t total_blocks_num;    // Size of file system
  uint32_t dblocks_num;         // Number of data blocks
  uint32_t iblocks_num;         // Number of inodes.
  uint32_t iblocks_start;       // Block number of first inode block
  uint32_t dbmap_start;         // Block number of first data map block
  uint32_t ibmap_start;         // Block number of first inode map block
} superblock_t;

enum {
    TYPE_DIR  = 1,
    TYPE_FILE = 2,
};

/* Size of inode - 32 bytes */
typedef struct inode
{
  uint16_t type;
  uint16_t nlink;
  uint32_t size;
  uint32_t addr[NDIRECT];
  volatile uint8_t align[8];
} inode_t;

typedef struct dirent
{
  uint8_t inum;
  uint32_t reclen;
  uint32_t strlen;
  volatile uint8_t align0[3];
  char name[MAXFNAME];
  volatile uint8_t align1[12];
} dirent_t;

struct file {
  enum { FD_NONE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint32_t off;          // FD_INODE
  short major;       // FD_DEVICE
};

extern superblock_t * sb;
extern inode_t *current_dir;
extern uintptr_t fs_base;

/* ======================================================================== */
/* Filesystem layout                                                        */
/* ======================================================================== */

#define inodeBitMapAddr   BSIZE
#define dataBitMapAddr    2*BSIZE
#define BITS_PER_BITMAP(T)  (T) (sizeof(T)*8)

#define inodeStartAddr    3*BSIZE
#define dataStartAddr     5*BSIZE

#define blk(inum)         (udiv4(inum*sizeof(inode_t), BSIZE))
// #define sector(blk)       ((blk*BSIZE + inodeStartAddr)/SECSIZE)

#define MAXFILES          (udiv4(dataStartAddr - inodeStartAddr,sizeof(inode_t)))
#define MAX_BLOCKS_PER_FILE NDIRECT

#define inum(off)         (udiv4(off - inodeStartAddr,sizeof(inode_t)))
#define inode_off(inum)   (inum*sizeof(inode_t))

#define BLOCK_NUM(addr)   (udiv4(addr,BSIZE))
#define BLOCK_ADDR(b_num) (BSIZE*(uintptr_t)b_num)


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
