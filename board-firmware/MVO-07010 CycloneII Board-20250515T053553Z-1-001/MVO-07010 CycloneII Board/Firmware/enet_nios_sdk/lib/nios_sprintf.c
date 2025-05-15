/*

	file: nios_sprintf.c

	contents: Implementation of printf engine
						that does not support floating point,
						but is nice and small.

	author: david van brink \ altera corporation

*/

#include "excalibur.h"

#include <stdarg.h>		// manages variable-length argument passing
#include <string.h>

typedef void (*PrintfCharRoutine)(int c,int context);

void PrivatePrintf(const char *fmt,PrintfCharRoutine cProc,int context,va_list args);
static void r_repchar(char c,int r,PrintfCharRoutine cProc,int context);
static void r_sprintf_EachChar(int c,int context);

enum
	{
	pfState_chars,
	pfState_firstFmtChar,
	pfState_otherFmtChar
	};

static void r_repchar(char c,int r,PrintfCharRoutine cProc,int context)
	{
	while(r-- > 0)
		(cProc)(c,context);
	}

int nr_sprintf(char *sOut, const char *fmt, ... )
	{
	va_list args;

	*sOut = 0;

	va_start (args, fmt);
	PrivatePrintf(fmt,r_sprintf_EachChar,(int)(&sOut),args);
	return 0;
	}

void r_sprintf_EachChar(int c,int context)
	{
	char **sp = (char **)context;

	**sp = c;
	(*sp)++;
	**sp = 0;
	}

void PrivatePrintf(const char *fmt,PrintfCharRoutine cProc,int context,va_list args)
	{
	const char *w;
	char c;
	int state;
	int fmtLeadingZero = 0; // init these all to 0 for -W warnings.
	int fmtLong = 0;
	int fmtBeforeDecimal = 0;
	int fmtAfterDecimal = 0;
	int fmtBase = 0;
	int fmtSigned = 0;
	int fmtCase = 0; // For hex format, if 1, A-F, else a-f.

	w = fmt;
	state = pfState_chars;

	while(0 != (c = *w++))
		{
		switch(state)
			{
			case pfState_chars:
				if(c == '%')
					{
					fmtLeadingZero = 0;
					fmtLong = 0;
					fmtBase = 10;
					fmtSigned = 1;
					fmtCase = 0; // Only %X sets this.
					fmtBeforeDecimal = -1;
					fmtAfterDecimal = -1;
					state = pfState_firstFmtChar;
					}
				else
					(cProc)(c,context);
				break;

			case pfState_firstFmtChar:
				if(c == '0')
					{
					fmtLeadingZero = 1;
					state = pfState_otherFmtChar;
					}
				else if(c == '%')
					{
					(cProc)(c,context);
					state = pfState_chars;
					}
				else
					{
					state = pfState_otherFmtChar;
					goto otherFmtChar;
					}
				break;

			case pfState_otherFmtChar:
otherFmtChar:
				if(c == '.')
					fmtAfterDecimal = 0;
				else if('0' <= c && c <= '9')
					{
					c -= '0';
					if(fmtAfterDecimal < 0)			// still before decimal
						{
						if(fmtBeforeDecimal < 0)
							fmtBeforeDecimal = 0;
						else
							fmtBeforeDecimal *= 10;
						fmtBeforeDecimal += c;
						}
					else
						fmtAfterDecimal = (fmtAfterDecimal * 10) + c;
					}
				else if(c == 'l')
					fmtLong = 1;
				else									// we're up to the letter which determines type
					{
					switch(c)
						{
						case 'd':
						case 'i':
doIntegerPrint:
								{
								unsigned long v;
								unsigned long p;	// biggest power of fmtBase
								unsigned long vShrink;	// used to count digits
								int sign;
								int digitCount;

								// Get the value
								if(fmtLong)
									if (fmtSigned)
										v = va_arg(args,long);
									else
										v = va_arg(args,unsigned long);
								else
									if (fmtSigned)
										v = va_arg(args,int);
									else
										v = va_arg(args,unsigned int);

								// Strip sign
								sign = 0;
									// (assumes sign bit is #31)
								if( fmtSigned && (v & (0x80000000)) )
									{
									v = ~v + 1;
									sign = 1;
									}

								// Count digits, and get largest place value
								vShrink = v;
								p = 1;
								digitCount = 1;
								while( (vShrink = vShrink / fmtBase) > 0 )
									{
									digitCount++;
									p *= fmtBase;
									}

								// Print leading characters & sign
								fmtBeforeDecimal -= digitCount;
								if(fmtLeadingZero)
									{
									if(sign)
										{
										(cProc)('-',context);
										fmtBeforeDecimal--;
										}
									r_repchar('0',fmtBeforeDecimal,cProc,context);
									}
								else
									{
									if(sign)
										fmtBeforeDecimal--;
									r_repchar(' ',fmtBeforeDecimal,cProc,context);
									if(sign)
										(cProc)('-',context);
									}

								// Print numbery parts
								while(p)
									{
									unsigned char d;

									d = v / p;
									d += '0';
									if(d > '9')
										d += (fmtCase ? 'A' : 'a') - '0' - 10;
									(cProc)(d,context);

									v = v % p;
									p = p / fmtBase;
									}
								}

							//nr_uart_txhex(va_arg(args,int));
							state = pfState_chars;
							break;

						case 'u':
							fmtSigned = 0;
							goto doIntegerPrint;
						case 'o':
							fmtSigned = 0;
							fmtBase = 8;
							goto doIntegerPrint;
						case 'x':
							fmtSigned = 0;
							fmtBase = 16;
							goto doIntegerPrint;
						case 'X':
							fmtSigned = 0;
							fmtBase = 16;
							fmtCase = 1;
							goto doIntegerPrint;

						case 'c':
							r_repchar(' ',fmtBeforeDecimal-1,cProc,context);
							(cProc)(va_arg(args,int),context);
							break;

						case 's':
								{
								char *s;

								s = va_arg(args,char *);
								r_repchar(' ',fmtBeforeDecimal-strlen(s),cProc,context);

								while(*s)
									(cProc)(*s++,context);
								}
							break;
						} /* switch last letter of fmt */
					state=pfState_chars;
					}
				break;
			} /* switch */
		} /* while chars left */
	} /* printf */


// end of file
