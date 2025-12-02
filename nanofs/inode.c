#include "types.h"
#include "math.h"
#include "console.h"
#include "string.h"

#include <nanofs/nanofs.h>

#define min(a, b) ((a < b) ? a : b)

/* ======================================================================== */
/* Support of inode operations                                              */
/* ======================================================================== */

uint8_t fs_low_level_storage
fs_inode_alloc()
{
    struct inmem_block *inmem_block;
    uint8_t *ibm;
    uint8_t inode_num = 0;

    inmem_block = inmem_read_block(sb->bmap_start_block);
    if (inmem_block == 0)
        return 0;

    ibm = (uint8_t *)(inmem_block->inmem_addr + sb->ibmap_start_byte);

    while (inode_num < BITS_PER_INODE_BITMAP) {
        if (*ibm & (1U << inode_num)) {
            inode_num++;
            continue;
        }
        *ibm |= (1U << inode_num);
        inmem_block_free(inmem_block, 1);
        return inode_num;
    }

    inmem_block_free(inmem_block, 0);
    return -1;
}

void fs_low_level_storage
fs_inode_free(uint8_t inum)
{
    struct inmem_block *inmem_block;
    uint8_t *ibm;

    inmem_block = inmem_read_block(sb->bmap_start_block);
    if (inmem_block == 0)
        return;

    ibm = (uint8_t *)(inmem_block->inmem_addr + sb->ibmap_start_byte);

    *ibm &= ~(1U << inum);
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
fs_get_inode_by_inum(uint8_t inum)
{
    struct inmem_block *inmem_block;
    inode_t *ip0 = inmem_lock_inode(), *ip1;

    if (inum >= MAXFILES)
        return 0;
    
    inmem_block = inmem_read_block(BLOCK_NUM(inodeStartAddr));
    if (inmem_block == 0)
        return 0;

    ip1 = (inode_t *)(inmem_block->inmem_addr + inum*sizeof(inode_t));
    *ip0 = *ip1;

    inmem_block_free(inmem_block, 0);

    return ip0;
}

int fs_low_level_storage
fs_store_inode_by_inum(inode_t *ip, uint8_t inum)
{
    struct inmem_block *inmem_block;
    inode_t *on_dev_ip;

    if (inum >= MAXFILES)
        return -1;

    inmem_block = inmem_read_block(sb->iblocks_start);
    if (inmem_block == 0)
        return -1;

    on_dev_ip = (inode_t *)(inmem_block->inmem_addr + inode_off(inum));
    *on_dev_ip = *ip;

    inmem_block_free(inmem_block, 1);

    return 0;
}

inode_t * fs_low_level_storage fs_find_inode_in_dir(inode_t *ip_parent, char *fname)
{
    struct inmem_block *inmem_block;
    inode_t *ip_child;
    char *current_block;
    dirent_t *curr_dirent;
    uint8_t curr_blk_num;

    if (ip_parent->type != TYPE_DIR)
        return 0;

    if (ip_parent->b_addr == 0)
        return 0;

    curr_blk_num = BLOCK_NUM(ip_parent->b_addr);

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

    return 0;
}

int fs_low_level_storage
fs_add_inode_in_dir(inode_t *ip_parent, char *fname, uint8_t inum_child)
{
    struct inmem_block *inmem_block;
    char *current_block;
    dirent_t * curr_dirent;
    uint8_t curr_blk_num;

    if (ip_parent->type != TYPE_DIR)
        return 1;

    if (ip_parent->b_addr == 0)
        return 0;

    curr_blk_num = BLOCK_NUM(ip_parent->b_addr);

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

inode_t *fs_create_inode(char *path, uint8_t type)
{
    char current_name[MAXFNAME];
    uint8_t inum_child;
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
    if (ip->b_addr == 0) {
        if (ip->b_size != 0)
            return 1;
        return 0;
    }
    fs_block_free(ip->b_addr);
    ip->b_addr = 0;
    ip->size = 0;
    return 0;
}

int fs_low_level_storage fs_write_to_inode(inode_t *ip, uint8_t off, 
                                            uint8_t size, char *buf)
{
    struct inmem_block *inmem_block;
    char *current_block;
    uint8_t in_block_off = umod4(off, BSIZE);
    int bytes_written = 0;

    if (off > ip->size || off + size < off)
        return -1;
    
    if (off + size > MAX_BLOCKS_PER_FILE * BSIZE)
        return -1;

    // may be need to allocate new block?
    if (ip->b_addr == 0)
        return -1;

    inmem_block = inmem_read_block(BLOCK_NUM(ip->b_addr));
    if (inmem_block == 0)
        return 0;

    current_block = inmem_block->inmem_addr;

    memmove((char *)(current_block + in_block_off), buf, size);
    inmem_block_free(inmem_block, 1);

    bytes_written += size;

    return bytes_written;
}

int fs_low_level_storage fs_read_from_inode(inode_t *ip, uint8_t off, 
                            uint8_t size, char * buf)
{
    struct inmem_block *inmem_block;
    char *current_block;
    uint8_t in_block_off = umod4(off, BSIZE);
    int bytes_read = 0;

    if (off > ip->size || off + size < off)
        return 0;
    
    if (off + size > ip->size) {
        size = ip->size - off;
    }

    if ((BLOCK_NUM(ip->b_addr)) == 0) {
        return 0;
    }

    uint8_t req_to_read_here = size;

    inmem_block = inmem_read_block(BLOCK_NUM(ip->b_addr));
    if (inmem_block == 0)
        return 0;

    current_block = inmem_block->inmem_addr;
    memmove(buf,(char *)(current_block + in_block_off), req_to_read_here);

    inmem_block_free(inmem_block, 0);

    bytes_read += req_to_read_here;

    return bytes_read;
}

superblock_t *inode_init_sb(uint8_t sb_num)
{
    struct inmem_block *inmem_sb;

    inmem_sb = inmem_read_block(sb_num);
    if (inmem_sb == 0) {
        printf("Failed to read superblock");
        return (superblock_t *)1;
    }

    return (superblock_t *)inmem_sb->inmem_addr;
}

int inode_init_root(uint8_t root_inum)
{
    current_dir = fs_get_inode_by_inum(root_inum);
    if (current_dir == 0)
        return 1;
    return 0;
}
