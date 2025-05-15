//     mem_test

// Ram test program.

// This program should be compiled to run at 0x130000 in flash.  Must erase
// flash before sending the .srec file to flash.

#include "nios.h"

int main(void)
	{
	volatile unsigned int *mem_ptr = na_SRAM1;
	int x;

	printf("Testing ram, base = 0x%x\n", mem_ptr);

	top: ;
	for (x=0; x<(na_SRAM1_size/4); x++)
		{
		//if (x==5000) mem_ptr[x]=0x99999999;
		//else mem_ptr[x] = 0;
		mem_ptr[x] = 0;
		}

	for (x=0; x<(na_SRAM1_size/4); x++)
		{
		if (mem_ptr[x] != 0)
			{
			printf("Ram failure, location 0x%x, expected 0 actual 0x%x\n", na_SRAM1_base+(x*4), mem_ptr[x]);
			break;
			}
		}

	printf("Tested all 0's\n");

	for (x=0; x<(na_SRAM1_size/4); x++)
		{
		//if (x==5000) mem_ptr[x]=0x99999999;
		//else mem_ptr[x] = 0x55555555;
		mem_ptr[x] = 0x55555555;
		}

	for (x=0; x<(na_SRAM1_size/4); x++)
		{
		if (mem_ptr[x] != 0x55555555)
			{
			printf("Ram failure, location 0x%x, expected 0x55555555 actual 0x%x\n", na_SRAM1_base+(x*4), mem_ptr[x]);
			break;
			}
		}

	printf("Tested 0x55555555\n");

	for (x=0; x<(na_SRAM1_size/4); x++)
		{
		//if (x==5000) mem_ptr[x]=0x99999999;
		//else mem_ptr[x] = 0xaaaaaaaa;
		mem_ptr[x] = 0xaaaaaaaa;
		}

	for (x=0; x<(na_SRAM1_size/4); x++)
		{
		if (mem_ptr[x] != 0xaaaaaaaa)
			{
			printf("Ram failure, location 0x%x, expected 0xaaaaaaaa actual 0x%x\n", na_SRAM1_base+(x*4), mem_ptr[x]);
			break;
			}
		}

	printf("Tested 0xaaaaaaaa\n");

	for (x=0; x<(na_SRAM1_size/4); x++)
		{
		//if (x==5000) mem_ptr[x]=0x99999999;
		//else mem_ptr[x] = 0xffffffff;
		mem_ptr[x] = 0xffffffff;
		}

	for (x=0; x<(na_SRAM1_size/4); x++)
		{
		if (mem_ptr[x] != 0xffffffff)
			{
			printf("Ram failure, location 0x%x, expected 0xffffffff actual 0x%x\n", na_SRAM1_base+(x*4), mem_ptr[x]);
			break;
			}
		}

	printf("Tested 0xffffffff\n");
	printf("Finished testing memory\n");
	goto top;
	}








