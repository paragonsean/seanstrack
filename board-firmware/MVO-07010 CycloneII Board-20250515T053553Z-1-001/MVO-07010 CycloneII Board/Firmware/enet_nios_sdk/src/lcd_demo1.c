// file: lcd_demo1.c


#include <stdio.h>
#include "pio_lcd16207.h"
#include "nios.h"

// Prototype for Interrupt Service Routine

void MyPIO_ISR(int context);

// Main program

int main(void)
	{
	np_pio *pio = na_button_pio;

	/*
	 * Start up with a greeting...
	 */

	printf("\n\nNow running lcd_demo1.\n");
	printf("Press the buttons (not the reset!) to\n");
	printf("interrupt the program and show the time.\n");

	/*
	 * Set up the PIO to generate interrupts
	 */

	nr_installuserisr(na_button_pio_irq,MyPIO_ISR,(int)pio);

	pio->np_pioedgecapture = 0;		// clear any existing IRQ condition
	pio->np_piodirection = 0;		// all input
	pio->np_piointerruptmask = 0xff;	// they all generate irq's!


	/*
	 * Loop forever, or at least until
	 * you press <Esc>,
	 * showing some messages in the display...
	 */

	/*
	 * And, oh, by the way, we have a nice set of
	 * LCD routines already written...
	 */
	
	nr_pio_lcdinit(na_lcd_pio);

	while(nr_uart_rxchar(0) != 27)	// until ESC key...
		{
		nr_pio_lcdwritescreen("Hello!          This is Nios.");
		
		nr_delay(1400);

		nr_pio_lcdwritescreen("Your message goes here!");

		nr_delay(1400);

		nr_pio_lcdwritescreen("You could show   another message, too.");

		nr_delay(300);	// only flash that last one.

		printf("*");	// just so we have some signs of life.
		}
	
	return 0;
	}


void MyPIO_ISR(int context)
	{
	np_pio *pio = (np_pio *)context;
	char s[256];				// a string

	pio->np_pioedgecapture = 0;		// clear the irq condition

	/*
	 * Print a message into a string (RAM) first
	 */
	sprintf(s,"The time is now %d",nr_timer_milliseconds()); // get milliseconds...

	/*
	 * and print that message to the LCD
	 */
	nr_pio_lcdwritescreen(s);
	}

