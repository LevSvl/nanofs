#ifndef NANOFS_LAYOUT_H
#define NANOFS_LAYOUT_H

/* ======================================================================== */
/* Image config                                                             */
/* ======================================================================== */

#define TARGET_IMAGE_NAME   "nanofs.img"
#define IMG(base, x)  (base + x)

/* ======================================================================== */
/* Filesystem config                                                        */
/* ======================================================================== */

#define FS_TOTAL_SIZE   (16)
#define FS_MAGIC        (0x4eb1)
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

/* Size of superblock - 8 bytes */
typedef struct superblock {
    uint16_t magic;               // Magic bytes
    uint8_t total_blocks_num;    // Size of file system
    uint8_t iblocks_num;         // Number of inodes.
    uint8_t dblocks_num;         // Number of data blocks
    uint16_t ibmap;
    uint16_t dbmap;
} superblock_t;

enum {
    TYPE_DIR  = 1,
    TYPE_FILE = 2,
};

/* Size of inode - 8 bytes */
typedef     struct inode
{
    uint8_t size;
    uint16_t b_addr;
    uint16_t b_size;
    uint8_t type;
    uint8_t align[8];
} inode_t;

/* Size of dirent - 16 bytes */
typedef struct dirent
{
    uint16_t reclen;
    uint16_t strlen;
    uint8_t inum;
    char name[8];
    uint8_t align[3];
} dirent_t;

struct file {
    enum { FD_NONE, FD_INODE, FD_DEVICE } type;
    int ref; // reference count
    char readable;
    char writable;
    struct inode *ip;  // FD_INODE and FD_DEVICE
    uint8_t off;          // FD_INODE
    uint16_t last_access_time_ms;
};


/* ======================================================================== */
/* Filesystem layout                                                        */
/* ======================================================================== */

#define inodeBitMapAddr   (BSIZE)
#define dataBitMapAddr    (2*BSIZE)

#define inodeBlkNum       (3)
#define dataBlkNum        (FS_TOTAL_SIZE - inodeBlkNum - 1)



#define BITS_PER_BITMAP(T)  (T) (sizeof(T)*8)
#define BITS_PER_INODE_BITMAP (udiv4(inodeBlkNum*BSIZE, sizeof(inode_t)))
#define BITS_PER_DATA_BITMAP  (dataBlkNum)

#define inodeStartAddr    (1*BSIZE)
#define dataStartAddr     (inodeStartAddr + inodeBlkNum*BSIZE)

#define blk(inum)         (udiv4(inum*sizeof(inode_t), BSIZE))
// #define sector(blk)       ((blk*BSIZE + inodeStartAddr)/SECSIZE)

#define MAXFILES          (BITS_PER_INODE_BITMAP)
#define MAX_BLOCKS_PER_FILE 1

#define inum(off)         (udiv4(off - inodeStartAddr,sizeof(inode_t)))
#define inode_off(inum)   (inum*sizeof(inode_t))

#define BLOCK_NUM(addr)   (udiv4(addr,BSIZE))
#define BLOCK_ADDR(b_num) (BSIZE*(uintptr_t)b_num)


#endif // NANOFS_LAYOUT_H
