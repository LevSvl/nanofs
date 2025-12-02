#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define udiv4(x, y) (x / y)
#define umod4(x, y) (x % y)

typedef uint16_t _daddr_t;

#include "layout.h"

uintptr_t img_base;


/* ======================================================================== */
/* Filesystem creation helpers                                              */
/* ======================================================================== */

superblock_t sb;
char tmp_buf[BSIZE];

int img_open()
{
    int img_fd;
    unsigned long bytes_written = 0;
    uint64_t null = 0;
    
    img_fd = open(TARGET_IMAGE_NAME, O_CREAT | O_RDWR, S_IRWXU);
    if (img_fd < 0)
        return img_fd;

    while (bytes_written < FS_TOTAL_SIZE * FS_BLOCK_SIZE) {
        unsigned long writen_curent;

        writen_curent = write(img_fd, &null, sizeof(null));
        if (writen_curent == 0)
            goto mmap_failed;
        bytes_written += writen_curent;
    }

    if ((img_base = (uintptr_t)mmap(NULL, FS_TOTAL_SIZE * FS_BLOCK_SIZE,
         PROT_READ | PROT_WRITE, MAP_SHARED, img_fd, 0)) == (uintptr_t)MAP_FAILED) {
        goto mmap_failed;
    }

    return img_fd;

mmap_failed:
    close(img_fd);
    return -1;
}

void img_close(int fd)
{
    munmap((void *)img_base, FS_TOTAL_SIZE * FS_BLOCK_SIZE);
    close(fd);
}

void superblock_update()
{
    memcpy((void *)img_base, &sb, sizeof(superblock_t));
}

void superblock_create()
{
    sb.magic = FS_MAGIC;
    sb.total_blocks_num = FS_TOTAL_SIZE;
    sb.iblocks_num = inodeBlkNum;
    sb.dblocks_num = dataBlkNum;
    sb.ibmap = 0;
    sb.dbmap = 0;

    assert(sizeof(sb) <= BSIZE);

    superblock_update();
}

_daddr_t ialloc()
{
    uint16_t *ibit_map = &sb.ibmap;

    for (int i = 0; i < BITS_PER_INODE_BITMAP; i++) {
        if (*ibit_map & (1 << i))
            continue;
        *ibit_map |= (1 << i);
        superblock_update();
        return inodeStartAddr+ (_daddr_t)i*sizeof(inode_t);
    }

    return 0; 
}

_daddr_t balloc()
{
    uint16_t *dbit_map = &sb.dbmap;

    for (int i = 0; i < BITS_PER_DATA_BITMAP; i++) {
        if (*dbit_map & (1U << i))
            continue;
        *dbit_map |= (1U << i);
        superblock_update();
        return dataStartAddr + (_daddr_t)i*BSIZE;
    }

    return 0;
}

void rootdir_create()
{
    inode_t inode_root;
    _daddr_t inode_addr_root;

    inode_root.size = 0;
    inode_root.type = TYPE_DIR;

    inode_root.b_addr = balloc();
    assert(inode_root.b_addr > 0);

    sb.ibmap |= (1 << ROOTINO);
    superblock_update();

    inode_addr_root = inodeStartAddr + ROOTINO*sizeof(inode_t);

    memcpy((void *)IMG(img_base, inode_addr_root), &inode_root, sizeof(inode_t));
}

void add_dirent(int parent_inum, dirent_t *dirent)
{
    uint32_t dirent_off;
    inode_t *inode_parent;

    inode_parent = (inode_t*)IMG(img_base, inodeStartAddr + parent_inum*sizeof(inode_t));
    assert(inode_parent->type != TYPE_FILE);

    dirent_off = inode_parent->b_addr;
    assert(dirent_off > 0);

    while(((dirent_t *)IMG(img_base, dirent_off))->strlen)
        dirent_off += sizeof(dirent_t);
  
    memcpy((void *)IMG(img_base, dirent_off), dirent, sizeof(dirent_t));
}

