/* QMAP: Quake level viewer
 *
 *   mode.c    Copyright 1997 Sean Barett
 *
 *   General screen functions (set graphics
 *   mode, blit to framebuffer, set palette)
 */

#if 0
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "s.h"
#include "mode.h"

#if 0
char *framebuffer = (char *) 0xA0000;
#endif

void blit(char *src)
{
#if 0
   memcpy(framebuffer, src, 320*200);
#endif
}

void set_pal(char *pal)
{
#if 0
   int i;
   outp(0x3c8, 0);
   for (i=0; i < 768; ++i)
      outp(0x3c9, pal[i] >> 2);
#endif
}

#if 0
#pragma aux set_mode = \
    "  int  10h" \
    parm [eax];
#endif

void set_mode(int mode);

bool graphics=0;
void set_lores(void)
{
#if 0
   graphics = 1;
   set_mode(0x13);
#endif
}
   
void set_text(void)
{
#if 0
   if (graphics) {
      set_mode(0x3);
      graphics = 0;
   }
#endif
}

void fatal_error(char *message, char *file, int line)
{
   set_text();
   printf("Error (%s line %d): %s\n", file, line, message);
   exit(1);
}
