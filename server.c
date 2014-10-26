#include "socks.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv)
{
  struct server s;
  char message[40];
  server_init(&s, atoi(argv[1]));
  server_listen(&s);
  server_accept(&s);
  blocking_recv(s.fd, message, 40); 
  printf("got message:%s\n", message);
  server_close(&s);
  return 0;
}
