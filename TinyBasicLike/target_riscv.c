/*
 * target_riscv.c      compiler support routines for bare-metal RISC-V rv32 target
 *
 * Replaces target_linux.c.  Uses ecall syscall wrappers instead of libc.
 * No stdio.h, no POSIX, no libc dependencies.
 */

#include  <stdint.h>

#include  "target_riscv.h"
#include  "TinyBasicLike.h"
#include  "syscalls.h"
#include  "rv32i_stdlib.h"


/*
 * GCC with -ffreestanding can still emit calls to memcpy, memset,
 * memmove, and memcmp.  Provide minimal implementations.
 */
void *memcpy(void *dest, const void *src, unsigned int n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;
	while (n--) *d++ = *s++;
	return dest;
}

void *memset(void *dest, int c, unsigned int n)
{
	unsigned char *d = dest;
	while (n--) *d++ = (unsigned char)c;
	return dest;
}

void *memmove(void *dest, const void *src, unsigned int n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;
	if (d < s) {
		while (n--) *d++ = *s++;
	} else {
		d += n; s += n;
		while (n--) *--d = *--s;
	}
	return dest;
}


#define  RISCV_VERSION_INFO		"v0.01"

const unsigned char					t_TargetIDMsg[] =
		"RISC-V rv32 bare-metal TinyBasicLike target " RISCV_VERSION_INFO;



/*
 * File I/O tracking.
 * The core owns rfp and wfp (declared as FILE* in TinyBasicLike.c).
 * We use static sentinel addresses so the core can check rfp != NULL
 * to know a file is open.  The actual kernel fd is tracked separately.
 */
static char				rfp_sentinel;
static char				wfp_sentinel;
static int				read_fd  = -1;
static int				write_fd = -1;

FILE					*rfp = NULL;		/* read file pointer — defined here because core's tentative def isn't emitted */
FILE					*wfp = NULL;		/* write file pointer */


int						OutputStream;
int						InputStream;


DATA_SIZE				*timeraddrs[T_NUM_TIMERS] = {0, 0, 0, 0};

unsigned char			t_program[TOTAL_RAM_REQUIRED];


/*
 * Virtual ports — on bare metal these could be mapped to real
 * hardware registers. For now they are RAM variables.
 */
static uint8_t			vPORTA;
static uint8_t			vPORTB;
static uint8_t			vPORTC;

static uint16_t			vPORTM;
static uint16_t			vPORTN;
static uint16_t			vPORTO;
static uint16_t			vPORTP;


/*
 * Port name table — must match the indexes in target_riscv.h
 */
const unsigned char		ports_tab[] =
{
	"PORTA "
	"PORTB "
	"PORTC "

	"PORTM "
	"PORTN "
	"PORTO "
	"PORTP "

	"\0"
};


/*
 * Port address/size table
 */
struct Port
{
	DATA_SIZE			portaddr;
	unsigned int		size;
};

struct Port 			porttable[] =
{
	{(DATA_SIZE) &vPORTA,	8},
	{(DATA_SIZE) &vPORTB,	8},
	{(DATA_SIZE) &vPORTC,	8},

	{(DATA_SIZE) &vPORTM,	16},
	{(DATA_SIZE) &vPORTN,	16},
	{(DATA_SIZE) &vPORTO,	16},
	{(DATA_SIZE) &vPORTP,	16},
};



/* ================================================================
 * Boot / Shutdown
 * ================================================================ */

void			t_ColdBoot(void)
{
	/* Ensure streams are set before main() prints the welcome message —
	 * on bare metal BSS may not be zeroed by crt0. */
	OutputStream = T_STREAM_SERIAL;
	InputStream  = T_STREAM_SERIAL;
}


void			t_WarmBoot(void)
{
	OutputStream = T_STREAM_SERIAL;
	InputStream  = T_STREAM_SERIAL;

	/* Close any open files */
	if (rfp && read_fd >= 0)  { sys_close(read_fd);  read_fd = -1;  rfp = NULL; }
	if (wfp && write_fd >= 0) { sys_close(write_fd); write_fd = -1; wfp = NULL; }
}


void			t_Shutdown(void)
{
	static const char msg[] = "\r\nShutting down...\r\n";
	sys_write(1, msg, sizeof(msg) - 1);
	sys_exit();
}



/* ================================================================
 * Character I/O
 * ================================================================ */

/*
 * t_OutChar — output a character to the active stream
 */
void  t_OutChar(int  c)
{
	char  ch;

	switch (OutputStream)
	{
		case T_STREAM_SERIAL:
			if (c == '\n')
			{
				ch = '\r';
				sys_write(1, &ch, 1);
			}
			ch = (char)c;
			sys_write(1, &ch, 1);
			break;

		case T_STREAM_FILE:
			if (wfp && write_fd >= 0)
			{
				ch = (char)c;
				sys_write(write_fd, &ch, 1);
			}
			break;

		default:
			break;
	}
}


/*
 * t_GetChar — blocking character input from the active stream
 */
int				t_GetChar(void)
{
	char		ch = 0;

	switch (InputStream)
	{
		case T_STREAM_SERIAL:
			sys_read_raw(0, &ch);
			if (ch == 127)  ch = '\b';		/* convert DEL to BS */
			return (int)ch;

		case T_STREAM_FILE:
			if (rfp && read_fd >= 0)
			{
				long n = sys_read(read_fd, &ch, 1);
				if (n <= 0)  return -1;		/* EOF or error */
				return (int)ch;
			}
			return -1;

		default:
			return 0;
	}
}