void print_litle_fs_config()
{
    printf("                        \n");
    printf("                        \n");
    printf("                        \n");
    printf("nanofs               \n");
    printf("Block size: %d bytes    \n", FS_BLOCK_SIZE);
    printf("Total size: %d blocks ( %d bytes)   \n", 
        sb.total_blocks_num, sb.total_blocks_num*BSIZE);
    printf("Data: %d blocks         \n", sb.dblocks_num);
    printf("Data start block: %d    \n", dataStartAddr / BSIZE);
    printf("Data bitmap byte: 0x%02x  \n", sb.dbmap);
    printf("Inodes: %d blocks       \n", sb.iblocks_num);
    printf("Inodes bitmap byte: 0x%02x  \n", sb.ibmap);
    printf("Magic: 0x%8x            \n", sb.magic);
    printf("                        \n");
    printf("                        \n");
    printf("                        \n");
    printf("Size of superblock: %ld bytes \n", sizeof(struct superblock));
    printf("Size of inode: %ld bytes \n", sizeof(inode_t));
    printf("Size of dirent: %ld bytes\n", sizeof(dirent_t));
    printf("Size of file: %ld bytes  \n", sizeof(struct file));
    printf("                        \n");
    printf("                        \n");
    printf("                        \n");
}

int main(int argc, char const *argv[])
{
    int little_fs_img_fd;

    if (argc < 2) {
        printf("No files provided\n");
        return 1;
    }
    
    little_fs_img_fd = img_open();
    assert(little_fs_img_fd > 0);

    superblock_create();
    rootdir_create();

    for (int file_cnt = 1; file_cnt < argc; file_cnt++) {
        int file_fd;
        int file_size = 0, blocks_req;
        inode_t inode;
        _daddr_t inode_off;
        dirent_t dirent;
        const char *file_name = argv[file_cnt];
        
        if (file_cnt > MAXFILES)
            break;
        
        file_fd = open(file_name, O_RDONLY);
        assert(file_fd > 0);


        /* Only count file size */
        for (int bytes_read = 0x1; bytes_read > 0;) {
            bytes_read = read(file_fd, tmp_buf, BSIZE);
            file_size += bytes_read;
        }
        printf("%s size: %d bytes \n", file_name, file_size);
        assert(file_size < MAX_BLOCKS_PER_FILE*BSIZE);
        /* Count blocks required */
        blocks_req = file_size / BSIZE;
        if (blocks_req == 0 && file_size > 0)
            blocks_req = 1;


        /* Create inode for file */
        inode_off = ialloc();
        assert(inode_off > 0);

        inode.size = file_size;
        inode.type = TYPE_FILE;

        for (int i = 0; i < MAX_BLOCKS_PER_FILE; i++) {
            if (i < blocks_req) {
                inode.b_addr = balloc();
                assert(inode.b_addr != 0);
            }
            else {
                inode.b_addr = 0;
            }
        }

        /* Place inode for file */
        memcpy((void *)IMG(img_base, inode_off), &inode, sizeof(inode_t));

        /* Place data for file */
        uint16_t i, bytes_left;
        for (i = 0, bytes_left = file_size; i < blocks_req; i++) {
            if (inode.b_addr == 0)
                continue;

            memset(tmp_buf, 0, BSIZE);
            lseek(file_fd, 0, SEEK_SET);
            bytes_left -= read(file_fd, tmp_buf, BSIZE);
            memcpy((void *)IMG(img_base, inode.b_addr), tmp_buf, BSIZE);
        }
        assert(bytes_left == 0);

        /* Add file into the root directory */
        dirent.inum = inum(inode_off);
        dirent.strlen = strlen(file_name);
        dirent.reclen = sizeof(dirent_t);
        memcpy(&dirent.name, file_name, strlen(file_name));
        dirent.name[strlen(file_name)] = 0;
        add_dirent(ROOTINO, &dirent);
    }

    img_close(little_fs_img_fd);
    print_litle_fs_config();
    
    return 0;
}
