/*

	file: nios_printf.c

	contents: Implementation of printf that sends
                  out via nr_uart_txchar().
                  Also implements fprintf,
		  where the "file" is just a uart
		  base address.

	author: david van brink / altera corporation

*/

#include <stdio.h>
#include <stdarg.h>		// manages variable-length argument passing
#include "excalibur.h"


#if defined(nm_printf_txchar) && defined(nasys_printf_uart)

typedef void (*PrintfCharRoutine)(int c,int context);

extern void PrivatePrintf(const char *fmt,PrintfCharRoutine cProc,int context,va_list args);

static void default_uart_txchar(int c,int context)
	{
	nr_txchar2(c,nk_stdout);
	}

int nr_printf(const char *fmt, ... )
	{
	va_list args;

	va_start (args, fmt);
	PrivatePrintf(fmt,default_uart_txchar,0,args);
	return (0);
	}

#endif // nasys_printf_uart and nm_printf_txchar

void nr_setbuf(FILE *stream, char *buf)
    {
    // replaces standard setbuf() function with
    // a tiny one: nr_printf never does buffering.

    return;
    }

// end of file
