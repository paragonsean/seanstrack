//					hi_res.c
//
//			Copyright 2007 Visi-Trak Worldwide

//#include "nios.h"
//#include "hi_res.h" // Defines in this file.

// High resolution sensor interface routines



void show_hi_res_commands(void)
	{
	char tempstr[81], tempstr1[81];
	PL;

	// Remember, send_string() will xmit onto the net or out the serial port depending
	// on g.spc.  If g.spc == 9999, the string will go out the serial port.
	
	// sprintf's aren't necessary here but we'll keep them incase we ever want to format values into the string.
	sprintf(tempstr, "\n# Hi-res sensor commands (y == axis number) \n\n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# H1_y Current hi-res position on axis y: \n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# H2_y Reset request on axis y: \n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# H3_y Calibration request on axis y: \n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# H4_x_y Diag. request code x on axis y: \n");
	send_string((void *)g.spc, tempstr);
	send_string((void *)g.spc, "\tDiag. request codes:\n");
	send_string((void *)g.spc, "\t0 == Firmware revision number:\n");
	send_string((void *)g.spc, "\t1 == Observed max ch. A:\n");
	send_string((void *)g.spc, "\t2 == Observed min ch. A:\n");
	send_string((void *)g.spc, "\t3 == Zero crossing point ch. A:\n");
	send_string((void *)g.spc, "\t4 == Raw inst. value ch. A:\n");
	send_string((void *)g.spc, "\t5 == Observed max ch. A:\n");
	send_string((void *)g.spc, "\t6 == Observed min ch. A:\n");
	send_string((void *)g.spc, "\t7 == Zero crossing point ch. A:\n");
	send_string((void *)g.spc, "\t8 == Raw inst. value ch. A:\n");

	sprintf(tempstr, "# H5_y Update flash on axis y: \n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# H6_y Enter BSL mode on axis y: \n");
	send_string((void *)g.spc, tempstr);

	
	sprintf(tempstr, "# HM	To view this menu\n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# To enter a command, type \"H\" command, underscore then the axis#,\n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# For example: H2_1<CR> To reset the hi-res sensor on axis #1\n\n");
	send_string((void *)g.spc, tempstr);
	}



void parse_hi_res_command(unsigned char *orig_cmd_str)
	{
	unsigned char *ptr;
	int command, axis_num, diag_request_code=0;
	volatile unsigned short int *halfword_ptr;
	PL;

	// If source_of_command == 1, command came from network, if zero it came from serial port.
	ptr = orig_cmd_str; ptr++;

		// Adjust for case
	if(*ptr >= 'A' && *ptr <= 'Z')
		*ptr = *ptr + ('a' - 'A');

	// Check for menu command, if present, don't convert, else convert ASCII to its corresponding number
	if (*ptr == 'm') command = (int)*ptr;
	else command = (int)(*ptr-'0');
	
	ptr+=2;
	if (command==4) // Diag. request code, get the request code.
		{
		diag_request_code = (int)(*ptr-'0');
		ptr+=2;
		}
	axis_num = (int)(*ptr-'0'); 

	//if (command < 1 || command > 6) command = 1;
	if (axis_num < 1 || axis_num > NUM_OF_AXES) axis_num = 1; // Set to axis #1 if entered value is out of range.
	if (SENSOR_TYPE(axis_num) != HI_RES) // If specified axis is not hi-res, no point in going any further.
		{
		send_string((void *)g.spc, "?Specified axis # is not set for hi-res!!!\n"); 
		goto skip;
		}
	if (diag_request_code > 8) diag_request_code = 0; // Set this to return the firmware revision number if out of range. 

	switch(command)
		{
		case 1: // Get the current hi-res position for the specified axis.
			sprintf(g.b,"# %d\n", HI_RES_POSITION(axis_num));
			send_string((void *)g.spc, g.b); 
			break;
		
		case 2: // Reset (set position to zero)
			send_hi_res_reset_request(axis_num);
			break;

		case 3: // Calibrate
			send_hi_res_cal_request(axis_num);
			break;

		case 4: // Diagnostic request
			send_hi_res_diag_request(diag_request_code, axis_num);
			break;

		case 5: // Update flash, not yet implemented
			break;

		case 6: // Enter BSL, not yet implemented
			break;

		case 'm': // Show hi-res sensor setup menu
			show_hi_res_commands(); // Send either to serial port or xmit on the network, depending on who asked
			break;	

		default: // Invalid hi-res command
			sprintf(g.b,"?%s\n",orig_cmd_str);
			send_string((void *)g.spc, g.b); 
			break;
		}
	
	skip: ;
	}



