/* QMAP: Quake level viewer
 *
 *   3d.c  Copyright 1997 Sean Barrett
 *
 *   general 3d math
 */

#include <math.h>
#include <string.h>
#include "3d.h"
#include "bspfile.h"

double dot_vec_dbl(double *a, vector *b)
{
   return a[0]*b->x + a[1]*b->y + a[2]*b->z;
}

void rot_x(double *v, fixang ang)
{
   double s = sin(ang * 3.141592 / 0x8000);
   double c = cos(ang * 3.141592 / 0x8000);
   double y,z;

   y =  c * v[1] + s * v[2];
   z = -s * v[1] + c * v[2];
   v[1] = y;
   v[2] = z;
}

void rot_y(double *v, fixang ang)
{
   double s = sin(ang * 3.141592 / 0x8000);
   double c = cos(ang * 3.141592 / 0x8000);
   double x,z;

   z =  c * v[2] + s * v[0];
   x = -s * v[2] + c * v[0];
   v[0] = x;
   v[2] = z;
}

void rot_z(double *v, fixang ang)
{
   double s = sin(ang * 3.141592 / 0x8000);
   double c = cos(ang * 3.141592 / 0x8000);
   double x,y;

   x =  c * v[0] + s * v[1];
   y = -s * v[0] + c * v[1];
   v[0] = x;
   v[1] = y;
}

void rotate(double *x, angvec *ang)
{
   rot_y(x, ang->ty);  // bank
   rot_x(x, ang->tx);  // pitch
   rot_z(x, ang->tz);  // yaw
}

static double main_matrix[3][3], view_matrix[3][3], translate[3];
static vector cam_loc;

static fix clip_x_low, clip_x_high, clip_y_low, clip_y_high;

static float proj_scale_x = 160, proj_scale_y = 160 * 200 / 240;
static float xcenter = 159.5, ycenter = 99.5;
static float near_clip = 0.01, near_code = 16.0;

double clip_scale_x, clip_scale_y;

void set_view_info(vector *loc, angvec *ang)
{
   int i;
   cam_loc = *loc;

   clip_x_low = -0x8000;
   clip_x_high = fix_make(320,0)-0x8000;
   clip_y_low = -0x8000;
   clip_y_high = fix_make(200,0)-0x8000;

   clip_scale_x = 1 / 160.0;
   clip_scale_y = 1 / 100.0;

   // compute rotation matrix
   memset(view_matrix, 0, sizeof(view_matrix));
   view_matrix[0][0] = view_matrix[1][1] = view_matrix[2][2] = 1;
   rotate(view_matrix[0], ang);
   rotate(view_matrix[1], ang);
   rotate(view_matrix[2], ang);
   memcpy(main_matrix, view_matrix, sizeof(view_matrix));

   // so (1,0,0) in camera space maps to view_matrix[0] in world space.
   // thus we multiply on the right by a worldspace vector to transform
   // it to camera space.
   // now, to account for translation, we just subtract the camera
   // center before multiplying
   translate[0] = loc->x;
   translate[1] = loc->y;
   translate[2] = loc->z;

   // roll projection math into transformation
   for (i=0; i < 3; ++i) {
      view_matrix[0][i] *= proj_scale_x;
      view_matrix[2][i] *= proj_scale_y;
   }
}

double dist2_from_viewer(vector *in)
{
   vector temp;
   temp.x = in->x - cam_loc.x;
   temp.y = in->y - cam_loc.y;
   temp.z = in->z - cam_loc.z;

   return temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;
}

void rotate_c2w(vector *dest, vector *src)
{
   dest->x = src->x * main_matrix[0][0]
           + src->y * main_matrix[1][0]
           + src->z * main_matrix[2][0];
   dest->y = src->x * main_matrix[0][1]
           + src->y * main_matrix[1][1]
           + src->z * main_matrix[2][1];
   dest->z = src->x * main_matrix[0][2]
           + src->y * main_matrix[1][2]
           + src->z * main_matrix[2][2];
}

void rotate_vec(vector *item)
{
   vector temp;
   temp = *item;
   rotate_c2w(item, &temp);
}

void compute_plane(dplane_t *plane, float x, float y, float z)
{
   vector temp, temp2;
   temp2.x = x; temp2.y = z; temp2.z = y;
   rotate_c2w(&temp, &temp2);
   plane->normal[0] = temp.x;
   plane->normal[1] = temp.y;
   plane->normal[2] = temp.z;
   plane->dist = temp.x*cam_loc.x + temp.y*cam_loc.y + temp.z*cam_loc.z;
}

void compute_view_frustrum(dplane_t *planes)
{
   compute_plane(planes+0, -1,  0, 1);
   compute_plane(planes+1,  1,  0, 1);
   compute_plane(planes+2,  0,  1, 1);
   compute_plane(planes+3,  0, -1, 1);
}

void transform_point_raw(vector *out, vector *in)
{
   vector temp;
   temp.x = in->x - translate[0];
   temp.y = in->y - translate[1];
   temp.z = in->z - translate[2];

   out->x = dot_vec_dbl(view_matrix[0], &temp);
   out->z = dot_vec_dbl(view_matrix[1], &temp);
   out->y = dot_vec_dbl(view_matrix[2], &temp);
}

void transform_vector(vector *out, vector *in)
{
   vector temp = *in;
   out->x = dot_vec_dbl(view_matrix[0], &temp);
   out->z = dot_vec_dbl(view_matrix[1], &temp);
   out->y = dot_vec_dbl(view_matrix[2], &temp);
}

void project_point(point_3d *p)
{
   if (p->p.z >= near_clip) {
      double div = 1.0 / p->p.z;
      p->sx = FLOAT_TO_FIX( p->p.x * div + xcenter);
      p->sy = FLOAT_TO_FIX(-p->p.y * div + ycenter);
   }
}

void code_point(point_3d *p)
{
   if (p->p.z >= near_code) {
      // if point is far enough away, code in 2d from fixedpoint (faster)
           if (p->sx < clip_x_low)  p->ccodes = CC_OFF_LEFT;
      else if (p->sx > clip_x_high) p->ccodes = CC_OFF_RIGHT;
      else                          p->ccodes = 0;
           if (p->sy < clip_y_low)  p->ccodes |= CC_OFF_TOP;
      else if (p->sy > clip_y_high) p->ccodes |= CC_OFF_BOT;
   } else {
      p->ccodes = (p->p.z > 0) ? 0 : CC_BEHIND;
      if (p->p.x * clip_scale_x < -p->p.z) p->ccodes |= CC_OFF_LEFT;
      if (p->p.x * clip_scale_x >  p->p.z) p->ccodes |= CC_OFF_RIGHT;
      if (p->p.y * clip_scale_y >  p->p.z) p->ccodes |= CC_OFF_TOP;
      if (p->p.y * clip_scale_y < -p->p.z) p->ccodes |= CC_OFF_BOT;
   }
}

void transform_point(point_3d *p, vector *v)
{
   transform_point_raw(&p->p, v);
   project_point(p);
   code_point(p);
}

void transform_rotated_point(point_3d *p)
{
   project_point(p);
   code_point(p);
}
