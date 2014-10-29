#include "proto.h"
#include "socks.h"
#include "from_mpi.h"

static enum proto_code recv_code(int fd)
{
  unsigned char ch;
  int ic;
  if (!rank_mpi()) {
    blocking_recv(fd, &ch, 1);
    ic = ch;
  }
  bcast_int(&ic);
  return ic;
}

double recv_double(int fd)
{
  double x;
  if (!rank_mpi()) {
    blocking_recv(fd, &x, sizeof(x));
    x = convert_double(x);
  }
  bcast_double(&x);
  return x;
}

void proto_main(int fd, handler const handlers[PROTO_CODES], void* u)
{
  enum proto_code c;
  do {
    c = recv_code(fd);
    handlers[c](u);
  } while (c != PROTO_STOP);
}

