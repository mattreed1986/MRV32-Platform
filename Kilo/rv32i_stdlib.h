/*
 * rv32i_stdlib.h — I/O helpers, string utilities, number conversion,
 *                  and software integer math for RISC-V RV32I
 *
 * Depends on: syscalls.h (for sys_read, sys_write, sys_exit)
 *
 * Fully self-contained: no libc, no libgcc. All multiply, divide, and
 * modulo operations use software shift-and-add / long-division so that
 * no __mulsi3 / __udivsi3 / __umodsi3 symbols are needed.
 *
 * Compile with your existing Makefile — just #include this after syscalls.h.
 */

#ifndef RV32I_STDLIB_H
#define RV32I_STDLIB_H

#include "syscalls.h"

/* ─────────────────────────────────────────────
 * Types (freestanding — no stdint.h)
 * ───────────────────────────────────────────── */

#ifndef NULL
#define NULL ((void *)0)
#endif

/* File descriptors */
#define STDIN  0
#define STDOUT 1
#define STDERR 2

/* ─────────────────────────────────────────────
 * String Utilities
 * ───────────────────────────────────────────── */

static inline size_t strlen(const char *s)
{
    size_t n = 0;
    while (s[n]) n++;
    return n;
}


static inline int strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return *(const uint8_t *)a - *(const uint8_t *)b;
}

static inline char *strcpy(char *dst, const char *src)
{
    char *d = dst;
    while ((*d++ = *src++));
    return dst;
}

static inline char *strcat(char *dst, const char *src)
{
    char *d = dst + strlen(dst);
    while ((*d++ = *src++));
    return dst;
}

/* ─────────────────────────────────────────────
 * Output Helpers
 * ───────────────────────────────────────────── */

/* Write a null-terminated string to stdout. */
static inline void print(const char *s)
{
    sys_write(STDOUT, s, strlen(s));
}

/* Write a null-terminated string to stderr. */
static inline void eprint(const char *s)
{
    sys_write(STDERR, s, strlen(s));
}

/* Write a single character to stdout. */


/* Write a string followed by \r\n to stdout (UART line ending). */
static inline void println(const char *s)
{
    print(s);
    sys_write(STDOUT, "\r\n", 2);
}

/* ─────────────────────────────────────────────
 * Integer Math (software — no M extension, no libgcc)
 *
 * This section must appear before Number Conversion
 * because itoa/utoa/atoi depend on these functions.
 *
 * IMPORTANT: These use ONLY addition, subtraction, shifts,
 * and bitwise ops — never C's * / % operators — so GCC
 * will not emit __mulsi3 / __udivsi3 / __umodsi3 calls.
 * ───────────────────────────────────────────── */

static inline int32_t abs_i32(int32_t x)
{
    return x < 0 ? -x : x;
}

