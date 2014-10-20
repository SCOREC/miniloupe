#include "space.h"
#include "base.h"
#include <math.h>

struct vec const vec_zero = {0,0,0};
struct vec const vec_x = VEC_X;
struct vec const vec_y = VEC_Y;
struct vec const vec_z = VEC_Z;

struct mat const mat_ident = MAT_IDENT;

struct frame const frame_ident = {
  MAT_IDENT,
  {0,0,0}
};

double const pi = 3.14159265359;

struct vec vec_add(struct vec a, struct vec b)
{
  a.x = a.x + b.x;
  a.y = a.y + b.y;
  a.z = a.z + b.z;
  return a;
}

struct vec vec_sub(struct vec a, struct vec b)
{
  a.x = a.x - b.x;
  a.y = a.y - b.y;
  a.z = a.z - b.z;
  return a;
}

struct vec vec_mul_s(struct vec v, double s)
{
  v.x = v.x * s;
  v.y = v.y * s;
  v.z = v.z * s;
  return v;
}

double vec_dot(struct vec a, struct vec b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

struct vec vec_new(double x, double y, double z)
{
  struct vec v;
  v.x = x;
  v.y = y;
  v.z = z;
  return v;
}

double vec_mag(struct vec v)
{
  return sqrt(vec_dot(v, v));
}

struct vec vec_norm(struct vec v)
{
  return vec_mul_s(v, 1.0 /  vec_mag(v));
}

struct mat mat_new(struct vec x, struct vec y, struct vec z)
{
  struct mat m;
  m.x = x;
  m.y = y;
  m.z = z;
  return m;
}

struct mat vec_cross_mat(struct vec a)
{
  return mat_new(vec_new(   0, a.z,-a.y),
                 vec_new(-a.z,   0, a.x),
                 vec_new( a.y,-a.x,   0));
}

struct mat mat_mul_s(struct mat m, double s)
{
  return mat_new(vec_mul_s(m.x, s),
                 vec_mul_s(m.y, s),
                 vec_mul_s(m.z, s));
}

struct vec mat_mul_vec(struct mat m, struct vec v)
{
  return vec_add(vec_mul_s(m.x, v.x),
         vec_add(vec_mul_s(m.y, v.y),
                 vec_mul_s(m.z, v.z)));
}

struct mat mat_add(struct mat a, struct mat b)
{
  return mat_new(vec_add(a.x, b.x),
                 vec_add(a.y, b.y),
                 vec_add(a.z, b.z));
}

struct mat mat_mul_mat(struct mat a, struct mat b)
{
  return mat_new(mat_mul_vec(a, b.x),
                 mat_mul_vec(a, b.y),
                 mat_mul_vec(a, b.z));
}

struct vec vec_cross(struct vec a, struct vec b)
{
  return mat_mul_vec(vec_cross_mat(a), b);
}

double vec_triple(struct vec a, struct vec b, struct vec c)
{
  return vec_dot(a, vec_cross(b, c));
}

struct mat vec_outer(struct vec a, struct vec b)
{
  return mat_new(vec_mul_s(a, b.x),
                 vec_mul_s(a, b.y),
                 vec_mul_s(a, b.z));
}

struct mat mat_rot(struct vec u, double t)
{
  return mat_add(mat_mul_s(mat_ident, cos(t)),
         mat_add(mat_mul_s(vec_cross_mat(u), sin(t)),
                 mat_mul_s(vec_outer(u, u), 1.0 - cos(t))));
}

struct mat mat_transp(struct mat m)
{
  return mat_new(vec_new(m.x.x, m.y.x, m.z.x),
                 vec_new(m.x.y, m.y.y, m.z.y),
                 vec_new(m.x.z, m.y.z, m.z.z));
}

double mat_det(struct mat m)
{
  return vec_triple(m.x, m.y, m.z);
}

struct mat mat_inv(struct mat m)
{
  return mat_mul_s(mat_transp(mat_new(vec_cross(m.y, m.z),
                                      vec_cross(m.z, m.x),
                                      vec_cross(m.x, m.y))),
                   1.0 / mat_det(m));
}

struct mat mat_scale(double s)
{
  return mat_mul_s(mat_ident, s);
}

struct vec frame_mul_vec(struct frame f, struct vec v)
{
  return vec_add(mat_mul_vec(f.r, v), f.t);
}

struct frame frame_new(struct mat r, struct vec t)
{
  struct frame f;
  f.r = r;
  f.t = t;
  return f;
}

struct frame frame_rot(struct vec u, double t)
{
  return frame_new(mat_rot(u, t), vec_zero);
}

struct frame frame_trans(struct vec v)
{
  return frame_new(mat_ident, v);
}

struct frame frame_scale(double s)
{
  return frame_new(mat_scale(s), vec_zero);
}

struct frame frame_mul_frame(struct frame a, struct frame b)
{
  return frame_new(mat_mul_mat(a.r, b.r),
                   vec_add(mat_mul_vec(a.r, b.t), a.t));
}

struct frame frame_inv(struct frame f)
{
  struct mat rinv = mat_inv(f.r);
  return frame_new(rinv, vec_mul_s(mat_mul_vec(rinv, f.t), -1));
}

struct vec tri_norm(struct tri t)
{
  return vec_norm(vec_cross(vec_sub(t.b, t.a), vec_sub(t.c, t.a)));
}

struct tri frame_mul_tri(struct frame f, struct tri t)
{
  t.a = frame_mul_vec(f, t.a);
  t.b = frame_mul_vec(f, t.b);
  t.c = frame_mul_vec(f, t.c);
  return t;
}

double plane_dist(struct plane pl, struct vec pt)
{
  return vec_dot(pl.n, pt) - pl.r;
}

int plane_has(struct plane pl, struct vec pt)
{
  return plane_dist(pl, pt) >= 0;
}

struct vec plane_line_touch(struct plane pl, struct line l)
{
  struct vec v;
  double b;
  double denom;
  /* pl.n * x - pl.r = 0
     x = l.a + b * (l.b - l.a)
     pl.n * (l.a + b * (l.b - l.a)) - pl.r = 0
     pl.n * l.a + b * (pl.n * (l.b - l.a)) = pl.r
     b = (pl.r - pl.n * l.a) / (pl.n * (l.b - l.a)) */
  v = vec_sub(l.b, l.a);
  denom = vec_dot(pl.n, v);
  EXCEPT_IF(denom == 0.0);
  b = (pl.r - vec_dot(pl.n, l.a)) / denom;
  return vec_add(l.a, vec_mul_s(v, b));
}

int plane_clip_line(struct plane pl, struct line* l)
{
  int has_a;
  int has_b;
  has_a = plane_has(pl, l->a);
  has_b = plane_has(pl, l->b);
  if ((!has_a) && (!has_b))
    return 0;
  if (has_a && has_b)
    return 1;
  if (has_a)
    l->b = plane_line_touch(pl, *l);
  else
    l->a = plane_line_touch(pl, *l);
  return 1;
}

static struct tri rot_tri(struct tri t)
{
  struct tri t2;
  t2.c = t.a;
  t2.a = t.b;
  t2.b = t.c;
  return t2;
}

static void cut_tri(struct plane pl, struct tri t,
    struct vec* ab, struct vec* ac)
{
  struct line l;
  l.a = t.a;
  l.b = t.b;
  *ab = plane_line_touch(pl, l);
  l.b = t.c;
  *ac = plane_line_touch(pl, l);
}

static int clip_tri_1(struct plane pl, struct tri t, struct tri o[])
{
  cut_tri(pl, t, &o->b, &o->c);
  o->a = t.a;
  return 1;
}

static int clip_tri_2(struct plane pl, struct tri t, struct tri o[])
{
  cut_tri(pl, t, &o[0].a, &o[1].a);
  o[0].b = t.b;
  o[0].c = t.c;
  o[1].b = o[0].a;
  o[1].c = t.c;
  return 2;
}

int plane_clip_tri(struct plane pl, struct tri t, struct tri o[2])
{
  int has_a;
  int has_b;
  int has_c;
  has_a = plane_has(pl, t.a);
  has_b = plane_has(pl, t.b);
  has_c = plane_has(pl, t.c);
  if (has_a && has_b && has_c) {
    o[0] = t;
    return 1;
  } else if (!has_a && !has_b && !has_c)
    return 0;
  else if (has_a && has_b && !has_c)
    return clip_tri_2(pl, rot_tri(rot_tri(t)), o);
  else if (has_a && !has_b && has_c)
    return clip_tri_2(pl, rot_tri(t), o);
  else if (has_a && !has_b && !has_c)
    return clip_tri_1(pl, t, o);
  else if (!has_a && has_b && has_c)
    return clip_tri_2(pl, t, o);
  else if (!has_a && has_b && !has_c)
    return clip_tri_1(pl, rot_tri(t), o);
  else if (!has_a && !has_b && has_c)
    return clip_tri_1(pl, rot_tri(rot_tri(t)), o);
  BUG_IF("switch is wrong");
  return 0;
}
