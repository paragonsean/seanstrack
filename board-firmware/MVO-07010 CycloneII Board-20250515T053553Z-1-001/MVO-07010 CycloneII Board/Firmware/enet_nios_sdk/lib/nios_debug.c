// file debug_core.c - this file contains some useful routines for controlling and
//        extracting information from the nios debug peripheral
//        JMB 7/24/01

#include "excalibur.h"

//nios-run sometimes loses sync when the nios is dumping large volumes of data
//at hight speed (as in trace_dump).  The need for and size of this delay varies 
//from system to system.  The following constant controls this delay.
#define DUMP_DELAY 800

// Return the total number of samples collected
unsigned long nr_debug_num_samples ()
{
    unsigned long n_samples;
    unsigned int temp;

    nm_debug_get_reg(temp,np_debug_n_samples_msb);
    n_samples = ((unsigned long) temp << 16);
    nm_debug_get_reg(temp,np_debug_n_samples_lsb);
    n_samples += temp;
    return (n_samples);
}


void nr_debug_get_sample (unsigned int *trace_addr, 
			unsigned int *trace_data, 
			unsigned char *trace_code)
{
    int valid;
    nm_debug_set_reg(1,np_debug_read_sample);	// issue read command

    nm_debug_get_reg(valid, np_debug_data_valid);
    while (!valid) nm_debug_get_reg(valid,np_debug_data_valid);	// wait until read completed

    nm_debug_get_reg(*trace_addr,np_debug_trace_address);
    nm_debug_get_reg(*trace_data,np_debug_trace_data);
    nm_debug_get_reg(*trace_code,np_debug_trace_code);
}

int nr_debug_get_skip_count ()
{
    int count;
    nm_debug_get_reg(count, np_debug_trace_code);
    count &= np_debug_trace_code_skp_cnt_mask;
    count = count >> 2;
    return count;
}

//must have a uart for these functions
#ifdef nasys_uart_count
#if (nasys_uart_count > 0)
void nr_debug_show_break (void *uart)
{
    int interrupt;

    nm_debug_get_reg(interrupt,np_debug_interrupt);
    switch (interrupt)
    {
    case np_debug_interrupt_code_ext_mask:
        printf ("DBG Interrupt on External Signal\n");
	break;
    case np_debug_interrupt_code_dbp0_mask:
        printf ("DBG Interrupt on Data Breakpoint 0\n");
	break;
    case np_debug_interrupt_code_ibp0_mask:
        printf ("DBG Interrupt on Instruction Breakpoint 0\n");
	break;
    case np_debug_interrupt_code_dbp1_mask:
        printf ("DBG Interrupt on Data Breakpoint 1\n");
	break;
    case np_debug_interrupt_code_ibp1_mask:
        printf ("DBG Interrupt on Instruction Breakpoint 1\n");
	break;
    case np_debug_interrupt_code_mem_mask:
        printf ("DBG Interrupt on Memory Limit \n");
	break;
    default:
        printf ("DBG Interrupt on unknown condition\n");
	break;
    }
}

void nr_debug_dump_trace (void *uart)
{ 
    unsigned long i;
    unsigned long n_samples;
    int status;
    unsigned int trace_address;
    unsigned int trace_data;
    unsigned char trace_code;

    // if trace exists, dump the data
    nm_debug_get_reg(status, np_debug_write_status);
    if(status & np_debug_write_status_trace_mask)
    {
        nm_debug_get_reg(status,np_debug_write_status);
        while(status&np_debug_write_status_writing_mask) 
            nm_debug_get_reg(status,np_debug_write_status);

        n_samples = nr_debug_num_samples ();

        for(i=0; i<n_samples; i++)
        { 
            volatile int j;
            nr_debug_get_sample (&trace_address, &trace_data, &trace_code);

            if (trace_code & np_debug_trace_code_skp_mask)
            {
                printf ("SKIP    0x%x 0x%x 0x%x",nr_debug_get_skip_count(),
                trace_address,trace_data);
            }
            else
            {
				switch (trace_code & np_debug_trace_code_op_mask)
				{
                case np_debug_trace_code_read:
                    printf ("READ     0x%x 0x%x",trace_address,trace_data);
                    break;
                case np_debug_trace_code_write:
                    printf ("WRITE    0x%x 0x%x",trace_address,trace_data);
                    break;
                case np_debug_trace_code_fetch:
                    printf ("FETCH    0x%x 0x%x",trace_data,trace_address);
                    break;
                }
                if (trace_code & np_debug_trace_code_intr_mask)
                    printf (" INTERRUPT");
            }

        	if (trace_code & np_debug_trace_code_fifo_full_mask)
                printf ("	FIFO FULL - SOME DATA LOST");
            printf ("\n");

            //delay to prevent nios-run from losing sync
            for (j=0; j<DUMP_DELAY; j++); 
        }
    }
}

void nr_debug_stop ()
{
    nm_debug_set_reg(1,np_debug_stop);
}

void nr_debug_start ()
{
    nm_debug_set_reg(1,np_debug_sw_reset);
    nm_debug_set_reg(1,np_debug_start);
}

void nr_debug_isr_halt(int context)
{
    void *uart;
    unsigned int p_int;
    void *p = nasys_reset_address;

    nm_debug_set_reg(1,np_debug_stop);

    p_int = ((unsigned int)nasys_reset_address) >> 1;

#ifdef nasys_printf_uart
    if (context == 0) uart = (void *)nasys_printf_uart;
    else uart = (void *)context;
#else
    uart = (void *)context;
#endif

    
    //show the cause of the break
    nr_debug_show_break (uart);

    //dump the trace data
    nr_debug_dump_trace (uart);

    //send End Of Transmission character
    printf ("\4");

    //return to germs monitor
    asm ( "jmp %0" : /*no outputs*/ : "r" (p) );
}

void nr_debug_isr_continue(int context)
{
    void *uart;

    nm_debug_set_reg(1,np_debug_stop);

#ifdef nasys_printf_uart
    if (context == 0) uart = (void *)nasys_printf_uart;
    else uart = (void *)context;
#else
    uart = (void *)context;
#endif

    
    //show the cause of the break
    nr_debug_show_break (uart);

    //dump the trace data
    nr_debug_dump_trace (uart);

    //re-start up debug core 
    nr_debug_start ();
}

#endif //nasys_uart_count > 0
#endif //nasys_uart_count
