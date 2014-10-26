#include "socks.h"
#include "base.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>

void server_init(struct server* s, int port)
{
  s->port = port;
  s->listenfd = -1;
  s->fd = -1;
}

void server_listen(struct server* s)
{
  struct sockaddr_in serv_addr;
  int err;
  s->listenfd = socket(AF_INET, SOCK_STREAM, 0);
  EXCEPT_IF(s->listenfd == -1);
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(s->port);
  err = bind(s->listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  EXCEPT_IF(err);
  err = listen(s->listenfd, 1);
  EXCEPT_IF(err == -1);
}

void server_accept(struct server* s)
{
  s->fd = accept(s->listenfd, NULL, NULL);
  EXCEPT_IF(s->fd == -1);
  close(s->listenfd);
  s->listenfd = -1;
}

void server_close(struct server* s)
{
  close(s->fd);
  s->fd = -1;
}

void client_init(struct client* c, const char* servname, int port)
{
  c->servname = 0;
  REALLOC(c->servname, strlen(servname) + 1);
  strcpy(c->servname, servname);
  c->port = port;
  c->fd = -1;
}

void client_destroy(struct client* c)
{
  FREE(c->servname);
}

void client_connect(struct client* c)
{
  struct hostent* server;
  struct sockaddr_in serv_addr;
  int err;
  c->fd = socket(AF_INET, SOCK_STREAM, 0);
  EXCEPT_IF(c->fd == -1);
  server = gethostbyname(c->servname);
  EXCEPT_IF(server == NULL);
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  serv_addr.sin_port = htons(c->port);
  err = connect(c->fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  EXCEPT_IF(err < 0);
}

void blocking_send(int fd, void* data, unsigned size)
{
  char* p;
  ssize_t written;
  p = data;
  while (size) {
    errno = 0;
    written = write(fd, p, size);
    if (written != -1) {
      size -= written;
      p += written;
    } else {
      EXCEPT_IF(errno != EAGAIN);
    }
  }
}

void blocking_recv(int fd, void* data, unsigned size)
{
  char* p;
  ssize_t nread;
  p = data;
  while (size) {
    errno = 0;
    nread = read(fd, p, size);
    if (nread != -1) {
      size -= nread;
      p += nread;
    } else {
      EXCEPT_IF(errno != EAGAIN);
    }
  }
}

