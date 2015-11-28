/*  QMAP: Quake level viewer
 *
 *   main.c    Copyright 1997 Sean Barett
 *
 *   General setup control, main "sim" loop
 */

#if 0
#include <conio.h>
#include <dos.h>
#endif

#include "bspfile.h"
#include "mode.h"
#include "3d.h"
#include "fix.h"
#include "scr.h"

#if 0
#include "char.h"
#endif

#include "tm.h"
#include "render.h"
#include "bsp.h"
#include "surface.h"
#include "poly.h"

#include <SDL.h>

double chop_temp;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

vector cam_loc, cam_vel, new_loc;
angvec cam_ang, cam_angvel;

uchar palette[768];
uchar *sdl_buf;
char *scr_buf;
int   scr_row;

#define ANG_STEP  0x0080
#define VEL_STEP  0.5

char colormap[64][256];

void run_sim(void)
{
   SDL_Event ev;
   vector temp;
   int x, y, c;
   uchar *q;
   char *p;

   scr_buf = malloc(320*200);
   sdl_buf = malloc(320*200*4);
   scr_row = 320;
   qmap_set_output(scr_buf, scr_row);

   cam_loc.x = 500;
   cam_loc.y = 240;
   cam_loc.z = 100;

   for (;;) {

      // RENDER

      set_view_info(&cam_loc, &cam_ang);
      render_world(&cam_loc);
#if 0
      blit(scr_buf);
#endif
      p = scr_buf;
      q = sdl_buf;
      for (y = 0; y < 200; y++) {
          for (x = 0; x < 320; x++) {
              c = *p++;
              *q++ = palette[c*3];
              *q++ = palette[c*3 + 1];
              *q++ = palette[c*3 + 2];
              *q++ = 255;
          }
      }
      SDL_RenderClear(renderer);
      SDL_UpdateTexture(texture, NULL, sdl_buf, 320*4);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);

      // UI
      while (SDL_PollEvent(&ev)) {
         switch (ev.type) {
         case SDL_QUIT:
             return;
         case SDL_KEYDOWN:
             switch (ev.key.keysym.sym) {
             case SDLK_ESCAPE:
                 return;

             case SDLK_v: cam_angvel.tx += ANG_STEP; break;
             case SDLK_r: cam_angvel.tx -= ANG_STEP; break;
             case SDLK_q: cam_angvel.ty += ANG_STEP; break;
             case SDLK_e: cam_angvel.ty -= ANG_STEP; break;
             case SDLK_d: cam_angvel.tz += ANG_STEP; break;
             case SDLK_a: cam_angvel.tz -= ANG_STEP; break;

             case SDLK_c: cam_vel.x += VEL_STEP; break;
             case SDLK_z: cam_vel.x -= VEL_STEP; break;
             case SDLK_1: cam_vel.z -= VEL_STEP; break;
             case SDLK_3: cam_vel.z += VEL_STEP; break;
             case SDLK_x: cam_vel.y -= VEL_STEP; break;
             case SDLK_w: cam_vel.y += VEL_STEP; break;

             case SDLK_SPACE:
                cam_vel.x = cam_vel.y = cam_vel.z = 0;
                cam_angvel.tx = cam_angvel.ty = cam_angvel.tz = 0;
                break;
             }
         }
      }
#if 0
      while (kbhit()) {
         int c = getch();
         switch(c) {
            case 'Q': case 27:
               return;

            case 'v': cam_angvel.tx += ANG_STEP; break;
            case 'r': cam_angvel.tx -= ANG_STEP; break;
            case 'q': cam_angvel.ty += ANG_STEP; break;
            case 'e': cam_angvel.ty -= ANG_STEP; break;
            case 'd': cam_angvel.tz += ANG_STEP; break;
            case 'a': cam_angvel.tz -= ANG_STEP; break;

            case 'c': cam_vel.x += VEL_STEP; break;
            case 'z': cam_vel.x -= VEL_STEP; break;
            case '1': cam_vel.z -= VEL_STEP; break;
            case '3': cam_vel.z += VEL_STEP; break;
            case 'x': cam_vel.y -= VEL_STEP; break;
            case 'w': cam_vel.y += VEL_STEP; break;
               
            case ' ':
               cam_vel.x = cam_vel.y = cam_vel.z = 0;
               cam_angvel.tx = cam_angvel.ty = cam_angvel.tz = 0;
               break;
         }
      }
#endif
      // "PHYSICS"

      cam_ang.tx += cam_angvel.tx;
      cam_ang.ty += cam_angvel.ty;
      cam_ang.tz += cam_angvel.tz;
      set_view_info(&cam_loc, &cam_ang);

      temp.x = cam_vel.x; temp.y = 0; temp.z = 0;
      rotate_vec(&temp);
      cam_loc.x  += temp.x; cam_loc.y += temp.y; cam_loc.z += temp.z;

      temp.x = 0; temp.y = cam_vel.y; temp.z = 0;
      rotate_vec(&temp);
      cam_loc.x  += temp.x; cam_loc.y += temp.y; cam_loc.z += temp.z;

      temp.x = 0; temp.y = 0; temp.z = cam_vel.z;
      rotate_vec(&temp);
      cam_loc.x  += temp.x; cam_loc.y += temp.y; cam_loc.z += temp.z;

      SDL_Delay(1000/30);
   }
}

void load_graphics(void)
{
   char pal[768];
   FILE *f;
   if ((f = fopen("palette.lmp", "rb")) == NULL)
      fatal("Couldn't read palette.lmp\n");
   fread(pal, 1, 768, f);
   fclose(f);
   memcpy(palette, pal, 768);
   set_pal(pal); 
   if ((f = fopen("colormap.lmp", "rb")) == NULL)
      fatal("Couldn't read colormap.lmp\n");
   fread(colormap, 256, 64, f);
   fclose(f);
}

int main(int argc, char **argv)
{
   if (argc != 2) {
      printf("Usage: qmap <bspfile>\n");
   } else {
      LoadBSPFile(argv[1]);
      set_lores();
      load_graphics();
      init_cache();
      setup_default_point_list();

      if (SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_AUDIO) < 0)
          fatal((char*)SDL_GetError());

      if (SDL_CreateWindowAndRenderer(320, 200, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
          fatal((char*)SDL_GetError());

      texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);
      if (!texture)
          fatal((char*)SDL_GetError());

      SDL_SetWindowTitle(window, "Quake Map Viewer");
      SDL_RenderSetLogicalSize(renderer, 320, 200);

      run_sim();      
      set_text();
   }
   return 0;
}
