#include "base.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void die(const char* why, ...)
{
  va_list ap;
  va_start(ap, why);
  vfprintf(stderr, why, ap);
  fprintf(stderr,"\n");
  abort();
}

void bug(const char* cond, const char* file, unsigned line)
{
  die("bug in %s:%u : %s", file, line, cond);
}

void except(const char* cond, const char* file, unsigned line)
{
  die("exception in %s:%u : %s", file, line, cond);
}

void* alloc(void* from, long bytes)
{
  void* new_memory;
  BUG_IF(bytes < 0);
  if (!bytes) {
    free(from);
    return 0;
  }
  new_memory = realloc(from, (size_t)bytes);
  EXCEPT_IF(!new_memory);
  return new_memory;
}

int growth(int n)
{
  return ((n + 1) * 3) / 2;
}

struct file* file_open(const char* path, const char* mode)
{
  FILE* f;
  f = fopen(path,mode);
  if (!f)
    die("could not open file \"%s\" mode \"%s\"",path,mode);
  return (struct file*)f;
}

void file_close(struct file* f)
{
  fclose((FILE*)f);
}

void file_read(struct file* f, long bytes, void *ptr)
{
  size_t bytes_read;
  size_t bytes_to_read;
  BUG_IF(bytes < 0);
  bytes_to_read = (size_t)bytes;
  bytes_read = fread(ptr, 1, bytes_to_read, (FILE*)f);
  EXCEPT_IF(bytes_read != bytes_to_read);
}

void file_print(struct file* f, const char* format, ...)
{
  va_list ap;
  int ret;
  va_start(ap, format);
  ret = vfprintf((FILE*)f, format, ap);
  EXCEPT_IF(ret < 0);
}

void debug(const char* format, ...)
{
  va_list ap;
  int ret;
  va_start(ap, format);
  ret = vfprintf(stderr,format,ap);
  EXCEPT_IF(ret < 0);
}
