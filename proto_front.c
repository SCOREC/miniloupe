#include "proto.h"
#include "socks.h"
#include "base.h"

void send_code(int fd, enum proto_code c)
{
  unsigned char ch = c;
  blocking_send(fd, &ch, 1);
}

int send_code_unsafe(int fd, enum proto_code c)
{
  unsigned char ch = c;
  return blocking_send_unsafe(fd, &ch, 1);
}

void send_double(int fd, double x)
{
  x = convert_double(x);
  blocking_send(fd, &x, sizeof(x));
}

int recv_int(int fd)
{
  int value;
  blocking_recv(fd, &value, sizeof(value));
  return convert_int(value);
}

char* recv_string(int fd)
{
  int l;
  char* s = 0;
  l = recv_int(fd);
  REALLOC(s, l + 1);
  blocking_recv(fd, s, l);
  s[l] = '\0';
  return s;
}
