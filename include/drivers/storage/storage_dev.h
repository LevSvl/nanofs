#ifndef DRIVERS_STORAGE_STORAGE_DEV
#define DRIVERS_STORAGE_STORAGE_DEV

#include "types.h"


/* ======================================================================== */
/* Storage device interface                                                 */
/* ======================================================================== */

struct storage_dev
{
    void (*fn_init)();
    void (*fn_write) (void *, const void *, uint32_t);
    void (*fn_read) (void *, const void *, uint32_t);
};

/*
 * Driver for storage device must initialize storage_dev
 * object with its own read and write methods
 */
extern struct storage_dev storage_dev;

#define STORAGE_DEV(i, r, w) \
    struct storage_dev storage_dev = {.fn_init =i, .fn_read = r, .fn_write = w};

#endif // DRIVERS_STORAGE_STORAGE_DEV
