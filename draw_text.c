#include "image.h"
#include "charbits.h"

static void draw_char(struct image* im, struct pix p,
    char ch, struct color c)
{
  unsigned char const* bits;
  int i,j;
  if ((p.x + CHARBITS_WIDTH >= im->w) ||
      (p.y + CHARBITS_HEIGHT >= im->h))
    return;
  bits = charbits(ch);
  for (i = 0; i < CHARBITS_HEIGHT; ++i)
  for (j = 0; j < CHARBITS_WIDTH; ++j)
    if (bits[i] & (1<<j))
      im->rows[p.y + i][p.x + j] = c;
}

void draw_text(struct image* im, struct pix p,
    const char* s, struct color c)
{
  int ix = p.x;
  for (; *s; ++s) {
    if (*s == '\n') {
      p.y += CHARBITS_HEIGHT;
      p.x = ix;
    } else {
      draw_char(im, p, *s, c);
      p.x += CHARBITS_WIDTH;
    }
  }
}
