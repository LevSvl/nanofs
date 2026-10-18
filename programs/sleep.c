#include "types.h"
#include "console.h"
#include "time.h"
#include "string.h"
#include "m328Pdef.h"
#include "traps.h"

int atoi(char *str){
  int res = 0;

  while(*str >= '0' && *str <= '9')
    res = res*10 + *str++ - '0';
  return res;
}

int sleep_main(int argc, char *argv[])
{
    int sleep_ms;

    if(argc < 2)
        return 1;

    sleep_ms = atoi(argv[1]);

    printf("%d\n", sleep_ms);

    delay_ms(sleep_ms);

    return 0;
}
