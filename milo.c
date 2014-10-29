#include "milo.h"
#include "base.h"
#include "image.h"
#include "space.h"
#include "render.h"
#include "scene.h"
#include "globe.h"
#include "socks.h"
#include "proto.h"

#include <math.h>

struct milo {
  struct globe globe;
  struct cam camera;
  struct scene scene;
  struct client client;
};

milo_t milo_new(int w, int h, const char* servname, int port)
{
  static struct color const black = {0,0,0};
  milo_t m;
  ALLOC(m);
  m->globe = globe_ident;
  cam_init(&m->camera, w, h);
  scene_init(&m->scene, black);
  client_init(&m->client, servname, port);
  client_connect(&m->client);
  return m;
}

void milo_free(milo_t m)
{
  cam_destroy(&m->camera);
  scene_destroy(&m->scene);
  client_close(&m->client);
  client_destroy(&m->client);
  FREE(m);
}

static struct color user_color(double* c)
{
  struct color c2;
  c2.r = c[0] * 255;
  c2.g = c[1] * 255;
  c2.b = c[2] * 255;
  return c2;
}

void milo_clear(milo_t m, double* color)
{
  scene_destroy(&m->scene);
  scene_init(&m->scene, user_color(color));
}

void milo_dot(milo_t m, double* point, double* color)
{
  scene_dot(&m->scene, *((struct vec*)point), user_color(color));
}

void milo_line(milo_t m, double* point_a, double* point_b, double* color)
{
  struct line l;
  l.a = *((struct vec*)point_a);
  l.b = *((struct vec*)point_b);
  scene_line(&m->scene, l, user_color(color));
}

void milo_triangle(milo_t m, double* point_a, double* point_b, double* point_c,
    double* color)
{
  struct tri t;
  t.a = *((struct vec*)point_a);
  t.b = *((struct vec*)point_b);
  t.c = *((struct vec*)point_c);
  scene_tri(&m->scene, t, user_color(color));
}

void milo_text(milo_t m, double* point, const char* text, double* color)
{
  scene_text(&m->scene, *((struct vec*)point), text, user_color(color)); 
}

void milo_zoom(milo_t m, double factor)
{
  globe_zoom(&m->globe, factor);
}

void milo_pan(milo_t m, double x, double y)
{
  globe_pan(&m->globe, x, y);
}

void milo_tilt(milo_t m, double radians)
{
  globe_tilt(&m->globe, radians);
}

void milo_spin(milo_t m, double radians)
{
  globe_spin(&m->globe, radians);
}

static void milo_render(milo_t m)
{
  m->camera.frm = globe_frame(&m->globe);
  scene_render(&m->scene, &m->camera);
}

static int milo_socket(milo_t m)
{
  return m->client.fd;
}

static void milo_send(milo_t m)
{
  struct image* im;
  im = &m->camera.dr.im;
  blocking_send(m->client.fd, &im->rows[0][0],
      im->w * im->h * sizeof(struct color));
}

static void handle_stop(void* u)
{
}

static void handle_zoom(void* u)
{
  milo_zoom(u, recv_double(milo_socket(u)));
}

static void handle_pan(void* u)
{
  double x,y;
  x = recv_double(milo_socket(u));
  y = recv_double(milo_socket(u));
  milo_pan(u, x, y);
}

static void handle_spin(void* u)
{
  milo_spin(u, recv_double(milo_socket(u)));
}

static void handle_tilt(void* u)
{
  milo_tilt(u, recv_double(milo_socket(u)));
}

static void handle_render(void* u)
{
  milo_render(u);
  milo_send(u);
}

static handler const handlers[PROTO_CODES] = {
  handle_stop,
  handle_zoom,
  handle_pan,
  handle_spin,
  handle_tilt,
  handle_render
};

static void focus(milo_t m)
{
  struct vec min,max,mid,diff;
  double r;
  struct image* im;
  scene_bounds(&m->scene, &min, &max);
  debug("min %f %f %f\n",min.x,min.y,min.z);
  debug("max %f %f %f\n",max.x,max.y,max.z);
  mid = vec_mul_s(vec_add(min,max),1.0/2.0);
  debug("mid %f %f %f\n",mid.x,mid.y,mid.z);
  scene_center(&m->scene, mid);
  diff = vec_sub(max,min);
  r = MAX(fabs(diff.x),MAX(fabs(diff.y),fabs(diff.z)));
  if (!r)
    r = 1.0;
  im = &m->camera.dr.im;
  r *= MIN(im->w, im->h);
  debug("setting zoom radius to %f\n",r);
  m->globe.center.z = r;
}

void milo_run(milo_t m)
{
  focus(m);
  proto_main(milo_socket(m), handlers, m);
}
