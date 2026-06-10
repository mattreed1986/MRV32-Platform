/* ============================================================================
 * minilib.h — Minimal C library for RISCVRAM kernel
 *
 * Provides just enough libc to run the Kilo editor:
 *   - Types and constants
 *   - String/memory functions
 *   - A simple heap allocator (static arena, first-fit)
 *   - Minimal snprintf/vsnprintf (%d, %u, %x, %s, %c, %%)
 *   - File I/O wrappers over raw syscalls
 *   - Character classification (isdigit, isprint, etc.)
 *   - Minimal errno
 * ============================================================================ */

#ifndef MINILIB_H
#define MINILIB_H

#include "syscalls.h"

/* ---- Fundamental types --------------------------------------------------- */

typedef unsigned long  size_t;
typedef long           ssize_t;
typedef long           off_t;
typedef long           time_t;

#define NULL ((void *)0)

#define UINT32_MAX 0xFFFFFFFFU
#define INT_MAX    0x7FFFFFFF

/* ---- File descriptor constants ------------------------------------------- */

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 1

/* ---- open() flags (mapped to your kernel's create_flag convention) -------- */

#define O_RDONLY 0
#define O_RDWR   0
#define O_CREAT  1  /* pass 1 to sys_open create_flag */

/* ---- errno --------------------------------------------------------------- */

extern int errno;

#define ENOENT  2
#define ENOTTY 25
#define EAGAIN 11

/* ---- va_list (compiler built-in) ----------------------------------------- */

typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_end(ap)         __builtin_va_end(ap)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)

/* ---- Character classification -------------------------------------------- */

static inline int isdigit(int c) { return c >= '0' && c <= '9'; }
static inline int isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
static inline int isalnum(int c) { return isalpha(c) || isdigit(c); }
static inline int isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
           c == '\f' || c == '\v';
}
static inline int isprint(int c) { return c >= 0x20 && c <= 0x7E; }
static inline int iscntrl(int c) { return (c >= 0 && c < 0x20) || c == 0x7F; }
static inline int isupper(int c) { return c >= 'A' && c <= 'Z'; }
static inline int islower(int c) { return c >= 'a' && c <= 'z'; }
static inline int toupper(int c) { return islower(c) ? c - 32 : c; }
static inline int tolower(int c) { return isupper(c) ? c + 32 : c; }

/* ---- String functions ---------------------------------------------------- */

size_t strlen(const char *s);
char  *strcpy(char *dst, const char *src);
char  *strncpy(char *dst, const char *src, size_t n);
int    strcmp(const char *a, const char *b);
int    strncmp(const char *a, const char *b, size_t n);
char  *strchr(const char *s, int c);
char  *strrchr(const char *s, int c);
char  *strstr(const char *hay, const char *needle);
char  *strdup(const char *s);
char  *strerror(int errnum);

/* ---- Memory functions ---------------------------------------------------- */

void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int   memcmp(const void *a, const void *b, size_t n);

/* ---- Heap allocator ------------------------------------------------------ */

#define HEAP_SIZE (32 * 1024)   /* 512 KB arena — tune for your system */

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void  free(void *ptr);

/* ---- Formatted output ---------------------------------------------------- */

int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int snprintf(char *buf, size_t size, const char *fmt, ...);

/* ---- Minimal stdio (just enough for kilo) -------------------------------- */

/* We don't implement real FILE*-based stdio.  editorOpen() is rewritten to
   use raw syscalls.  These wrappers exist for the few remaining call sites
   (perror, printf used in error paths). */

void  mini_perror(const char *prefix);
void  mini_printf(const char *fmt, ...);
void  mini_fprintf_stderr(const char *fmt, ...);

/* Map the standard names so the rest of the code compiles. */
#define perror(s)         mini_perror(s)
#define printf(...)       mini_printf(__VA_ARGS__)
#define fprintf(f, ...)   mini_fprintf_stderr(__VA_ARGS__)

/* ---- File I/O wrappers --------------------------------------------------- */

/* These thin wrappers translate POSIX-style calls into your syscalls. */

int   open(const char *path, int create);  /* ignores mode */
int   close(int fd);
long  lseek(int fd, long offset, int whence);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int   ftruncate(int fd, off_t length);

/* ---- Miscellaneous ------------------------------------------------------- */

void  exit(int status);
int   atexit(void (*func)(void));
time_t time(time_t *t);        /* returns a simple tick counter */

/* sscanf — only supports the "%d;%d" pattern kilo needs. */
int   mini_sscanf(const char *str, const char *fmt, ...);
#define sscanf mini_sscanf

/* isatty — always returns 1, we assume fd 0 is a terminal */
static inline int isatty(int fd) { (void)fd; return 1; }

#endif /* MINILIB_H */
