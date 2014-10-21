#ifndef MILO_H
#define MILO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct milo* milo_t;

milo_t milo_new(int w, int h);
void milo_clear(milo_t m, double* color);
void milo_dot(milo_t m, double* point, double* color);
void milo_line(milo_t m, double* point_a, double* point_b, double* color);
void milo_triangle(milo_t m, double* point_a, double* point_b, double* point_b,
    double* color);
void milo_zoom(milot_t m, double factor);
void milo_pan(milot_t m, double x, double y);
void milo_tilt(milot_t m, double radians);
void milo_spin(milot_t m, double radians);
void milo_render(milo_t m);
void milo_write_png(milo_t m, const char* filename);
void milo_free(milo_t m);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