/*
 * t_ConsoleCharAvailable — non-blocking check for console input
 *
 * Returns TRUE if a character is waiting, FALSE otherwise.
 *
 * TODO: This needs syscall 7 (char_available) added to your kernel.
 *       The kernel handler should check the UART RX FIFO status
 *       and return 1 if data is available, 0 if not.
 *       Until that syscall exists, this always returns FALSE,
 *       which means BREAK detection during program execution
 *       will not work.
 */
int				t_ConsoleCharAvailable(void)
{
	/* Uncomment when syscall 7 is implemented in kernel: */
	/* return (int)(sys_char_available(0) > 0); */

	return FALSE;
}


/*
 * t_GetCharFromConsole — blocking read from console, ignoring InputStream
 */
int				t_GetCharFromConsole(void)
{
	char		ch = 0;
	sys_read_raw(0, &ch);
	if (ch == 127)  ch = '\b';
	return (int)ch;
}



/* ================================================================
 * Stream Selection
 * ================================================================ */

int				t_SetOutputStream(int  s)
{
	if (s != T_STREAM_FILE && s != T_STREAM_SERIAL)  return -1;
	OutputStream = s;
	return 0;
}

int				t_SetInputStream(int  s)
{
	if (s != T_STREAM_FILE && s != T_STREAM_SERIAL)  return -1;
	InputStream = s;
	return 0;
}



/* ================================================================
 * Timers — stubbed for now
 * ================================================================ */

void			t_SetTimerRate(unsigned int  usecs)
{
	(void)usecs;
	/* No timer hardware support yet */
}

int				t_AddTimer(DATA_SIZE  *t)
{
	(void)t;
	return -1;
}

int				t_DeleteTimer(DATA_SIZE  *t)
{
	(void)t;
	return -1;
}



/* ================================================================
 * File I/O — using kernel sys_open / sys_close / sys_read / sys_write
 * ================================================================ */

/*
 * t_OpenFile — open a file via kernel syscall
 *
 * Returns a FILE* (which wraps the kernel fd), or NULL_FILE on failure.
 * The mode string determines read vs write:
 *   'r' = open for reading
 *   'w' = open/create for writing
 */
FILE			*t_OpenFile(char  *name, char  *mode)
{
	int		flags;
	long	fd;

	if ((name == 0) || (*name == '\0'))  return NULL;
	if (*mode == 'w')
		flags = 1;		/* O_WRONLY — adjust if your kernel uses different flags */
	else if (*mode == 'r')
		flags = 0;		/* O_RDONLY */
	else
		return NULL;
	//strcat(name, '\0');
	fd = sys_open(name, 0, flags);
	if (fd < 0)  return NULL;
	if (*mode == 'w')
	{
		write_fd = (int)fd;
		wfp = (FILE *)&wfp_sentinel;
		return wfp;
	}
	else
	{
		read_fd = (int)fd;
		rfp = (FILE *)&rfp_sentinel;
		return rfp;
	}
}


int			t_CloseFile(FILE  *fp)
{
	if (fp == (FILE *)&rfp_sentinel && read_fd >= 0)
	{
		sys_close(read_fd);
		read_fd = -1;
	}
	if (fp == (FILE *)&wfp_sentinel && write_fd >= 0)
	{
		sys_close(write_fd);
		write_fd = -1;
	}

	/* Clear the global pointer if it matches */
	if (fp == rfp)  rfp = NULL;
	if (fp == wfp)  wfp = NULL;
	
	return 0;
}



/* ================================================================
 * I/O Ports
 * ================================================================ */

DATA_SIZE		t_ReadPort(int  index)
{
	if (index >= NUM_PORTS)  return 0;

	switch (index)
	{
		case PORTS_PORTA:  return (DATA_SIZE)vPORTA;
		case PORTS_PORTB:  return (DATA_SIZE)vPORTB;
		case PORTS_PORTC:  return (DATA_SIZE)vPORTC;
		case PORTS_PORTM:  return (DATA_SIZE)vPORTM;
		case PORTS_PORTN:  return (DATA_SIZE)vPORTN;
		case PORTS_PORTO:  return (DATA_SIZE)vPORTO;
		case PORTS_PORTP:  return (DATA_SIZE)vPORTP;
		default:           return 0;
	}
}


void			t_WritePort(DATA_SIZE  value, int  index)
{
	if (index >= NUM_PORTS)  return;

	switch (index)
	{
		case PORTS_PORTA:  vPORTA = (uint8_t)value;   break;
		case PORTS_PORTB:  vPORTB = (uint8_t)value;   break;
		case PORTS_PORTC:  vPORTC = (uint8_t)value;   break;
		case PORTS_PORTM:  vPORTM = (uint16_t)value;  break;
		case PORTS_PORTN:  vPORTN = (uint16_t)value;  break;
		case PORTS_PORTO:  vPORTO = (uint16_t)value;  break;
		case PORTS_PORTP:  vPORTP = (uint16_t)value;  break;
		default:  break;
	}
}



/* ================================================================
 * Missing stubs — features not available on bare-metal rv32
 * ================================================================ */

int   t_CheckToneStatus(void) { return 0; }
void  t_SetTone(int freq, int dur) { (void)freq; (void)dur; }
char *t_GetFirstFileName(void) { return 0; }
char *t_GetNextFileName(char *prev) { (void)prev; return 0; }
int   t_DeleteFile(char *name) { (void)name; return -1; }
int   t_FileExistsQ(char *name) { (void)name; return 0; }
void  t_ForceWriteToFlash(void) { }
int   t_CheckForHWBreak(void) { return 0; }
void  t_ForceHWReset(void) { sys_exit(); }



/* ================================================================
 * Test
 * ================================================================ */

void		t_Test(void)
{
	static const char msg[] = "\r\nt_Test() executed.\r\n";
	sys_write(1, msg, sizeof(msg) - 1);
}
