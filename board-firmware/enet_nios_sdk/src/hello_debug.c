// file hello_debug.c
// 
// This file demonstrates the use of the Nios debug core using the provided
// debug libraries.
//
// All debug routines are preceded by nr_debug_
// References to all available debug library facilities can be found in nios.h
//
// This program uses the debug core to trace the execution of a simple code
// sequence, break on a certain condition, and then dump that trace data out
// the serial port in a human readable format.
//
// While the trace data dump is human readable, please note that information
// is compressed.  This means that only the following information is provided:
//
// FETCH src  dst
// READ  addr data
// WRITE addr data
// SKIP  count data data
//
// FETCH indicates where branches in the instruction sequence occur.
// Sequential instruction fetches are not logged.
// READ indicates a read address and data.
// WRITE indicates a write address and data.
// SKIP indicates a log of skip instructions and how they evaluated.
//
// Note that this information is reported in reverse time order.  The first
// Trace sample printed is the last trace sample stored.
//
// The FETCH data can be followed by hand by referencing the objdump
// file and following the sequence of branches.  However SKIP and READ/WRITE
// transactions are more difficult to match up by hand.
//
// We have provided a tool to process the trace dump and provide a full
// trace listing with skips, reads, and writes associated with their
// instructions.  This tool can be used as follows:
//
//     nios_build hello_debug.c
//     nios_run hello_debug.srec > hello_debug.tmp
//     tracelink hello_debug.out hello_debug.tmp > hello_debug.trace
//
// The first line builds the application.
// The second line runs the application and directs the output a file.
// The third line processes the object file and the trace dump into a
// full trace listing which is directed to hello_debug.trace.
//

#include "nios.h"

// set this to 1 to dump trace on a breakpoint
// set this to 0 to dump trace without a breakpoint
#define USE_BREAKPOINT 0

// global data array for observing traced WRITEs
unsigned int array[32];

// variable used to trigger a data breakpoint
unsigned int finished;

// Simple routime to write predictable values to memory
void do_write (int i)
{
    	array[i] = 1<<i;
}

main()
{ 
    int i=0;

    if (nasys_debug_core == 0)
    {
        printf ("There is no debug core in this design.\n");
        printf ("To add a debug core, you must select the\n");
        printf ("'Include debug core' check box in the\n");
        printf ("Nios CPU wizard, on the 'Other Configuration'\n");
        printf ("page.\n");
        return;
    }

//  Install the debug core's IRQ handler
    nr_installuserisr(nasys_debug_core_irq, 
    		nr_debug_isr_halt,           // this library irq handler
		                             // will print the cause of
					     // the break, dump the trace 
					     // data, and return to the 
					     // germs monitor.
					     // To continue tracing after the
					     // dump, use nr_debug_isr_continue

		0);			     // You can pass the address of
					     // an alternate UART in this field
					     // 0 will use the default printf
					     // UART

//  The debug core can break on 2 independent conditions sets.
//  Set the conditions that will trigger breakpoint 0
    nm_debug_set_bp0 ((unsigned int) &finished,// Set the break address to
    					       // the address of finished
    				   0xffffffff, // Set the address mask to
				   	       // look at all addresses
    		     		   0xffffffff, // Set the break data to
				   	       // 0xffffffff
				   0xffffffff, // Set the data mask to look
				   	       // at all values
	   np_debug_break_code_write_mask);    // Set the break transaction
	   				       // code to break on a write
					       // tranaction.

//  Activate the debug core
    nr_debug_start();

//  Simple code sequence that will be traced

    for (i=0; i<32; i++)
    {
//    	array[i] = 1<<i;
        do_write (i);
    }
    for (i=0; i<32; i++)
    {
//    	array[i] = 1<<i;
    	do_write(i);
    }


#if USE_BREAKPOINT

//  Write the previously specified break value to the previously specified
//  break address.  This will trigger the debug core's ISR which will
//  dump the trace buffer and return to the GERMS monitor
    finished = 0xffffffff;

//  If this were to execute, the breakpoint would have been missed
    printf ("We've come too far!!!\n\4");

#else

//  Deactivate the debug core
    nr_debug_stop ();

//  Manually dump the trace buffer
    nr_debug_dump_trace (0); 		// 0 prints to printf UART 
    					// or you can pass the address of your
					// favorite 

#endif

}

