//     mem_tst1.c 

/*

  1-0-07 mem_tst.c
		Basic SRAM test.  Writes 0's, 5's A's and F's into each memory location above base+THIS_PROG_SIZE.  Does
		a byte r/w test, halfword r/w test and a full word r/w test.  Then it does a "hammer" 32 bit test in which
		all locations are initialized to 0x12345678.  Then the base address and top address are hammered 1000 times,
		then the initialization values are restored and the entire memory is checked again.  

		Ram below THIS_PROG_SIZE is not tested.  If we're successfully executing code from this low memory and
		all of the other ram is testing out, then the low memory is likely good also.  Memory writing is interleved 
		with all exen addresses written first then the odd addresses.  An "*" is printed after each test.
		Any errors will be printed as well.

		Using uart routines uses less memory than printf's, so we'll use them here.

*/


//#include <stdio.h>
#include "nios.h"



/*///////////////////// BUILDING THIS VERSION /////////////////////////

1, From the SOPC builder command shell

	a, nios2-build -b 0x2000000 mem_tst.c<CR> if CycloneII - OR - nios2-build -b 0x400000 mem_tst.c<CR> if Apex20KE

	
*/

#define THIS_PROG_SIZE 0xa74 // From the nm file



int main(void)
	{
	int x, y, ctr=0, test_ctr=1, num_of_bytes;
	unsigned int test_value=0x00000000, z;


	
	while(1)
		{
		
		// First test each data bit
		for(x=0; x<32; x++)
			{
			if (x==0) z=0x00000001;
			z<<=x;
			*((unsigned int *)(na_SRAM1_base+THIS_PROG_SIZE)) = z;
			if (*((unsigned int *)(na_SRAM1_base+THIS_PROG_SIZE)) != z)
				{
				nr_uart_txstring("\nRAM err testing data bits, bit "); nr_uart_txhex(z); nr_uart_txstring("\n");
				}
			}

		// Now test address bits, we'll stay above 0x00001000
#if 0
		x = na_SRAM1_base+0x00001000;
		do
			{
			*((unsigned int *)x) = 0x12345678;
			if (*((unsigned int *)x) != 0x12345678;
				{
				nr_uart_txstring("\nRAM err testing data bits, bit "); nr_uart_txhex(z); nr_uart_txstring("\n");
				}
			
			*((unsigned int *)(x|0x1)) = 0x12345678;
			if (*((unsigned int *)x) != 0x12345678;
				{
				nr_uart_txstring("\nRAM err testing data bits, bit "); nr_uart_txhex(z); nr_uart_txstring("\n");
				}
			} while ();
#endif
			
		for (y=0; y<32; y++)
			{
			//ctr=0;
			z=0x00000001<<y;
			for (x=na_SRAM1_base+THIS_PROG_SIZE; x < na_SRAM1_base+na_SRAM1_size; x+=4)
				{
				if (z&0x80000000) z = 0x00000001;
				*((unsigned int *)x) = z<<ctr;
				//ctr++;
				//if (ctr == 32) ctr=0;
				z<<=1;
				}

			//ctr=0;
			z=0x00000001<<y;
			for (x=na_SRAM1_base+THIS_PROG_SIZE; x < na_SRAM1_base+na_SRAM1_size; x+=4)
				{
				if (z&0x80000000) z = 0x00000001;
				if (*((unsigned int *)x) != z<<ctr) 
					{
					nr_uart_txstring("\nRAM err testing alternating bits, addr "); nr_uart_txhex(x); 
					nr_uart_txstring(" expected "); nr_uart_txhex(z<<ctr);  
					nr_uart_txstring(" actual "); nr_uart_txhex(*((unsigned char *)x)); nr_uart_txstring("\n"); 
					}
				//ctr++;
				//if (ctr == 32) ctr=0;
				z<<=1;
				}
			}	
		

		test_ctr++; // Maybe we'll use this for something
		nr_uart_txstring("*");
		}
	}








