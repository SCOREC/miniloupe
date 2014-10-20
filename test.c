#include "image.h"
#include "space.h"
#include "render.h"
#include "from_png.h"
#include "globe.h"

static void draw_square(struct cam* cam, struct globe* g,
    struct tri t[2], struct color c)
{
  int i;
  for (i = 0; i < 2; ++i)
    render_tri(cam, frame_mul_tri(globe_frame(g), t[i]), c);
}

static void draw_cube(struct cam* cam)
{
  struct globe g = globe_ident;
  struct tri t[2];
  struct color c[4] = {RED,GREEN,BLUE,WHITE};
  int i;
  t[0].a = vec_new(-1,-1,1);
  t[0].b = vec_new( 1,-1,1);
  t[0].c = vec_new(-1, 1,1);
  t[1] = t[0];
  t[1].a = vec_new( 1, 1,1);
  for (i = 0; i < 4; ++i) {
    g.spin = i * pi / 2;
    draw_square(cam, &g, t, c[i]);
  }
}

int main()
{
  struct cam c;
  struct globe g = globe_ident;
  globe_zoom(&g, 40);
  globe_spin(&g, -3 * pi / 8);
  globe_tilt(&g, pi / 9);
  cam_init(&c, 200, 200);
  cam_clear(&c, black);
  c.frm = globe_frame(&g);
  draw_cube(&c);
  draw_text(&c.dr.im, pix_new(40,160), "BossMan", red);
  write_png("test.png", &c.dr.im);
  cam_destroy(&c);
  return 0;
}
