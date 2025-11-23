#include "m328Pdef.h"
#include "types.h"
#include "math.h"
#include "console.h"
#include "string.h"
#include "traps.h"
#include "time.h"

#include <drivers/storage/storage_dev.h>
#include <drivers/storage/eeprom.h>

/* ======================================================================== */
/* Support of low level filesystem operations with eeprom memory            */
/* ======================================================================== */

static void eeprom_load_byte(char *dst, const char *src)
{
    uint16_t addr = (uint16_t)src;

    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE))
        ;

    /* Set up address register */
    EEARL = MAKE_EEARL(addr);
    EEARH = MAKE_EEARH(addr);

    /* Start eeprom read by writing EERE */
    EECR |= (1<<EERE);

    /* Read data from Data Register */
    *dst = EEDR;
}

static void eeprom_store_byte(char *dst, const char *src)
{
    uint16_t addr = (uint16_t)dst;

    traps_disable();

    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE))
        ;

    /* Set up address register */
    EEARL = MAKE_EEARL(addr);
    EEARH = MAKE_EEARH(addr);

    /* Set up data register */
    EEDR = *src;

    /* Fast path */
    /* Write logical one to EEMPE */
    /* Start eeprom write by setting EEPE */
    asm volatile (
        "sbi %0, %1\n\t"
        "sbi %0, %2\n\t"
        ::"I"(IO_ADDR(EECR)),"I"(EEMPE), "I"(EEPE)
    );

    traps_enable();
}

void eeprom_load(void *dst, const void *src, uint32_t size)
{
    for (int i = 0; i < size; i++) {
        eeprom_load_byte((char *)dst + i, (char *)src + i);
    }
}

void eeprom_store(void *dst, const void *src, uint32_t size)
{
    for (int i = 0; i < size; i++) {
        eeprom_store_byte((char *)dst + i, (char *)src + i);
    }
}

void eeprom_init()
{
}

STORAGE_DEV(eeprom_init, eeprom_load, eeprom_store);
