/*

    file: nios_fprintf.c

    contents: Implementation of fprintf that sends
                  out via nr_uart_txchar().

    adapted from nios_printf.c by dspitzer

*/

#include "excalibur.h"
#include <stdio.h>
#include <stdarg.h>		// manages variable-length argument passing


#if defined(nm_printf_txchar) && defined(nasys_printf_uart)

typedef void (*PrintfCharRoutine)( int c, int context );

extern void PrivatePrintf( const char *fmt,PrintfCharRoutine cProc, int context, va_list args );


static void default_txchar( int c, int channel )
{
    nr_txchar2( c, channel );
}


static void uart_txchar( int c, int channel )
{
	// nr_txchar2 (in nios_uart.c) calls r_ensure_printf_init()...but
	// we can't from here...that should be okay though, since our UARTs
	// don't need initialization (nm_printf_initialize isn't defined)

	nr_uart_txchar( c, (void*)channel );		// assume that channel must be a UART (and that the UART has been initialized)
}


static void jtag_txchar( int c, int channel )
{
	nr_jtag_txchar( c, (void*)channel );		// assume that channel must be the OCI
}


int nr_fprintf( FILE *fp, const char *fmt, ... )
{
    int     channel = (int)fp;
    va_list args;
	void (*txchar_function)(int,int) = default_txchar;

    if ( fp == stderr )
    {
#ifdef nasys_oci_core
        if ( (void*)nasys_printf_uart == (void*)nasys_oci_core )
            channel = nasys_oci_core + 2;   // OCI stderr is stdout + 2
        else
#endif // #ifdef nasys_oci_core
            channel = nk_stdout;            // stderr and stdout are the same with UARTs
    }
    else if ( fp == stdout )
    {
        channel = nk_stdout;
    }
#ifdef nasys_oci_core
	else
	{
		if ( (void*)nasys_oci_core == (void*)nasys_printf_uart )
		{
			// Host Communication is set to OCI...

			if ( (void*)channel != (void*)nasys_oci_core )
			{
				// ...but fprintf isn't going to the OCI,
				// so we'll assume it's going to a UART

				txchar_function = uart_txchar;
			}
		}
		else
		{
			// Host Communication is set to a UART

			if ( (void*)channel == (void*)nasys_oci_core )
			{
				// ...but fprintf is going to the OCI

				txchar_function = jtag_txchar;
			}
		}
	}
#endif // #ifdef nasys_oci_core

    va_start( args, fmt );

    PrivatePrintf( fmt, txchar_function, channel, args );

    return (0);
}

#endif // nasys_printf_uart and nm_printf_txchar

// end of file
