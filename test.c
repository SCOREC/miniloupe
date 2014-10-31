#include "milo.h"
#include "from_mpi.h"

/* the following two lines bring in M_PI */
#define _XOPEN_SOURCE 500
#include <math.h>

#include <stdlib.h>
#include <stdio.h>

static void draw_quad(milo_t m)
{
  double green[3] = {0,1,0};
  double a[3] = {-1,-1,1};
  double b[3] = { 1,-1,1};
  double c[3] = {-1, 1,1};
  double d[3] = { 1, 1,1};
  milo_triangle(m, a, b, c, green);
  milo_triangle(m, b, c, d, green);
}

int main(int argc, char** argv)
{
  double black[3] = {0,0,0};
  double red[3] = {1,0,0};
  double text_point[3] = {1,1,0};
  milo_t m;
  if (argc != 3) {
    printf("usage: %s server port\n", argv[0]);
    return 0;
  }
  start_mpi();
  m = milo_new(argv[1], atoi(argv[2]));
  milo_zoom(m, 40);
  milo_spin(m, -3 * M_PI / 8);
  milo_tilt(m, M_PI / 9);
  milo_clear(m, black);
  draw_quad(m);
  milo_text(m, text_point, "BossMan", red);
  milo_run(m);
  milo_free(m);
  stop_mpi();
  return 0;
}
