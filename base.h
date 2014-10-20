void die(const char* why, ...) __attribute__((noreturn,format(printf,1,2)));

void bug(const char* cond, const char* file, unsigned line)
  __attribute__((noreturn));
#ifdef NDEBUG
#define BUG_IF(cond) ((void)0)
#else
#define BUG_IF(cond) ((cond) ? \
    bug(#cond, __FILE__, __LINE__) : ((void)0))
#endif

void except(const char* cond, const char* file, unsigned line)
  __attribute__((noreturn));
#ifdef NEXCEPT
#define EXCEPT_IF(cond) ((void)0)
#else
#define EXCEPT_IF(cond) ((cond) ? \
    except(#cond, __FILE__, __LINE__) : ((void)0))
#endif

void* alloc(void* from, long bytes);
#define ALLOC(p) ((p)=alloc(0,sizeof(*(p))))
#define REALLOC(p,n) ((p)=alloc(p,(n)*sizeof(*(p))))
#define FREE(p) ((p)=alloc(p,0))

int growth(int n);

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(*(a)))

struct file* file_open(const char* path, const char* mode);
void file_close(struct file* f);
void file_read(struct file* f, long b, void *ptr);
void file_print(struct file* f, const char* format, ...)
  __attribute__((format(printf,2,3)));

#define ABS(x) (((x)<0)?(-(x)):(x))
#define MIN(a,b) (((b)<(a))?(b):(a))
#define MAX(a,b) (((b)>(a))?(b):(a))

void debug(const char* format, ...) __attribute__((format(printf,1,2)));
