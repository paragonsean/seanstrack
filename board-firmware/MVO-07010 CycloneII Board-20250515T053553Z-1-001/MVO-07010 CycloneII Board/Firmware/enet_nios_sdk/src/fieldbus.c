//					fieldbus.c
//
//			Copyright 2010 Visi-Trak Worldwide

//#include "nios.h"
//#include "discrete_io.h // Defines in this file.

#if 0
/////////////////////////////////////////// Uart2 ISR's ///////////////////////////////////////////////
// Western reserve Controls 1782-JDC Uart2 receive data ISR
void u2_1782_jdc_rx_isr(int context)
	{
	static int ctr;
	static unsigned char record_num, prev_record_num;
	unsigned char byte;
	PL;

	if (na_uart2->np_uartstatus & np_uartstatus_rrdy_mask) // Receive character
		{
		byte = na_uart2->np_uartrxdata;
		//printf("[%02x] ",byte );
					
		uart2_rx_message[ctr++]=byte;	
		if (ctr==fieldbus.num_of_input_bytes) // We have received a full message from the gateway.
			{
			ctr=0;
			
			// Update ISW2, OSW2 and OSW3
			record_num = uart2_rx_message[0];
			if (record_num != prev_record_num) // We'll only look at incoming data from the gateway when the record number changes
				{
				prev_record_num = record_num; // Update prev.
				ISW2 = (unsigned int)uart2_rx_message[4];
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
///////////////////////////////////////////////////////////////////////////////////////////////////////			


/////////////////////////////////////////// Periodic time based routines /////////////////////////////////////////////
// Western reserve Controls 1782-JDC xmit data based on perodic time
void tb_1782_jdc_rx_isr(void)
	{
	
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

//////////////////// MODBUS functions ////////////////////////

// This function attempts to make an initial contact with a modbus device.
int initialize_modbus(void)
	{
	int x, return_val=0;

	for (x=0; x<10; x++)
		{
		return_val=modbus_read_multiple_inputs(0, NO_ACK);
		//printf("return_val = %d\n", return_val);
		if (return_val!=0) { } // If ARP timeout, keep trying, MODBUS device not ready.
		else 
			{
			printf("Successfully transmitted to the MODBUS device on attempt # %d\n", x);
			break;
			}
		}
	if (x==10) printf("Unable to transmit an initial message to the MODBUS device\n");
		
	return(return_val);
	}




////////////////////// MODBUS physical I/O functions ///////////////////////

int modbus_write_single_coil(unsigned short address, int on_off, int ack)
	{
	int return_val=0;
	struct
		{
		unsigned short identifier; 
		unsigned short protocol_identifier; // Always 0
		net_16 length;
		unsigned char slave_address;
		unsigned char function_code;
		net_16 ref_num;
		unsigned short on_off;  // 0xff00 turn on, 0x0000 turn off
		} modbus_set_single_coil_cmd;

	if(on_off) on_off=0x00ff; // Turn on code

	modbus_set_single_coil_cmd.identifier = 0;
	modbus_set_single_coil_cmd.protocol_identifier = 0;
	modbus_set_single_coil_cmd.length = nr_h2n16(0x0006); 
	modbus_set_single_coil_cmd.slave_address = 0x01; // Not used
	modbus_set_single_coil_cmd.function_code = 0x05; // Set single coil
	modbus_set_single_coil_cmd.ref_num = nr_h2n16(address);
	modbus_set_single_coil_cmd.on_off = on_off;
	
	// If we have a "connection", we'll xmit and wait for an ACK.  An ACK in this case
	// means any response from the MODBUS device.  If no connection we'll just spit it out
	// and hope for the best.
	if (ack && io_net_settings.connected)
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_set_single_coil_cmd, sizeof(modbus_set_single_coil_cmd), ACK, 0, 0);
	else
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_set_single_coil_cmd, sizeof(modbus_set_single_coil_cmd), NO_ACK, remote_io_net_settings.ip_address, remote_io_net_settings.port);

	return(return_val);
	}
		


// Sets up to 32 bits of digital (coil) outputs
int modbus_write_multiple_coils(unsigned short address, unsigned int data_word, int ack)
	{
	int return_val=0;
	unsigned char *ptr;
	struct
		{
		unsigned short identifier; 
		unsigned short protocol_identifier; // Always 0
		net_16 length;
		unsigned char slave_address;
		unsigned char function_code;
		net_16 ref_num;
		net_16 bit_count;
		unsigned char byte_count;
		unsigned char data_byte1;
		unsigned char data_byte2;
		unsigned char data_byte3;
		unsigned char data_byte4;
		} modbus_set_mul_coils_cmd;

	ptr = (char *)&data_word;
	modbus_set_mul_coils_cmd.identifier = 0;
	modbus_set_mul_coils_cmd.protocol_identifier = 0;
	modbus_set_mul_coils_cmd.length = nr_h2n16(11); 
	modbus_set_mul_coils_cmd.slave_address = 0x01; // Not used
	modbus_set_mul_coils_cmd.function_code = 0x0f; // Force multiple coils
	modbus_set_mul_coils_cmd.ref_num = nr_h2n16(address);
	modbus_set_mul_coils_cmd.bit_count = nr_h2n16(32);
	modbus_set_mul_coils_cmd.byte_count = 4;
	modbus_set_mul_coils_cmd.data_byte1 = ptr[0];
	modbus_set_mul_coils_cmd.data_byte2 = ptr[1];
	modbus_set_mul_coils_cmd.data_byte3 = ptr[2];
	modbus_set_mul_coils_cmd.data_byte4 = ptr[3];
	
	// If we have a "connection", we'll xmit and wait for an ACK.  An ACK in this case
	// means any response from the MODBUS device.  If no connection we'll just spit it out
	// and hope for the best.
	if (ack && io_net_settings.connected)
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_set_mul_coils_cmd, sizeof(modbus_set_mul_coils_cmd), ACK, 0, 0);
	else
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_set_mul_coils_cmd, sizeof(modbus_set_mul_coils_cmd), NO_ACK, remote_io_net_settings.ip_address, remote_io_net_settings.port);

	return(return_val);
	}



// Will return 32 bits (2 words) of descrete inputs.  Inputs not returned here.
// They will be updated be the network receive function when the MODBUS device sends
// back its response.
int modbus_read_multiple_inputs(unsigned short address, int ack)
	{
	int return_val=0;
	struct
		{
		unsigned short identifier; 
		unsigned short protocol_identifier; // Always 0
		net_16 length;
		unsigned char slave_address;
		unsigned char function_code;
		net_16 ref_num;
		net_16 word_count;
		} modbus_read_mul_inputs_cmd;

	modbus_read_mul_inputs_cmd.identifier = 0;
	modbus_read_mul_inputs_cmd.protocol_identifier = 0;
	modbus_read_mul_inputs_cmd.length = nr_h2n16(6); 
	modbus_read_mul_inputs_cmd.slave_address = 0x01; // Not used
	modbus_read_mul_inputs_cmd.function_code = 0x04; // Read multiple inputs
	modbus_read_mul_inputs_cmd.ref_num = nr_h2n16(address);
	modbus_read_mul_inputs_cmd.word_count = nr_h2n16(2); // 2 words, 32 bits

	// If we have a "connection", we'll xmit and wait for an ACK.  An ACK in this case
	// means any response from the MODBUS device.  If no connection we'll just spit it out
	// and hope for the best.
	if (ack && io_net_settings.connected)
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_read_mul_inputs_cmd, sizeof(modbus_read_mul_inputs_cmd), ACK, 0, 0);
	else
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_read_mul_inputs_cmd, sizeof(modbus_read_mul_inputs_cmd), NO_ACK, remote_io_net_settings.ip_address, remote_io_net_settings.port);

	return(return_val);
	}





