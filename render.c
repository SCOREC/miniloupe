#include "image.h"
#include "space.h"
#include "render.h"
#include "base.h"

static double const dot_z_offset = -2.0;
static double const line_z_offset = -1.0;

void cam_init(struct cam* cam, int w, int h)
{
  drawing_init(&cam->dr, w, h);
  cam->frm = frame_ident;
}

void cam_destroy(struct cam* cam)
{
  drawing_destroy(&cam->dr);
}

void cam_clear(struct cam* cam, struct color c)
{
  drawing_clear(&cam->dr, c);
}

static struct plane clip_plane(struct cam* cam, int which)
{
  struct plane pl;
  pl.n = vec_zero;
  if (which / 2 == 0) {
    pl.n.x = 1;
    pl.r = - (cam->dr.im.w - 1.0) / 2.0;
  } else {
    pl.n.y = 1;
    pl.r = - (cam->dr.im.h - 1.0) / 2.0;
  }
  if (which % 2 == 0)
    pl.n = vec_mul_s(pl.n, -1);
  return pl;
}

static struct vec midpoint(struct cam* cam)
{
  struct vec m;
  m.x = cam->dr.im.w / 2.0;
  m.y = cam->dr.im.h / 2.0;
  m.z = 0;
  return m;
}

int cam_sees(struct cam* cam, struct vec p)
{
  int which;
  for (which = 0; which < 4; ++which)
    if (!plane_has(clip_plane(cam, which), p))
      return 0;
  return 1;
}

static struct dot make_dot(struct vec p)
{
  struct dot d;
  d.p.x = p.x;
  d.p.y = p.y;
  d.z = p.z;
  return d;
}

void render_dot(struct cam* cam, struct vec p, struct color c)
{
  struct dot d;
  p = frame_mul_vec(cam->frm, p);
  if (!cam_sees(cam, p))
    return;
  p = vec_add(midpoint(cam), p);
  d = make_dot(p);
  d.z += dot_z_offset;
  draw_thick_dot(&cam->dr, d, c);
}

void render_line(struct cam* cam, struct line l, struct color c)
{
  struct dline dl;
  int i;
  l.a = frame_mul_vec(cam->frm, l.a);
  l.b = frame_mul_vec(cam->frm, l.b);
  for (i = 0; i < 4; ++i)
    if (!plane_clip_line(clip_plane(cam, i), &l))
      return;
  l.a = vec_add(midpoint(cam), l.a);
  l.b = vec_add(midpoint(cam), l.b);
  dl.a = make_dot(l.a);
  dl.a.z += line_z_offset;
  dl.b = make_dot(l.b);
  dl.b.z += line_z_offset;
  draw_line(&cam->dr, dl, c);
}

static void render_tri_leaf(struct cam* cam, struct tri t, struct color c)
{
  struct dtri dt;
  t.a = vec_add(midpoint(cam), t.a);
  t.b = vec_add(midpoint(cam), t.b);
  t.c = vec_add(midpoint(cam), t.c);
  dt.a = make_dot(t.a);
  dt.b = make_dot(t.b);
  dt.c = make_dot(t.c);
  draw_tri(&cam->dr, dt, c);
}

static void render_tri_tree(struct cam* cam, struct tri t, struct color c,
    int p);

static void render_tri_branch(struct cam* cam, struct tri t, struct color c,
    int p)
{
  struct plane pl;
  int nt;
  struct tri ts[2];
  int i;
  pl = clip_plane(cam, p);
  nt = plane_clip_tri(pl, t, ts);
  for (i = 0; i < nt; ++i)
    render_tri_tree(cam, ts[i], c, p + 1);
}

static void render_tri_tree(struct cam* cam, struct tri t, struct color c,
    int p)
{
  if (p == 4)
    render_tri_leaf(cam, t, c);
  else
    render_tri_branch(cam, t, c, p);
}

void render_tri(struct cam* cam, struct tri t, struct color c)
{
  struct vec n;
  t.a = frame_mul_vec(cam->frm, t.a);
  t.b = frame_mul_vec(cam->frm, t.b);
  t.c = frame_mul_vec(cam->frm, t.c);
  n = tri_norm(t);
  if (n.z < 0) {
    struct vec tmp = t.c;
    t.c = t.b;
    t.b = tmp;
    n.z = -n.z;
  }
  c.r *= n.z;
  c.g *= n.z;
  c.b *= n.z;
  render_tri_tree(cam, t, c, 0);
}

void render_text(struct cam* cam, struct vec p, const char* t,
    struct color c)
{
  p = frame_mul_vec(cam->frm, p);
  if (!cam_sees(cam, p))
    return;
  p = vec_add(midpoint(cam), p);
  draw_text(&cam->dr, pix_new(p.x, p.y), t, c);
}
