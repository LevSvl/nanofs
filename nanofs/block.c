#include "types.h"
#include "math.h"
#include "console.h"
#include "string.h"

#include <nanofs/nanofs.h>
#include <drivers/storage/storage_dev.h>


#define PAGE_MASK   (0x1FC0)
#define WORD_MASK   (0x3F)

#define WORD_SIZE   (sizeof(uint16_t))

#define PAGE(addr) (addr & PAGE_MASK)
#define WORD(addr) (addr & WORD_MASK)
#define MAKE_FLASH_PTR(addr) ((PAGE(addr) | WORD(addr)) << 1)

/* ======================================================================== */
/* Support of low level filesystem operations with storage device           */
/* ======================================================================== */


uint32_t fs_low_level_storage
fs_block_alloc()
{
    struct inmem_block *inmem_block;
    uint32_t *dbm0, *dbm;
    uint32_t max_off = inodeStartAddr - dataBitMapAddr;
    int data_block_num = 0;

    inmem_block = inmem_read_block(sb->dbmap_start);
    if (inmem_block == 0)
        return 0;

    dbm0 = (uint32_t *)inmem_block->inmem_addr;

    for (dbm = dbm0;
        (uint8_t *)dbm - (uint8_t *)dbm0 < max_off;
         dbm++)  
        {
            int i = 0;
            for (i = 0; i < BITS_PER_BITMAP(uint32_t); i++) {
                if (*dbm & (1U << i))
                    continue;
                *dbm |= (1U << i);
                inmem_block_free(inmem_block, 1);
                return dataStartAddr + data_block_num + i*BSIZE;
        }
        data_block_num += i;
    }

    inmem_block_free(inmem_block, 0);
    return 0;
}

void fs_low_level_storage
fs_block_free(uint32_t dev_block_addr)
{
    struct inmem_block *inmem_block;
    uint32_t *dbm;
    int data_block_num;

    inmem_block = inmem_read_block(sb->dbmap_start);
    if (inmem_block == 0)
        return;
    
    data_block_num = BLOCK_NUM(dev_block_addr - dataStartAddr);
    dbm = (uint32_t *)inmem_block->inmem_addr + \
        (data_block_num / BITS_PER_BITMAP(uint32_t));

    *dbm &= ~(1U << (data_block_num % BITS_PER_BITMAP(uint32_t)));
    inmem_block_free(inmem_block, 1);
}

void block_init(void)
{
    fs_base = (uintptr_t)0;
}

void block_read(char *dst, int block_num)
{
    storage_dev.fn_read(dst, (void *)BLOCK_ADDR(block_num), BSIZE);
}

void block_write(char *src, int block_num)
{
    storage_dev.fn_write((void *)BLOCK_ADDR(block_num), src, BSIZE);
}
