struct scene {
  struct color background;
  struct vec* dots;
  struct line* lines;
  struct tri* tris;
  struct vec* text_points;
  char** texts;
  struct color* colors[4];
  int count[4];
  int cap[4];
};

void scene_init(struct scene* s, struct color bg);
void scene_dot(struct scene* s, struct vec p, struct color c);
void scene_line(struct scene* s, struct line l, struct color c);
void scene_tri(struct scene* s, struct tri t, struct color c);
void scene_text(struct scene* s, struct vec p, const char* t, struct color c);
void scene_destroy(struct scene* s);
void scene_render(struct scene* s, struct cam* cam);
void scene_bounds(struct scene* s, struct vec* min, struct vec* max);
void scene_center(struct scene* s, struct vec c);