//////////////////////// MODBUS register functions /////////////////////////

int modbus_write_single_register(unsigned short int address, unsigned short int value, int ack)
	{
	int return_val=0;
	struct
		{
		unsigned short int identifier; 
		unsigned short int protocol_identifier; // Always 0
		net_16 length;
		unsigned char slave_address;
		unsigned char function_code;
		net_16 ref_num;
		unsigned short int value;  // 0xff00 turn on, 0x0000 turn off
		} modbus_set_single_register_cmd;

	//if(on_off) on_off=0x00ff; // Turn on code

	modbus_set_single_register_cmd.identifier = 0;
	modbus_set_single_register_cmd.protocol_identifier = 0;
	modbus_set_single_register_cmd.length = nr_h2n16(0x0006); 
	modbus_set_single_register_cmd.slave_address = 0x01; // Not used
	modbus_set_single_register_cmd.function_code = 0x06; // Write single register
	modbus_set_single_register_cmd.ref_num = nr_h2n16(address);
	modbus_set_single_register_cmd.value = nr_h2n16(value);
	
	// If we have a "connection", we'll xmit and wait for an ACK.  An ACK in this case
	// means any response from the MODBUS device.  If no connection we'll just spit it out
	// and hope for the best.
	if (ack && io_net_settings.connected)
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_set_single_register_cmd, sizeof(modbus_set_single_register_cmd), ACK, 0, 0);
	else
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_set_single_register_cmd, sizeof(modbus_set_single_register_cmd), NO_ACK, remote_io_net_settings.ip_address, remote_io_net_settings.port);

	return(return_val);
	}



