#include "image.h"
#include "space.h"
#include "render.h"
#include "from_png.h"

int main()
{
  struct cam c;
  struct tri t = {{0,0,0},
                  {10,0,0},
                  {0,10,0}};
  cam_init(&c, 200, 200);
  cam_clear(&c, black);
  render_tri(&c, t, red);
  write_png("test.png", &c.dr.im);
  cam_destroy(&c);
  return 0;
}
