#include "socks.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
  struct client c;
  char message[40];
  client_init(&c, argv[1], atoi(argv[2]));
  client_connect(&c);
  memset(message, 0, 40);
  snprintf(message, 40, "check one two");
  blocking_send(c.fd, message, 40);
  client_close(&c);
  client_destroy(&c);
  return 0;
}
