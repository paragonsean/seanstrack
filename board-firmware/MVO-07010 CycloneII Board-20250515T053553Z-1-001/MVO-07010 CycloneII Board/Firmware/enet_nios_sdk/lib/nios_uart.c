/*

	file: nios_uart.c

	contents: the tiniest routines, which
	          talk to printf and debug uarts.
	author: david van brink \ altera corporation

	2002-11-21	dspitzer	nr_txchar2 (takes a channel argument)

*/

#include "excalibur.h"

// +---------------------------
// | printf uart routines
// |

static void r_ensure_printf_init(void)
	{
	static int inited = 0;

	if(!inited)
		{
#ifdef nm_printf_initialize
		nm_printf_initialize(nasys_printf_uart);
#endif
		inited = 1;
		}
	}

#ifdef nm_printf_txchar
void nr_txchar(int c)
	{
	r_ensure_printf_init();
	nm_printf_txchar(c, nasys_printf_uart);
	}

#define TOP_NIBBLE (sizeof(int) * 8 - 4)

void nr_txhex(int x)
    {
    int i;
    int k;

    for(i = (2 * sizeof(int)); i > 0; i--)
        {
        k = (x >> TOP_NIBBLE) & 0x000f;
        if(k < 10)
            k += '0';
        else
            k += 'a' - 10;

        nr_txchar(k);

        x <<= 4;
        }
    }

void nr_txchar2(int c, int channel)
  	{
  	r_ensure_printf_init();
    if ( ( channel == nk_stdout ) || ( channel == nk_stderr ) )
  	    nm_printf_txchar(c, nasys_printf_uart );    // stdout and stderr are the same for UARTs
    else
  	    nm_printf_txchar(c, (void*)channel );       // assume that channel must be a UART (and that the UART has been initialized)
  	}                                               // (or it could be nasys_oci_core + 2 (which doesn't need initializing) from nr_fprintf()

void nr_txstring(char *s)
	{
	while(*s)
		nr_txchar(*s++);
	}
#endif

#ifdef nm_printf_rxchar
int nr_rxchar(void)
	{
	r_ensure_printf_init();
	return nm_printf_rxchar(nasys_printf_uart);
	}
#endif

// +---------------------------
// | debug uart routines
// |

#ifdef nasys_debug_uart

static void r_ensure_debug_init(void)
	{
	static int inited = 0;

	if(!inited)
		{
#ifdef nm_debug_initialize
		nm_debug_initialize(nasys_debug_uart);
#endif
		inited = 1;
		}
	}

#ifdef nm_debug_txchar
void nr_debug_txchar(int c)
	{
	r_ensure_debug_init();
	nm_debug_txchar(c,nasys_debug_uart);
	}

void nr_debug_txstring(char *s)
	{
	while(*s)
		nr_debug_txchar(*s++);
	}
#endif

#ifdef nm_debug_rxchar
int nr_debug_rxchar(void)
	{
	r_ensure_debug_init();
	return nm_debug_rxchar(nasys_debug_uart);
	}
#endif

#endif // nasys_debug_uart

// end of file
