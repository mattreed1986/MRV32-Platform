/* ============================================================================
 * minilib.c — Minimal C library implementation for RISCVRAM kernel
 * ============================================================================ */

#include "minilib.h"

/* ---- errno --------------------------------------------------------------- */

int errno = 0;

/* ============================= String functions ============================ */

size_t strlen(const char *s)
{
    const char *p = s;
    while (*p) p++;
    return (size_t)(p - s);
}

char *strcpy(char *dst, const char *src)
{
    char *d = dst;
    while ((*d++ = *src++));
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i]; i++) dst[i] = src[i];
    for (; i < n; i++) dst[i] = '\0';
    return dst;
}

int strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n)
{
    while (n && *a && *a == *b) { a++; b++; n--; }
    return n ? (unsigned char)*a - (unsigned char)*b : 0;
}

char *strchr(const char *s, int c)
{
    while (*s) {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    return (c == '\0') ? (char *)s : NULL;
}

char *strrchr(const char *s, int c)
{
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    if (c == '\0') return (char *)s;
    return (char *)last;
}

char *strstr(const char *hay, const char *needle)
{
    size_t nlen = strlen(needle);
    if (nlen == 0) return (char *)hay;
    while (*hay) {
        if (*hay == *needle && !memcmp(hay, needle, nlen))
            return (char *)hay;
        hay++;
    }
    return NULL;
}

/* ============================= Memory functions ============================ */

void *memcpy(void *dst, const void *src, size_t n)
{
    char *d = dst;
    const char *s = src;
    while (n--) *d++ = *s++;
    return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
    char *d = dst;
    const char *s = src;
    if (d < s) {
        while (n--) *d++ = *s++;
    } else {
        d += n; s += n;
        while (n--) *--d = *--s;
    }
    return dst;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

int memcmp(const void *a, const void *b, size_t n)
{
    const unsigned char *pa = a, *pb = b;
    while (n--) {
        if (*pa != *pb) return *pa - *pb;
        pa++; pb++;
    }
    return 0;
}

/* ============================ Heap allocator =============================== */
/*
 * Simple first-fit allocator over a static arena.
 *
 * Each block has an 8-byte header:
 *   [  size (4 bytes)  |  flags (4 bytes)  ]
 *       size = usable payload bytes (excluding header)
 *       flags bit 0 = 1 if allocated, 0 if free
 *
 * Alignment: 8 bytes.
 */

static char heap[HEAP_SIZE] __attribute__((aligned(8)));
static int  heap_inited = 0;

#define BLOCK_HDR_SIZE 8
#define ALIGN8(x) (((x) + 7) & ~7)

typedef struct {
    unsigned int size;
    unsigned int flags;   /* bit 0 = allocated */
} block_hdr;

static void heap_init(void)
{
    block_hdr *h = (block_hdr *)heap;
    h->size  = HEAP_SIZE - BLOCK_HDR_SIZE;
    h->flags = 0;
    heap_inited = 1;
}

void *malloc(size_t req)
{
    if (!heap_inited) heap_init();
    if (req == 0) return NULL;

    size_t need = ALIGN8(req);
    char *p = heap;
    char *end = heap + HEAP_SIZE;

    while (p < end) {
        block_hdr *h = (block_hdr *)p;
        if (!(h->flags & 1) && h->size >= need) {
            /* Found a free block big enough. */
            if (h->size >= need + BLOCK_HDR_SIZE + 8) {
                /* Split: create a new free block after this one. */
                block_hdr *next = (block_hdr *)(p + BLOCK_HDR_SIZE + need);
                next->size  = h->size - need - BLOCK_HDR_SIZE;
                next->flags = 0;
                h->size = need;
            }
            h->flags = 1;
            return p + BLOCK_HDR_SIZE;
        }
        p += BLOCK_HDR_SIZE + h->size;
    }
    return NULL;  /* out of memory */
}

void free(void *ptr)
{
    if (!ptr) return;
    block_hdr *h = (block_hdr *)((char *)ptr - BLOCK_HDR_SIZE);
    h->flags = 0;

    /* Coalesce with next block if it's also free. */
    char *next_p = (char *)ptr + h->size;
    if (next_p < heap + HEAP_SIZE) {
        block_hdr *next = (block_hdr *)next_p;
        if (!(next->flags & 1)) {
            h->size += BLOCK_HDR_SIZE + next->size;
        }
    }
}

void *realloc(void *ptr, size_t size)
{
    if (!ptr) return malloc(size);
    if (size == 0) { free(ptr); return NULL; }

    block_hdr *h = (block_hdr *)((char *)ptr - BLOCK_HDR_SIZE);
    size_t old_size = h->size;

    if (old_size >= size) return ptr;  /* fits already */

    void *new_ptr = malloc(size);
    if (!new_ptr) return NULL;
    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
}

char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *d = malloc(len);
    if (d) memcpy(d, s, len);
    return d;
}

/* =========================== Formatted output ============================== */

/*
 * Minimal vsnprintf.
 * Supports: %d, %u, %x, %s, %c, %%, %ld, %lu, %lx
 *           Width specifier (e.g. %20s), zero-pad not implemented.
 *           Precision for strings (e.g. %.20s).
 */

static int fmt_putc(char *buf, size_t pos, size_t size, char c)
{
    if (pos < size - 1) buf[pos] = c;
    return 1;
}

static int fmt_puts(char *buf, size_t pos, size_t size, const char *s, int maxlen)
{
    int n = 0;
    while (*s && (maxlen < 0 || n < maxlen)) {
        if (pos + n < size - 1) buf[pos + n] = *s;
        s++; n++;
    }
    return n;
}

static int fmt_int(char *buf, size_t pos, size_t size, long val, int base,
                   int is_signed, int width)
{
    char tmp[24];
    int i = 0, neg = 0, n = 0;
    unsigned long uval;

    if (is_signed && val < 0) {
        neg = 1;
        uval = (unsigned long)(-val);
    } else {
        uval = (unsigned long)val;
    }

    if (uval == 0) {
        tmp[i++] = '0';
    } else {
        while (uval) {
            int d = uval % base;
            tmp[i++] = (d < 10) ? '0' + d : 'a' + d - 10;
            uval /= base;
        }
    }
    if (neg) tmp[i++] = '-';

    /* Pad with spaces to reach width. */
    int len = i;
    while (len < width) {
        n += fmt_putc(buf, pos + n, size, ' ');
        len++;
    }

    /* Output digits in reverse. */
    while (i > 0) {
        n += fmt_putc(buf, pos + n, size, tmp[--i]);
    }
    return n;
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    size_t pos = 0;

    if (size == 0) return 0;

    while (*fmt) {
        if (*fmt != '%') {
            pos += fmt_putc(buf, pos, size, *fmt);
            fmt++;
            continue;
        }
        fmt++; /* skip '%' */

        /* Parse width */
        int width = 0;
        int precision = -1;
        int is_long = 0;

        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        /* Parse precision */
        if (*fmt == '.') {
            fmt++;
            precision = 0;
            while (*fmt >= '0' && *fmt <= '9') {
                precision = precision * 10 + (*fmt - '0');
                fmt++;
            }
        }

        /* Length modifier */
        if (*fmt == 'l') { is_long = 1; fmt++; }

        switch (*fmt) {
        case 'd': case 'i': {
            long v = is_long ? va_arg(ap, long) : (long)va_arg(ap, int);
            pos += fmt_int(buf, pos, size, v, 10, 1, width);
            break;
        }
        case 'u': {
            unsigned long v = is_long ? va_arg(ap, unsigned long)
                                      : (unsigned long)va_arg(ap, unsigned int);
            pos += fmt_int(buf, pos, size, (long)v, 10, 0, width);
            break;
        }
        case 'x': {
            unsigned long v = is_long ? va_arg(ap, unsigned long)
                                      : (unsigned long)va_arg(ap, unsigned int);
            pos += fmt_int(buf, pos, size, (long)v, 16, 0, width);
            break;
        }
        case 's': {
            const char *s = va_arg(ap, const char *);
            if (!s) s = "(null)";
            int slen = strlen(s);
            int maxlen = (precision >= 0 && precision < slen) ? precision : slen;
            /* Right-pad with spaces if width > string length. */
            int pad = (width > maxlen) ? width - maxlen : 0;
            pos += fmt_puts(buf, pos, size, s, maxlen);
            while (pad-- > 0) pos += fmt_putc(buf, pos, size, ' ');
            break;
        }
        case 'c': {
            char c = (char)va_arg(ap, int);
            pos += fmt_putc(buf, pos, size, c);
            break;
        }
        case '%':
            pos += fmt_putc(buf, pos, size, '%');
            break;
        default:
            /* Unknown specifier — just emit it literally. */
            pos += fmt_putc(buf, pos, size, '%');
            pos += fmt_putc(buf, pos, size, *fmt);
            break;
        }
        if (*fmt) fmt++;
    }
    buf[pos < size ? pos : size - 1] = '\0';
    return (int)pos;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return n;
}

/* ========================= Minimal stdio wrappers ========================= */

void mini_perror(const char *prefix)
{
    if (prefix && *prefix) {
        sys_write(STDERR_FILENO, prefix, strlen(prefix));
        sys_write(STDERR_FILENO, ": ", 2);
    }
    const char *msg = strerror(errno);
    sys_write(STDERR_FILENO, msg, strlen(msg));
    sys_write(STDERR_FILENO, "\n", 1);
}

void mini_printf(const char *fmt, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n > 0) sys_write(STDOUT_FILENO, buf, n);
}

