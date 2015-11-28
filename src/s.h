#ifndef INC_S_H
#define INC_S_H

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short fixang;
typedef unsigned short ushort;
typedef int bool;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef long int32;

#define FALSE 0
#define TRUE  1

// convert from 64-bit IEEE little-endian to 32-bit int, faster
// than built-in Intel ops, but doesn't obey rounding rule and
// doesn't deal well with overflow

extern double chop_temp;
#define FLOAT_TO_INT(x)  ((chop_temp = (x) + BIG_NUM), *(int*)(&chop_temp))
#define FLOAT_TO_FIX(x)  \
             ((chop_temp = (x) + BIG_NUM/65536.0), *(int*)(&chop_temp))

#define BIG_NUM     ((float) (1 << 26) * (1 << 26) * 1.5)

extern void fatal_error(char *message, char *file, int line);
#define fatal(s)   fatal_error(s, __FILE__, __LINE__)

#endif
