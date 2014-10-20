#include "image.h"
#include "from_png.h"
#include "base.h"
#include <stdio.h>
#include <png.h>

void write_png(const char* filename, struct image* im)
{
  static const int bit_depth = 8;
  png_bytepp row_pointers = 0;
  int y;
  struct file* f = file_open(filename,"w");
  png_structp png_ptr = png_create_write_struct(
      PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_init_io(png_ptr,(FILE*)f);
  png_set_IHDR(png_ptr, info_ptr, (png_uint_32)im->w, (png_uint_32)im->h,
      bit_depth,PNG_COLOR_TYPE_RGB,PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
  REALLOC(row_pointers,im->h);
  for (y=0; y < im->h; ++y)
    row_pointers[y] = (png_bytep)(im->rows[y]);
  png_set_rows(png_ptr,info_ptr,row_pointers);
  png_write_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,NULL);
  png_destroy_write_struct(&png_ptr,&info_ptr);
  FREE(row_pointers);
  file_close(f);
}
