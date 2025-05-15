/* Everyone needs an Hello World program, right?
 * Here's Nios's very special one.
 * (c) 2001-2003 Altera Corporation
 */ 

#include "excalibur.h"


void show_decimal(int x)
	{
	nr_pio_showhex((x / 10) * 16 + x % 10);
	}



int main(void)
	{
	long i,delay;

    setbuf(stdout,0);  // turn off buffering for printf

	printf("\n\n\n");
	printf("Hello, from Nios!\n");
	printf("Please observe the Nios board...\n");

	// Now, count the LED's:
	for(i = 99; i >= 0; i--)
		{
		show_decimal(i);
		printf("%d ",i);
		nr_delay(150);
		}

	printf("Good bye.\n\004");	// control-D tells terminal program, We are done.
	} // The end.
