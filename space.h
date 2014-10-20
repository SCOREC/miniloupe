struct vec { double x,y,z; };
struct mat { struct vec x,y,z; };
struct line { struct vec a,b; };
struct tri { struct vec a,b,c; };
struct tet { struct vec a,b,c,d; };
struct plane { struct vec n; double r; };
struct frame { struct mat r; struct vec t; };

extern struct vec const vec_zero;
extern struct mat const mat_ident;
extern struct frame const frame_ident;
extern double const pi;

struct vec vec_add(struct vec a, struct vec b);
struct vec vec_sub(struct vec a, struct vec b);
struct vec vec_mul_s(struct vec v, double s);
double vec_mag(struct vec v);
struct vec vec_norm(struct vec v);
struct vec vec_new(double x, double y, double z);

struct mat mat_rot(struct vec u, double t);

struct vec frame_mul_vec(struct frame f, struct vec v);
struct frame frame_rot(struct vec u, double t);
struct frame frame_trans(struct vec v);
struct frame frame_scale(double s);
struct frame frame_mul_frame(struct frame a, struct frame b);

struct vec tri_norm(struct tri t);
struct tri frame_mul_tri(struct frame f, struct tri t);

int plane_has(struct plane pl, struct vec pt);
int plane_clip_line(struct plane pl, struct line* l);
int plane_clip_tri(struct plane pl, struct tri t, struct tri o[2]);
