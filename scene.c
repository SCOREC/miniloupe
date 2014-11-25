#include "image.h"
#include "space.h"
#include "render.h"
#include "scene.h"
#include "base.h"

#include <string.h>

enum {
  DOT,
  LINE,
  TRI,
  TEXT,
  TYPES
};

void scene_init(struct scene* s, struct color bg)
{
  memset(s, 0, sizeof(*s));
  s->background = bg;
}

void scene_dot(struct scene* s, struct vec p, struct color c)
{
  if (s->count[DOT] == s->cap[DOT]) {
    s->cap[DOT] = growth(s->cap[DOT]);
    REALLOC(s->dots, s->cap[DOT]);
    REALLOC(s->colors[DOT], s->cap[DOT]);
  }
  s->dots[s->count[DOT]] = p;
  s->colors[DOT][s->count[DOT]] = c;
  ++(s->count[DOT]);
}

void scene_line(struct scene* s, struct line l, struct color c, int is_thick)
{
  if (s->count[LINE] == s->cap[LINE]) {
    s->cap[LINE] = growth(s->cap[LINE]);
    REALLOC(s->lines, s->cap[LINE]);
    REALLOC(s->colors[LINE], s->cap[LINE]);
    REALLOC(s->is_thick, s->cap[LINE]);
  }
  s->lines[s->count[LINE]] = l;
  s->colors[LINE][s->count[LINE]] = c;
  s->is_thick[s->count[LINE]] = is_thick ? 1 : 0;
  ++(s->count[LINE]);
}

void scene_tri(struct scene* s, struct tri t, struct color c)
{
  if (s->count[TRI] == s->cap[TRI]) {
    s->cap[TRI] = growth(s->cap[TRI]);
    REALLOC(s->tris, s->cap[TRI]);
    REALLOC(s->colors[TRI], s->cap[TRI]);
  }
  s->tris[s->count[TRI]] = t;
  s->colors[TRI][s->count[TRI]] = c;
  ++(s->count[TRI]);
}

void scene_text(struct scene* s, struct vec p, const char* t, struct color c)
{
  if (s->count[TEXT] == s->cap[TEXT]) {
    s->cap[TEXT] = growth(s->cap[TEXT]);
    REALLOC(s->text_points, s->cap[TEXT]);
    REALLOC(s->texts, s->cap[TEXT]);
    REALLOC(s->colors[TEXT], s->cap[TEXT]);
  }
  s->text_points[s->count[TEXT]] = p;
  s->texts[s->count[TEXT]] = 0;
  REALLOC(s->texts[s->count[TEXT]], strlen(t) + 1);
  strcpy(s->texts[s->count[TEXT]], t);
  s->colors[TEXT][s->count[TEXT]] = c;
  ++(s->count[TEXT]);
}

void scene_destroy(struct scene* s)
{
  int i;
  FREE(s->dots);
  FREE(s->lines);
  FREE(s->tris);
  FREE(s->text_points);
  for (i = 0; i < s->count[TEXT]; ++i)
    FREE(s->texts[i]);
  FREE(s->texts);
  for (i = 0; i < TYPES; ++i)
    FREE(s->colors[i]);
}

void scene_render(struct scene* s, struct cam* cam)
{
  int i;
  cam_clear(cam, s->background);
  for (i = 0; i < s->count[DOT]; ++i) {
    render_dot(cam, s->dots[i], s->colors[DOT][i]);
  }
  for (i = 0; i < s->count[LINE]; ++i) {
    if (s->is_thick[i])
      render_thick_line(cam, s->lines[i], s->colors[LINE][i]);
    else
      render_line(cam, s->lines[i], s->colors[LINE][i]);
  }
  for (i = 0; i < s->count[TRI]; ++i) {
    render_tri(cam, s->tris[i], s->colors[TRI][i]);
  }
  for (i = 0; i < s->count[TEXT]; ++i) {
    render_text(cam, s->text_points[i], s->texts[i], s->colors[TEXT][i]);
  }
}

static int update_bounds(struct vec* p, int n,
    struct vec* min, struct vec* max, int init)
{
  int i;
  if (!init && n) {
    *min = *max = p[0];
    init = 1;
  }
  for (i = 1; i < n; ++i) {
    *min = vec_min(*min, p[i]);
    *max = vec_max(*max, p[i]);
  }
  return init;
}

void scene_bounds(struct scene* s, struct vec* min, struct vec* max)
{
  int init = 0;
  init = update_bounds(s->dots, s->count[DOT],
      min, max, init);
  init = update_bounds((struct vec*)s->lines, s->count[LINE] * 2,
      min, max, init);
  init = update_bounds((struct vec*)s->tris, s->count[TRI] * 3,
      min, max, init);
  init = update_bounds(s->text_points, s->count[TEXT],
      min, max, init);
  if (!init) {
    *min = *max = vec_new(0,0,0);
  }
}

static void center_points(struct vec* p, int n, struct vec c)
{
  int i;
  for (i = 0; i < n; ++i) {
    p[i] = vec_sub(p[i], c);
  }
}

void scene_center(struct scene* s, struct vec c)
{
  center_points(s->dots, s->count[DOT], c);
  center_points((struct vec*)s->lines, s->count[LINE] * 2, c);
  center_points((struct vec*)s->tris, s->count[TRI] * 3, c);
  center_points(s->text_points, s->count[TEXT], c);
}
