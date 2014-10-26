struct server {
  int port;
  int listenfd;
  int fd;
};

struct client {
  char* servname;
  int port;
  int fd;
};

void server_init(struct server* s, int port);
void server_listen(struct server* s);
void server_accept(struct server* s);
void server_close(struct server* s);
void client_init(struct client* c, const char* servname, int port);
void client_destroy(struct client* c);
void client_connect(struct client* c);
void blocking_send(int fd, void* data, unsigned size);
void blocking_recv(int fd, void* data, unsigned size);
