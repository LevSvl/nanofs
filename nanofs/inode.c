#include "types.h"
#include "math.h"
#include "console.h"
#include "string.h"

#include <nanofs/nanofs.h>

#define min(a, b) ((a < b) ? a : b)

/* ======================================================================== */
/* Support of inode operations                                              */
/* ======================================================================== */

int fs_low_level_storage
fs_inode_alloc()
{
    struct inmem_block *inmem_block;
    uint32_t *ibm0, *ibm;
    uint32_t max_off = dataBitMapAddr - inodeBitMapAddr;
    int inode_num = 0;

    inmem_block = inmem_read_block(sb->ibmap_start);
    if (inmem_block == 0)
        return 0;

    ibm0 = (uint32_t *)inmem_block->inmem_addr;

    for (ibm = ibm0;
        (uint8_t *)ibm - (uint8_t *)ibm0 < max_off;
         ibm++)  
        {
            int i = 0;
            for (i = 0; i < BITS_PER_BITMAP(uint32_t); i++) {
                if (*ibm & (1U << i))
                    continue;
                *ibm |= (1U << i);
                inmem_block_free(inmem_block, 1);
                inode_num += i;
                return inode_num;
        }
        inode_num += i;
    }

    inmem_block_free(inmem_block, 0);
    return -1;
}

void fs_low_level_storage
fs_inode_free(int inum)
{
    struct inmem_block *inmem_block;
    uint32_t *ibm;

    inmem_block = inmem_read_block(sb->ibmap_start);
    if (inmem_block == 0)
        return;

    ibm = (uint32_t *)inmem_block->inmem_addr + \
        (inum / BITS_PER_BITMAP(uint32_t));

    *ibm &= ~(1U << (inum % BITS_PER_BITMAP(uint32_t)));
    inmem_block_free(inmem_block, 1);
}

char * fs_strtok_by_sep(char * path, char * name)
{
    char *name_curr = name;

    if (*path == '/')
        path++;

    while (*path && *path != '/') {
        *name_curr = *path;
        name_curr++;
        path++;
    }
    *name_curr = 0;

    if (*path == 0)
        return 0;

    return path;
}

inode_t * fs_low_level_storage fs_low_level_inode
fs_get_inode_by_inum(int inum)
{
    struct inmem_block *inmem_block;
    inode_t *ip0 = inmem_lock_inode(), *ip1;
    char *inode_block;

    if (inum >= MAXFILES)
        return 0;
    
    inmem_block = inmem_read_block(udiv4(inodeStartAddr, BSIZE)+ blk(inum));
    if (inmem_block == 0)
        return 0;
    
    inode_block = inmem_block->inmem_addr;

    ip1 = (inode_t *)(inode_block + umod4((inum*sizeof(inode_t)), BSIZE));
    *ip0 = *ip1;

    inmem_block_free(inmem_block, 0);

    return ip0;
}

int fs_low_level_storage
fs_store_inode_by_inum(inode_t *ip, int inum)
{
    struct inmem_block *inmem_block;
    inode_t *on_dev_ip;
    char *inode_block;


    if (inum >= MAXFILES)
        return -1;

    inmem_block = inmem_read_block((inodeStartAddr / BSIZE + blk(inum)));
    if (inmem_block == 0)
        return -1;
    
    inode_block = inmem_block->inmem_addr;

    on_dev_ip = (inode_t *)(inode_block + inode_off(inum));
    *on_dev_ip = *ip;

    inmem_block_free(inmem_block, 1);

    return 0;
}

