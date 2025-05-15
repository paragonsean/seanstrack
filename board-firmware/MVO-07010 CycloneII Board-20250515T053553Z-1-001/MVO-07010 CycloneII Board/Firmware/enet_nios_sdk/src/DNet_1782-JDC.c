//					DNet_1782-JDC.c
//
//			Copyright 2007 Visi-Trak Worldwide

//#include "nios.h"
//#include "hi_res.h" // Defines in this file.

// DeviceNet gateway WRC 1782-JDC interface routines

// Uart2 recieve isr
void u2_1782_jdc_rx_isr(int context)
	{
	static int ctr, length_is_correct, run;
	static unsigned char record_no, prev_record_no, length;
	unsigned char byte;
	PL;

	// Recieve format: 
	//
	// Byte 0 - Record number
	// Byte 1 - Length
	// Byte 2 - Pad byte, always 0x55
	// Byte 3 - Pad byte, always 0xaa
	// Bytes 4 - 7, data bytes = OSW2
	if (na_uart2->np_uartstatus & np_uartstatus_rrdy_mask) // Receive character
		{
		byte = na_uart2->np_uartrxdata;
		//printf("[%02x] ",byte );
			
		if (ctr >= fieldbus.num_of_input_bytes) ctr = 0;  // Something's gone wrong, force ctr within bounds
		
		fieldbus_rx_message[ctr++]=byte;
		if (run != 0x55555555) // We'll assume this is the first byte received after power up, set everything accordingly
			{
			run = 0x55555555;
			ctr = 1;
			fieldbus_rx_message[0]=byte;
			record_no = byte;
			}
		else
			{
			if (ctr==1) // Byte 0, record number
				{
				record_no = byte; // Save for future use
				if (record_no == prev_record_no || record_no == prev_record_no + 1)
					{
					prev_record_no = record_no;
					}
				else  // Record number doesn't make any sense, perhaps we're out of sync with the message
					{
					printf("Bad record number\n"); // For test
					}
				}
			else if (ctr==2) // Byte 1, max length of the entire message.
				{
				length = byte;
				if (length == fieldbus.num_of_input_bytes) length_is_correct = TRUE; // Length and max message size add up
				else length_is_correct = FALSE;	
				}
			else if (ctr==length) // We have received a complete message.
				{
				ctr=0;
					
				ISW2 = (unsigned int)fieldbus_rx_message[4];
				printf("Updated ISW2\n"); // For test
				}
			}
		}	
	else
		{
		printf("Major UART2 error!!!, unrecognized interrupt!!!\n");
		}
			
			
		
		
skip: ;
	na_uart2->np_uartstatus = 0; // Clear the interrupt
	}


	
// Uart2 recieve isr
void u2_1782_jdc_rx_isr(int context)
	{
	static int ctr, length_is_correct, run;
	static unsigned char record_no, prev_record_no, length;
	PL;

	// Recieve format: 
	//
	// Byte 0 - Record number
	// Byte 1 - Length
	// Byte 2 - Pad byte, always 0x55
	// Byte 3 - Pad byte, always 0xaa
	// Bytes 4 - 7, data bytes = OSW2
	if (na_uart2->np_uartstatus & np_uartstatus_rrdy_mask) // Receive character
		{
		byte = na_uart2->np_uartrxdata;
		printf("[%02x] ",byte );
		}	
		
skip: ;
	na_uart2->np_uartstatus = 0; // Clear the interrupt
	}



	
	
// Uart2 transmit routine	
int u2_1782_jdc_tx(unsigned char *message, int axis)
	{
	// First calc. the checksum
	message[4] = crc8(message, 4);

	// Then xmit the message
	PRINT_CHAR_UART2(message[0]);
	PRINT_CHAR_UART2(message[1]);	
	PRINT_CHAR_UART2(message[2]);
	PRINT_CHAR_UART2(message[3]);
	PRINT_CHAR_UART2(message[4]);

	return(0);
	}



