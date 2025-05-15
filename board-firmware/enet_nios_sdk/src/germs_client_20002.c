//START_MODULE_HEADER////////////////////////////////////////////////////////
//
// Filename:    nios-run.c
//
// Description: code downloader for Nios development board
//
// Authors:     mfairman
//
//              Copyright (c) Altera Corporation 1997 - 2000
//              All rights reserved.
//
//END_MODULE_HEADER//////////////////////////////////////////////////////////

//START_ALGORITHM_HEADER/////////////////////////////////////////////////////
//
//
//END_ALGORITHM_HEADER///////////////////////////////////////////////////////
/*
** $Log$
*/


/* This program is built with the following command line

	gcc germs_client_20002.c -w -DF_TARGET_WINDOWS=1 -o germs_client_20002

	
	4-4-03
	This is a modified version of Altera's germs_client that uses TCP as the network
	protocol.  

	The char send routine has been change to send an entire string insted of one char
	at a time while in network mode.  Serial mode should work as before.
	
*/


// TARGET SETUP
// Depending on what F_TARGET_(target name) is set, we configure
// a few #defines
//
// F_USE_WINDOWS_COMM -- use WIN32 serial API, not termios
// F_INCLUDE_INTTYPES -- works around some horrible solaris termios.h omission
// F_TOP_BAUD_RATE -- highest supported (and the default) baud rate
// F_SERIAL_PORT_NAME -- name of the serial driver

#if F_TARGET_WINDOWS
	
	#define F_USE_WINDOWS_COMM	1
	#define F_INCLUDE_INTTYPES	0
	#define F_TOP_BAUD_RATE		115200
	#define F_SERIAL_PORT_NAME	"COM1:"

	#include <sys/socket.h>

#elif F_TARGET_HPUX
	
	#define F_USE_WINDOWS_COMM	0
	#define F_INCLUDE_INTTYPES	0
	#define F_TOP_BAUD_RATE		115200
	#define F_SERIAL_PORT_NAME	"/dev/tty0p0"

	#include <sys/socket.h>
	#include <sys/int_types.h>

#elif F_TARGET_SOLARIS

	#define F_USE_WINDOWS_COMM	0
	#define F_INCLUDE_INTTYPES	1
	#define F_TOP_BAUD_RATE		115200
	#define F_SERIAL_PORT_NAME	"/dev/ttyb"

	#include <sys/socket.h>
	#include <sys/int_types.h>

#else

	#pragma error "Define a valid target, please."

#endif


// INCLUDE FILES ////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

	#if F_INCLUDE_INTTYPES
	// needed by termios.h under Solaris, ick. (dvb)
	#include <sys/int_types.h>
	#endif

#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#if F_USE_WINDOWS_COMM
#include <windows.h>
#include <arpa/inet.h>
#include <cygwin/in.h>
#endif


// MACRO DEFINITIONS ////////////////////////////////////////////////////////
#define LINEBUF_SIZE	1024
#define FILEPATH_SIZE	1024
#define MAX_EXECUTE_CMDS	128

// ENUMERATIONS /////////////////////////////////////////////////////////////
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

// TYPEDEFS /////////////////////////////////////////////////////////////////
#if !F_USE_WINDOWS_COMM
typedef int BOOL;
typedef unsigned long DWORD;
#endif

// CLASS AND STRUCTURE DEFINITIONS //////////////////////////////////////////

// CLASS STATIC DATA ////////////////////////////////////////////////////////

static long perCharSlowdown = 0;		// global, so port_write may use it.
struct sockaddr_in * gSocketStructPtr;

typedef struct _ePortStruct
{
  int socket_id;
  struct sockaddr_in * socket;
 
  // stuff used by the select functions for timeouts, etc...
  fd_set set;
  struct timeval time;
 
  
} ePortStruct;

ePortStruct * gPortInfoPtr;

// INLINE FUNCTIONS /////////////////////////////////////////////////////////

// LOCAL VARIABLE DECLARATIONS //////////////////////////////////////////////

