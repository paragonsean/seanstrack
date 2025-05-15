// file: "hello_dma.c"

// |
// | This program exercises the DMA in a rather
// | trivial fashion. It uses two of the
// | library routines, first to copy characters
// | from the UART to itself, and then from a
// | string to the UART.
// |
// | The UART must have its "Enable streaming data"
// | checkbox set for it to work; it's the only way
// | the DMA peripheral knows to slow down for it.
// |
// | These aren't really practical uses of the DMA
// | peripheral, since the library routines are
// | synchronous; the CPU is just waiting around for
// | the DMA to finish up most of the time.
// |
// | But this code, and the library routine code,
// | should give you a good starting point for
// | further adventures.
// |


#include "nios.h"


void demo1(void)
	{
	int character_count = 13;

	printf("DMA Demo 1\n");
	printf("The next %d characters you type will be\n",character_count);
	printf("echoed from the uart's rxdata directly to\n");
	printf("the txdata by the DMA peripheral.\n\n");
	printf("Start typing...\n\n");

	// This isn't terribly useful, especially since
	// this routine is synchronous, but it is an
	// interesting demonstration of the DMA peripheral.

	nr_dma_copy_1_to_1
		(
		nasys_dma_0,
		1,
		(void *)&nasys_printf_uart->np_uartrxdata,
		(void *)&nasys_printf_uart->np_uarttxdata,
		character_count
		);
	
	printf("\n\nAll done.\n\n\n");
	}

void demo2(void)
	{
	#define k_string "It has often struck me that an analysis of the causes\n" \
			"of wit would be a very interesting subject of enquiry.\n" \
			" -- Charles Babbage\n";
	
	char *s = k_string;

	printf("DMA Demo 2\n");
	printf("A string will be piped to the uart by the DMA peripheral.\n\n");
	printf("Press <space> to begin...\n\n");

	// wait for a key

	while(nr_uart_rxchar(0) != ' ')
		;

	// This isn't terribly useful, especially since
	// this routine is synchronous, but it is an
	// interesting demonstration of the DMA peripheral.

	nr_dma_copy_range_to_1
		(
		nasys_dma_0,
		1,
		(void *)s,
		(void *)&nasys_printf_uart->np_uarttxdata,
		strlen(s)
		);
	
	printf("\n\nAll done.\n\n\n");
	}

int main(void)
	{

	demo1();
	demo2();

	return 0;
	}



