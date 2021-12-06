/* QMAP: Quake level viewer
 *
 *   mode.c    Copyright 1997 Sean Barett
 *
 *   General screen functions (set graphics
 *   mode, blit to framebuffer, set palette)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "s.h"
#include "mode.h"

void fatal_error(char *message, char *file, int line)
{
   printf("Error (%s line %d): %s\n", file, line, message);
   exit(1);
}
