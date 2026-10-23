#include "types.h"
#include "console.h"
#include "time.h"
#include "m328Pdef.h"
#include "traps.h"

#include <nanofs/nanofs.h>

#include <drivers/storage/storage_dev.h>
#include <drivers/storage/eeprom.h>

int main()
{
    console_init();
    traps_enable();
    timer_init();

    storage_dev.fn_init();
    printk("storage dev init done\n");

    fs_init();
    printk("nanofs init init done\n");

    printk("starting shell\n");
    run_shell();

    traps_disable();
    while (1) {} ;
    
    return 0;
}