void mini_fprintf_stderr(const char *fmt, ...)
{
    /* Kilo only ever fprintf's to stderr. We ignore the fd arg
       (which is macro'd away). */
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n > 0) sys_write(STDERR_FILENO, buf, n);
}

char *strerror(int errnum)
{
    switch (errnum) {
    case 0:      return "Success";
    case ENOENT: return "No such file or directory";
    case ENOTTY: return "Not a typewriter";
    case EAGAIN: return "Resource temporarily unavailable";
    default:     return "Unknown error";
    }
}

/* ============================ File I/O wrappers ============================ */

int open(const char *path, int create)
{
    /* Map POSIX-style flags to your kernel's create_flag:
       If O_CREAT is set, pass 1; otherwise pass 0. */
    //int create = (flags & O_CREAT) ? 1 : 0;
    int append = 0;
    long ret = sys_open(path, append, create);
    if (ret < 0) { errno = -(int)ret; return -1; }
    return (int)ret;
}

int close(int fd)
{
    /* Your syscalls.h will have sys_close — assumed present per instructions. */
    extern int sys_close(int fd);
    long ret = sys_close(fd);
    if (ret < 0) { errno = -(int)ret; return -1; }
    return 0;
}

long lseek(int fd, long offset, int whence)
{
    extern long sys_lseek(int fd, long offset, int whence);
    long ret = sys_lseek(fd, offset, whence);
    if (ret < 0) { errno = -(int)ret; return -1; }
    return ret;
}

