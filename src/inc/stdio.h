#ifndef __INC_STDIO_H__
#define __INC_STDIO_H__

#include <stdarg.h> // Enable infinite parameters

/* Define NULL if not defined */
#ifndef NULL
    #define NULL ((void *) 0x0)
#endif

// kern/console.c
void cputchar(int c);
int getchar(void);
int iscons(int fd);

// lib/printfmt.c
void printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...);
void vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list);
int	snprintf(char *str, int size, const char *fmt, ...);
int	vsnprintf(char *str, int size, const char *fmt, va_list);

// lib/printf.c
int	cprintf(const char *fmt, ...);
int	vcprintf(const char *fmt, va_list);

// lib/fprintf.c
int	printf(const char *fmt, ...);
int	fprintf(int fd, const char *fmt, ...);
int	vfprintf(int fd, const char *fmt, va_list);

// lib/readline.c
char *readline(const char *prompt);

#endif /* __INC_STDIO_H__ */