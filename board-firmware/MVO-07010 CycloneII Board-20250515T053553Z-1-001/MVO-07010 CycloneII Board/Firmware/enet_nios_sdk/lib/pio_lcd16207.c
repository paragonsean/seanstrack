// file: pio_lcd16207.c
// author: Altera Santa Cruz \ 2000
// purpose: Control routines for the Optrex DMC16207
//          LCD display, as connected to an Excalibur
//          PIO at address na_lcd_pio
//

#include "pio_lcd16207.h"
#include "nios.h"	/* Descriptions of standard peripherals */


static void delayShort(void);


// Device constants


// Internal LCD signals

enum{
	LCD_E  = 0x400,
	LCD_RS = 0x200,
	LCD_RW = 0x100,
	LCD_DATA = 0x0FF
};


// LCD Device Commands
enum{
	LCD_CMD_CLEAR = 0x01,
	LCD_CMD_HOME  =	0x02,
	LCD_CMD_MODES = 0x04,
	LCD_CMD_ONOFF = 0x08,
	LCD_CMD_SHIFT = 0x10,
	LCD_CMD_FUNC  = 0x20,
	LCD_CMD_RAMCGR= 0x40,
	LCD_CMD_RAMDDR= 0x80,
	LCD_CMD_FILLER= 0x00
};


// Device Parameters
enum{
	LCD_INTERFACE = 0x10,		// interface width;1 (8 BITS)/0 (4BITS)
	LCD_NUMROWS =	0x08,		// # of display rows; 1 (2 ROWS)/0 (1ROW) 
	LCD_PIXEL	=	0x04,		// character pixel;	1(5x10 dots)/0 (5x7 dots)
	LCD_MOVE	=	0x02,		// character move direction; 1(increment)/0(decrement)
	LCD_SHIFT	=	0x01,		// move character with display shift; 1(shift)/0(no shift)
	LCD_DISP_SHIFT=	0x08		// action after character; 1(display shift)/0(character shift)
};

enum{
	LCD_COM_RESET = 0x30
};


// Some LCD parameters
#define LCD_BUS_WIDTH 8
#define LCD_PIXEL_WIDTH 5
#define LCD_PIXEL_HEIGHT 8






// The hardcoded definition of where the LCD lives at 
// PIO port address

np_pio *lcd = 0;

void nr_pio_lcdcursorvisible(int bVisible)
{
	if(bVisible)
		nr_pio_lcdwritecommand(0x0E);
	else 
		nr_pio_lcdwritecommand(0x0C);
}

void nr_pio_lcdon(int bOn)
{
	if(bOn)
		nr_pio_lcdwritecommand(0x0C);
	else 
		nr_pio_lcdwritecommand(0x08);
}

void nr_pio_lcdclearscreen(void)
{
	nr_pio_lcdwritecommand(LCD_CMD_CLEAR);
}


void nr_pio_lcdsetcursor(int row, int col)
{
	unsigned char address;

	if(row > LCD_NUM_ROW)
		row = LCD_NUM_ROW;

	if(col > LCD_NUM_COL)
		col = LCD_NUM_COL;

	if(row == 0)
		address = 0;
	else if (row == 1)
		address = 0x40;
	else if (row == 2)
		address = 0x20;
	else
		address=0x60;

	address += col;

	nr_pio_lcdwritecommand(LCD_CMD_RAMDDR | address);
}

void nr_pio_lcdprintstringslowly(char *string, int time)
{
	char tempBuffer[256];
	int i;

	i=0;
	sprintf(tempBuffer, string);

	// Show as much of the string as we can...
	while(tempBuffer[i])
	{
		nr_pio_lcdwritecharacter(tempBuffer[i]);
		nr_delay(time);
		i++;
	}
}


/*
 * nr_pio_lcdwritescreen(char *string);
 *
 * This is probably the most useful routine. It
 * takes a string, and prints it to the LCD starting
 * from the first character. If the string is less
 * than 32 characters, fill the rest of the display
 * with blanks. If the string exceeds 32 characters,
 * truncate it. So sorry!
 */