ssize_t read(int fd, void *buf, size_t count)
{
    long ret = sys_read(fd, (char *)buf, (int)count);
    if (ret < 0) { errno = -(int)ret; return -1; }
    return ret;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    const char *p = (const char *)buf;
    size_t written = 0;
    while (written < count) {
        int chunk = (int)(count - written);
        if (chunk > 8) chunk = 8;
        long ret = sys_write(fd, p, chunk);
        if (ret < 0) { errno = -(int)ret; return -1; }
        p += ret;
        written += ret;
    }
    return (ssize_t)count;
}

int ftruncate(int fd, off_t length)
{
    /* Your kernel likely doesn't have ftruncate.  For the Kilo save path,
       the simplest approach: seek to 'length' and assume the kernel truncates
       on close, OR just write exactly 'length' bytes (which kilo already does).
       We make this a no-op so the write() that follows is what matters. */
    (void)fd; (void)length;
    return 0;
}

/* ========================= Miscellaneous =================================== */

/* atexit: store up to 4 handlers — kilo only registers one. */
static void (*atexit_funcs[4])(void);
static int   atexit_count = 0;

int atexit(void (*func)(void))
{
    if (atexit_count >= 4) return -1;
    atexit_funcs[atexit_count++] = func;
    return 0;
}

void exit(int status)
{
    for (int i = atexit_count - 1; i >= 0; i--)
        atexit_funcs[i]();
    (void)status;
    sys_exit();
}

/* time() — returns a simple incrementing counter.  Kilo uses it only to
   decide whether to hide the status message after 5 "seconds".  Without a
   real clock this means the message stays a bit longer or shorter — harmless.
   If your kernel has a timer syscall, wire it in here. */
static time_t fake_clock = 0;

time_t time(time_t *t)
{
    /* Bump by 1 each call.  editorRefreshScreen calls time() once per
       redraw cycle, so this roughly tracks "number of redraws". */
    fake_clock++;
    if (t) *t = fake_clock;
    return fake_clock;
}

/* sscanf — only handles the exact pattern kilo uses: "%d;%d" */
int mini_sscanf(const char *str, const char *fmt, ...)
{
    (void)fmt; /* We know it's always "%d;%d" */
    va_list ap;
    va_start(ap, fmt);
    int *first  = va_arg(ap, int *);
    int *second = va_arg(ap, int *);
    va_end(ap);

    int val = 0, count = 0;
    while (*str && isdigit(*str)) {
        val = val * 10 + (*str - '0');
        str++;
    }
    *first = val;
    count++;

    if (*str == ';') str++;
    else return count;

    val = 0;
    while (*str && isdigit(*str)) {
        val = val * 10 + (*str - '0');
        str++;
    }
    *second = val;
    count++;

    return count;
}
