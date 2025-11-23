#ifndef NANOFS_BLOCK_H
#define NANOFS_BLOCK_H

#include <nanofs/info.h>

/* ======================================================================== */
/* Block level interface                                                    */
/* ======================================================================== */

void fs_low_level_storage fs_block_free(uint32_t dev_block_addr);

uint32_t fs_low_level_storage fs_block_alloc();

void fs_low_level_storage block_init(void);

void block_read(char *dst, int block_num);

void block_write(char *src, int block_num);


#endif // NANOFS_BLOCK_H
