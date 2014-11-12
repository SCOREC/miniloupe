#include "image.h"
#include "from_mpi.h"
#include "base.h"
#include <mpi.h>
#include <string.h>

void start_mpi(void)
{
  MPI_Init(0,0);
}

void stop_mpi(void)
{
  MPI_Finalize();
}

int rank_mpi(void)
{
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  return rank;
}

int size_mpi(void)
{
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  return size;
}

void bcast_int(int* x)
{
  MPI_Bcast(x, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void bcast_double(double* x)
{
  MPI_Bcast(x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

static int pixels;

static double* reduce_drawing_z(void* buf)
{
  return (double*)(((char*)buf) + pixels * 3);
}

static void reduce_drawing_op(void* a, void* b, int* len, MPI_Datatype* dt)
{
  struct color* ac;
  struct color* bc;
  double* az;
  double* bz;
  int i;
  BUG_IF(*len != 1);
  ac = a;
  bc = b;
  az = reduce_drawing_z(a);
  bz = reduce_drawing_z(b);
  for (i = 0; i < pixels; ++i) {
    if (az[i] < bz[i]) {
      bz[i] = az[i];
      bc[i] = ac[i];
    }
  }
}

static int drawing_reduce_bytes(void)
{
  return pixels * (3 + sizeof(double));
}

static void* alloc_drawing_reduce_buf(void)
{
  void* buf = 0;
  REALLOC(buf, drawing_reduce_bytes());
  return buf;
}

static void pack_drawing_reduce(struct drawing* d, void* buf)
{
  double* zp;
  memcpy(buf, &d->im.rows[0][0], pixels * 3);
  zp = reduce_drawing_z(buf);
  memcpy(zp, &d->z_rows[0][0], pixels * sizeof(double));
}

static void unpack_drawing_reduce(struct drawing* d, void* buf)
{
  double* zp;
  memcpy(&d->im.rows[0][0], buf, pixels * 3);
  zp = reduce_drawing_z(buf);
  memcpy(&d->z_rows[0][0], zp, pixels * sizeof(double));
}

static void prepare_drawing_for_mpi(struct drawing* d,
    MPI_Datatype* type, void** sendbuf, MPI_Op* op)
{
  pixels = d->im.w * d->im.h;
  MPI_Type_contiguous(drawing_reduce_bytes(), MPI_BYTE, type);
  MPI_Type_commit(type);
  *sendbuf = alloc_drawing_reduce_buf();
  pack_drawing_reduce(d, *sendbuf);
  MPI_Op_create(reduce_drawing_op, 1, op);
}

void reduce_drawing_mpi(struct drawing* d)
{
  MPI_Datatype type;
  void* sendbuf;
  void* recvbuf;
  MPI_Op op;
  prepare_drawing_for_mpi(d, &type, &sendbuf, &op);
  if (!rank_mpi()) {
    recvbuf = alloc_drawing_reduce_buf();
  } else {
    recvbuf = 0;
  }
  MPI_Reduce(sendbuf, recvbuf, 1, type, op, 0, MPI_COMM_WORLD);
  if (!rank_mpi()) {
    unpack_drawing_reduce(d, recvbuf);
    FREE(recvbuf);
  }
  MPI_Type_free(&type);
  MPI_Op_free(&op);
}

void max_doubles_mpi(double* x, int n)
{
  double* tmp = 0;
  REALLOC(tmp, n);
  MPI_Allreduce(x, tmp, n, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  memcpy(x, tmp, n * sizeof(double));
  FREE(tmp);
}

void min_doubles_mpi(double* x, int n)
{
  double* tmp = 0;
  REALLOC(tmp, n);
  MPI_Allreduce(x, tmp, n, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
  memcpy(x, tmp, n * sizeof(double));
  FREE(tmp);
}