// FOWARD FUNCTION PROTOTYPES ///////////////////////////////////////////////
int key_read(void);
int serial_port_open(char *portName, int baudRate);
int serial_port_write(int portHandle, char *pData, int nCount);
int serial_port_read(int portHandle, char *pData, int nCount);
int port_close(int portHandle);
int network_port_write(int portHandle, char *pData, int nCount);
int network_port_read(int portHandle, char *pData, int nCount);
typedef int (*fPtrOp)(int portHandle, char *pData, int nCount);
int setup_network_port(ePortStruct *port);
int init_network_port(ePortStruct **port);

void delay_milliseconds(int milliseconds);
void delay_useconds(int useconds);

// MESSAGE MAP //////////////////////////////////////////////////////////////

// CLASS STATIC FUNCTION BODIES /////////////////////////////////////////////

// CLASS MEMBER FUNCTION BODIES /////////////////////////////////////////////

// GLOBAL SCOPE FUNCTION BODIES /////////////////////////////////////////////


int isPromptChar(char c)
	{
	return (c == '+');
	}

int main(int argc, char *argv[])
{
	char **argvPtr;
	char c, sendstr[81];
	char cLast, clast;
	int key, cctr;
	short resultCode = 0;
	char portName[FILEPATH_SIZE], srecName[FILEPATH_SIZE];
	char executeCmd[MAX_EXECUTE_CMDS][FILEPATH_SIZE];
	int numofexecuteCmds = 0;
	int baudRate;
	int portHandle;
	int timeoutSeconds;
	FILE *f = NULL;
	BOOL bTerminalDone;
	BOOL bDebug = FALSE;
	BOOL bTimeout = FALSE;
	BOOL bTimeStamp = FALSE;	// if set, print time before each line
	BOOL bTerminal = FALSE;
	BOOL bExit = FALSE;
	BOOL bNetwork = FALSE;
	struct timeval lastLineStartTime;		// base time for timestamping...
	struct timeval lineStartTime;			// time of each carriage return...
	struct timeval timeoutStartTime;		// base time for timeout...
	struct timeval timeoutCheckTime;			// time of each check for timeout...
	int charpos = 0;						// position on line
	char ipAddress[256], tcp_port[256];
	
	fPtrOp port_write, port_read;

	cctr = 0;

	gettimeofday(&lastLineStartTime,NULL);

	// default parameters
	baudRate = F_TOP_BAUD_RATE;
	strcpy(portName, F_SERIAL_PORT_NAME);

	// parse command line
	if (argc < 2)
		goto show_usage;
	--argc;
	argvPtr = &argv[1];
	while (argc > 0)		// for as long as we have parameters
	{	char *argPtr;
	
		argPtr = *argvPtr;
		c = *argPtr++;
		if (c != '-') break;		// break when done with switches
		c = *argPtr++;			// get switch character
		--argc; ++argvPtr;		// move to next parm
		switch (tolower(c))
		{
			case 'b':	// baud
				if (argc > 0)
				{
					if (sscanf(*argvPtr, "%i", &baudRate) != 1)
						baudRate = 115200;
					--argc; ++argvPtr;
				}
				break;

			case 'd':
				bDebug = TRUE;
				break;

			case 'e':	// execute
				if (argc > 0)
				{
					strcpy(executeCmd[numofexecuteCmds], *argvPtr);
					--argc; ++argvPtr;
					printf ("executeCmd[%d] is %s\n", numofexecuteCmds, executeCmd[numofexecuteCmds]);
					numofexecuteCmds++;
				}
				break;	
			case 'o':	// timeout
				if (argc > 0)
				{
					if (sscanf(*argvPtr, "%i", &timeoutSeconds) != 1)
						timeoutSeconds = 10;
					--argc; ++argvPtr;
				} else {
					timeoutSeconds = 10;
				}
				bTimeout = TRUE;
				break;

			case 'p':	// port
				if (argc > 0)
				{
					strcpy(portName, *argvPtr);
					--argc; ++argvPtr;
				}
				break;	

			case 's':	// slowdown
				if(argc > 0)
				{
					sscanf(*argvPtr,"%d",&perCharSlowdown);
					printf ("per char slowdown = %d\n",perCharSlowdown);
					--argc; ++argvPtr;
				}
				break;

			case 't':
				bTerminal = TRUE;
				break;
				
			case 'x':
				bExit = TRUE;
				break;

			case 'z':
				bTimeStamp = TRUE;
				break;

			case 'h':
			case '?':
				goto show_usage;
				return 0;
			break;

			case 'n':
				bNetwork=TRUE;
				if(argc > 0)
				{
					sscanf(*argvPtr,"%s",ipAddress);
					printf ("GERMS server IP address = %s\n",ipAddress);
					--argc; ++argvPtr;
				}
				else
				{
					printf ("No IP address specified for GERMS server!\n");
					printf ("Exiting\n");
					return 0;
				}
			break;

			case 'r':
				if(argc > 0)
				{
					sscanf(*argvPtr,"%s",tcp_port);
					printf ("tcp port number = %s\n",tcp_port);
					--argc; ++argvPtr;
				}
				else
				{
					printf ("No tcp port number specified for GERMS server!\n");
					printf ("Exiting\n");
					return 0;
				}
			break;
		}
	}

	// setup zee function pointers for the given hardware
	if(bNetwork)
 	{
	  port_write=network_port_write;
	  port_read=network_port_read;
	}
	else
	{
	  port_write=&serial_port_write;
	  port_read=&serial_port_read;
	}

	// get filename to download, make sure we can open file
	if (!bTerminal)
	{
		if (argc < 1)
		{
			printf("### nios-run: please specify a filename on the command line\n");
			printf("###           or use '-t' to enter terminal mode\n");
			return -1;
		}
		strcpy(srecName, *argvPtr);

		// (We used to append .srec to name, but now take it raw. dvb 2000 june)
		//if (strstr(srecName, ".srec") == (char *) NULL)
		//	strcat(srecName, ".srec");

		f = fopen(srecName, "r");
		if (f == NULL)
		{
			printf("### nios-run: could not open %s.\n", srecName);
			return -1;
		}
	}

	if(bNetwork)
	{

	  // load the structures with data (IP, Netmask, etc.)
	  init_network_port(&gPortInfoPtr);

          gPortInfoPtr->socket->sin_family = AF_INET;
  	  gPortInfoPtr->socket->sin_port = htons((unsigned short)atoi(tcp_port));
	  //gPortInfoPtr->socket->sin_port = htons(20002);  
	  gPortInfoPtr->socket->sin_addr.s_addr = inet_addr(ipAddress);

	  gPortInfoPtr->socket_id = socket(PF_INET, SOCK_STREAM, PF_UNSPEC);


          setup_network_port(gPortInfoPtr);
  	  portHandle = gPortInfoPtr->socket_id;
	}
	else
	{
	  // open serial port
	  portHandle = serial_port_open(portName, baudRate);
	}

	if (portHandle != 0)
	{	char c, lineBuf[LINEBUF_SIZE];
		struct termios stdin_state, stdin_save;

		// take over stdin for download/terminal mode
		tcgetattr(0, &stdin_save);
		if(tcgetattr(0,&stdin_state) == 0)
		{
			stdin_state.c_lflag &= ~(ISIG | ICANON | ECHO);
			stdin_state.c_cc[VMIN] = 1; stdin_state.c_cc[VTIME] = 0;
			if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&stdin_state) == -1)
				printf("tcsetattr failed\n");

			//if (fcntl(STDIN_FILENO, F_SETFL, FNDELAY) == -1)
				//printf("fcntl failed\n");
		}

		// wait for prompt (unless we're jumping to terminal mode)
		if (!bTerminal)
		{	struct timeval curTime, lastTime;
			int promptedUserForReset=0;			// prompt user at most once...

			// ---------
			// We might not need a user-reset -- send " \n" to try to
			// evoke a promt
			lineBuf[0] = '.';
			lineBuf[1] = 13;
			lineBuf[2] = 0;
		   port_write(portHandle, lineBuf, strlen(lineBuf));

			delay_milliseconds(100);	// time to respond to " \n" prodding


			c = 0;
			gettimeofday(&lastTime, NULL);
			while ((key = key_read()) != 0x03)
			{
				// read as much as is available...
				while (port_read(portHandle, &c, sizeof(c)) != 0)
				{
					if (isPromptChar(c))	// board responded?
						goto gotMonitorChar;
					if (bDebug)
						printf("rx: %c(%02X)\n", c, c);
				}
				sleep(0);

				if(!promptedUserForReset)
					{
					promptedUserForReset = 1;

					printf("\n");
					printf("nios-run: Ready to download %s over %s at %d bps\n", srecName, portName, baudRate);
					printf("        : Press SW3 (CLEAR) on target board to begin download\n");
					printf("        : Type Control-C to exit the program\n");
					printf("nios-run: Waiting for target");
					fflush(stdout);
					}

				gettimeofday(&curTime, NULL);
				if (curTime.tv_sec != lastTime.tv_sec)
				{
					printf("."); fflush(stdout);
					lastTime = curTime;
				}
			}
gotMonitorChar:
			if (key == 0x03)
				goto end_terminal;

			// download S-records
			printf("\nnios-run: Downloading");
			if (bDebug)
				printf("\n");
			fflush(stdout);
			while (fgets(lineBuf, sizeof(lineBuf), f) != NULL)
			{	int len;
				int bIsGoCommand;	// TRUE if it's an S[789] or G command.

				// make sure EOL sequence is: 0x0d, 0x00
				len = strlen(lineBuf);

				// Remove the CR or LF or whatever

				while(len && lineBuf[len - 1] < 0x20)
					len--;

				// Append a CR

				lineBuf[len++] = 0x0d;
				lineBuf[len++] = 0;

				bIsGoCommand = (lineBuf[0] == 'G')
					|| (lineBuf[0] == 'g')
					||  (lineBuf[0] == 'S'
					&& lineBuf[1] >= '7'
					&& lineBuf[1] <= '9');
					
				port_write(portHandle, lineBuf, strlen(lineBuf));
				if (!bDebug)
					printf(".");
				else
					printf("down: %s", lineBuf);
				fflush(stdout);

				// wait until target got it and responds
				// new monitor that echoes all characters
				{
				int k;
				int c;

				do
					{
					k = key_read();
					if(k == 3)			// ctrl C
						goto end_terminal;

					c = port_read(portHandle, lineBuf, 1);
					if(c)
						{
						c = lineBuf[0];
						if(c == '!')
							{
							printf("!");
							fflush(stdout);
							}
						if (bDebug)
							printf("rx: %c(%02X)\n", c, c);
						}
					sleep(0);
					} while(c != 0x0d);	// wait for CR
				
				// Print anything between CR and next prompt...
				// Could be an error message, etc.
				// Unless it's a "Go" command...
				if(!bIsGoCommand)
					do
					{
						if(key_read() == 3)		// ctrl C
							goto end_terminal;
	
						c = port_read(portHandle, lineBuf, 1);
						if(c)
						{
							c = lineBuf[0];
							if(c >= ' ' && !isPromptChar(c))
								printf("%c",c);
						}
						
					} while(!isPromptChar(c));	// wait for prompt
				}
				
			}
		}

		if (numofexecuteCmds != 0 ) {
			int i;

			// ---------
			// We might not need a user-reset -- send " \n" to try to
			// evoke a prompt
			lineBuf[0] = 13;
			lineBuf[1] = 0;
		
		   port_write(portHandle, lineBuf, strlen(lineBuf));

			delay_milliseconds(100);	// time to respond to " \n" prodding


			c = 0;
			while ((key = key_read()) != 0x03)
			{

				// read as much as is available...
				while (port_read(portHandle, &c, sizeof(c)) != 0)
				{
					if (isPromptChar(c))	// board responded?
						goto ExecutegotMonitorChar;
					if (bDebug) {
						printf("rx: %c(%02X)\n", c, c);
					} else {
						printf("%c",c);
					}
				}
				sleep(0);
			}
ExecutegotMonitorChar:
			if (key == 0x03)
				goto end_terminal;
			for (i=0; i<numofexecuteCmds; i++)
			{	int len;

				// make sure EOL sequence is: 0x0d, 0x00
				len = strlen(executeCmd[i]);

				// Remove the CR or LF or whatever

				while(len && executeCmd[i][len - 1] < 0x20)
					len--;

				// Append a CR

				executeCmd[i][len++] = 0x0d;
				executeCmd[i][len++] = 0;

				port_write(portHandle, executeCmd[i], strlen(executeCmd[i]));
				fflush(stdout);
				do
				{
					if(key_read() == 3)		// ctrl C
						goto end_terminal;

					c = port_read(portHandle, lineBuf, sizeof(char));
					if(c)
					{
						c = lineBuf[0];
//						if(c >= ' ' && !isPromptChar(c))
						if(c >= ' ')
//						if(!isPromptChar(c))
							printf("%c",c);
						if(c == 0x0d || c==0x0a)
							printf("\n");

					}
					
				} while(!isPromptChar(c));	// wait for prompt
			}

		}


		if (bExit)
			goto end_terminal;

		// enter terminal mode
		printf("\nnios-run: Terminal mode (Control-C exits)");
		printf("\n-----------------------------------------\n");
		fflush(stdout);
		bTerminalDone = FALSE;
		if (bTimeout) {
			gettimeofday(&timeoutStartTime,NULL);
		}

		while (!bTerminalDone)
		{
			int num;
//zzzz
			if ((key = key_read()) != -1)
			{
				c = (char) key;
				if (c == 0x03)
					break;

				if(bNetwork) // Accumulate an entire string and then send if network mode
					{	
					if (c == 13 || c == 10) // CR or LF?
						{
						// 2nd if causes the LF after a CR or CR after LF to do nothing.
						//if((c == 10 && clast != 13) || (c == 13 && clast != 10))
						if (c == 10); // Ignore LF if present
						else
							{
							sendstr[cctr] = c; // Write CR to string
							sendstr[cctr+1] = 0; // Terminate string
							printf("\n");
							
				
						#if 0	
							// For test
							printf("String inputted is %s\n", sendstr);
							cctr=0;

							do 
								{
								printf("[%d]", sendstr[cctr]);
								} while (sendstr[cctr++] != 0);
							printf("\n");
							///////////							
						#endif

							port_write(portHandle, sendstr, strlen(sendstr));
							cctr = 0;
							//fflush(stdout);
							}
						}
					else // Accumulate chars and store in sendstr, waiting for a CR.
						{
						sendstr[cctr++] = c;
						printf("%c", c);
						//fflush(stdout);
						}
					//printf("0x%x", c);
					fflush(stdout);
					clast = c;
					//printf("clast = %d\n", c);
					//port_write(portHandle, &c, sizeof(char));
					}
				else // Send single character in in serial mode
					{
					//printf("0x%x", c);
					//fflush(stdout);
					port_write(portHandle, &c, sizeof(char));
					}
			}


			if (port_read(portHandle, &c, sizeof(c)))
			{
				if ((c == 0x0A || c == 0x0D))
				{
					// 2nd if causes the LF after a CR or CR after LF to do nothing.
					if( (c == 0xa && cLast != 0xd)
							|| (c == 0xd && cLast != 0xa) )
					{
						printf("\x0A");
						charpos = 0;
					}
				}
				else if(c == 0x04)		// control D from target ends terminal session
				{
					bTerminalDone = TRUE;
				}
				else					// regular character print it...
				{
					//
					// But first, if timestamping on, and first char of line, show time.
					//
					if(bTimeStamp && charpos == 0)
					{
						struct timeval difference;

						gettimeofday(&lineStartTime,NULL);
						
						difference = lineStartTime;
						difference.tv_sec -= lastLineStartTime.tv_sec;
						difference.tv_usec -= lastLineStartTime.tv_usec;
						while(difference.tv_usec < 0)
						{
							difference.tv_sec --;
							difference.tv_usec += 1000000;
						}

						printf("t(%4d.%03d): ",
						difference.tv_sec,
						difference.tv_usec / 1000);

						lastLineStartTime = lineStartTime;
					}

					printf("%c", c);
					charpos++;
				}

				cLast = c;

				fflush(stdout);
			} else {
				sleep(0);
			}

			if (bTimeout) {
				struct timeval difference;

				gettimeofday(&timeoutCheckTime,NULL);
				
				difference = timeoutCheckTime;
				difference.tv_sec -= timeoutStartTime.tv_sec;
				difference.tv_usec -= timeoutStartTime.tv_usec;
				while(difference.tv_usec < 0)
					{
					difference.tv_sec --;
					difference.tv_usec += 1000000;
					}

				if (difference.tv_sec >= timeoutSeconds) {
					resultCode++;
					printf ("***Exiting on Timeout\n");
					bTerminalDone = TRUE;
				}
			}
		}

