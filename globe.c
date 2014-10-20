#include "space.h"
#include "globe.h"
#include "base.h"

#include <math.h>

struct globe const globe_ident = {
  {0,0,0},
  0,
  0
};

struct frame globe_frame(struct globe* g)
{
  static struct vec y = {0,1,0};
  static struct vec x = {1,0,0};
  return frame_mul_frame(frame_trans(g->center),
         frame_mul_frame(frame_rot(x, g->tilt),
                         frame_rot(y, g->spin)));
}

void globe_pan(struct globe* g, double x, double y)
{
  g->center.x += x;
  g->center.y += y;
}

void globe_zoom(struct globe* g, double z)
{
  g->center.z += z;
}

void globe_tilt(struct globe* g, double by)
{
  g->tilt += by;
  g->tilt = MIN(g->tilt, pi / 2);
  g->tilt = MAX(g->tilt,-pi / 2);
}

void globe_spin(struct globe* g, double by)
{
  g->spin += by;
  g->spin = fmod(g->spin, 2 * pi);
}

