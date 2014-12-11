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

double convert_double(double x);
int convert_int(int x);
void send_code(int fd, enum proto_code c);
int send_code_unsafe(int fd, enum proto_code c);
void send_double(int fd, double x);
double recv_double(int fd);
int recv_int(int fd);
void send_int(int fd, int value);
void send_string(int fd, const char* s);
char* recv_string(int fd);
void proto_main(int fd, handler const handlers[PROTO_CODES], void* u);
