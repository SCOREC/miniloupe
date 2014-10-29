void start_mpi(void);
void stop_mpi(void);
int rank_mpi(void);
int size_mpi(void);
void bcast_int(int* x);
void bcast_double(double* x);
struct drawing;
void reduce_drawing_mpi(struct drawing* d);
