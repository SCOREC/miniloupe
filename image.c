#include "image.h"
#include "base.h"
#include <float.h>

struct color const white = {255,255,255};
struct color const black = {0,0,0};
struct color const red = {255,0,0};
struct color const green = {0,255,0};
struct color const blue = {0,0,255};
struct color const cyan = {0,255,255};
struct color const magenta = {255,0,255};
struct color const yellow = {255,255,0};

void image_init(struct image* im, int w, int h)
{
  int r;
  BUG_IF(w < 1);
  BUG_IF(h < 1);
  im->w = w;
  im->h = h;
  im->rows = 0;
  REALLOC(im->rows, h);
  im->rows[0] = 0;
  REALLOC(im->rows[0], w * h);
  for (r = 1; r < h; ++r)
    im->rows[r] = im->rows[r - 1] + w;
}

void image_destroy(struct image* im)
{
  FREE(im->rows[0]);
  FREE(im->rows);
}

void image_fill(struct image* im, struct color c)
{
  int x,y;
  for (y = 0; y < im->h; ++y)
    for (x = 0; x < im->w; ++x)
      im->rows[y][x] = c;
}

void drawing_init(struct drawing* d, int w, int h)
{
  int r;
  image_init(&d->im, w, h);
  d->z_rows = 0;
  REALLOC(d->z_rows, h);
  d->z_rows[0] = 0;
  REALLOC(d->z_rows[0], w * h);
  for (r = 1; r < h; ++r)
    d->z_rows[r] = d->z_rows[r - 1] + w;
}

void drawing_destroy(struct drawing* d)
{
  image_destroy(&d->im);
  FREE(d->z_rows[0]);
  FREE(d->z_rows);
}

void drawing_clear(struct drawing* d, struct color c)
{
  int x,y;
  for (y = 0; y < d->im.h; ++y)
    for (x = 0; x < d->im.w; ++x)
      d->z_rows[y][x] = DBL_MAX;
  image_fill(&d->im, c);
}

struct vox color_vox(struct color c)
{
  struct vox v;
  v.x = c.r;
  v.y = c.g;
  v.z = c.b;
  return v;
}

void draw_dot(struct drawing* dr, struct dot d, struct color c)
{
  if (d.z <= dr->z_rows[d.p.y][d.p.x]) {
    dr->z_rows[d.p.y][d.p.x] = d.z;
    dr->im.rows[d.p.y][d.p.x] = c;
  }
}

static int pix_dot(struct pix a, struct pix b)
{
  return a.x * b.x + a.y * b.y;
}

static struct pix pix_sub(struct pix a, struct pix b)
{
  struct pix c;
  c.x = a.x - b.x;
  c.y = a.y - b.y;
  return c;
}

static struct pix pix_abs(struct pix a)
{
  struct pix b;
  b.x = ABS(a.x);
  b.y = ABS(a.y);
  return b;
}

static struct vox vox_sub(struct vox a, struct vox b)
{
  struct vox c;
  c.x = a.x - b.x;
  c.y = a.y - b.y;
  c.z = a.z - b.z;
  return c;
}

static int pix_eq(struct pix a, struct pix b)
{
  return a.x == b.x && a.y == b.y;
}

void draw_line(struct drawing* dr, struct dline l, struct color c)
{
  struct pix v, dp;
  double dz;
  int sx, sy, err, vdv, e2;
  struct dot dot;
  if (pix_eq(l.a.p, l.b.p)) {
    draw_dot(dr, l.a, c);
    draw_dot(dr, l.b, c);
    return;
  }
  v = pix_sub(l.b.p, l.a.p); /* line vector */
  dz = l.b.z - l.a.z; /* z gradient */
  dp = pix_abs(v); /* line width/height */
  sx = (v.x > 0)?1:-1; /* x direction sign */
  sy = (v.y > 0)?1:-1; /* y direction sign */
  err = dp.x - dp.y; /* deviation from true line */
  vdv = pix_dot(v,v); /* interpolation denominator */
  dot = l.a;
  while (1) {
    /* interpolation numerator */
    int cdv = pix_dot(pix_sub(dot.p,l.a.p),v);
    /* interpolate z */
    dot.z = l.a.z + (dz*cdv)/vdv;
    /* interpolate color */
    draw_dot(dr, dot, c);
    /* Bresenham stepping rules */
    if (pix_eq(dot.p, l.b.p))
      return;
    e2 = 2*err;
    if (e2 > -dp.y) {
      err -= dp.y;
      dot.p.x += sx;
    }
    if (e2 < dp.x) {
      err += dp.x;
      dot.p.y += sy;
    }
  }
}

static struct pix pix_min(struct pix a, struct pix b)
{
  struct pix c;
  c.x = MIN(a.x,b.x);
  c.y = MIN(a.y,b.y);
  return c;
}

static struct pix pix_max(struct pix a, struct pix b)
{
  struct pix c;
  c.x = MAX(a.x,b.x);
  c.y = MAX(a.y,b.y);
  return c;
}

static struct pix pix_perp(struct pix a)
{
  struct pix p;
  p.x = -a.y;
  p.y = a.x;
  return p;
}

void draw_tri_lines(struct drawing* dr, struct dtri t, struct color c)
{
  struct dline l;
  l.a = t.a;
  l.b = t.b;
  draw_line(dr, l, c);
  l.a = t.b;
  l.b = t.c;
  draw_line(dr, l, c);
  l.a = t.c;
  l.b = t.a;
  draw_line(dr, l, c);
}

void draw_tri(struct drawing* dr, struct dtri t, struct color c)
{
  struct pix min,max;
  struct pix ab,bc,ca;
  struct pix nab,nbc,nca;
  int area;
  struct dot dot;
  struct vox x,off;
  min = pix_min(pix_min(t.a.p, t.b.p), t.c.p);
  max = pix_max(pix_max(t.a.p, t.b.p), t.c.p);
  bc = pix_sub(t.c.p, t.b.p);
  nbc = pix_perp(bc);
  off.x = pix_dot(t.b.p, nbc);
  area = pix_dot(t.a.p, nbc) - off.x;
  if (!area) {
    draw_tri_lines(dr, t, c);
    return;
  }
  ca = pix_sub(t.a.p, t.c.p);
  nca = pix_perp(ca);
  off.y = pix_dot(t.c.p,nca);
  BUG_IF(area != pix_dot(t.b.p, nca) - off.y);
  ab = pix_sub(t.b.p, t.a.p);
  nab = pix_perp(ab);
  off.z = pix_dot(t.a.p,nab);
  BUG_IF(area != pix_dot(t.c.p, nab) - off.z);
  for (dot.p.y = min.y; dot.p.y <= max.y; dot.p.y++)
  for (dot.p.x = min.x; dot.p.x <= max.x; dot.p.x++) {
    x.x = pix_dot(dot.p,nbc);
    x.y = pix_dot(dot.p,nca);
    x.z = pix_dot(dot.p,nab);
    x = vox_sub(x, off);
    if (x.x >= 0 && x.y >= 0 && x.z >= 0) {
      dot.z = (t.a.z * x.x + t.b.z * x.y + t.c.z * x.z) / area;
      draw_dot(dr, dot, c);
    }
  }
}
