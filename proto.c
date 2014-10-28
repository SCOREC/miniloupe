#include "proto.h"
#include "socks.h"

static unsigned short endian_canary = 0xFF00;

static int is_big_endian(void)
{
  unsigned char* p = (unsigned char*)&endian_canary;
  return p[0];
}

void send_code(int fd, enum proto_code c)
{
  unsigned char ch = c;
  blocking_send(fd, &ch, 1);
}

static enum proto_code recv_code(int fd)
{
  unsigned char ch;
  blocking_recv(fd, &ch, 1);
  return ch;
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

static double convert_double(double x)
{
  if (!is_big_endian())
    return swap_double(x);
  else
    return x;
}

void send_double(int fd, double x)
{
  x = convert_double(x);
  blocking_send(fd, &x, sizeof(x));
}

double recv_double(int fd)
{
  double x;
  blocking_recv(fd, &x, sizeof(x));
  return convert_double(x);
}

void proto_main(int fd, handler const handlers[PROTO_CODES], void* u)
{
  enum proto_code c;
  do {
    c = recv_code(fd);
    handlers[c](u);
  } while (c != PROTO_STOP);
}