end_terminal:
		// restore stdin
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &stdin_save);

		// close serial port
		port_close(portHandle);
	} else
		printf("### nios-run: could not open %s.\n", portName);

	if (f != NULL)
		fclose(f);
	
	printf("\nnios-run: exiting.\n");
	fflush(stdout);
	return resultCode;

show_usage:
	printf("Usage: nios-run [OPTION(S)] [FILENAME]\n");
	printf("Download code to Nios development board and perform terminal I/O.\n");
	printf("Download through either RS232 serial or network (UDP) connection.\n");
	printf("\n");
	printf("  OPTION            DESCRIPTION\n");
	printf("  -b <baud-rate>    sets the serial port baud rate (default = %d)\n", F_TOP_BAUD_RATE);
	printf("  -d                provides additional debugging information during download\n");
	printf("  -e \"<command>\"  execute a monitor command before entering terminal mode (experimental)\n");
	printf("  -o <seconds>      quit after <seconds> seconds in terminal mode\n");
	printf("  -p <port-name>    specifies serial port (default = %s)\n", portName);
	printf("  -r <tcp port#>	specifies tcp port number of remote host\n");
	printf("  -s <millisecs>    specifies a per-character delay (useful for reluctant flash)\n");
	printf("  -t                enters terminal mode without downloading code\n");
	printf("  -x                exit immediately after downloading code\n");
	printf("  -z                shows timestamp for each line, useful for benchmarking\n");
	printf("  -n <IP address>   use network connection with target at IP address\n");
	printf("\n");
	return 0;
}