static inline int32_t min_i32(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

static inline int32_t max_i32(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

static inline uint32_t min_u32(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

static inline uint32_t max_u32(uint32_t a, uint32_t b)
{
    return a > b ? a : b;
}

static inline int32_t clamp_i32(int32_t val, int32_t lo, int32_t hi)
{
    return val < lo ? lo : (val > hi ? hi : val);
}

/*
 * Software unsigned multiply. Shift-and-add algorithm.
 * Replaces the * operator to avoid __mulsi3.
 */
static __attribute__((noinline)) uint32_t mul_u32(uint32_t a, uint32_t b)
{
    uint32_t result = 0;
    while (b) {
        if (b & 1) result += a;
        a <<= 1;
        b >>= 1;
    }
    return result;
}

/* Signed multiply wrapper. */
static inline int32_t mul_i32(int32_t a, int32_t b)
{
    int neg = 0;
    uint32_t ua, ub;

    if (a < 0) { neg ^= 1; ua = (uint32_t)(-a); } else { ua = (uint32_t)a; }
    if (b < 0) { neg ^= 1; ub = (uint32_t)(-b); } else { ub = (uint32_t)b; }

    uint32_t result = mul_u32(ua, ub);
    return neg ? -(int32_t)result : (int32_t)result;
}

/*
 * Software unsigned division. Returns quotient.
 * Long-division algorithm using only shifts and subtraction.
 * Replaces the / operator to avoid __udivsi3.
 * Divisor of 0 returns 0xFFFFFFFF.
 */
static __attribute__((noinline)) uint32_t div_u32(uint32_t num, uint32_t den)
{
    if (den == 0) return 0xFFFFFFFF;

    uint32_t quot = 0;
    uint32_t rem  = 0;
    int i;

    for (i = 31; i >= 0; i--) {
        rem = (rem << 1) | ((num >> i) & 1);
        if (rem >= den) {
            rem -= den;
            quot |= (1U << i);
        }
    }
    return quot;
}

/*
 * Software unsigned modulo. Returns remainder.
 * Replaces the % operator to avoid __umodsi3.
 */
static __attribute__((noinline)) uint32_t mod_u32(uint32_t num, uint32_t den)
{
    if (den == 0) return num;

    uint32_t rem = 0;
    int i;

    for (i = 31; i >= 0; i--) {
        rem = (rem << 1) | ((num >> i) & 1);
        if (rem >= den) rem -= den;
    }
    return rem;
}

/* Signed division. */
static inline int32_t div_i32(int32_t num, int32_t den)
{
    if (den == 0) return -1;
    int neg = 0;
    uint32_t unum, uden;

    if (num < 0) { neg ^= 1; unum = (uint32_t)(-num); } else { unum = (uint32_t)num; }
    if (den < 0) { neg ^= 1; uden = (uint32_t)(-den); } else { uden = (uint32_t)den; }

    uint32_t q = div_u32(unum, uden);
    return neg ? -(int32_t)q : (int32_t)q;
}

/* Signed modulo (sign follows dividend, matching C99). */
static inline int32_t mod_i32(int32_t num, int32_t den)
{
    if (den == 0) return num;
    int neg = (num < 0);
    uint32_t unum = neg ? (uint32_t)(-num) : (uint32_t)num;
    uint32_t uden = (den < 0) ? (uint32_t)(-den) : (uint32_t)den;

    uint32_t r = mod_u32(unum, uden);
    return neg ? -(int32_t)r : (int32_t)r;
}

/* Integer square root (floor). */
static inline uint32_t isqrt(uint32_t n)
{
    if (n < 2) return n;
    uint32_t x = n;
    uint32_t y = (x + 1) >> 1;
    while (y < x) {
        x = y;
        y = (x + div_u32(n, x)) >> 1;
    }
    return x;
}

/* Greatest common divisor (Euclidean). */
static inline uint32_t gcd_u32(uint32_t a, uint32_t b)
{
    while (b) {
        uint32_t t = mod_u32(a, b);
        a = b;
        b = t;
    }
    return a;
}

/* Power: base^exp (unsigned, mod 2^32). */
static inline uint32_t pow_u32(uint32_t base, uint32_t exp)
{
    uint32_t result = 1;
    while (exp) {
        if (exp & 1) result = mul_u32(result, base);
        base = mul_u32(base, base);
        exp >>= 1;
    }
    return result;
}

/* Count leading zeros. */
static inline int clz_u32(uint32_t x)
{
    if (x == 0) return 32;
    int n = 0;
    if (!(x & 0xFFFF0000)) { n += 16; x <<= 16; }
    if (!(x & 0xFF000000)) { n +=  8; x <<=  8; }
    if (!(x & 0xF0000000)) { n +=  4; x <<=  4; }
    if (!(x & 0xC0000000)) { n +=  2; x <<=  2; }
    if (!(x & 0x80000000)) { n +=  1; }
    return n;
}

/* Log base 2 (floor). Returns -1 for input 0. */
static inline int log2_u32(uint32_t x)
{
    return x ? (31 - clz_u32(x)) : -1;
}

/* ─────────────────────────────────────────────
 * Number ↔ String Conversion
 *
 * These use div_u32/mod_u32/mul_i32 instead of C's / % *
 * operators, so no libgcc symbols are needed.
 * ───────────────────────────────────────────── */

/*
 * Convert a signed 32-bit integer to a decimal string.
 * `buf` must be at least 12 bytes (−2147483648\0).
 * Returns pointer to the start of the string within `buf`.
 */
static inline char *itoa(int32_t val, char *buf, size_t bufsz)
{
    char *p = buf + bufsz - 1;
    int neg = 0;
    uint32_t uval;
    volatile uint32_t base = 10;

    *p = '\0';

    if (val < 0) {
        neg = 1;
        uval = (uint32_t)(-(val + 1)) + 1;
    } else {
        uval = (uint32_t)val;
    }

    if (uval == 0) {
        *(--p) = '0';
    } else {
        while (uval > 0) {
            *(--p) = '0' + (char)mod_u32(uval, base);
            uval = div_u32(uval, base);
        }
    }

    if (neg) *(--p) = '-';
    return p;
}

/*
 * Convert an unsigned 32-bit integer to a decimal string.
 * `buf` must be at least 11 bytes (4294967295\0).
 */
static inline char *utoa(uint32_t val, char *buf, size_t bufsz)
{
    char *p = buf + bufsz - 1;
    volatile uint32_t base = 10;
    *p = '\0';

    if (val == 0) {
        *(--p) = '0';
    } else {
        while (val > 0) {
            *(--p) = '0' + (char)mod_u32(val, base);
            val = div_u32(val, base);
        }
    }
    return p;
}

/*
 * Convert an unsigned 32-bit integer to a hexadecimal string.
 * `buf` must be at least 11 bytes (0xFFFFFFFF\0).
 * Only uses shifts and masks — no libgcc needed.
 */
static inline char *utohex(uint32_t val, char *buf, size_t bufsz)
{
    static const char hex[] = "0123456789abcdef";
    char *p = buf + bufsz - 1;
    *p = '\0';

    if (val == 0) {
        *(--p) = '0';
    } else {
        while (val > 0) {
            *(--p) = hex[val & 0xF];
            val >>= 4;
        }
    }
    *(--p) = 'x';
    *(--p) = '0';
    return p;
}

/* Print a signed integer to stdout followed by \r\n. */
static inline void print_int(int32_t val)
{
    char buf[12];
    println(itoa(val, buf, 12));
}

/* Print an unsigned integer to stdout followed by \r\n. */
static inline void print_uint(uint32_t val)
{
    char buf[11];
    println(utoa(val, buf, 11));
}

/* Print an unsigned integer in hex to stdout followed by \r\n. */
static inline void print_hex(uint32_t val)
{
    char buf[11];
    println(utohex(val, buf, 11));
}

/*
 * Parse a decimal integer from a string. Handles optional leading '-'.
 * Stops at first non-digit. Uses mul_i32 instead of * operator.
 */
static inline int32_t atoi(const char *s)
{
    int32_t val = 0;
    int neg = 0;
    volatile int32_t base = 10;

    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;

    if (*s == '-')      { neg = 1; s++; }
    else if (*s == '+') { s++; }

    while (*s >= '0' && *s <= '9') {
        val = mul_i32(val, base) + (*s - '0');
        s++;
    }

    return neg ? -val : val;
}

/* ─────────────────────────────────────────────
 * Input Helpers
 * ───────────────────────────────────────────── */

/* Read a single character from stdin. Returns the char or -1 on EOF/error. */


/*
 * Read a line from stdin into `buf` (up to bufsz-1 chars).
 * Your OS buffers input until CR is received, then the read
 * syscall copies the entire TTY buffer into the user buffer.
 * This function strips trailing \r / \n and null-terminates.
 * Returns number of characters read (excluding null).
 */
static inline int readline(char *buf, int bufsz)
{
    int r = (int)sys_read(STDIN, buf, bufsz - 1);
    if (r < 0) return -1;
    /* Strip trailing \r or \n */
    while (r > 0 && (buf[r - 1] == '\r' || buf[r - 1] == '\n')) r--;
    buf[r] = '\0';
    return r;
}

/* Read an integer from stdin (reads a line, then parses it). */
static inline int32_t read_int(void)
{
    char buf[16];
    readline(buf, 16);
    return atoi(buf);
}

#endif /* RV32I_STDLIB_H */