inode_t * fs_low_level_storage fs_find_inode_in_dir(inode_t *ip_parent, char *fname)
{
    struct inmem_block *inmem_block;
    inode_t *ip_child;


    if (ip_parent->type != TYPE_DIR)
        return 0;
    
    for (int i = 0; i < NDIRECT; i++) {
        char *current_block;
        dirent_t * curr_dirent;
        uint32_t curr_blk_num;

        if (ip_parent->addr[i] == 0)
            continue;

        curr_blk_num = BLOCK_NUM(ip_parent->addr[i]);

        inmem_block = inmem_read_block(curr_blk_num);
        if (inmem_block == 0)
            return 0;

        current_block = inmem_block->inmem_addr;

        curr_dirent = (dirent_t *)current_block;

        while ((uintptr_t)curr_dirent - (uintptr_t)current_block < BSIZE) {
            if (strncmp(curr_dirent->name, fname, strlen(fname)) == 0) {
                ip_child = fs_get_inode_by_inum(curr_dirent->inum);
                inmem_block_free(inmem_block, 0);
                return ip_child;
            }
            curr_dirent++;
        }

        inmem_block_free(inmem_block, 0);
    }

    return 0;
}

int fs_low_level_storage
fs_add_inode_in_dir(inode_t *ip_parent, char *fname, int inum_child)
{
    struct inmem_block *inmem_block;

    if (ip_parent->type != TYPE_DIR)
        return 1;
    
    for (int i = 0; i < NDIRECT; i++) {
        char *current_block;
        dirent_t * curr_dirent;
        uint32_t curr_blk_num;

        if (ip_parent->addr[i] == 0)
            continue;

        curr_blk_num = BLOCK_NUM(ip_parent->addr[i]);

        inmem_block = inmem_read_block(curr_blk_num);
        if (inmem_block == 0)
            return 1;

        current_block = inmem_block->inmem_addr;

        curr_dirent = (dirent_t *)current_block;

        while ((uintptr_t)curr_dirent - (uintptr_t)current_block < BSIZE) {
            if (curr_dirent->strlen == 0) {
                /* Found free dirent, place here */
                curr_dirent->inum = inum_child;
                curr_dirent->strlen = strlen(fname);
                curr_dirent->reclen = sizeof(dirent_t);

                memmove(curr_dirent->name, fname, strlen(fname));
                curr_dirent->name[strlen(fname)] = 0;

                inmem_block_free(inmem_block, 1);
                return 0;
            }
            curr_dirent++;
        }

        inmem_block_free(inmem_block, 0);
    }

    return 1;
}

inode_t * fs_low_level_inode fs_get_inode(char *path)
{
    char current_name[MAXFNAME];
    inode_t *ip_parent, *ip_child;

    if (*path == '/') {
        ip_parent = fs_get_inode_by_inum(ROOTINO);
        path++;
    } else if (*path == 0) {
        return (inode_t *)0;
    } else {
        ip_parent = current_dir;
    }

    while ((path = fs_strtok_by_sep(path, current_name)) != 0) {
        printf("%s\n", path);
        // ip = fs_find_inode_in_dir(ip, current_name);
    }

    ip_child = fs_find_inode_in_dir(ip_parent, current_name);
    /* 
     * Since ip_parent won't be used anymore, 
     * we need to free inmem inode that was 
     * locked by fs_get_inode_by_inum
     */
    inmem_free_inode(ip_parent);

    return ip_child;
}

inode_t *fs_create_inode(char *path, int type)
{
    char current_name[MAXFNAME];
    int inum_child;
    inode_t *ip_parent, *ip_child;

    if (*path == '/') {
        ip_parent = fs_get_inode_by_inum(ROOTINO);
        path++;
    } else if (*path == 0) {
        return (inode_t *)0;
    } else {
        ip_parent = current_dir;
    }

    /* Allocate inode on storage device first */
    inum_child = fs_inode_alloc();
    if (inum_child < 0)
        return (inode_t *)0;

    /* Create inmemory inode */
    ip_child = inmem_lock_inode();
    if (ip_child == 0) {
        fs_inode_free(inum_child);
        inmem_free_inode(ip_child);
        inmem_free_inode(ip_parent);
        return (inode_t *)0;
    }
    ip_child->size = 0;
    ip_child->type = type;
    ip_child->nlink = 1;

    while ((path = fs_strtok_by_sep(path, current_name)) != 0) {
        printf("%s\n", path);
        // ip = fs_find_inode_in_dir(ip, current_name);
    }

    /* Place inode in parent's dirent */
    if (fs_add_inode_in_dir(ip_parent, current_name, inum_child)) {
        fs_inode_free(inum_child);
        inmem_free_inode(ip_child);
        inmem_free_inode(ip_parent);
        return (inode_t *)0;
    }

    /* Finally place inode store new inode on storage device */
    fs_store_inode_by_inum(ip_child, inum_child);

    /* 
     * Since ip_parent won't be used anymore, 
     * we need to free inmem inode that was 
     * locked by fs_get_inode_by_inum
     */
    inmem_free_inode(ip_parent);

    return ip_child;
}

