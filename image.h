struct color { unsigned char r,g,b; };

struct image {
  int w,h;
  struct color** rows;
};

struct drawing {
  struct image im;
  double** z_rows;
};

struct pix { int x,y; };

struct vox { int x,y,z; };

struct dot {
  struct pix p;
  double z;
};

struct dline {
  struct dot a,b;
};

struct dtri {
  struct dot a,b,c;
};

void image_init(struct image* im, int w, int h);
void image_destroy(struct image* im);

void image_fill(struct image* im, struct color c);

void drawing_init(struct drawing* d, int w, int h);
void drawing_destroy(struct drawing* d);

void drawing_clear(struct drawing* d, struct color c);

struct pix pix_new(int x, int y);

void draw_dot(struct drawing* dr, struct dot d, struct color c);
void draw_thick_dot(struct drawing* dr, struct dot d, struct color c);
void draw_line(struct drawing* dr, struct dline l, struct color c);
void draw_thick_line(struct drawing* dr, struct dline l, struct color c);
void draw_tri(struct drawing* dr, struct dtri t, struct color c);
void draw_tri_lines(struct drawing* dr, struct dtri t, struct color c);
void draw_text(struct drawing* im, struct pix p,
    const char* s, struct color c);
