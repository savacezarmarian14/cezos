/*	$NetBSD: stdarg.h,v 1.12 1995/12/25 23:15:31 mycroft Exp $	*/
/* 
 * This header provide a way to provide infinite number of arguments 
 * to a function. Needed for printf functions 
 */
#ifndef __INC_STDARG_H_
#define	__INC_STDARG_H_

typedef __builtin_va_list va_list;

#define va_start(ap, last) __builtin_va_start(ap, last)

#define va_arg(ap, type) __builtin_va_arg(ap, type)

#define va_end(ap) __builtin_va_end(ap)

#endif	/* __INC_STDARG_H__ */
