// file: nios_gdb_stub_IO.c
//
// Single character I/O for Nios GDB Stub

#include "excalibur.h"
#include "nios_gdb_stub.h"

#ifdef nasys_debug_uart
	#define GDB_UART nasys_debug_uart
#endif

char GDBGetChar(void)
{
	char c = 0;

#ifdef GDB_UART
	while( (c = (char)nr_uart_rxchar(GDB_UART)) < 0 )
			;
#endif

	return c;
}

void GDBPutChar(char c)
{
#ifdef GDB_UART
	nr_uart_txchar(c, GDB_UART);
#endif
}

// End of file
