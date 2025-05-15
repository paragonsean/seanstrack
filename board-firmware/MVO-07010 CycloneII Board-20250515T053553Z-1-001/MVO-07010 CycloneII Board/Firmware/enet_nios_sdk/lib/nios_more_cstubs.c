/*

	file: nios_more_cstubs.c

	contents:	Implementation of the _read and _write
				routines required to support stdio.

	author:		Daryl Spitzer - altera corporation
				(based on the previous assembly routines
				 from nios_cstubs.s by David Van Brink)

				Converted to C so that nm_printf_rxchar
				& nm_printf_txchar don't have to be
				defined in excalibur.s, which was
				causing them to get linked in
				(regardless of whether they're used)
				resulting in the boot monitor ROM
				exceeding 1024 bytes

*/

#include "excalibur.h"
#include <stdio.h>


// ----------------------------
// _read( fd, buf, nbytes )
// Get nBytes into buf.
// We'll ignore fd: stdin only
//
// Oddly enough, we'll only return
// one byte, ever. (getchar doesn't
// get individual keystrokes otherwise.)

#ifdef nasys_printf_uart

size_t _read( int fd, char* buf, size_t nbytes )
{
	int c;

	while ( ( c = nm_printf_rxchar( nasys_printf_uart ) ) < 0 )
		;

	nm_printf_txchar( c, nasys_printf_uart );	// echo character

	buf[0] = c;

	return 1;
}

#endif	// #ifdef nasys_printf_uart

// ----------------------------
// _write (fd, buf, nbytes)
// Put nBytes from buf.
// We'll assume stdout (and ignore the fd) if it doesn't indicate stderr

#ifdef nasys_printf_uart

size_t _write( int fd, const char* buf, size_t nbytes )
{
	void* uart = (void*)nasys_printf_uart;
    size_t i;

#ifdef nasys_oci_core
    if ( ( nk_stderr == fd ) && ( uart == (void*)nasys_oci_core ) )
    {
        uart += 2;  // OCI stderr is stdout + 2  (otherwise stdterr is the same as stdout)
    }
#endif  // #ifdef nasys_oci_core

	for ( i = 0; i < nbytes; ++i )
	{
		nm_printf_txchar( buf[i], uart );
	}

	return nbytes;
}

#endif	// #ifdef nasys_printf_uart
