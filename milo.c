#include "milo.h"
#include "base.h"
#include "image.h"
#include "space.h"
#include "render.h"
#include "scene.h"
#include "globe.h"
#include "from_png.h"
#include "socks.h"
#include "proto.h"

struct milo {
  struct globe globe;
  struct cam camera;
  struct scene scene;
  struct client client;
};

milo_t milo_new(int w, int h, const char* servname, int port)
{
  milo_t m;
  ALLOC(m);
  m->globe = globe_ident;
  cam_init(&m->camera, w, h);
  scene_init(&m->scene);
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
  scene_init(&m->scene);
  cam_clear(&m->camera, user_color(color));
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

void milo_render(milo_t m)
{
  m->camera.frm = globe_frame(&m->globe);
  scene_render(&m->scene, &m->camera);
}

void milo_write_png(milo_t m, const char* filename)
{
  write_png(filename, &m->camera.dr.im);
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

static void handle_start(void* u)
{
  milo_send(u);
}

static void handle_stop(void* u)
{
}

static void handle_zoom(void* u)
{
  milo_zoom(u, recv_double(milo_socket(u)));
  milo_send(u);
}

static void handle_pan(void* u)
{
  double x,y;
  x = recv_double(milo_socket(u));
  y = recv_double(milo_socket(u));
  milo_pan(u, x, y);
  milo_send(u);
}

static void handle_tilt(void* u)
{
  milo_tilt(u, recv_double(milo_socket(u)));
  milo_send(u);
}

static void handle_spin(void* u)
{
  milo_spin(u, recv_double(milo_socket(u)));
  milo_send(u);
}

static handler const handlers[PROTO_CODES] = {
  handle_start,
  handle_stop,
  handle_zoom,
  handle_pan,
  handle_tilt,
  handle_spin
};

void milo_run(milo_t m)
{
  proto_main(milo_socket(m), handlers, m);
}
