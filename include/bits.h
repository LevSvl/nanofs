/* Bit operations */
#ifndef BITS_H
#define BITS_H

#define BITMASK_MAX                    0xFFFFFFFFFFFFFFFFULL
#define BITOFS_MAX                     63

#define BIT(x)                         (0x1UL << (x))
#define BITS(msb, lsb)                 (((BITMASK_MAX) >> ((BITOFS_MAX) - (msb))) & ((BITMASK_MAX) << (lsb)))
#define BITS_GET(val, msb, lsb)        (((val) & BITS((msb),(lsb))) >> (lsb))
#define BITS_PUT(val, msb, lsb)        ((((val) & (BITMASK_MAX)) << (lsb)) & BITS((msb),(lsb)))

#endif // BITS_H
