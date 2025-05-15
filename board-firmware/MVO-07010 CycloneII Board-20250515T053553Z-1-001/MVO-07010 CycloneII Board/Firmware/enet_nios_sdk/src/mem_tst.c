//     mem_tst.c 

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

#define THIS_PROG_SIZE 0xc88 // From the nm file
//#define THIS_PROG_SIZE 0x1000 


int main(void)
	{
	int x, y, ctr=0, test_ctr=1, num_of_bytes;
	unsigned int test_value=0x00000000;

	
	while(1)
		{
		ctr=0;

		for (num_of_bytes=1; num_of_bytes<5; num_of_bytes++) // 1 == bytes, 2 == halfwords, 3 == skip and 4 == full 32 bit words
			{
			if (num_of_bytes==3) { } // Skip 
			else
				{
				while(ctr<4) // Four values written per test, 0's, 5's, A's and F's
					{
					if(ctr==0) test_value = 0x00000000;
					else if (ctr==1) test_value = 0x55555555;
					else if (ctr==2) test_value = 0xaaaaaaaa;
					else if (ctr==3) test_value = 0xffffffff;
					
					// First clear ram, probably not necessary, enable if desired
					for (x=na_SRAM1_base+THIS_PROG_SIZE; x < na_SRAM1_base+na_SRAM1_size; x++)
						{
						*((unsigned char *)x) = 0x00; // Clear byte by byte
						}
					
					// Interleave value to be written, first do even words
					for (x=na_SRAM1_base+THIS_PROG_SIZE; x < na_SRAM1_base+na_SRAM1_size; x+=(num_of_bytes*2))
						{
						if (num_of_bytes == 1) *((unsigned char *)x) = (unsigned char)test_value; // bytes
						else if (num_of_bytes == 2) *((unsigned short int *)x) = (unsigned short int)test_value; // halfwords
						else if (num_of_bytes == 4) *((unsigned int *)x) = test_value; // words
						else nr_uart_txstring("Error!!! invalid num_of_bytes when writing evens\n");
						}

					//*((int *)(na_SRAM1_base+0x8000)) = 0x12345678; // For test, force an error

					// check ram
					for (x=na_SRAM1_base+THIS_PROG_SIZE, y=0; x < na_SRAM1_base+na_SRAM1_size; x+=num_of_bytes, y++)
						{
						if (num_of_bytes == 1) // bytes
							{
							if ((!(y&1) && (*((unsigned char *)x) != (unsigned char)test_value)) ||
								(y&1 && (*((unsigned char *)x) != 0x00))) 
								{
								nr_uart_txstring("\nRAM err testing bytes, addr "); nr_uart_txhex(x); 
								nr_uart_txstring(" expected "); nr_uart_txhex((unsigned char)test_value);  
								nr_uart_txstring(" actual "); nr_uart_txhex(*((unsigned char *)x)); nr_uart_txstring("\n"); 
								}
							}
						else if (num_of_bytes == 2) // halfwords
							{
							if ((!(y&1) && (*((unsigned short int *)x) != (unsigned short int)test_value)) ||
								(y&1 && (*((unsigned short int *)x) != 0x0000))) 
								{
								nr_uart_txstring("\nRAM err testing halfwords, addr "); nr_uart_txhex(x); 
								nr_uart_txstring(" expected "); nr_uart_txhex((unsigned short int)test_value);  
								nr_uart_txstring(" actual "); nr_uart_txhex(*((unsigned short int *)x)); nr_uart_txstring("\n"); 
								}
							}
						else if (num_of_bytes == 4) // words
							{
							if ((!(y&1) && (*((unsigned int *)x) != test_value)) ||
								(y&1 && (*((unsigned int *)x) != 0x00000000))) 
								{
								nr_uart_txstring("\nRAM err testing words, addr "); nr_uart_txhex(x); 
								nr_uart_txstring(" expected "); nr_uart_txhex(test_value);  
								nr_uart_txstring(" actual "); nr_uart_txhex(*((unsigned int *)x)); nr_uart_txstring("\n"); 
								}
							}
						else nr_uart_txstring("Error!!! invalid num_of_bytes when checking ram\n");
						}
					
					// Clear ram again  
					for (x=na_SRAM1_base+THIS_PROG_SIZE; x < na_SRAM1_base+na_SRAM1_size; x++)
						{
						*((unsigned char *)x) = 0x00; // Clear byte by byte
						}

					// Then write odd
					for (x=na_SRAM1_base+THIS_PROG_SIZE+num_of_bytes; x < na_SRAM1_base+na_SRAM1_size; x+=(num_of_bytes*2))
						{
						if (num_of_bytes == 1) *((unsigned char *)x) = (unsigned char)test_value; // bytes
						else if (num_of_bytes == 2) *((unsigned short int *)x) = (unsigned short int)test_value; // halfwords
						else if (num_of_bytes == 4) *((unsigned int *)x) = test_value; // words
						else nr_uart_txstring("Error!!! invalid num_of_bytes when writing odds\n");
						}
					
					//*((int *)(na_SRAM1_base+0x8000)) = 0x12345678; // For test, force an error
					
					// check ram again
					for (x=na_SRAM1_base+THIS_PROG_SIZE, y=0; x < na_SRAM1_base+na_SRAM1_size; x+=num_of_bytes, y++)
						{
						if (num_of_bytes == 1) // bytes
							{
							if ((y&1 && (*((unsigned char *)x) != (unsigned char)test_value)) ||
								(!(y&1) && (*((unsigned char *)x) != 0x00))) 
								{
								nr_uart_txstring("\nRAM err testing bytes, addr "); nr_uart_txhex(x); 
								nr_uart_txstring(" expected "); nr_uart_txhex((unsigned char)test_value);  
								nr_uart_txstring(" actual "); nr_uart_txhex(*((unsigned char *)x)); nr_uart_txstring("\n"); 
								}
							}
						else if (num_of_bytes == 2) // halfwords
							{
							if ((y&1 && (*((unsigned short int *)x) != (unsigned short int)test_value)) ||
								(!(y&1) && (*((unsigned short int *)x) != 0x0000))) 
								{
								nr_uart_txstring("\nRAM err testing halfwords, addr "); nr_uart_txhex(x); 
								nr_uart_txstring(" expected "); nr_uart_txhex((unsigned short int)test_value);  
								nr_uart_txstring(" actual "); nr_uart_txhex(*((unsigned short int *)x)); nr_uart_txstring("\n"); 
								}
							}
						else if (num_of_bytes == 4) // words
							{
							if ((y&1 && (*((unsigned int *)x) != test_value)) ||
								(!(y&1) && (*((unsigned int *)x) != 0x00000000))) 
								{
								nr_uart_txstring("\nRAM err testing words, addr "); nr_uart_txhex(x); 
								nr_uart_txstring(" expected "); nr_uart_txhex(test_value);  
								nr_uart_txstring(" actual "); nr_uart_txhex(*((unsigned int *)x)); nr_uart_txstring("\n"); 
								}
							}
						else nr_uart_txstring("Error!!! invalid num_of_bytes when checking ram\n");
						}
					ctr++;
					}
				ctr=0;
				}
			}

		// Now do a 32 bit memory test which hammers the low address along with the top address.
		// The purpose of this is to switch large numbers of address bits to see if we can find
		// memory locations changing.

		// First initialize ram, we have to do this for this test
		for (x=na_SRAM1_base+THIS_PROG_SIZE; x < na_SRAM1_base+na_SRAM1_size; x+=4)
			{
			*((unsigned int *)x) = 0x12345678; // Initialize to this
			}

		// Now hammer low address followed by to address
		for(x=0; x<1000; x++)
			{
			*((unsigned int *)(na_SRAM1_base+THIS_PROG_SIZE)) = 0xffffffff; // low
			*((unsigned int *)((na_SRAM1_base+na_SRAM1_size)-4)) = 0xffffffff; // top

			*((unsigned int *)(na_SRAM1_base+THIS_PROG_SIZE)) = 0x00000000; // low again
			*((unsigned int *)((na_SRAM1_base+na_SRAM1_size)-4)) = 0x00000000; // top again
			}
		
		// Restore the initialization values
		*((unsigned int *)(na_SRAM1_base+THIS_PROG_SIZE)) = 0x12345678; 
		*((unsigned int *)((na_SRAM1_base+na_SRAM1_size)-4)) = 0x12345678; 

		//*((int *)(na_SRAM1_base+0x8000)) = 0x99999999; // For test, force an error

		// Check to see if all the ram is still set to the initialization value
		for (x=na_SRAM1_base+THIS_PROG_SIZE; x < na_SRAM1_base+na_SRAM1_size; x+=4)
			{
			if (*((unsigned int *)x) != 0x12345678) 
				{
				nr_uart_txstring("\nRAM err hammer test, addr "); nr_uart_txhex(x); 
				nr_uart_txstring(" expected 0x12345678");   
				nr_uart_txstring(" actual "); nr_uart_txhex(*((unsigned int *)x)); nr_uart_txstring("\n"); 
				}
			//else nr_uart_txstring(" cool ");
			}

		test_ctr++; // Maybe we'll use this for something
		nr_uart_txstring("*");
		}
	}








