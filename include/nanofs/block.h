#ifndef NANOFS_BLOCK_H
#define NANOFS_BLOCK_H

#include <nanofs/info.h>

/* ======================================================================== */
/* Block level interface                                                    */
/* ======================================================================== */

void fs_low_level_storage fs_block_free(uint16_t dev_block_addr);

uint8_t fs_low_level_storage fs_block_alloc();

void fs_low_level_storage block_init(void);

void block_read(char *dst, uint8_t block_num);

void block_write(char *src, uint8_t block_num);


#endif // NANOFS_BLOCK_H
