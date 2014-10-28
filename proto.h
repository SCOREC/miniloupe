enum proto_code {
  PROTO_STOP,
  PROTO_ZOOM,
  PROTO_PAN,
  PROTO_SPIN,
  PROTO_TILT,
  PROTO_RENDER,
  PROTO_CODES
};

typedef void (*handler)(void* u);

double convertd(double x);
void send_code(int fd, enum proto_code c);
void send_double(int fd, double x);
double recv_double(int fd);
void proto_main(int fd, handler const handlers[PROTO_CODES], void* u);
