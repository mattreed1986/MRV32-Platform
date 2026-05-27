/*
 * target_riscv.h        header file specific to bare-metal RISC-V rv32 target
 *
 * Replaces target_linux.h for use on a bare-metal RISC-V system
 * with custom syscalls (ecall).
 */

#ifndef  TARGET_RISCV_H
#define  TARGET_RISCV_H

#include  <stdint.h>
#include  <stdio.h>			/* get FILE type from newlib — used as opaque pointer only */


/*
 * Define the size (in bits) of various data elements for the rv32 target.
 * Using 32-bit integers to match the native word size and avoid
 * needing 64-bit division helpers from libgcc.
 */
#define		DATA_SIZE			int32_t
#define		DATA_SIZE_MAX		INT32_MAX
#define		UDATA_SIZE			uint32_t
#define		UDATA_SIZE_MAX		UINT32_MAX

/*
 * Define target characteristics and resources of interest to the core program.
 */
#define		T_NUM_TIMERS		4
#define		T_STREAM_SERIAL		0
#define		T_STREAM_FILE		1
#define		T_SUPPORTS_FILES	TRUE

/*
 * Port indexes — must match the order in ports_tab[] in target_riscv.c
 */
#define  PORTS_PORTA		0
#define  PORTS_PORTB		1
#define  PORTS_PORTC		2
#define  PORTS_PORTM		3
#define  PORTS_PORTN		4
#define  PORTS_PORTO		5
#define  PORTS_PORTP		6
#define  PORTS_UNKNOWN		7

#define  NUM_PORTS  PORTS_UNKNOWN


extern const unsigned char				ports_tab[];


/*
 * Define the amount of RAM available for the user's program, stacks, and variables.
 * Adjust this based on how much RAM your RISC-V system has available.
 */
#define  T_PROGSPACE_SIZE			8000



/*
 * Function prototypes — same interface as target_linux.h
 */
void			t_ColdBoot(void);
void			t_WarmBoot(void);
void			t_Shutdown(void);

void			t_OutChar(int  c);
int				t_GetChar(void);
int				t_ConsoleCharAvailable(void);
int				t_GetCharFromConsole(void);

void			t_SetTimerRate(unsigned int  usecs);
int				t_AddTimer(DATA_SIZE  *t);
int				t_DeleteTimer(DATA_SIZE  *t);

int				t_SetOutputStream(int  s);
int				t_SetInputStream(int  s);

FILE			*t_OpenFile(char  *name, char  *mode);
int				t_CloseFile(FILE  *fp);

DATA_SIZE		t_ReadPort(int  index);
void			t_WritePort(DATA_SIZE  value, int  index);

void			t_Test(void);

/*
 * Additional target functions required by the core.
 * Without these prototypes, the compiler treats calls as UB
 * and may optimize away critical if-checks.
 */
int				t_CheckToneStatus(void);
void			t_SetTone(int freq, int dur);
char			*t_GetFirstFileName(void);
char			*t_GetNextFileName(char *prev);
int				t_DeleteFile(char *name);
int				t_FileExistsQ(char *name);
void			t_ForceWriteToFlash(void);
int				t_CheckForHWBreak(void);
void			t_ForceHWReset(void);


#endif		/* TARGET_RISCV_H */
