#ifndef BITOPS_H
#define BITOPS_H

#define LOW16(data) ((data & 0xFF))
#define HIGH16(data) ((data >> 8) & 0xFF)

#endif // BITOPS_H
