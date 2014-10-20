struct globe {
  struct vec center;
  double tilt;
  double spin;
};

extern struct globe const globe_ident;

struct frame globe_frame(struct globe* g);
void globe_pan(struct globe* g, double x, double y);
void globe_zoom(struct globe* g, double z);
void globe_tilt(struct globe* g, double by);
void globe_spin(struct globe* g, double by);
