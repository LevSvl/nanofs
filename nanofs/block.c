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


uint8_t fs_low_level_storage
fs_block_alloc()
{
    uint16_t *dbm;
    uint8_t data_block_num = 0;


    dbm = (uint16_t *)&sb->dbmap;

    while (data_block_num < BITS_PER_DATA_BITMAP) {
        if (*dbm & (1U << data_block_num)) {
            data_block_num++;
            continue;
        }
        *dbm |= (1U << data_block_num);
        write_sb();
        return data_block_num;
    }

    return 0;
}

void fs_low_level_storage
fs_block_free(uint16_t dev_block_addr)
{
    uint16_t *dbm;
    uint8_t data_block_num;
    
    data_block_num = BLOCK_NUM(dev_block_addr - dataStartAddr);
    dbm = &sb->dbmap;

    *dbm &= ~(1U << data_block_num);
    write_sb();
}

void block_init(void)
{
    fs_base = (uintptr_t)0;
}

void block_read(char *dst, uint8_t block_num)
{
    storage_dev.fn_read(dst, (void *)BLOCK_ADDR(block_num), BSIZE);
}

void block_write(char *src, uint8_t block_num)
{
    storage_dev.fn_write((void *)BLOCK_ADDR(block_num), src, BSIZE);
}