// Will return the contents of 2 registers.  Content not returned here.
// They will be updated by the network receive function when the MODBUS device sends
// back its response.
int modbus_read_multiple_registers(unsigned short address, int ack)
	{
	int return_val=0;
	struct
		{
		unsigned short identifier; 
		unsigned short protocol_identifier; // Always 0
		net_16 length;
		unsigned char slave_address;
		unsigned char function_code;
		net_16 ref_num;
		net_16 word_count;
		} modbus_read_registers_cmd;

	modbus_read_registers_cmd.identifier = 0;
	modbus_read_registers_cmd.protocol_identifier = 0;
	modbus_read_registers_cmd.length = nr_h2n16(6); 
	modbus_read_registers_cmd.slave_address = 0x01; // Not used
	modbus_read_registers_cmd.function_code = 0x03; // Read multiple registers
	modbus_read_registers_cmd.ref_num = nr_h2n16(address);
	modbus_read_registers_cmd.word_count = nr_h2n16(2); // 2 16 bit registers, 32 bits

	// If we have a "connection", we'll xmit and wait for an ACK.  An ACK in this case
	// means any response from the MODBUS device.  If no connection we'll just spit it out
	// and hope for the best.
	if (ack && io_net_settings.connected)
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_read_registers_cmd, sizeof(modbus_read_registers_cmd), ACK, 0, 0);
	else
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_read_registers_cmd, sizeof(modbus_read_registers_cmd), NO_ACK, remote_io_net_settings.ip_address, remote_io_net_settings.port);

	return(return_val);
	}



// Writes 2 16 bit registers
int modbus_write_multiple_registers(unsigned short address, unsigned int data_word, int ack)
	{
	int return_val=0;
	unsigned short int *ptr;
	struct
		{
		unsigned short identifier; 
		unsigned short protocol_identifier; // Always 0
		net_16 length;
		unsigned char slave_address;
		unsigned char function_code;
		net_16 ref_num;
		net_16 register_count;
		unsigned char byte_count;
		net_16 register1;
		net_16 register2;
		} modbus_set_mul_registers_cmd;

	ptr = (unsigned short int *)&data_word;
	modbus_set_mul_registers_cmd.identifier = 0;
	modbus_set_mul_registers_cmd.protocol_identifier = 0;
	modbus_set_mul_registers_cmd.length = nr_h2n16(12); 
	modbus_set_mul_registers_cmd.slave_address = 0x01; // Not used
	modbus_set_mul_registers_cmd.function_code = 0x10; // Write multiple coils
	modbus_set_mul_registers_cmd.ref_num = nr_h2n16(address);
	modbus_set_mul_registers_cmd.register_count = nr_h2n16(2);
	modbus_set_mul_registers_cmd.byte_count = 4;
	modbus_set_mul_registers_cmd.register1 = nr_h2n16(ptr[0]);
	modbus_set_mul_registers_cmd.register2 = nr_h2n16(ptr[1]);
		
	// If we have a "connection", we'll xmit and wait for an ACK.  An ACK in this case
	// means any response from the MODBUS device.  If no connection we'll just spit it out
	// and hope for the best.
	if (ack && io_net_settings.connected)
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_set_mul_registers_cmd, sizeof(modbus_set_mul_registers_cmd), ACK, 0, 0);
	else
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_set_mul_registers_cmd, sizeof(modbus_set_mul_registers_cmd), NO_ACK, remote_io_net_settings.ip_address, remote_io_net_settings.port);

	return(return_val);
	}