int fs_trunc_inode(inode_t *ip)
{
    for (int i = 0; i < NDIRECT; i++) {
        if (ip->addr[i] == 0)
            continue;
        fs_block_free(ip->addr[i]);
        ip->addr[i] = 0;
        ip->size = 0;
    }
    return 0;
}

int fs_low_level_storage fs_write_to_inode(inode_t *ip, uint32_t off, 
                            uint32_t size, char * buf)
{
    struct inmem_block *inmem_block;
    int bytes_written = 0;

    if (off > ip->size || off + size < off)
        return -1;
    
    if (off + size > MAX_BLOCKS_PER_FILE * BSIZE) {
        return -1;
    }

    while (bytes_written < size) {
        char *current_block;
        uint32_t current_block_num = BLOCK_NUM(off);
        uint32_t in_block_off = off % BSIZE;
        uint32_t req_to_write_here = min(size - bytes_written, BSIZE - in_block_off);

        if (ip->addr[current_block_num] == 0) {
            break;
        }

        inmem_block = inmem_read_block(BLOCK_NUM(ip->addr[current_block_num]));
        if (inmem_block == 0)
            return 0;

        current_block = inmem_block->inmem_addr;

        memmove((char *)(current_block + in_block_off), buf, req_to_write_here);

        inmem_block_free(inmem_block, 1);

        bytes_written += req_to_write_here;
        off += req_to_write_here;
    }

    return bytes_written;
}

int fs_low_level_storage fs_read_from_inode(inode_t *ip, uint32_t off, 
                            uint32_t size, char * buf)
{
    struct inmem_block *inmem_block;
    int bytes_read = 0;

    if (off > ip->size || off + size < off)
        return 0;
    
    if (off + size > ip->size) {
        size = ip->size - off;
    }

    while (bytes_read < size) {
        char *current_block;
        uint32_t current_block_num = BLOCK_NUM(off);
        uint32_t in_block_off = off % BSIZE;
        uint32_t req_to_read_here = min(size - bytes_read, BSIZE - in_block_off);

        if ((BLOCK_NUM(ip->addr[current_block_num])) == 0) {
            break;
        }

        inmem_block = inmem_read_block(BLOCK_NUM(ip->addr[current_block_num]));
        if (inmem_block == 0)
            return 0;

        current_block = inmem_block->inmem_addr;

        memmove(buf,(char *)(current_block + in_block_off), req_to_read_here);

        inmem_block_free(inmem_block, 0);

        bytes_read += req_to_read_here;
        off += req_to_read_here;
    }

    return bytes_read;
}

superblock_t *inode_init_sb(int sb_num)
{
    struct inmem_block *inmem_sb;

    inmem_sb = inmem_read_block(sb_num);
    if (inmem_sb == 0) {
        printf("Failed to read superblock");
        return (superblock_t *)1;
    }

    return (superblock_t *)inmem_sb->inmem_addr;
}

int inode_init_root(int root_inum)
{
    current_dir = fs_get_inode_by_inum(root_inum);
    if (current_dir == 0)
        return 1;
    return 0;
}
