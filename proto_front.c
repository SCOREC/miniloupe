#include "proto.h"
#include "socks.h"

void send_code(int fd, enum proto_code c)
{
  unsigned char ch = c;
  blocking_send(fd, &ch, 1);
}

void send_double(int fd, double x)
{
  x = convert_double(x);
  blocking_send(fd, &x, sizeof(x));
}

