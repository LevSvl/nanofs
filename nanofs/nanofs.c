#include "types.h"
#include "math.h"
#include "console.h"
#include "string.h"

#include <nanofs/nanofs.h>

extern uintptr_t nanofs_start[];
extern uintptr_t nanofs_end[];

uintptr_t fs_base;
inode_t *current_dir;
superblock_t * sb;

extern struct inmem_block inmem_blocks[INMEM_BLOCKS_NUM];
extern struct inmem_inode inmem_inodes[INMEM_INODES_NUM];

int opened_files_cnt = 0;
struct file opened_files[MAX_FILES_OPENED];


/* ======================================================================== */
/* Filesystem operations                                                    */
/* ======================================================================== */


int fs_open(char *path, int flags)
{
    int fd;
    struct file *f;
    inode_t *ip;

    if (opened_files_cnt == MAX_FILES_OPENED) {
        return -1;
    }


    if (flags & FS_O_CREATE) {
        ip = fs_create_inode(path, TYPE_FILE);
        if (ip == 0)
            return -1;
    } else {
        ip = fs_get_inode(path);
        if (ip == 0)
            return -1;

        if (ip->type == TYPE_DIR && flags != FS_O_RDONLY)
            return -1;

        if (flags & FS_O_TRUNC) {
            if (fs_trunc_inode(ip))
                return -1;
        }
    }

    fd = opened_files_cnt++;
    f = &opened_files[fd];
    f->type = FD_INODE;
    f->off = 0;
    f->ip = ip;
    f->ref = 1;
    f->readable = !(flags & FS_O_WRONLY);
    f->writable = (flags & FS_O_WRONLY) || (flags & FS_O_RDWR);

    return fd;
}

int fs_low_level_inode fs_close(int fd)
{
    struct file *f;

    if (fd > MAX_FILES_OPENED)
        return -1;

    f = &opened_files[fd];
    inmem_free_inode(f->ip);
    memset(f, 0, sizeof (struct file));
    opened_files_cnt -= 1;

    return 0;
}

int fs_read(int fd, void *buf, uint32_t size)
{
    struct file *f;
    int bytes_read = 0;

    if (fd < 0) {
        return -1;
    }

    f = &opened_files[fd];

    if (!f->readable) {
        return -1;
    }
    
    if (f->type == FD_INODE) {
        bytes_read = fs_read_from_inode(f->ip, f->off, 
                                    size, (char *)buf);
        f->off += bytes_read;
    }

    return bytes_read;
}

int fs_write(int fd, void *buf, uint32_t size)
{
    struct file *f;
    int bytes_written = 0;

    if (fd < 0) {
        return -1;
    }

    f = &opened_files[fd];

    if (!f->writable)
        return -1;

    if (f->type == FD_INODE) {
        bytes_written = fs_write_to_inode(f->ip, f->off, 
                                    size, (char *)buf);
        f->off += bytes_written;
    }

    return bytes_written;
}


/* ======================================================================== */
/* Filesystem initialization                                                */
/* ======================================================================== */

int fs_do_initial_checks();

int fs_init()
{
    block_init();
    inmem_init();
    sb = inode_init_sb(0);

    if ((sb->magic != FS_MAGIC) || inode_init_root(ROOTINO))
        return 1;

    // printf("                        \n");
    // printf("nanofs                  \n");
    // printf("Block size: %d bytes    \n", FS_BLOCK_SIZE);
    // printf("Total size: %d blocks ( %d bytes)   \n", 
    //     sb->total_blocks_num, sb->total_blocks_num*BSIZE);
    // printf("Data: %d blocks         \n", sb->dblocks_num);
    // printf("Data start block: %d    \n", dataStartAddr / BSIZE);
    // printf("Data bitmap start block: %d  \n", sb->dbmap_start);
    // printf("Inodes: %d blocks       \n", sb->iblocks_num);
    // printf("Inodes start block: %d  \n", sb->iblocks_start);
    // printf("Inodes bitmap start block: %d  \n", sb->ibmap_start);
    // printf("Magic: %p               \n", sb->magic);
    // printf("                        \n");
    // printf("                        \n");
    // printf("                        \n");
    // printf("Size of superblock: %d bytes \n", sizeof(struct superblock));
    // printf("Size of inode: %d bytes \n", sizeof(inode_t));
    // printf("Size of dirent: %d bytes\n", sizeof(dirent_t));
    // printf("Size of file: %d bytes  \n", sizeof(struct file));
    // printf("                        \n");
    // printf("                        \n");
    // printf("                        \n");

    return fs_do_initial_checks();
}

void fs_panic()
{
    printf("panic...\n");
    while (1);
}

// #define CONFIG_DEBUG_DATA
// #define README "/Readme"
// #define README_SIZE (16)

// #define info(fmt, ...) (printf("Info: "fmt"\n", ## __VA_ARGS__))
// #define warn(fmt, ...) (printf("Warning: "fmt"\n", ## __VA_ARGS__))
// #define err(fmt, ...) (printf("Error: "fmt"\n", ## __VA_ARGS__))

int fs_do_initial_checks()
{
    printf("done OK\n");

    return 0;
}
