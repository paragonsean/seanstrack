// nios_debug_monitor.c
// The is a simple GDB debug monitor.
// It is donwloaded by the GDB command file (nios-build -d)
// which then spawns Insight, connects to this monitor, and,
// finally, downloads the user program

#include "excalibur.h"

main ()
{
#ifndef nasys_oci_core
#ifdef nasys_printf_uart
	  nr_uart_txstring("(Running Nios GDB Stub)\n");
#endif
#endif

	nios_gdb_install (1);
	nios_gdb_breakpoint ();
	while (1)
		// loop forever, responding to uart chars via interrupt.
		;
}
