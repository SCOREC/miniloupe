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

void scene_init(struct scene* s)
{
  memset(s, 0, sizeof(*s));
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

void scene_line(struct scene* s, struct line l, struct color c)
{
  if (s->count[LINE] == s->cap[LINE]) {
    s->cap[LINE] = growth(s->cap[LINE]);
    REALLOC(s->lines, s->cap[LINE]);
    REALLOC(s->colors[LINE], s->cap[LINE]);
  }
  s->lines[s->count[LINE]] = l;
  s->colors[LINE][s->count[LINE]] = c;
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
  for (i = 0; i < s->count[DOT]; ++i)
    render_dot(cam, s->dots[i], s->colors[DOT][i]);
  for (i = 0; i < s->count[LINE]; ++i)
    render_line(cam, s->lines[i], s->colors[LINE][i]);
  for (i = 0; i < s->count[TRI]; ++i)
    render_tri(cam, s->tris[i], s->colors[TRI][i]);
  for (i = 0; i < s->count[TEXT]; ++i)
    render_text(cam, s->text_points[i], s->texts[i], s->colors[TEXT][i]);
}