// For now, we'll use uart2 for the hi-res sensor interface.  We'll also use this to initiate a position based 
// servo loop.
void hi_res_rx_isr(int context)
	{
	static int ctr, servo_ctr, monitor_ctr, first_time, next_20P_X1_equiv_forward, next_20P_X1_equiv_backward;
	static unsigned int prev_latched_q1, prev_hi_res_pos;
	int axis_num=1, x;
	unsigned short int diag_response;
	//static unsigned char rx_hold_reg[5]; 
	unsigned char *byte_ptr, byte, temp_str[81];
	PL;

	if (na_uart2->np_uartstatus & np_uartstatus_rrdy_mask) // Receive character
		{
		if (SENSOR_TYPE(1) == HI_RES) // Right now only axis 1 supported for hi-res.
			{
			CLR_HI_RES_SERVO_LOOP_INTERRUPT;
			
			byte = na_uart2->np_uartrxdata;
			//printf("[%02x] ",byte );
			if (ctr==0 && (byte<POS_UPDATE || byte>DIAG_RESPONSE)) {/*printf("[$%02x]\n",byte );*/ goto skip;} // This can't be the start of a message
			
			hi_res_rx_message[ctr++]=byte;	
			if (ctr==5) // We have received all 5 bytes of a complete message.
				{
				ctr=0;
				if (crc8(&hi_res_rx_message[0], 4) == hi_res_rx_message[4]) // Checksum matched
					{
					//printf("CRC good!!!\n");
					if (hi_res_rx_message[0] == POS_UPDATE) // Position update message
						{
						//printf("P");
						// Update hi-res position
						byte_ptr = (unsigned char *)&HI_RES_POSITION(1); // Only axis #1 supported now, ptr points to LSB
						*byte_ptr++ = hi_res_rx_message[3]; // LSB
						*byte_ptr++ = hi_res_rx_message[2]; // Middle byte
						*byte_ptr++ = hi_res_rx_message[1]; // MSB
						if (hi_res_rx_message[1]&0x80)  *byte_ptr=0xff; // Extend upper bits if negative
						else *byte_ptr=0;
						
						// Determine hi-res velocity, we'll keep velocity a positive number regardless of direction
						if (HI_RES_POSITION(axis_num) >= prev_hi_res_pos) HI_RES_VELOCITY(axis_num) = HI_RES_POSITION(axis_num) - prev_hi_res_pos;
						else HI_RES_VELOCITY(axis_num) = prev_hi_res_pos - HI_RES_POSITION(axis_num);
						prev_hi_res_pos = HI_RES_POSITION(axis_num);
						
						// Determine 20P equivalents which may be useful 
						HI_RES_20P_EQUIV_VEL(axis_num) = ((HI_RES_VELOCITY(axis_num)*HI_RES_UPDATES_PER_SECOND)/HI_RES_X4_PULSES_PER_INCH)*20; // 20P X1 pps 
						HI_RES_20P_EQUIV_POS(axis_num) =  HI_RES_POSITION(axis_num)/HI_RES_IMPROVEMENT_FACTOR; // 20P X4 counts
						
						#if 0
						if (MONITOR_ENABLE)
							{
							// Have we moved a .050" increment (one thread)?
							if (HI_RES_POSITION(axis_num) >= (unsigned int)((pb_ctr+1)*HI_RES_PULSES_PER_THREAD)) // pb_ctr resets to zero, so we'll add one to it here.
								{
								HI_RES_LATCHED_Q1(axis_num) = RD_PV_RAW_VEL_CTR(axis_num);
								HI_RES_20P_EQUIV_VEL(axis_num) = HI_RES_LATCHED_Q1(axis_num) - prev_latched_q1;
								prev_latched_q1 = HI_RES_LATCHED_Q1(axis_num);

								SET_HI_RES_MON_INT; // We've moved a .050" increment, send an interrupt to the pos. based monitor isr.
								}
							else
								{
								CLR_HI_RES_MON_INT;
								}
							}
						else
							{
							CLR_HI_RES_MON_INT;
							}
						#endif
						
						
						// Here we'll reproduce a "fake" 20P quadrature from the hi-res position and send it to the
						// PV unit through a quadrature encoder circuit in hardware.  We'll create a 20P x4 and U/D signal.
						// The PV unit, limit switches and monitoring will work exactly as with a 20P sensor.
						if (first_time != 0x55555555) // First position update after power up
							{
							first_time = 0x55555555;
							next_20P_X1_equiv_forward = HI_RES_POSITION(axis_num) + HI_RES_PULSES_PER_THREAD; 
							next_20P_X1_equiv_backward = HI_RES_POSITION(axis_num) - HI_RES_PULSES_PER_THREAD;
							}
						else
							{
							if (HI_RES_POSITION(axis_num) == 0) // Keep resetting until the position moves off zero
								{
								next_20P_X1_equiv_forward = HI_RES_PULSES_PER_THREAD; 
								next_20P_X1_equiv_backward = 0-HI_RES_PULSES_PER_THREAD;
								}
							else if ((int)HI_RES_POSITION(axis_num) >= next_20P_X1_equiv_forward) // Moving forward
								{
								SET_HI_RES_UD;   // Set the U/D signal to up
								PULSE_HI_RES_X4; // Pulse the x4 signal to the PV unit 4 times, one thread
								PULSE_HI_RES_X4;
								PULSE_HI_RES_X4;
								PULSE_HI_RES_X4;
								next_20P_X1_equiv_backward = next_20P_X1_equiv_forward-HI_RES_PULSES_PER_THREAD; 
								next_20P_X1_equiv_forward+=HI_RES_PULSES_PER_THREAD;
								}
							else if ((int)HI_RES_POSITION(axis_num) <= next_20P_X1_equiv_backward) // Moving backward
								{
								CLR_HI_RES_UD;   // Set the U/D signal to down
								PULSE_HI_RES_X4; // Pulse the x4 signal to the PV unit 4 times, one thread
								PULSE_HI_RES_X4;
								PULSE_HI_RES_X4;
								PULSE_HI_RES_X4;
								next_20P_X1_equiv_forward = next_20P_X1_equiv_backward+HI_RES_PULSES_PER_THREAD;
								next_20P_X1_equiv_backward-=HI_RES_PULSES_PER_THREAD;
								}
							}
							
						// Check and see if it's time for a servo loop
						//if (servo_ctr++ == (HI_RES_UPDATES_PER_SERVO_LOOP(1) - 1)) 
						//	{
						//	servo_ctr=0;
						//	SET_HI_RES_SERVO_LOOP_INTERRUPT; // Since this is a lower priority interrupt, it won't execute until after this function exits.
						//	}
						//else
						//	{
						//	CLR_HI_RES_SERVO_LOOP_INTERRUPT;
						//	}	
						
						//AD_CMD1=1; // Do an A/D loop every time we receive a position update message
						} 
					else if (hi_res_rx_message[0] == CAL_RESPONSE) // Calibration response message
						{
						if (hi_res_rx_message[1] == 0) {send_string((void *)g.spc,"# Hi-res calibration success!!!");}
						else {send_string((void *)g.spc,"# Hi-res calibration failed!!!");}
						}
					else if (hi_res_rx_message[0] == DIAG_RESPONSE) // Diagnostic response message
						{
						// First extract the diag response value from the message
						byte_ptr = (unsigned char *)&diag_response;
						byte_ptr[0] = hi_res_rx_message[3]; // LSB
						byte_ptr[1] = hi_res_rx_message[2]; // MSB
						
						// Now get the response type
						if (hi_res_rx_message[1] == FIRMWARE_REV) {sprintf(temp_str, "#  Hi-res firmware = %x\n", diag_response);}
						else if (hi_res_rx_message[1] == MAX_A) {sprintf(temp_str, "#  Observed max. ch. A = %d\n", diag_response);}
						else if (hi_res_rx_message[1] == MIN_A) {sprintf(temp_str, "#  Observed min. ch. A = %d\n", diag_response);}
						else if (hi_res_rx_message[1] == ZERO_CROSS_A) {sprintf(temp_str, "#  Zero crossing point ch. A = %d\n", diag_response);}
						else if (hi_res_rx_message[1] == RAW_A) {sprintf(temp_str, "#  Raw inst. value ch. A = %d\n", diag_response);}
						else if (hi_res_rx_message[1] == MAX_B) {sprintf(temp_str, "#  Observed max. ch. B = %d\n", diag_response);}
						else if (hi_res_rx_message[1] == MIN_B) {sprintf(temp_str, "#  Observed min. ch. B = %d\n", diag_response);}
						else if (hi_res_rx_message[1] == ZERO_CROSS_B) {sprintf(temp_str, "#  Zero crossing point ch. B = %d\n", diag_response);}
						else if (hi_res_rx_message[1] == RAW_B) {sprintf(temp_str, "#  Raw inst. value ch. B = %d\n", diag_response);}
						else {sprintf(temp_str, "#  Invalid diag. response from sensor\n");}
						
						// Finally transmit the message
						send_string((void *)g.spc,temp_str);
						}
					}
				else  // Transmission error or we're out of sync with the incomming message
					{
					// Do nothing at this time
					//printf("crc8 failed!!!\n");
					//printf("*");
							//for(x=0; x<5; x++) printf("[%02x] ", hi_res_rx_message[x]);
				 			//printf("\n");
					}
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




int send_message_hi_res(unsigned char *message, int axis)
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


int send_hi_res_cal_request(int axis)
	{
	hi_res_tx_message[0]=CAL_REQUEST;
	hi_res_tx_message[1]=0;
	hi_res_tx_message[2]=0;
	hi_res_tx_message[3]=0;

	send_message_hi_res(hi_res_tx_message, axis);

	return(0);
	}


int send_hi_res_reset_request(int axis)
	{
	hi_res_tx_message[0]=RESET_REQUEST;
	hi_res_tx_message[1]=0;
	hi_res_tx_message[2]=0;
	hi_res_tx_message[3]=0;

	send_message_hi_res(hi_res_tx_message, axis);

	return(0);
	}


int send_hi_res_diag_request(unsigned char item_to_diag, int axis)
	{
	if (item_to_diag > RAW_B) item_to_diag = FIRMWARE_REV; // Force to FIRMWARE_REV if item_to_diag is not valid

	hi_res_tx_message[0]=DIAG_REQUEST;
	hi_res_tx_message[1]=item_to_diag;
	hi_res_tx_message[2]=0;
	hi_res_tx_message[3]=0;

	send_message_hi_res(hi_res_tx_message, axis);

	return(0);
	}


int send_hi_res_bsl_request(int password, int axis) // Bootstrap loader request, uppermost byte of password is ignored
	{
	unsigned char *byte_ptr;
	
	byte_ptr = (unsigned char *)&password;

	hi_res_tx_message[0]=BSL_REQUEST;
	hi_res_tx_message[1]=byte_ptr[2];
	hi_res_tx_message[2]=byte_ptr[1];
	hi_res_tx_message[3]=byte_ptr[0];
	
	send_message_hi_res(hi_res_tx_message, axis);
	
	return(0);
	}