void nr_pio_lcdwritescreen(char *string)
{
	int row;
	int col;
	int done;
	int count;

	count=0;

	for(row=0; row < LCD_NUM_ROW; row++)
	{
		for(col=0; col < LCD_NUM_COL; col++)
		{
			nr_pio_lcdsetcursor(row,col);
			if(string[count] == 0x00)	// hit the end and stick there.
				nr_pio_lcdwritecharacter(0xA0);
			else
				nr_pio_lcdwritecharacter(string[count++]);
		}
	}
}

void nr_pio_lcdinit(np_pio *lcdPio)
{
	static int beenInitialized;	// uninitialized static variables get cleared each run!

	if(beenInitialized)
		return;
	
	if(!lcdPio)
		lcdPio = (np_pio *)0x480;	// pio in reference design...
	lcd = lcdPio;

	// Send first Reset
	nr_delay(15);	
	nr_pio_lcdwritecommand(LCD_COM_RESET);

	// Send second Reset
	nr_delay(5);	
	nr_pio_lcdwritecommand(LCD_COM_RESET);

	// Send third Reset
	nr_delay(1);
	nr_pio_lcdwritecommand(LCD_COM_RESET);

	// Setup interface parameters: 8 bit bus, 2 rows, 5x7 font
	nr_delay(1);
	nr_pio_lcdwritecommand(LCD_CMD_FUNC | LCD_INTERFACE | LCD_NUMROWS);
	
	// Turn Display Off
	nr_delay(1);
	nr_pio_lcdwritecommand(LCD_CMD_ONOFF);

	// Clear Display
	nr_delay(1);
	nr_pio_lcdwritecommand(LCD_CMD_CLEAR);
	
	// Setup Default Mode
	nr_delay(1);
	nr_pio_lcdwritecommand(LCD_CMD_MODES | LCD_MOVE);

	//ON
	nr_delay(1);
	nr_pio_lcdon(1);
	
	beenInitialized = 1;
}


void nr_pio_lcdwritecommand(unsigned char command)
{
	// preserve the data on the lines, but clear the RS and RW line
	lcd->np_piodata=(lcd->np_piodata & ~(LCD_RS | LCD_RW | LCD_E));
	
	// output the RS and RW lines.. but nothing else
	lcd->np_piodirection=(lcd->np_piodirection | (LCD_RS | LCD_RW | LCD_E));
	delayShort();

	// toggle the E line HIGH to latch the command
	lcd->np_piodata=(lcd->np_piodata | LCD_E);
	delayShort();
	
	// output command word whilst E line is high
	lcd->np_piodata=(lcd->np_piodata & ~LCD_DATA);
	lcd->np_piodata=(lcd->np_piodata | command);
	lcd->np_piodirection=(lcd->np_piodirection | LCD_DATA);

	// toggle the E line LOW to latch the data
	lcd->np_piodata=(lcd->np_piodata & ~LCD_E);
	delayShort();
}

void nr_pio_lcdwritecharacter(unsigned char character)
{
	// preserve the data on the lines, but clear the RW and E line, set RS
	lcd->np_piodata=(lcd->np_piodata & ~(LCD_RW | LCD_E));
	lcd->np_piodata=(lcd->np_piodata | LCD_RS);
	

	// output the RS and RW lines.. but nothing else
	lcd->np_piodirection=(lcd->np_piodirection | (LCD_RW | LCD_E | LCD_RS));
	delayShort();

	// toggle the E line HIGH to latch the command
	lcd->np_piodata=(lcd->np_piodata | LCD_E);
	delayShort();
	
	// output command word whilst E line is high
	lcd->np_piodata=(lcd->np_piodata & ~LCD_DATA);
	lcd->np_piodata=(lcd->np_piodata | character);
	lcd->np_piodirection=(lcd->np_piodirection | LCD_DATA);

	// toggle the E line LOW to latch the data
	lcd->np_piodata=(lcd->np_piodata & ~LCD_E);
}




void delayShort(void)
{
	int i;
	volatile int v = 0;
	for(i=0;i<50;i++)
		v++;
}		