// Little states for key_read's state machine
enum
{
	krs_normal,
	krs_got0ASent0D,
	krs_got0ASent0A,
	krs_got0DSent0D,
	krs_got0DSent0A
};
	

// Whenever we receive a CR or LF, we *report*
// that we got CR and then LF. If we get CR
// and then LF, or LF and CR, we don't report
// CRLF twice, of course.

int key_read()
{
#if 1
	fd_set rfds;
	struct timeval tv;
	static int krs = krs_normal;

	FD_ZERO (&rfds);
	FD_SET (0, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	// Handle the forged 2nd char of CRLF pairs

	if(krs == krs_got0ASent0D)
		{
		krs = krs_got0ASent0A;
		return 0x0a;
		}
	else if(krs == krs_got0DSent0D)
		{
		krs = krs_got0ASent0A;
		return 0x0a;
		}

	select (1, &rfds, 0, 0, &tv);
	if (FD_ISSET(0, &rfds)) 
	{
		int k;
		int lastKRS;

		lastKRS = krs;
		krs = krs_normal;

		k = getc(stdin);

		// Possibly throw away this character...
		if( (lastKRS == krs_got0ASent0A && k == 0x0d)
				|| (lastKRS == krs_got0DSent0A && k == 0x0a) )
			return -1;

		//!!! debugging char monitor if(k > 0)
			//!!! debugging char monitor printf("(%02x)",k);

		// Possibly enter a CRLF cycle
		if( k == 0x0d)
			{
			krs = krs_got0DSent0D;
			return 0x0d;
			}
		else if( k == 0x0a)
			{
			krs = krs_got0ASent0D;
			return 0x0d;
			}

		return k;
	}
	return -1;
#else
	char c;

	if (read(0, (void*) &c, sizeof(c)) == -1)
		return -1;

	return (int) c;
#endif
}

#if F_USE_WINDOWS_COMM

int serial_port_open(char *portName, int baudRate)
{
	HANDLE h;
	COMMTIMEOUTS cto;
	DCB dcb;
	DWORD dwError;

	h = CreateFile(
			portName,
			GENERIC_READ | GENERIC_WRITE,
			0,     // exclusive access
			NULL,  // no security attrs
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	if (h == INVALID_HANDLE_VALUE)
		return 0;

	ClearCommError(h, &dwError, NULL);

	// set timeouts
	cto.ReadIntervalTimeout = 0;
	cto.ReadTotalTimeoutMultiplier = 10;
	cto.ReadTotalTimeoutConstant = 50;
	cto.WriteTotalTimeoutMultiplier = 10;
	cto.WriteTotalTimeoutConstant = 1000;
	SetCommTimeouts(h, &cto);

	// set baud rate
	if (GetCommState(h, &dcb))
	{
		dcb.BaudRate = baudRate;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = TWOSTOPBITS;

		// constants
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDsrSensitivity = 0;
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fOutX = 0;
		dcb.fInX = 0;
		dcb.fBinary = TRUE;

		SetCommState(h, &dcb);
	} else {
		port_close((int) h);
		h = (HANDLE) 0;
	}


	return (int) h;
}

int serial_port_write(int portHandle, char *pData, int nCount)
{
	HANDLE h = (HANDLE) portHandle;
	DWORD dwLength;
	int i;

	if (nCount == 0)
		nCount = strlen(pData);
	
	for(i = 0; i < nCount; i++)
		{
		if (!WriteFile(h, (LPVOID)pData, (DWORD) 1, &dwLength, NULL))
			return 0;
		if(perCharSlowdown)
			delay_milliseconds(perCharSlowdown);
		pData++;
		}

	//if (!WriteFile(h, (LPVOID)pData, (DWORD) nCount, &dwLength, NULL))
		//return 0;

	return nCount;	// AOK.
}

int serial_port_read(int portHandle, char *pData, int nCount)
{
	HANDLE h = (HANDLE) portHandle;
	COMSTAT comstat;
	DWORD dwLength, dwError;

//	if (!ClearCommError(h, &dwError, &comstat))
//	{	printf("!"); return 0; }

//	if (comstat.cbInQue < nCount)
//		return 0;

	if (!ReadFile(h, (LPVOID)pData, (DWORD) nCount, &dwLength, NULL))
	{
		return 0;
	}
	return dwLength;
}

int port_close(int portHandle)
{
	CloseHandle((HANDLE) portHandle);
	return TRUE;
}

#else

int serial_port_open(char *portName, int baudRate)
{
	int portHandle;
	int err = 0;
	DWORD dwError;
	struct termios comm_state;
	speed_t baud_speed;

	switch (baudRate)
	{
		case 9600:
			baud_speed = B9600;
		break;

		case 19200:
			baud_speed = B19200;
		break;

		case 38400:
			baud_speed = B38400;
		break;

		case 57600:
			baud_speed = B57600;
		break;

		case 115200:
			baud_speed = B115200;
		break;

		default:
			return 0;	// invalid baud rate
	}

	portHandle = open(portName, (O_RDWR | O_NDELAY | O_NOCTTY));
	if (portHandle == -1)
		return 0;

	if(!tcgetattr(portHandle,&comm_state))
	{
		comm_state.c_iflag &= ~IXON;
		comm_state.c_iflag &= ~IXOFF;
		comm_state.c_iflag &= ~ICRNL;
		comm_state.c_iflag &= ~INLCR;
		comm_state.c_iflag &= ~IGNCR;
		comm_state.c_oflag = 0;
		comm_state.c_cflag &= ~PARENB;
		comm_state.c_cflag |= CSTOPB;

// These next conditionals are so idiosyncratic, we just use the TARGET.

#if F_TARGET_HPUX
		comm_state.c_cflag &= ~CIBAUD;
		comm_state.c_cflag |= ( CS8 | CREAD | HUPCL | CLOCAL | B19200);
#endif

#if F_TARGET_SOLARIS
		comm_state.c_cflag &= ~CIBAUD;
		comm_state.c_cflag &= ~CRTSCTS;
		comm_state.c_cflag |= (CBAUDEXT | CIBAUDEXT | CRTSXOFF | CS8 | CREAD | HUPCL | CLOCAL | B19200);
#endif

#if F_TARGET_X86
		comm_state.c_cflag &= ~CRTSCTS;
		comm_state.c_cflag |= (CBAUDEX | CRTSXOFF | CS8 | CREAD | HUPCL | CLOCAL | B19200);
#endif

		comm_state.c_lflag = 0;
		comm_state.c_cc[VMIN] = 0;
		comm_state.c_cc[VTIME] = 0;
		cfsetospeed(&comm_state, baud_speed);
		cfsetispeed(&comm_state, baud_speed);
		tcsetattr(portHandle,TCSANOW, &comm_state);
	} else
		port_close(portHandle);

	return portHandle;
}

int serial_port_write(int portHandle, char *pData, int nCount)
{
	int i;

	for(i = 0; i < nCount; i++)
		{
		if (write(portHandle, pData++, 1) == -1)
			return FALSE;
		if(perCharSlowdown)
			delay_milliseconds(perCharSlowdown);
		}

	return nCount;
}

int serial_port_read(int portHandle, char *pData, int nCount)
{
	int readResult;
	int nRead = 0;

	while (nCount > 0)
	{
		// read some data
		readResult = read(portHandle, pData, nCount);
		if (readResult == 0 || readResult == -1)
			break;

		// update ptr, length
		nRead += readResult;
		nCount -= readResult;
		pData += readResult;
	}
	return nRead;
}

int port_close(int portHandle)
{
	close(portHandle);
	return TRUE;
}

#endif




// ---------------------------------------
// Network Routines -- sockets-based,
// should work across unix & win32
//

int init_network_port(ePortStruct **port)
{
  *port = (ePortStruct *)malloc(sizeof(ePortStruct));
  (*port)->socket = (struct sockaddr_in *)malloc(sizeof(struct sockaddr));
  (*port)->socket->sin_family = AF_INET;
}

int setup_network_port(ePortStruct *port) 
{
  if(connect(port->socket_id, (struct sockaddr *)port->socket, sizeof(struct sockaddr))==0)
  {
#if DEBUG
    printf("*** setup_network_port: connected ***\n");
#endif 
    return 0;	    
  }
  else
  {
    printf("error in connection\n");
    return -1;
  }
}

int network_port_read(int portHandle, char *pData, int nCount)
{
  int recvSize;
  int socketStructLength;
  int nready;
  int n=0;
  static initialized=0;

  // check to make sure that there is actually something there


    FD_ZERO(&(gPortInfoPtr->set));
    FD_SET(portHandle, &(gPortInfoPtr->set));
    
    (gPortInfoPtr->time).tv_sec = 0;
    (gPortInfoPtr->time).tv_usec = 0;    
     
    if((nready=select(portHandle + 1, &(gPortInfoPtr->set), NULL, NULL, &(gPortInfoPtr->time)))>0)
    {

 	  n = read(portHandle, pData, nCount);
 	  //b[end]=0;
	  //printf("Receive Bytes: %c\n", *pData);
  	  //sleep(1);
          //b[0] = (b[0] & 0x1f) % 26 + 'A';
	  //write(socket_id, b, strlen(b));
  	  //printf("Send Bytes: %s\n", b);
	  
    }

return n;

}

int network_port_write(int portHandle, char *pData, int nCount)
{
  char lineFeed = 0xd;
  char munged_data[LINEBUF_SIZE];
  int i;
  
  char c;
  static char last_c = 0;
  int munged_len = 0;

  for(i = 0; i < nCount; i++)
  	{
	c = pData[i];
	if(c == 13)
		{
		munged_data[munged_len++] = 13; // cr
		munged_data[munged_len++] = 10; // lf
		}
	else if(c == 10)
		{
		if(last_c != 13)
			{
			munged_data[munged_len++] = 13; // cr
			munged_data[munged_len++] = 10; // lf
			}
		else
			{
			// ignore the LF
			}
		}
	else
		munged_data[munged_len++] = c;
	
	last_c = c;
	}

  	write(portHandle, munged_data, munged_len);
	return 0;


//	*(pData+nCount)=0xd;
  	write(portHandle, pData, nCount);

}

// (end of network routines)
// ---------------------------------------







void delay_milliseconds(int milliseconds)
	{
	struct timeval t1;
	struct timeval t2;
	int interval;

	gettimeofday(&t1, NULL);

	do
		{
		gettimeofday(&t2, NULL);
		interval = (t2.tv_sec - t1.tv_sec) * 1000;
		interval += (t2.tv_usec - t1.tv_usec) / 1000;
		} while(interval < milliseconds);

	return;
	}

void delay_useconds(int useconds)
	{
	struct timeval t1;
	struct timeval t2;
	int interval;

	gettimeofday(&t1, NULL);

	do
		{
		gettimeofday(&t2, NULL);
		interval = (t2.tv_sec - t1.tv_sec);
		interval += (t2.tv_usec - t1.tv_usec);
		} while(interval < useconds);

	return;
	}