// This function will write a 32 bit (2 word) value to a MODBUS register and will read back
// a 32 bit 2 word register value.  The read value is not returned here.  It will be
// written by the network receive function when the MODBUS response is read.
int modbus_rd_wr_registers(int read_address, int wr_address, unsigned int wr_data, int ack)
	{
	int return_val=0;
	unsigned short int *ptr;
	struct
		{
		unsigned short identifier; 
		unsigned short protocol_identifier; // Always 0
		net_16 length;
		unsigned char slave_address;
		unsigned char function_code;
		net_16 read_address;
		net_16 read_word_count;
		net_16 write_address;
		net_16 write_word_count;
		unsigned char write_byte_count;
		unsigned short int write_word1;
		unsigned short int write_word2;
		} modbus_rd_wr_registers_cmd;

	ptr = (unsigned short int *)&wr_data;
	modbus_rd_wr_registers_cmd.identifier = 0;
	modbus_rd_wr_registers_cmd.protocol_identifier = 0;
	modbus_rd_wr_registers_cmd.length = nr_h2n16(15); 
	modbus_rd_wr_registers_cmd.slave_address = 0x01; // Not used
	modbus_rd_wr_registers_cmd.function_code = 0x17; // rd/wr registers
	modbus_rd_wr_registers_cmd.read_address = nr_h2n16(read_address);
	modbus_rd_wr_registers_cmd.read_word_count = nr_h2n16(2);
	modbus_rd_wr_registers_cmd.write_address = nr_h2n16(wr_address);
	modbus_rd_wr_registers_cmd.write_word_count = nr_h2n16(2);
	modbus_rd_wr_registers_cmd.write_byte_count = 4;
	modbus_rd_wr_registers_cmd.write_word1 = nr_h2n16(ptr[0]);
	modbus_rd_wr_registers_cmd.write_word2 = nr_h2n16(ptr[1]);
	
	// If we have a "connection", we'll xmit and wait for an ACK.  An ACK in this case
	// means any response from the MODBUS device.  If no connection we'll just spit it out
	// and hope for the best.
	if (ack && io_net_settings.connected)
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_rd_wr_registers_cmd, sizeof(modbus_rd_wr_registers_cmd), ACK, 0, 0);
	else
		return_val=network_xmit((void *)&io_net_settings, (void *)&modbus_rd_wr_registers_cmd, sizeof(modbus_rd_wr_registers_cmd), NO_ACK, remote_io_net_settings.ip_address, remote_io_net_settings.port);

	return(return_val);
	}


////////////////// MODBUS response handler ///////////////////

#if 0
// Original
int process_modbus_response(char *str, int length)
	{
	int i;
	
	printf("Received a MODBUS response!!!\n"); // For test
	for(i=0; i<length; i++) printf("[%x]", str[i]);
	printf("\n");
	}
#endif


int process_modbus_response(char *response, int length)
	{
	int return_val=0, i;
	unsigned char response_code;

	printf("Received a MODBUS response!!!\n"); // For test
	for(i=0; i<length; i++) printf("[%x]", response[i]);
	printf("\n");

	response_code = response[7];
	
	switch(response_code)
		{
		// We'll only worry about exception that occured with write commands.
		case 0x85:
		case 0x8F:
		case 0x86:
		case 0x90:
		case 0x97:

			break;

		case 0x04: // Read discrete inputs, update ISW

			break;

		case 0x03: // Read registers and also update ISW.  In future possibly check for the address as this could also be A/D or something else.

			break;

		case 0x17: // Also registers to read.

			break;

		default: // For now, nothing to do.

			break;
		}

	return(return_val);
	}



