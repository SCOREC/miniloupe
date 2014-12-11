#include "proto.h"
#include "socks.h"
#include "from_mpi.h"

static unsigned short endian_canary = 0xFF00;

static int is_big_endian(void)
{
  unsigned char* p = (unsigned char*)&endian_canary;
  return p[0];
}

static double swap_double(double in)
{
  unsigned char* p;
  unsigned char* q;
  double out;
  int i;
  p = (unsigned char*)&in;
  q = (unsigned char*)&out;
  for (i = 0; i < sizeof(double); ++i)
    q[sizeof(double) - i - 1] = p[i];
  return out;
}

double convert_double(double x)
{
  if (!is_big_endian())
    return swap_double(x);
  else
    return x;
}

static int swap_int(int in)
{
  unsigned char* p;
  unsigned char* q;
  int out;
  int i;
  p = (unsigned char*)&in;
  q = (unsigned char*)&out;
  for (i = 0; i < sizeof(int); ++i)
    q[sizeof(int) - i - 1] = p[i];
  return out;
}

int convert_int(int x)
{
  if (!is_big_endian())
    return swap_double(x);
  else
    return x;
}
