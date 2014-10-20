struct cam {
  struct drawing dr;
  struct frame frm;
};

typedef struct color (*render_eval)(struct vec xi);

void cam_init(struct cam* cam, int w, int h);
void cam_destroy(struct cam* cam);

void cam_clear(struct cam* cam, struct color c);

int cam_sees(struct cam* cam, struct vec p);

void render_dot(struct cam* cam, struct vec p, struct color c);
void render_line(struct cam* cam, struct line l, struct color c);
void render_tri(struct cam* cam, struct tri t, struct color c);
