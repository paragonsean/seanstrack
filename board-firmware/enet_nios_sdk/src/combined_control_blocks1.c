//					combined_control_blocks1.c
//
//			Copyright 2003, Visi-Trak Worldwide

//#include "nios.h"
//#include "control_program_blocks6.h"

// 7-15-03  control_blocks6.c  Supports 2 position axes.

#ifndef NULL
 #define NULL 0
#endif

// Debug enables
#define COND_COMPILE_DEBUG 0
#define DWELL_COMPILE_DEBUG 0
#define POS_COMPILE_DEGUG 0
#define VEL_COMPILE_DEBUG 0
#define JUMP_COMPILE_DEBUG 0
#define UPDATE_COMPILE_DEBUG 0
#define END_COMPILE_DEBUG 0

#define COND_EXE_DEBUG 0
#define DWELL_EXE_DEBUG 0
#define JUMP_EXE_DEBUG 0
#define UPDATE_EXE_DEBUG 0
#define VEL_EXE_DEBUG 0		// This could cause the WD timer to timeout resetting the board
#define EXT_LOOP_DEBUG 0	// Ditto

#define BEGINNING 0
#define DURING 1
#define AT_END 2



#if 0
// Framework for executing the various block types.  Jump blocks will be somewhat different.
int ??????_block_exe(c_block control_program, int type_of_execution)
	{
	int return_val=0, cond_result;
	static int saved_execution_type;
	?b *?????_block=0;
	np_timer *timer2 = na_timer2;
	
	saved_execution_type = type_of_execution;

	if (saved_execution_type==IMMED) 
		?????_block = (?b *)&control_program[IMMED_EXECUTE_BLOCK][0];
	else
		?????_block = (?b *)&control_program[vtg.prog_ctr][0];
		
	cond_result = block_conditional_exe((ub *)?????_block, BEGINNING); 
	
	if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
		{
		return_val=cond_result;	
		} 
	else if (cond_result < 20) // OK to proceed but there may be some conditions.
		{
		// This is the main body of the block	
		

		// Will have to check if conditional callback DURING is necessary here.

		// If we got to this point, the block terminated normally as opposed to
		// a condition not being met.  Therefore even if we were requested to
		// call back during the block, there may be a set output item that needs to
		// be serviced here.   
		if (cond_result != KEEP_GOING_DONT_CALL_BACK) // May be something to set or clear
			block_conditional_exe((ub *)?????_block, AT_END); // Checking back at end per block_conditional()'s request.	
		}
	else if (cond_result == COND_NOT_MET); // Nothing more to do 
		
	else  // This should not happen - if it does, deal with it here.
		{
		return(BLOCK_EXECUTION_ERROR); // This will trigger a fatal error
		}
		
			
	ext: ;	
	if (saved_execution_type==PROGRAM) vtg.prog_ctr++; // Increment prog ctr		
	
	return(return_val);
	}
#endif



int end_block_compile(char *str_to_compile)
	{
	eb *end_block=0; // Use update block for template
	int return_val = 0, stepnum, x;
	volatile char *ptr=0;


	// All this does is place an END_ block ID in the block.
	// There is not any conditional checking or anything else. 
	#if END_COMPILE_DEBUG
		printf("string to compile = %s\n", str_to_compile);	
	#endif		
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
	#if END_COMPILE_DEBUG
		printf("Comment has been stripped - see %s\n", str_to_compile);
	#endif	
	//Compile and save step to SRAM, get step number
	//The first digits will be the step number
	stepnum = atoi(str_to_compile); 
	if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
	end_block = (eb *)&vtg.control_program[stepnum][0];
	
	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		vtg.control_program[stepnum][x] = 0;
		}

#if END_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	end_block->blk_id = END_;
	//end_block->blk_to_execute = (control_block_function)end_block_exe;

#if END_COMPILE_DEBUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
		printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif

	ext: ;
	return (return_val);		
	}


int end_block_exe(c_block control_block, int type_of_execution, int zone) // ALL PASSED PARAMETERS ARE IGNORED!!!
	{
	int axis_num=1;
	np_pio *misc_out_pio = na_misc_outs;
	np_pio *control_int = na_control_int;

	STOP_CONTROL;
	return(0);
	//return (PROGRAM_STOPPED_BY_USER); // This will trigger a fatal error
	}


// This is used to start the control program running.  It is never executed as part of
// a control program, thus there is not a "start_block_compile."  All passed parameters
// are ignored.  This function is called from within a control interrupt, therefore the 
// function framework mimics a control block exe.
int start_block_exe(c_block control_block, int type_of_execution, int zone)
		{
		np_pio *misc_out_pio = na_misc_outs;
		np_pio *control_int = na_control_int;
		np_pio *cont_int_output = na_cont_int_output;
		int x, axis_num=1;
	
		// ALL PASSED PARAMETERS ARE IGNORED!!!

		// Also need to clear the watchdog output and the warning output here	
		SET_PROGRAM_RUNNING;
		CLR_FATAL_ERROR;
		CLR_WARNING;
		//OSW1 &= TURN_OFF_CONT_OUTPUTS_EXCEPT_LS_WARN_MASK; 
		TURN_OFF_OUTPUTS_EXCEPT_LS_WARN;
		WARNING_WORD=0;
		FATAL_ERROR_WORD=0;	
		
		//out_dac(CONT_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE);
		out_dac(AXIS1_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 1 to the retract setting
		axis_num=2;
		// If there is not a valid DAC channel specified for axis2, out_dac() will not do anything
		out_dac(AXIS2_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 2 to zero

		// Need to disable WD and warning output here.
		set_clr_discrete_output(WARNING_OUTPUT, TURN_ON); // Disable warning by turning on
		PULSE_WATCHDOG_RESET;
		update_control_parameters();
		//#if SCI_DEBUG
		//	print_again = nr_timer_milliseconds() + 1000;
		//#endif

		// From here we'll go down to the regular control supervisor and begin
		// executing blocks.
		return (0);
		}



int dwell_block_compile(char *str_to_compile)
	{
	db *dwell_block=0;
	int return_val=0, stepnum, x, tempi, immed_exe=0;
	volatile char *ptr=0;
#if DWELL_COMPILE_DEBUG
	int beg, dur, atend; // For test
#endif

	#if DWELL_COMPILE_DEBUG
		printf("string to compile = %s\n", str_to_compile);	
	#endif		
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
	#if DWELL_COMPILE_DEBUG
		printf("Comment has been stripped - see %s\n", str_to_compile);
	#endif	
	if (str_to_compile[0] == 'D') //Command for immediate execution, first compile.
		{
		#if DWELL_COMPILE_DEBUG
			printf("IMMED exe\n");	
		#endif
		immed_exe=1;	
		dwell_block = (db *)&vtg.control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		#if DWELL_COMPILE_DEBUG
			printf("Compile and save\n");	
		#endif
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		dwell_block = (db *)&vtg.control_program[stepnum][0];
		}

	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		vtg.control_program[stepnum][x] = 0;
		}

#if DWELL_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	dwell_block->blk_id = DWELL;

	// Set ptr to dwell_block_exe().
	//dwell_block->blk_to_execute = (control_block_function)dwell_block_exe;

#if DWELL_COMPILE_DEBUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d 0x%x\n", x, vtg.control_program[stepnum][x]);
#endif

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		// Always cast the update block structure when calling block conditional.
		// For all block types, the conditional, set output stuff will be the 
		// same.  The update block structure in this case is being used as a 
		// template that is good for all block types.
		return_val = block_conditional_compile((c_block)dwell_block, (char *)ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 
		
	#if DWELL_COMPILE_DEBUG
		printf("Got past block_conditional_compile()\n");
	#endif	

	//At this point, any comment and conditional/set output items have been
	//removed.   
	if ((ptr = (char *)strchr(str_to_compile, 'D')) == NULL) {return_val = 1; goto clr_id;}	
		ptr++;
	switch (*ptr) 
		{
		case '+': // Integer constant
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'h':
		case 'H':
			// First get integer and save to tempi
			if (*ptr == 'h' || *ptr == 'H') tempi = htoi((char *)ptr+1);
			else tempi = atoi(ptr);
				
			dwell_block->db_indicator_bits |= db_const_mask;
			dwell_block->dwell_df = tempi;
			break;
			
		case 'V': //Variable
			ptr++;
			if ((tempi = atoi(ptr)) > MAX_VAR) {return_val = 1; goto clr_id;}
			
			dwell_block->db_indicator_bits |= db_var_mask;
			dwell_block->dwell_df = tempi;
			break;

		default: //Error, invalid dwell specification found
			return_val = 1;
			break;

		}	
	
	
	clr_id: ;
	if (return_val != 0) // Error of some kind, clear the block ID
		//dwell_block->blk_to_execute = NULL;
		dwell_block->blk_id = NULL;
			
	// Now print the block for debug
	#if DWELL_COMPILE_DEBUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, vtg.control_program[stepnum][x]);	
	#endif
	
	
	ext: ;

	if (immed_exe && (return_val==0))
		{
		// If the control program is running, the IMMED block will be initiated
		// by the control supervisor when it finishes running its current block.
		// Otherwise we'll initiate the block here since there should not be
		// any danger of recursion.
		//printf("PROGRAM_RUNNING=%d\n", PROGRAM_RUNNING); // For test
		if (!PROGRAM_RUNNING)
			{
			#if DWELL_COMPILE_DEBUG
				printf("Control program not running, we'll call dwell_block_exe() here\n");	
			#endif
			// Execute the block we just compiled. 
			dwell_block_exe((c_block)dwell_block, IMMED, 1);  
			}
		}
					
	return (return_val);		
	}




// Execute dwell block
int dwell_block_exe(c_block control_block, int type_of_execution, int zone)
	{
	static db *block;
	static c_block saved_control_block;
	static int cond_result, return_val, saved_execution_type, do_set_output;
	//int input_state; // For test
	unsigned int dwell_time_mS=0, timer_load_value;
	volatile unsigned short int *halfword_ptr;
	//unsigned int time_met_counts, current_raw_vel_ctr_val;
	//long initial_timer_mS;
	np_timer *timer2 = na_timer2;
	np_pio *misc_ins_pio = na_misc_ins;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;

	if (zone == 1) // We're executing the block from the beginning.
		{
		return_val = 0;
		saved_execution_type = type_of_execution;
		do_set_output=0;
		saved_control_block = control_block;

		block = (db *)control_block;
		#if DWELL_EXE_DEBUG
			printf("Dwell blk exe-control_block[0]=0x%x and control_block=0x%x\n", control_block[0], control_block);
			printf("Dwell blk exe-vtg.control_program[0][0] = 0x%x\n", vtg.control_program[0][0]);
			printf("Dwell blk exe-&control_block[0]=0x%x, control_block[0]=0x%x, &vtg.control_program[0][0]=0x%x and *control_block[0]=0x%x\n", &control_block[0], control_block[0], &vtg.control_program[0][0], *control_block[0]);
		#endif
			
		cond_result = block_conditional_exe(control_block); 
				
		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			#if DWELL_EXE_DEBUG
				printf("Dwell blk exe-Some sort of CONDITIONAL exe error\n");
			#endif	

			return_val = cond_result;
			} 
		else if (cond_result < 20) // OK to proceed, WHILE interrupt may have been armed if BUILD_FOR_INTERRUPT_CONTROL. 
			{
			#if DWELL_EXE_DEBUG
				printf("Dwell blk exe, zone 1-OK to proceed, blk#%d\n", vtg.prog_ctr);
			#endif	
			// This is the main body of the dwell block	

			// Set/clr any outputs here
			if (SET_OUT_BEG_OR_DUR)
				{
				#if DWELL_EXE_DEBUG
					printf("Dwell blk exe-Set output beg. or dur., df=%d, turn_on_off=%d\n", block->set_out_df, block->cond_indicator_bits & turn_on_turn_off_mask);
				#endif
				SET_CLR_CONT_OUTPUT_NOW;
				}

			if (block->db_indicator_bits & db_const_mask)
				{
				dwell_time_mS = block->dwell_df;
				}
			else if (block->db_indicator_bits & db_var_mask)
				{
				dwell_time_mS = vtg.v[block->dwell_df];
				}
			else  // Can't execute, invalid indicator bit settings.
				{
				return_val = EXE_FAULT_IND_BITS; // dwell_time_mS already defaulted to zero
				}	
				
			if (dwell_time_mS > 0)
				{
				#if DWELL_EXE_DEBUG
					printf("Dwell blk exe, zone 1-Setting dwell time %d mS\n", dwell_time_mS);
				#endif
				// Now set up interrupt structure for timeout
				convert_uS_to_timer((dwell_time_mS*1000), &timer_load_value);
				halfword_ptr = (unsigned short int *)&timer_load_value;
			
				#if DWELL_EXE_DEBUG
					printf("Dwell blk exe, zone 1-About to arm timer\n");
				#endif

				#if BUILD_FOR_INTERRUPT_CONTROL
					// Arm for a single timeout, reenter to zone 2 at timeout interrupt.
					ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], ONCE, 2);
					return (REENTER_ON_INTERRUPT); // We'll return to zone 2
				#else // Polled control, we'll go right to zone 2 and wait for timeout after arming timer.
					ARM_TIMER2_FOR_GP_TIMING(halfword_ptr[0], halfword_ptr[1], ONCE);
					zone=2;
				#endif

				#if DWELL_EXE_DEBUG
					printf("Dwell blk exe, zone 1-Timer2 armed\n");
				#endif
				}

			do_set_output=1;
			}
		else if (cond_result == COND_NOT_MET)
			{
			// Nothing more to do 
			#if DWELL_EXE_DEBUG
				printf("Dwell blk exe-condition not met\n");
			#endif
			}
		else  // This should not happen - if it does, deal with it here.
			{
			#if DWELL_EXE_DEBUG
				printf("Dwell blk exe-We should not be here!!! Must find out why\n");
			#endif	
			return(BLOCK_EXECUTION_ERROR); // This will trigger a fatal error
			}
		}		
	
	if (zone == 2)
		{
		#if !BUILD_FOR_INTERRUPT_CONTROL // Polled control, wait for timeout here.
			while (!vtg.timer2_timeout)
				{
				if (cond_result == KEEP_GOING_CALL_BACK_DURING)
					{ 
					if (block_conditional_exe(control_block) == KEEP_GOING_CALL_BACK_DURING);		
					else break;    // Condition was not met.  Any outputs to
								  // set_clr will have already been taken care
								 // of by block_conditional() therfore we jump
								// past the AT_END checking.
					}
				// For test	
				//if (SW8) {vtg.warning = TEST_WARNING; goto ext;} 
				//if (SW9) {vtg.fatal_error = TEST_FATAL_ERROR; goto ext;} 	
				}
		#endif

		#if DWELL_EXE_DEBUG
			printf("Dwell blk exe-Zone 2, timeout has occured or while condition failed.\n");
		#endif

		do_set_output=1;
		}

	if (zone == WHILE_CONDITIONAL_FAILED)
		{
		#if DWELL_EXE_DEBUG
			printf("Dwell blk exe-WHILE_CONDITIONAL_FAILED, exiting block\n");
		#endif

		do_set_output=1;
		}

	ext: ;

	// Handle any outputs to set/clr now.
	if (do_set_output)
		{
		if (SET_OUT_DUR) INVERT_CONT_OUTPUT_NOW; // Set output back to its original state.
		else if (SET_OUT_AT_END) SET_CLR_CONT_OUTPUT_NOW;
		}

	#if BUILD_FOR_INTERRUPT_CONTROL
		// Disarm any potentially pending interrupts we may have used executing this block.
		DISARM_TIMER2_CONT_INTERRUPT;
		DISARM_IO1_CONT_INTERRUPT; // Conditional checking may have armed this interrupt
	#endif

	// Increment program counter
	if (saved_execution_type==PROGRAM)
		{
		vtg.prog_ctr++; // Increment prog ctr.		
		}
	return (return_val);
	}



int position_block_compile(char *str_to_compile)
	{
	pb *position_block=0;
	int return_val = 0, stepnum, x, tempi, item_being_scanned, immed_exe=0;
	volatile char *ptr=0;

	#if POS_COMPILE_DEGUG
		printf("string to compile = %s\n", str_to_compile);		
	#endif	
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0;
	#if POS_COMPILE_DEGUG 
		printf("Comment has been stripped - see %s\n", str_to_compile);
	#endif	
	if (str_to_compile[0] == 'L') //Command for immediate execution, first compile.
		{
		immed_exe=1;	
		position_block = (pb *)&vtg.control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		position_block = (pb *)&vtg.control_program[stepnum][0];
		}

	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		vtg.control_program[stepnum][x] = 0;
		}
#if POS_COMPILE_DEGUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif	
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	position_block->blk_id = POSITION;
	//position_block->blk_to_execute = (control_block_function)position_block_exe;
	
#if POS_COMPILE_DEGUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		// Always cast the update block structure when calling block conditional.
		// For all block types, the conditional, set output stuff will be the 
		// same.  The update block structure in this case is being used as a 
		// template that is good for all block types.
		return_val = block_conditional_compile((c_block)position_block, (char *)ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 
#if POS_COMPILE_DEGUG	
	printf("Got past block_conditional_complie()\n");
#endif	

	//At this point, any comment and conditional/set output items have been
	//removed.   
	#define PB_POS 0 
	#define PB_VEL 1 
		
	// A for loop will loop between the 2 possible choices of items in
	// a position block, POS and VEL.
	// The cases in the switch statement are the possible data
	// types the scanned items can be, only constants and variables
	// in this case.  
	for (item_being_scanned=PB_POS; item_being_scanned<=PB_VEL; item_being_scanned++)
		{
		#if POS_COMPILE_DEGUG	
			printf("item_being_scanned = %d\n", item_being_scanned);
		#endif	
		// Move ptr to point to correct part of string based on item being scanned
		if (item_being_scanned == PB_POS)
			{
			if ((ptr = (char *)strchr(str_to_compile, 'L')) == NULL) {return_val = 1; goto clr_id;}	
			else
				{
				ptr+=2;	
				if ((*ptr > NUM_OF_AXES+48) || (*ptr < 49)) {return_val = 1; goto clr_id;} // Axis# out of range	
				else if (*ptr == '1') position_block->pb_indicator_bits |= pb_axis1_mask;
				else if (*ptr == '2') position_block->pb_indicator_bits |= pb_axis2_mask;
				else if (*ptr == '3') position_block->pb_indicator_bits |= pb_axis3_mask;
				else if (*ptr == '4') position_block->pb_indicator_bits |= pb_axis4_mask;
				#if POS_COMPILE_DEGUG
					printf("Axis number is %c\n", *ptr);
				#endif		
				ptr++; // Move past the axis specifier
				}
			#if POS_COMPILE_DEGUG	
				printf("found a L %s\n", ptr-3);
			#endif	
			}
		else if (item_being_scanned==PB_VEL)
			{
			if ((ptr = (char *)strchr(str_to_compile, 'F')) == NULL) continue; // No accel to process, loop again 
			else ptr++;
			#if POS_COMPILE_DEGUG	
				printf("found a F %s\n", ptr-1);
			#endif	
			}
					
		switch (*ptr) 
			{
			case '+': // Integer constant
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'h':
			case 'H':
				// First get integer and save to tempi
				if (*ptr == 'h' || *ptr == 'H') tempi = htoi((char *)ptr+1);
				else tempi = atoi(ptr);
				
				if (item_being_scanned == PB_POS)
					{
					position_block->pb_indicator_bits |= pb_pos_const_mask; 	
					position_block->pos_df = tempi;
					}
				else if (item_being_scanned == PB_VEL)
					{
					position_block->pb_indicator_bits |= pb_vel_const_mask; 	
					position_block->vel_df = tempi;	
					}	
				break;
			
			case 'V': //Variable
				ptr++;
				if ((tempi = atoi(ptr)) > MAX_VAR) {return_val = 1; goto clr_id;}
			
				if (item_being_scanned == PB_POS)
					{
					position_block->pb_indicator_bits |= pb_pos_var_mask; 
					position_block->pos_df = tempi;
					}
				else if (item_being_scanned == PB_VEL)
					{
					position_block->pb_indicator_bits |= pb_vel_var_mask; 
					position_block->vel_df = tempi; 	
					}
				break;
				
			default: //Error, valid data type not found
				return_val = 1;
				goto clr_id;
				break;

			}
		}// For loop
	

	clr_id: ;
	if (return_val != 0) // Error of some kind, clear the block ID
		//position_block->blk_to_execute = NULL;
		position_block->blk_id = NULL;
			
	// Now print the block for debug
	#if POS_COMPILE_DEGUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, vtg.control_program[stepnum][x]);	
	#endif		

	ext: ;
		
	if (immed_exe && return_val==0)
		{
		// If the control program is running, the IMMED block will be initiated
		// by the control supervisor when it finishes running its current block.
		// Otherwise we'll initiate the block here since there should not be
		// any danger of recursion.
		if (!PROGRAM_RUNNING)
			{
			// Execute the block we just compiled. 
			position_block_exe((c_block)position_block, IMMED, 1);  
			}
		}

	return (return_val);		
	}


// Execute position block
int position_block_exe(c_block control_block, int type_of_execution, int zone)
	{

	}




int velocity_block_compile(char *str_to_compile)
	{
	vb *velocity_block=0;
	int return_val = 0, stepnum, x, tempi, item_being_scanned, immed_exe=0;
	volatile char *ptr=0;


	#if VEL_COMPILE_DEBUG
		printf("string to compile = %s\n", str_to_compile);		
	#endif	
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
	#if VEL_COMPILE_DEBUG
		printf("Comment has been stripped - see %s\n", str_to_compile);
	#endif	
	if (str_to_compile[0] == 'V') //Command for immediate execution, first compile.
		{
		immed_exe=1;	
		velocity_block = (vb *)&vtg.control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		velocity_block = (vb *)&vtg.control_program[stepnum][0];
		}

	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		vtg.control_program[stepnum][x] = 0;
		}
		
#if VEL_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	velocity_block->blk_id = VELOCITY;
	//velocity_block->blk_to_execute = (control_block_function)velocity_block_exe;

#if VEL_COMPILE_DEBUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		// Always cast the update block structure when calling block conditional.
		// For all block types, the conditional, set output stuff will be the 
		// same.  The update block structure in this case is being used as a 
		// template that is good for all block types.
		return_val = block_conditional_compile((c_block)velocity_block, (char *)ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 

#if VEL_COMPILE_DEBUG	
	printf("Got past block_conditional_compile()\n");
#endif	

	//At this point, any comment and conditional/set output items have been
	//removed.   
	#define VB_VEL 0 
	#define VB_ACCEL 1 
	#define VB_EP 2 
	
	// A for loop will loop between the 3 possible choices of items in
	// a velocity block, ACCEL, VEL and ending pos.
	// The cases in the switch statement are the possible data
	// types the scanned items can be, only constants and variables
	// in this case.  
	for (item_being_scanned=VB_VEL; item_being_scanned<=VB_EP; item_being_scanned++)
		{
		#if VEL_COMPILE_DEBUG	
			printf("item_being_scanned = %d\n", item_being_scanned);
		#endif	
		// Move ptr to point to correct part of string based on item being scanned
		if (item_being_scanned == VB_VEL)
			{
			if ((ptr = (char *)strchr(str_to_compile, 'V')) == NULL) {return_val = 1; goto clr_id;}	
			else
				{
				ptr+=2;	
				if ((*ptr > NUM_OF_AXES+48) || (*ptr < 49)) {return_val = 1; goto clr_id;} // Axis# out of range	
				else if (*ptr == '1') velocity_block->vb_indicator_bits |= vb_axis1_mask;
				else if (*ptr == '2') velocity_block->vb_indicator_bits |= vb_axis2_mask;
				else if (*ptr == '3') velocity_block->vb_indicator_bits |= vb_axis3_mask;
				else if (*ptr == '4') velocity_block->vb_indicator_bits |= vb_axis4_mask;
				#if VEL_COMPILE_DEBUG
					printf("Axis number is %c\n", *ptr);	
				#endif	
				ptr++; // Move past the axis specifier
				}
			#if VEL_COMPILE_DEBUG	
				printf("found a V %s\n", ptr-3);
			#endif	
			}
		else if (item_being_scanned==VB_ACCEL)
			{
			if ((ptr = (char *)strstr(str_to_compile, "AC")) == NULL) continue; // No accel to process, loop again 
			else ptr+=2;
			#if VEL_COMPILE_DEBUG	
				printf("found a A %s\n", ptr-2);
			#endif	
			}
		else if (item_being_scanned==VB_EP)
			{
			if ((ptr = (char *)strchr(str_to_compile, 'P')) == NULL) break; // No ending pos to process 
			else ptr++;
			#if VEL_COMPILE_DEBUG	
				printf("found a P %s\n", ptr-1);
			#endif	
			}
			
		switch (*ptr) 
			{
			case '+': // Integer constant
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'h':
			case 'H':
				// First get integer and save to tempi
				if (*ptr == 'h' || *ptr == 'H') tempi = htoi((char *)ptr+1);
				else tempi = atoi(ptr);
				
				if (item_being_scanned == VB_VEL || item_being_scanned == VB_ACCEL)
					{
					// VEL and ACCEL cannot be neg.  The setting itself, the
					// lower 16 bits can be signed in the case of VEL.
					// Bit 15 indicates the sign.
					if (*ptr == '-') {return_val = 1; goto clr_id;}
					else
						{
						if (item_being_scanned == VB_VEL)
							{
							velocity_block->vb_indicator_bits |= vb_vel_const_mask;	
							velocity_block->vel_df = tempi;
							}
						else
							{
							velocity_block->vb_indicator_bits |= vb_accel_const_mask;	
							velocity_block->accel_df = tempi;	
							}
						}
					}
				else if (item_being_scanned == VB_EP)
					{
					if (tempi > 8388607) tempi = 8388607;
					else if (tempi < -8388608) tempi = -8388608;
					velocity_block->vb_indicator_bits |= vb_endpos_const_mask;
					velocity_block->endpos_df = tempi;	
					}	
				break;
			
			case 'V': //Variable
				ptr++;
				if ((tempi = atoi(ptr)) > MAX_VAR) {return_val = 1; goto clr_id;}
			
				if (item_being_scanned == VB_VEL)
					{
					velocity_block->vb_indicator_bits |= vb_vel_var_mask;
					velocity_block->vel_df = tempi;
					}
				else if (item_being_scanned == VB_ACCEL)
					{
					velocity_block->vb_indicator_bits |= vb_accel_var_mask;
					velocity_block->accel_df = tempi; 	
					}
				else if (item_being_scanned == VB_EP)
					{
					velocity_block->vb_indicator_bits |= vb_endpos_var_mask;
					velocity_block->endpos_df = tempi; 	
					}		
				break;
				
			default: //Error, valid data type not found
				return_val = 1;
				goto clr_id;
				break;

			}
		}// For loop
	

	clr_id: ;
	if (return_val != 0) // Error of some kind, clear the block ID
		//velocity_block->blk_to_execute = NULL;
		velocity_block->blk_id = NULL;
		
	// Now print the block for debug	
	#if VEL_COMPILE_DEBUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, vtg.control_program[stepnum][x]);	
	#endif		

	ext: ;
	
	if (immed_exe && return_val==0)
		{
		// If the control program is running, the IMMED block will be initiated
		// by the control supervisor when it finishes running its current block.
		// Otherwise we'll initiate the block here since there should not be
		// any danger of recursion.
		if (!PROGRAM_RUNNING)
			{
			// Execute the block we just compiled. 
			velocity_block_exe((c_block)velocity_block, IMMED, 1); 
			}
		}

	return (return_val);		
	}




// Execute velocity block
int velocity_block_exe(c_block control_block, int type_of_execution, int zone)
	{
	static int accel, int_vel_cmd, accel_increments, loop_num, starting_pos;
	static int saved_execution_type, vac_wait, vac_on, do_set_output, fill_test;
	static int axis_num, commanded_raw_dac, first_loop, slow, dac_increment;
	static int return_val, goose, cond_result, closed_loop, dv, position;
	static short int commanded_vel;
	static vb *block;
	static c_block saved_control_block;
	
	int temp_pos, input_state, act_vel, temp_dac_command;
	int current_raw_dac, next_pos;
	//unsigned int initial_timer_mS, send_op_time;
	unsigned int tempu=0, loop_time;
	unsigned int initial_timer_mS=0; // For polled control
	volatile unsigned short int *halfword_ptr;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	np_timer *timer2 = na_timer2;
	//vp_pvu *pv1 = va_pv1;
	//vp_pvu *pv2 = va_pv2;
	//int print_again; // For test


	if (zone == 1) // Initial entry of block
		{
		saved_execution_type = type_of_execution;
		block = (vb *)control_block;
		saved_control_block = control_block;
		do_set_output=0;
		return_val=0;
		goose=0;
		closed_loop=1;
		vac_wait=0;

		cond_result = block_conditional_exe(control_block); 
		
		
		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			return_val=cond_result;	
			} 
		else if (cond_result < 20) // OK to proceed.
			{
			#if VEL_EXE_DEBUG
				printf("\n\n\n****************************************************************\n");
				printf("vel blk exe-zone 1, OK to proceed, block #%d\n", vtg.prog_ctr);
			#endif

			// Set/clr any outputs here
			if (SET_OUT_BEG_OR_DUR) SET_CLR_CONT_OUTPUT_NOW;

			/////////////////// Initial setups ////////////////////
			//initial_timer_mS = nr_timer_milliseconds();
			//send_op_time = initial_timer_mS + 500; // Time to send first op data packet

			//print_again = initial_timer_mS+750; // For test
			
			// Check if vac_on and fill test
			chk_discrete_input(VAC_ON_INPUT, &input_state);	
			vac_on = input_state; // 1 if on 0 if off
			chk_discrete_input(FILL_TEST_INPUT, &input_state);	
			fill_test = input_state;
			accel = 0;
			commanded_raw_dac = 0;
			first_loop=1;
			
			// Check if closed loop or open loop
			if (block->vb_indicator_bits & vb_vel_var_mask)
				tempu = vtg.v[block->vel_df];
			else if (block->vb_indicator_bits & vb_vel_const_mask)
				tempu = block->vel_df;
			else tempu=0x7fff; // Skip code
				
			if (tempu & 0x80000000)
				{
				closed_loop=0; // Open loop
				#if VEL_EXE_DEBUG
					printf("tempu & 0x80000000==TRUE_this is an open loop step\n");
				#endif
				}

			if (tempu & 0x40000000)
				{
				vac_wait=1; // This is a vacuum wait step.
				#if VEL_EXE_DEBUG
					printf("tempu & 0x40000000==TRUE_this is a vacuum wait step!!!\n");
				#endif
				}

			commanded_vel = (short int)tempu; // Velocity cmd is the lower half word, can be pos. or neg.
			if (commanded_vel == 0x7fff) goto ext; // Skip entire block without any test inputs/set outputs.
			// If open loop, convert commanded velocity from voltage to raw DAC value.
			if (!closed_loop)
				commanded_raw_dac = (unsigned int)(8192 + ((819 * (int)commanded_vel)/1000));
					
				
			// Set accel_increments / goose
			if (block->vb_indicator_bits & vb_accel_var_mask)
				tempu = vtg.v[block->accel_df];
			else if (block->vb_indicator_bits & vb_accel_const_mask)
				tempu = block->accel_df;
			else tempu=0;
					
			if (tempu>0)
				{
				if (tempu & 0x80000000) 
					{
					#if VEL_EXE_DEBUG
						printf("Goose detected!!!\n");
					#endif
   					goose = tempu & 0x0000ffff; // Goose value in volts X 1000, always assumed to be positive.
					}
				else
					{
					#if VEL_EXE_DEBUG
						printf("Accel detected!!!\n");
					#endif
   					accel = tempu & 0x0000ffff; // Accel in X1 counts, always assumed to be positive.
					}
				}
				
			// Get position
			if (block->vb_indicator_bits & vb_endpos_var_mask)
				tempu = vtg.v[block->endpos_df];
			else if (block->vb_indicator_bits & vb_endpos_const_mask)
				tempu = block->endpos_df;
			else tempu=0; // No position
			position = tempu;

			if (block->vb_indicator_bits & vb_axis2_mask) axis_num = 2;
			else axis_num = 1;
			#if VEL_EXE_DEBUG
				printf("Velocity axis# is %d\n", axis_num);
			#endif
			///////////////////////////////////////////////////////	
			
			
			// Determine what zone to go to next here
			if (closed_loop)
				{
				if (goose) zone = 2;
				else if (accel) zone = 3; 
				else zone = 4;
				}
			else  // Open loop
				{
				if (accel) zone = 5;
				else zone = 6;
				}

			// Set up ending position and 10 block timeout
			// interrupts here.  Either of thse will end the block.
			#if BUILD_FOR_INTERRUPT_CONTROL
				ARM_BLK_TIMEOUT_CONT_INTERRUPT(10000, 7);
				if (position)
					{
					if (vac_wait && vac_on) {ARM_GP_POS1_CONT_INTERRUPT(position+240, 9);} // If vacuum shot, don't allow cylinder to move more than 3" past the ending position.
					else {ARM_GP_POS1_CONT_INTERRUPT(position, 8);}
					}
				
				// If vac_wait && !vac_on, disarm the WHILE conditional interrupt.
				// If the vacuum is not on, the normal ending position will terminate the
				// the block.  We don't care about an ok for fs signal here.
				if (vac_wait && !vac_on) {DISARM_IO1_CONT_INTERRUPT;}
			#else // Polled control, set up block timeout
				initial_timer_mS = nr_timer_milliseconds();
			#endif
			}
		else if (cond_result == COND_NOT_MET)  
			{
			// Nothing more to do
			}
		else
			{
			// This should not happen - if it does, deal with it here.
			return(BLOCK_EXECUTION_ERROR); // This will trigger a fatal error
			}
		} // zone 1 

	if (zone == 2) // Goose
		{
		#if VEL_EXE_DEBUG
			printf("vel blk exe-zone 2, Goose\n");
		#endif

		#include "LOOP_EXIT_CONDITIONS3.H" // Check if we need to abort for some reason
      	//out_dac(CONT_DAC_CHANNEL, goose, VOLTAGE_X_1000, ABSOLUTE);
		out_dac(DAC_OUTPUT_CHANNEL, goose, VOLTAGE_X_1000, ABSOLUTE); // Goose it here
		zone=4; // Move to zone 4 and begin closing the velocity loop
		}	

	if (zone == 3) // Closed loop accel
		{
		if (first_loop)
				{
				#if VEL_EXE_DEBUG
					printf("vel blk exe-zone 3, Closed loop accel, first time through\n");
				#endif

				first_loop=0;
								
      			// Closed loop acceleration defined in X1 pps per X1 count
      			loop_num = 100;  // For diagnostics 
		
				starting_pos = RD_PV_POS;
				act_vel = axis[axis_num].vel_ctr_freq/RD_PV_VEL; // Vel in pps
				int_vel_cmd = act_vel;

      			// First determine vel change per increment. 
      			accel_increments = ((int)commanded_vel - act_vel) / accel;

				#if BUILD_FOR_INTERRUPT_CONTROL
					// Arm for X1 interrupt, reenter at zone 3
					ARM_X1_PULSE_CONT_INTERRUPT(3);
				#else // Polled control
					next_pos = starting_pos+4; 
				#endif
				}

	accel_loop: ;
		temp_pos = RD_PV_POS;
		#if !BUILD_FOR_INTERRUPT_CONTROL 
			next_pos = temp_pos+4; // For polled control, X1 pulse
		#endif

		int_vel_cmd += accel_increments;
		act_vel = axis[axis_num].vel_ctr_freq/RD_PV_VEL; // Vel in pps
		dv = int_vel_cmd - act_vel;
		
		// Temp_dac_command is the change in dac output before adding to
		// the existing dac output  

		// Boost gain ONLY if accelerating when moving slowly. 
		if (((axis[axis_num].vel_ctr_freq / int_vel_cmd) > 40000) && (accel_increments > 0)) // less than about 20 IPS
			temp_dac_command = (dv * (VEL_LOOP_GAIN * 5)) / 6993; // Raw DAC counts
			//temp_dac_command = (dv * (VEL_LOOP_GAIN * 5)) / 5727;  // Voltage * 1000
		else
			temp_dac_command = (dv * VEL_LOOP_GAIN) / 6993;  // Raw DAC counts
			//temp_dac_command = (dv * VEL_LOOP_GAIN) / 5727;   // Voltage * 1000


		// Now check gain breaks.
		if ((act_vel >= VEL_LOOP_BK_VEL3) && (temp_pos > 240))
			temp_dac_command = (temp_dac_command * VEL_LOOP_BKGN3) / 1000;

		else if ((act_vel >= VEL_LOOP_BK_VEL2) && (temp_pos > 240))
			temp_dac_command = (temp_dac_command * VEL_LOOP_BKGN2) / 1000;

		else if ((act_vel >= VEL_LOOP_BK_VEL1) && (temp_pos > 240))
			temp_dac_command = (temp_dac_command * VEL_LOOP_BKGN1) / 1000;

		// Put dither code here. 

		// Add temp_dac_command the the existing DAC command
		//out_dac(CONT_DAC_CHANNEL, temp_dac_command, VOLTAGE_X_1000, INCREMENTAL);
		//out_dac(CONT_DAC_CHANNEL, temp_dac_command, RAW_VAL, INCREMENTAL);
		out_dac(DAC_OUTPUT_CHANNEL, temp_dac_command, RAW_VAL, INCREMENTAL);

		#if BUILD_FOR_INTERRUPT_CONTROL  
			#include "LOOP_EXIT_CONDITIONS3.H"
		#else // Loop if polled control
			do
				{
				
				#include "LOOP_EXIT_CONDITIONS3.H"

				}while ((int)RD_PV_POS < next_pos);  // Wait for next sample 
		#endif

		if (--accel == 0) // Finished accel, set up for closed loop velocity
			{
			#if BUILD_FOR_INTERRUPT_CONTROL
				DISARM_X1_PULSE_CONT_INTERRUPT;
			#endif
			first_loop=1;
			zone=4;
			}
		else 
			{
			#if BUILD_FOR_INTERRUPT_CONTROL 
				return(REENTER_ON_INTERRUPT);
			#else // Polled control
				goto accel_loop;
			#endif
			}
		}
		
	if (zone == 4) // Closed velocity loop
		{
		if (first_loop)
			{
			#if VEL_EXE_DEBUG
				printf("vel blk exe, zone 4, Closing velocity loop, first time through\n");
			#endif

			first_loop=0;
					
   			// Determine loop time based on commanded velocity. 
			act_vel = axis[axis_num].vel_ctr_freq/RD_PV_VEL; // Vel in pps

   			// Fast loop if decelerating from above the slow point, 12 IPS. 
			loop_time = FAST_LOOP;
			slow = 0;
			
			if ((act_vel > (int)commanded_vel) && (act_vel > 240));
			else if ((int)commanded_vel < 240) // Less than 12 IPS
				{
				loop_time = SLOW_LOOP;
				slow = 1;	
				}
			//else ;	
			
   			#if VEL_EXE_DEBUG
				printf("loop_time = %d\n", loop_time);
			#endif

   			loop_num = 300;  // For diagnostics 
		
			halfword_ptr = (unsigned short int *)&loop_time;
			#if BUILD_FOR_INTERRUPT_CONTROL
				// Set up for return on interrupt to zone 4 here
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 4);
			#else // Polled control
				ARM_TIMER2_FOR_GP_TIMING(halfword_ptr[0], halfword_ptr[1], CONTINUOUS);
			#endif
			}
		
	vel_loop: ;
		vtg.timer2_timeout = 0; 
		act_vel = axis[axis_num].vel_ctr_freq/RD_PV_VEL; // Vel in pps
		dv = (int)commanded_vel - act_vel;
      	
      	if (slow)  // slow 
			{
			// If slow, increase gain by a factor of 10. The loop time
			// has already been increased by a factor of 10.  This will
			// provide much improved slow speed velocity control. 
			temp_dac_command = (dv * (VEL_LOOP_GAIN * 10)) / 6993; // Constant assuming raw DAC counts
			//temp_dac_command = (dv * (VEL_LOOP_GAIN * 10)) / 5727; // Constant assuming volts * 1000
			}
      	else
			// temp_dac_command is the change in dac output before adding to
			//	the existing dac output 
			temp_dac_command = (dv * VEL_LOOP_GAIN) / 6993; // Raw dac counts
			//temp_dac_command = (dv * VEL_LOOP_GAIN) / 5727; // Volts * 1000

		// Now check gain breaks.
		temp_pos = RD_PV_POS; // Get current position
		
		if ((act_vel >= VEL_LOOP_BK_VEL3) && (temp_pos > 240))
			temp_dac_command = (temp_dac_command * VEL_LOOP_BKGN3) / 1000;

		else if ((act_vel >= VEL_LOOP_BK_VEL2) && (temp_pos > 240))
			temp_dac_command = (temp_dac_command * VEL_LOOP_BKGN2) / 1000;

		else if ((act_vel >= VEL_LOOP_BK_VEL1) && (temp_pos > 240))
			temp_dac_command = (temp_dac_command * VEL_LOOP_BKGN1) / 1000;

		// Put dither code here. 

		// Add temp_dac_command to the current DAC command
		//out_dac(CONT_DAC_CHANNEL, temp_dac_command, RAW_VAL, INCREMENTAL);
		out_dac(DAC_OUTPUT_CHANNEL, temp_dac_command, RAW_VAL, INCREMENTAL);
		
		#if BUILD_FOR_INTERRUPT_CONTROL 
			#include "LOOP_EXIT_CONDITIONS3.H"
			return(REENTER_ON_INTERRUPT);
		#else 
			do // Wait for next sample 
	     		{
	     	
	     		#include "LOOP_EXIT_CONDITIONS3.H"
			
	     		}while (!vtg.timer2_timeout);  // Wait for next sample 

			goto vel_loop; // Loop until something in LOOP_EXIT_CONDITIONS3.H kicks us out.
		#endif
		}


	if (zone == 5) // Open loop accel
		{
		if (first_loop)
			{
			#if VEL_EXE_DEBUG
				printf("vel blk exe-zone 5, Open loop accel, first time through\n");
			#endif
			first_loop=0;
			starting_pos = RD_PV_POS;
			#if !BUILD_FOR_INTERRUPT_CONTROL // Polled control
				next_pos = starting_pos+4;
			#endif
      		// Open loop accel is the # of X4 counts to
			// accelerate from the previous dac_command to the new one.
			// commanded_vel is the final open loop DAC setting in
			// volts X 1000 / % X 100, same thing.
			//get_dac(CONT_DAC_CHANNEL, VOLTAGE_X_1000, &current_dac_voltage); 
      		//dac_increment = ((int)commanded_vel - current_dac_voltage) / accel; // Remember, commanded_vel is an OL percentage here.

			
			//get_dac(CONT_DAC_CHANNEL, RAW_VAL, &current_raw_dac); 
			get_dac(DAC_OUTPUT_CHANNEL, RAW_VAL, &current_raw_dac); 
      		dac_increment = (commanded_raw_dac - current_raw_dac) / accel; // Remember, commanded_vel is an OL percentage here.

      		if (dac_increment == 0)  // Don't bother controlling accel 
				{
				#if VEL_EXE_DEBUG
					printf("dac_increment=0, no accel\n");
				#endif
				}
			else
				{
				loop_num = 400;  // For diagnostics
				
				#if BUILD_FOR_INTERRUPT_CONTROL
					// Arm for X1 interrupt, reenter at zone 5
					ARM_X1_PULSE_CONT_INTERRUPT(5);
				#endif
				}
			}

	ol_accel_loop: ;
		temp_pos = RD_PV_POS;
		#if !BUILD_FOR_INTERRUPT_CONTROL 
			next_pos = temp_pos+4; // For polled control, X1 pulse
		#endif
		//#if VEL_EXE_DEBUG
		//	printf("dac_increment=%d, accel=%d, commanded_raw_dac=%d, cur_raw_dac=%d\n", dac_increment, accel, commanded_raw_dac, current_raw_dac);
		//#endif
		
				
		// Add dac_increment to current DAC voltage
		//out_dac(CONT_DAC_CHANNEL, dac_increment, VOLTAGE_X_1000, INCREMENTAL);
		//out_dac(CONT_DAC_CHANNEL, dac_increment, RAW_VAL, INCREMENTAL);
		out_dac(DAC_OUTPUT_CHANNEL, dac_increment, RAW_VAL, INCREMENTAL);

		#if BUILD_FOR_INTERRUPT_CONTROL
			#include "LOOP_EXIT_CONDITIONS3.H"
		#else
			do
				{
				
				#include "LOOP_EXIT_CONDITIONS3.H"

				}while ((int)RD_PV_POS < next_pos);  // Wait for next sample 
		#endif

		if (--accel == 0) // Finished accel, set up for closed loop velocity
			{
			#if BUILD_FOR_INTERRUPT_CONTROL
				DISARM_X1_PULSE_CONT_INTERRUPT;
			#endif
			first_loop=1;
			zone=6;
			}
		else
			{
			#if BUILD_FOR_INTERRUPT_CONTROL 
				return(REENTER_ON_INTERRUPT);
			#else // Polled control
				goto ol_accel_loop;
			#endif
			}
		}

	if (zone == 6) // Open loop control
		{
		if (first_loop)  // There's only one loop here
			{
			#if BUILD_FOR_INTERRUPT_CONTROL
				first_loop=0;
				convert_uS_to_timer(50000, &tempu);	// 50 mS
			
				// Set up for return on interrupt to zone 6 here.  This is done for ZSPEED
				// checking.  If we did not periodically come back, we could not detect
				// ZSPEED in open loop.
				halfword_ptr = (unsigned short int *)&tempu;
			
				// We only need to do this stuff once since this is an OL step.
					  			
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 6);
			#endif

			#if VEL_EXE_DEBUG
				printf("vel blk exe-zone 6, Outputting open loop DAC setting = %d\n\n", commanded_raw_dac);
			#endif

			out_dac(DAC_OUTPUT_CHANNEL, commanded_raw_dac, RAW_VAL, ABSOLUTE);
			}

		
		//#if VEL_EXE_DEBUG
		//	printf("vel blk exe-zone 6, ol step, blowing on through, we're in block = %d\n\n", vtg.prog_ctr);
		//#endif

		#if BUILD_FOR_INTERRUPT_CONTROL
			// Assuming a ZSPEED has not occured, we'll just exit here and wait for another interrupt
			vtg.timer2_timeout = 0; // Not using this for anything but should clear it anyway
			#include "LOOP_EXIT_CONDITIONS3.H"
			return(REENTER_ON_INTERRUPT);
		#else // Polled control
			do  // Wait for something to terminate the block 
	    		{
	     		
	     		#include "LOOP_EXIT_CONDITIONS3.H"

	     		}while (1);  // Loop until something terminates the loop 
		#endif
   		}

	///////////////////// Block ending zones, interrupt control only  ///////////////////////////////
	if (zone == 7) // 10 second block timeout.
		{
		#if VEL_EXE_DEBUG
			printf("vel blk exe-zone 7, 10 second block timeout\n");
		#endif 
		return_val = BLOCK_TIMEOUT; // This will trigger a warning
		}

	if (zone == 8) // Ending position reached
		{
		#if VEL_EXE_DEBUG
			printf("vel blk exe-zone 8, ending position reached\n");
		#endif 
		// If vac wait and fill_test, halt plunger movement.
		if (vac_wait && fill_test && !vac_on)
			{
			#if VEL_EXE_DEBUG
				printf("Exiting, fill time abort\n");
			#endif		
			return_val = FILL_TEST_ABORT; // This will trigger a warning
			}
		else
			{
			// Block's done
			#if VEL_EXE_DEBUG
				printf("Exiting, position met, actpos=%d, EP setpt = %d\n", RD_PV_POS, position);
			#endif

			do_set_output=1;		
			}
		}

	if (zone == 9) // Vac abort, moved 3" past ending position.
		{
		#if VEL_EXE_DEBUG
			printf("vel blk exe-zone 9, VACUUM ERROR, Exiting, cyl. exceeded EP+240, pos=%d\n", RD_PV_POS);
		#endif	
		return_val = VAC_ABORT_POS_TOO_LONG; // This will trigger a warning
		}

	if (zone == WHILE_CONDITIONAL_FAILED) // Conditional WHILE interrupt
		{
		#if VEL_EXE_DEBUG
			printf("vel blk exe-zone WHILE_CONDITIONAL_FAILED\n");
		#endif 
		// If vac_wait && !vac_on, this interrupt has already been disarmed.
		// In the unlikely event that it triggers anyway, we'll ignore it here.
		if (vac_wait && !vac_on) // Skip conditional checking
			{
			#if VEL_EXE_DEBUG
				printf("vel blk exe-zone WHILE_CONDITIONAL_FAILED, (vac_wait && !vac_on), skipping abort, will cont.\n");
			#endif

			DISARM_IO1_CONT_INTERRUPT;
			return(REENTER_ON_INTERRUPT); // Wait for something else to interrupt us.
			}
		else  do_set_output=1; // We'll exit here
		}
	
	
	ext: ;

	// Handle any outputs to set/clr now. 
	if (!return_val && do_set_output) 
		{
		if (SET_OUT_DUR) INVERT_CONT_OUTPUT_NOW; // Set output back to its original state.
		else if (SET_OUT_AT_END) SET_CLR_CONT_OUTPUT_NOW;
		}

	#if BUILD_FOR_INTERRUPT_CONTROL
		// Disarm any potentially pending interrupts we may have used executing this block.
		DISARM_BLK_TIMEOUT_CONT_INTERRUPT;
		DISARM_X1_PULSE_CONT_INTERRUPT;
		DISARM_GP_POS1_CONT_INTERRUPT;
		DISARM_TIMER2_CONT_INTERRUPT;
		DISARM_IO1_CONT_INTERRUPT; // Conditional checking may have armed this interrupt
	#else // Polled control
		STOP_TIMER2;
	#endif

	if (saved_execution_type==PROGRAM)
		{
		vtg.prog_ctr++; // Increment prog ctr.		
		}

	return(return_val);
	}




int jump_block_compile(char *str_to_compile)
	{
	jb *jump_block=0;
	int return_val = 0, stepnum, x, tempi, rel=0, immed_exe=0, type_normal=0;
	volatile char *ptr=0;


	#if JUMP_COMPILE_DEBUG
		printf("string to compile = %s\n", str_to_compile);		
	#endif	
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
	#if JUMP_COMPILE_DEBUG
		printf("Comment has been stripped - see %s\n", str_to_compile);
	#endif	
	if (str_to_compile[0] == 'J' || str_to_compile[0] == '-') //Command for immediate execution, first compile.
		{
		jump_block = (jb *)&vtg.control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		immed_exe = 1;
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		jump_block = (jb *)&vtg.control_program[stepnum][0];
		}

	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		vtg.control_program[stepnum][x] = 0;
		}

#if JUMP_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	jump_block->blk_id = JUMP;
	//jump_block->blk_to_execute = (control_block_function)jump_block_exe;
	
#if JUMP_COMPILE_DEBUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x, ID = %d\n", x, vtg.control_program[stepnum][x], JUMP);
#endif

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		// Always cast the update block structure when calling block conditional.
		// For all block types, the conditional, set output stuff will be the 
		// same.  The update block structure in this case is being used as a 
		// template that is good for all block types.
		return_val = block_conditional_compile((c_block)jump_block, (char *)ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 
	
#if JUMP_COMPILE_DEBUG	
	printf("Got past block_conditional_compile()\n");
#endif	

	//At this point, any comment and conditional/set output items have been
	//removed.   
	if ((ptr = (char *)strchr(str_to_compile, 'J')) == NULL) {return_val = 1; goto clr_id;}	
	else
		{
		if (*(ptr-1) == '-')
			{
			jump_block->jb_indicator_bits |= jb_rel_abs_specifier_mask;
			rel = 1;
			}
		#if JUMP_COMPILE_DEBUG
			printf("Found rel jump %s\n", ptr-1);
		#endif	
		ptr++; // Now point to the jump type
		tempi = atoi(ptr);
		if (tempi < 0 || tempi > 2) {return_val = 1; goto clr_id;} // Jump type out of range
		else if (tempi == 0) {jump_block->jb_indicator_bits |= jb_type_normal_mask; type_normal=1;}
		else if (tempi == 1) jump_block->jb_indicator_bits |= jb_type_sub_mask;
		else if (tempi == 2) {jump_block->jb_indicator_bits |= jb_type_return_mask; goto ext;}
		#if JUMP_COMPILE_DEBUG
			printf("Jump type = %d\n", tempi);
		#endif	
		ptr+=2;
		if (rel)
			{
			if (type_normal)
				{		
				if (*ptr == '-') ptr++; // jb_rel_dir_bit already zero.
				else if (*ptr == '+') {jump_block->jb_indicator_bits |= jb_rel_dir_mask; ptr++;}
				else jump_block->jb_indicator_bits |= jb_rel_dir_mask; // No sign, must be a forward jump, don't increment ptr
				}
			else
				{
				// Type subroutine or return from subroutine cannot be a relative type	
				return_val=1;
				goto clr_id;	
				}
			}
			
		// Now get the block to jump to or if rel, the # of steps to move
		switch (*ptr) 
			{
			case '0': // Integer constant
			case '1': 
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// First get integer and save to tempi
				if ((tempi = atoi(ptr)) > MAX_STEP) {return_val = 1; goto clr_id;}
				jump_block->jb_indicator_bits |= jb_jump_const_mask;	
				jump_block->jump_df = tempi;
				break;
			
			case 'V': //Variable
				ptr++;
				if ((tempi = atoi(ptr)) > MAX_VAR) {return_val = 1; goto clr_id;}
				jump_block->jb_indicator_bits |= jb_jump_var_mask;	
				jump_block->jump_df = tempi;
				break;

			case '$': // Jump to the current block
				if (type_normal)
					{
					jump_block->jb_indicator_bits |= jb_jump_const_mask; // Treat this as a constant
					if (rel) jump_block->jump_df = 0;   // Stay at current step
					else jump_block->jump_df = stepnum; // Ditto
					}
				else
					{
					// Type subroutine or return from subroutine cannot jump to current block	
					return_val=1;
					goto clr_id;	
					}
				break;
				
			default: //Error, valid data type not found
				return_val = 1;
				goto clr_id;
				break;

			}	
		}


	clr_id: ;
	if (return_val != 0) // Error of some kind, clear the block ID
		//jump_block->blk_to_execute = NULL;
		jump_block->blk_id = NULL;
		
	// Now print the block for debug
	#if JUMP_COMPILE_DEBUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, vtg.control_program[stepnum][x]);	
	#endif		

	ext: ;
	
	if (immed_exe && return_val==0)
		{
		// If the control program is running, the IMMED block will be initiated
		// by the control supervisor when it finishes running its current block.
		// Otherwise we'll initiate the block here since there should not be
		// any danger of recursion.
		if (!PROGRAM_RUNNING)
			{
			// Execute the block we just compiled. 
			jump_block_exe((c_block)jump_block, IMMED, 1);  
			}
		}

	return (return_val);		
	}



// Execute jump block
int jump_block_exe(c_block control_block, int type_of_execution, int zone)
	{
	static int saved_execution_type, dont_increment_prog_ctr, return_val, cond_result;
	static int do_set_output;
	static unsigned int where_to_jump;
	static jb *block=0;
	static c_block saved_control_block;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;

	if (zone == 1)
		{
		saved_execution_type = type_of_execution;
		saved_control_block = control_block;
		
		block = (jb *)control_block;
			
		cond_result = block_conditional_exe(control_block);
		return_val=0;
		dont_increment_prog_ctr=0;
		where_to_jump=0;
		do_set_output=0;

		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
			else {} // vtg.prog_ctr will be incremented. 	
			return_val=cond_result;	
			} 
		else if (cond_result < 20) // OK to proceed but there may be some conditions.
			{
			// This is the main body of the block	
			
			#if JUMP_EXE_DEBUG
				printf("Jump blk exe-zone 1, Made it to main body of jump block\n");
			#endif

			// Set/clr any outputs here
			if (SET_OUT_BEG_OR_DUR) SET_CLR_CONT_OUTPUT_NOW;

			if (block->jb_indicator_bits & jb_type_return_mask) // Return from subroutine
   				{
   				// If vtg.stack_ptr == 0, can't be a return from subroutine.
   				if (vtg.stack_ptr == 0) 
					{
					if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
					else {} // vtg.prog_ctr will be incremented.
					}
   				else
					{
					vtg.prog_ctr = pop();
					dont_increment_prog_ctr=1;
					#if JUMP_EXE_DEBUG
						printf("Ret. from sub, going to blk#%d\n", vtg.prog_ctr);
					#endif
					}
   				}
			else // Must be a type normal or type subroutine
   				{
				// First get where_to_jump, var or const?
				if (block->jb_indicator_bits & jb_jump_var_mask)
					where_to_jump = vtg.v[block->jump_df];
				else // Assume constant
					where_to_jump = block->jump_df;
					
		
   				if (block->jb_indicator_bits & jb_type_normal_mask)
					{
					if (block->jb_indicator_bits & jb_rel_abs_specifier_mask) // Relative jump forward
      					{
						if (where_to_jump==0) // Jump to current block
							{
							// Since we're staying at the same block, it is
							// possible we have a while condition.
							#if JUMP_EXE_DEBUG
								printf("We're jumping to the current block (type relative)\n");
							#endif
							
						   #if BUILD_FOR_INTERRUPT_CONTROL
							if (cond_result == KEEP_GOING_INTERRUPT_ARMED) return(REENTER_ON_INTERRUPT);
						   #else // Polled control
							if (cond_result == KEEP_GOING_CALL_BACK_DURING)
								{
								while (block_conditional_exe(control_block) == KEEP_GOING_CALL_BACK_DURING)
									{
									// Could do something here later	
									}
								}
						   #endif
							else
								{	
								// Otherwise a jump to the current block without any conditions
								// would be a forever loop, we won't let it happen.
								// Increment the prog_ctr and move on.
								if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
								else {} // vtg.prog_ctr will be incremented 	
								}
							}
						else // We're actually relative jumping somewhere
							{	
							if (block->jb_indicator_bits & jb_rel_dir_mask) // Forward relative jump
								{
								vtg.prog_ctr += where_to_jump;	
								}
							else // Backward relative jump
								{
								vtg.prog_ctr -= where_to_jump;	
								}
							dont_increment_prog_ctr=1;	
							}
						}
					else  // Absolute jump
						{
						if (vtg.prog_ctr == where_to_jump) // We're jumping nowhere
							{
							// Since we're staying at the same block, it is
							// possible we have a while condition.
							#if JUMP_EXE_DEBUG
								printf("We're jumping to the current block (type normal)\n");
							#endif

						   #if BUILD_FOR_INTERRUPT_CONTROL
							if (cond_result == KEEP_GOING_INTERRUPT_ARMED) return(REENTER_ON_INTERRUPT);
						   #else // Polled control
							if (cond_result == KEEP_GOING_CALL_BACK_DURING)
								{
								while (block_conditional_exe(control_block) == KEEP_GOING_CALL_BACK_DURING)
									{
									// Could do something here later	
									}
								}
						   #endif
							else	 
								{
								// Otherwise a jump to the current block without any conditions
								// would be a forever loop, we won't let it happen.
								// Increment the prog_ctr and move on.	
								if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
								else {} // vtg.prog_ctr will be incremented. 		
								}
							}
						else // We're jumping somewhere 
							{		
							vtg.prog_ctr = where_to_jump;	
							dont_increment_prog_ctr=1;
							#if JUMP_EXE_DEBUG
								printf("Abs jump to %d\n", vtg.prog_ctr);
							#endif
							}
						}		
					}
				else if (block->jb_indicator_bits & jb_type_sub_mask) // Jump to subroutine 
					{
					if (vtg.prog_ctr == where_to_jump) // Can't jump to sub that is the current block!
						if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
						else {} // Otherwise vtg.prog_ctr will be incremented
					else
						{
						if (saved_execution_type==IMMED)
							{		
							// If IMMED, push the current block since that is where
							// we want to come back to in this case.	
							if(push(vtg.prog_ctr)) {return_val=STACK_OVERFLOW; goto ext;}
							#if JUMP_EXE_DEBUG
								printf("Blk %d jmp to sub IMMED, pushing pc=%d, ", vtg.prog_ctr, vtg.prog_ctr);
							#endif
							}
						else
							{
							// Otherwise, if executing the control program, 
							// pust the next block.
							if(push(vtg.prog_ctr+1)) {return_val=STACK_OVERFLOW; goto ext;} 
							#if JUMP_EXE_DEBUG
								printf("Blk %d jmp to sub PROGRAM, pushing pc=%d, ", vtg.prog_ctr, vtg.prog_ctr+1);
							#endif	
							}	
						vtg.prog_ctr = where_to_jump;
						dont_increment_prog_ctr=1;
						#if JUMP_EXE_DEBUG
							printf("jmpin to blk %d\n", vtg.prog_ctr);
						#endif	
						}
					}
   				}

			do_set_output=1;
			}
		else if (cond_result == COND_NOT_MET) // Nothing more to do 
			{
			if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
			else {} // vtg.prog_ctr will be incremented. 	
			}
			
		else  // This should not happen - if it does, deal with it here.
			{
			if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
			else {} // vtg.prog_ctr will be incremented. 
			return(BLOCK_EXECUTION_ERROR);	// This will trigger a fatal error
			}
		} // zone 1
	
	if (zone == 2) // No interrupts to zone 2 at this time, for future expansion.
		{
		#if JUMP_EXE_DEBUG
			printf("Jump blk exe-Zone 2\n");
		#endif

		do_set_output=1;
		}

	if (zone == WHILE_CONDITIONAL_FAILED) // Interrupt control only
		{
		#if JUMP_EXE_DEBUG
			printf("Jump blk exe-WHILE_CONDITIONAL_FAILED, exiting block\n");
		#endif

		   // Condition no longer met.  Any outputs to
		  // set_clr will have already been taken care
		 // of by block_conditional() therfore we jump
		// past the AT_END checking.
		if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
		else {} // vtg.prog_ctr will be incremented.

		do_set_output=1; 
		}

	ext: ;

	// Handle any outputs to set/clr now.
	if (do_set_output)
		{
		if (SET_OUT_DUR) INVERT_CONT_OUTPUT_NOW; // Set output back to its original state.
		else if (SET_OUT_AT_END) SET_CLR_CONT_OUTPUT_NOW; 
		}

	#if BUILD_FOR_INTERRUPT_CONTROL
		// Disarm any potentially pending interrupts we may have used executing this block.
		DISARM_IO1_CONT_INTERRUPT; // Conditional checking may have armed this interrupt
	#endif

	if (!dont_increment_prog_ctr)
		vtg.prog_ctr++; // Move on to the next block.
		
	
	// Before we exit, do a sanity check on prog_ctr.
	if (vtg.prog_ctr>MAX_STEP)
		{
		// Something's wrong, stop program execution.	
		vtg.prog_ctr = 0xffffffff; // Stop
		return_val = PROG_CTR_OUT_OF_RANGE;
		}
	
	#ifdef JUMP_BLK_EXE_DEBUG	
		printf("jump block exe-vtg.prog_ctr=%d\n", vtg.prog_ctr);
	#endif

	return(return_val);
	}




int update_block_compile(char *str_to_compile)
	{
	ub *update_block=0;
	int return_val = 0, stepnum, x, tempi, item_being_scanned, immed_exe=0;
	int dac_accel_was_present=0, itu_was_dac_command=0;
	volatile char *ptr=0;
 

	#if UPDATE_COMPILE_DEBUG
		printf("string to compile = %s\n", str_to_compile);		
	#endif	
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
	#if UPDATE_COMPILE_DEBUG
		printf("Comment has been stripped - see %s\n", str_to_compile);
	#endif	
	if (str_to_compile[0] == 'U') //Command for immediate execution, first compile.
		{
		immed_exe=1;
		update_block = (ub *)&vtg.control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		update_block = (ub *)&vtg.control_program[stepnum][0];
		}

	//Clear all 10 update block locations
	for (x = 0; x<=9; x++)
		{
		vtg.control_program[stepnum][x] = 0;
		}

#if UPDATE_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	update_block->blk_id = UPDATE;
	//update_block->blk_to_execute = (control_block_function)update_block_exe;
	
#if UPDATE_COMPILE_DEBUG	
	printf("Update ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, vtg.control_program[stepnum][x]);
#endif

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		return_val = block_conditional_compile((c_block)update_block, (char *)ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 

#if UPDATE_COMPILE_DEBUG	
	printf("Got past block_conditional_compile()\n");
#endif

	//At this point, any comment and conditional/set output items have been
	//removed.   
		
	#define ITU 0 // Item to update
	#define OP1 1 // 1st. operand
	#define OP2 2 // 2nd. operand
	
	// A for loop will loop between the 3 possible choices of items in
	// a update block, Item to update, 1st. operand or 2nd. operand.
	// The cases in the switch statement are all of the possible data
	// types the scanned items can be.  If a paticular data type is not
	// possible for the item being scanned, an error is generated.
	for (item_being_scanned=ITU; item_being_scanned<=OP2; item_being_scanned++)
		{
		// Move ptr to point to correct part of string based on item being scanned
		if (item_being_scanned == ITU)
			{
			if ((ptr = (char *)strchr(str_to_compile, 'U')) == NULL) {return_val = 1; goto clr_id;}	
			else ptr++;
			}
		else if (item_being_scanned==OP1)
			{
			if ((ptr = (char *)strchr(str_to_compile, '=')) == NULL) {return_val = 1; goto clr_id;} 
			else ptr++;	
			}
		//else if (item_being_scanned==OP2);
			// ptr should already point 1st. char after the operator
			// if an operator was found.	
			
		switch (*ptr) 
			{
			case '@': // Incremental DAC command, can only be op1 and ITU must have been update DAC
				if ((item_being_scanned == OP1) && itu_was_dac_command && !dac_accel_was_present) // For now, can't do this if accel was specified.
					{
					update_block->ub_indicator_bits |= ub_dac_increment_mask;
					ptr++;
					
					if (*ptr == 'V') goto variable;
					else if (*ptr=='+' || *ptr=='-' || (*ptr>='0' && *ptr<='9')) goto constant; // No hex values allowed here
					else {return_val = 1; goto clr_id;}
					}
				else {return_val = 1; goto clr_id;}
				
		constant: ;
			case '+': // Integer constant
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'h':
			case 'H':
				if (item_being_scanned == ITU) {return_val = 1; goto clr_id;} // Item to update cannot be a constant	
				else // Only OP1 or OP2 can be constants
					{
					// First get integer and save to tempi
					if (*ptr == 'h' || *ptr == 'H') tempi = htoi((char *)ptr+1);
					else tempi = atoi(ptr);
				
					if (item_being_scanned == OP1) 
						{
						update_block->ub_indicator_bits |= ub_op1_const_mask;
						update_block->op1_df = tempi;
						}
					else if (item_being_scanned == OP2) 
						{
						update_block->ub_indicator_bits |= ub_op2_const_mask;
						update_block->op2_df = tempi;
						}
					}
				break;
			
		variable: ;
			case 'V': //Variable
				// ITU, OP1 and OP2 can all be a variable.
				ptr++;
				if ((tempi = atoi(ptr)) > MAX_VAR) {return_val = 1; goto clr_id;}
			
				if (item_being_scanned == ITU)
					{
					update_block->ub_indicator_bits |= ub_itu_var_mask;
					update_block->itu_df = tempi;
					}
				else if (item_being_scanned == OP1)
					{
					update_block->ub_indicator_bits |= ub_op1_var_mask;
					update_block->op1_df = tempi; 	
					}
				else if (item_being_scanned == OP2)
					{
					update_block->ub_indicator_bits |= ub_op2_var_mask;
					update_block->op2_df = tempi; 	
					}		
				break;

			case 'A': //Axis position
				// OP2 cannot be axis position
				if (item_being_scanned == OP2) {return_val = 1; goto clr_id;}
				else
					{
					ptr++;
					tempi = atoi(ptr); 
					if (tempi < 1 || tempi > NUM_OF_AXES) {return_val = 1; goto clr_id;} 	
				
					if (item_being_scanned == ITU)
						{
						update_block->ub_indicator_bits |= ub_itu_pos_mask;
						update_block->itu_df = tempi;	
						}
					else if (item_being_scanned == OP1)
						{
						update_block->ub_indicator_bits |= ub_op1_pos_mask;
						update_block->op1_df = tempi; 	
						}
					}
				break;
				
			case 'C': //Command
				// Only ITU can be a command
				if (item_being_scanned == OP1 || item_being_scanned == OP2)
					{return_val = 1; goto clr_id;}
				else
					{
					ptr++;
					if ((tempi = atoi(ptr)) > MAX_CMD) {return_val = 1; goto clr_id;}	
					update_block->ub_indicator_bits |= ub_itu_command_mask;
					update_block->itu_df = tempi; 
					}
				break;

			case 'D': //DAC output channel
				// OP2 cannot be a DAC channel
				if (item_being_scanned == OP2) {return_val = 1; goto clr_id;}	
				else
					{
					ptr++;
					tempi = atoi(ptr);
					if (tempi < 1 || tempi > MAX_DAC_CH) {return_val = 1; goto clr_id;}	
				
					if (item_being_scanned == ITU)
						{
						update_block->ub_indicator_bits |= ub_itu_dac_out_mask;
						update_block->itu_df = tempi;
						itu_was_dac_command=1; // Flag for incremental DAC function
						// Now check for accel
						ptr++; 
						if (*ptr == 'A') //Accel specified
							{
							dac_accel_was_present=1;
							ptr += 2; //Get past the "AC"
							if (*ptr == 'V') //Variable
								{
								ptr++;
								tempi = atoi(ptr);
								if (tempi<0 || tempi>MAX_VAR) {return_val = 1; goto clr_id;}
								else
									{
									update_block->ub_indicator_bits |= ub_vramp_var_mask;
									update_block->vramp_df = tempi;
									}
								}
							else //Constant
								{
								update_block->ub_indicator_bits |= ub_vramp_const_mask;
								if (*ptr == '+' || (*ptr >= '0' && *ptr <= '9')) 
									update_block->vramp_df = atoi(ptr);
								else if (*ptr == 'h' || *ptr == 'H') //hex
									update_block->vramp_df = htoi((char *)ptr++);
								else {return_val = 1; goto clr_id;} //Syntax error
								}
							} 	
						}
					else if (item_being_scanned == OP1)
						{
						update_block->ub_indicator_bits |= ub_op1_dac_mask;
						update_block->op1_df = tempi; 	
						}	
					}
				break;
			
			case 'N': //Analog input channel
				// Only OP1 can be an ADC input channel
				if (item_being_scanned == ITU || item_being_scanned == OP2)
					{return_val = 1; goto clr_id;}
				else
					{	
					ptr++;
					tempi = atoi(ptr);

					// Only channels 1 - 8 are allowed.  These are the 8 dynamic channels
					// selected by the analog mode, from 1 - 5 as defined in V429 - not
					// necessarily physical channels 1 - 8 unless analog mode is 1.
					//if (tempi<1 || tempi>MAX_ADC_CH) {return_val = 1; goto clr_id;}
					if (tempi<1 || tempi>8) {return_val = 1; goto clr_id;}

					if (item_being_scanned == OP1)
						{
						update_block->ub_indicator_bits |= ub_op1_adc_mask;
						update_block->op1_df = tempi; 	
						}
					}
				break;

			case 'F': //Velocity
				// Only OP1 can be velocity
				if (item_being_scanned == ITU || item_being_scanned == OP2)
					{return_val = 1; goto clr_id;}
				else
					{	
					ptr++;
					tempi = atoi(ptr);
					if (tempi<1 || tempi>NUM_OF_AXES) {return_val = 1; goto clr_id;}
				
					if (item_being_scanned == OP1)
						{
						update_block->ub_indicator_bits |= ub_op1_vel_mask;
						update_block->op1_df = tempi;	
						}
					}
				break;
		
			default: //Error, valid data type not found
				return_val = 1;
				goto clr_id;
				break;

			}
		
		// Look for an operator if 1st. operand was parsed this loop
		if (item_being_scanned == OP1)
			{
			ptr++; // Increment so that if a sign is present, it's not confused with an operator
			do
				{
				if (*ptr == NULL) break;
				else if (*ptr == '+') {update_block->ub_indicator_bits |= ub_add_mask; break;}
				else if (*ptr == '-') {update_block->ub_indicator_bits |= ub_sub_mask; break;}
				else if (*ptr == '*') {update_block->ub_indicator_bits |= ub_mul_mask; break;}
				else if (*ptr == '/') {update_block->ub_indicator_bits |= ub_div_mask; break;}
				else if (*ptr == '&') {update_block->ub_indicator_bits |= ub_and_mask; break;}
				else if (*ptr == '|') {update_block->ub_indicator_bits |= ub_or_mask; break;}
				else if (*ptr == '^') {update_block->ub_indicator_bits |= ub_xor_mask; break;}
				ptr++;
				} while (1);

			if (*ptr == NULL) break; // Quit for loop
			else ptr++; // Move past the operator	
			}
		}// For loop


	clr_id: ;
	if (return_val != 0) // Error of some kind, clear the block ID
		//update_block->blk_to_execute = NULL;
		update_block->blk_id = NULL;
		
	// Now print the block for debug
	#if UPDATE_COMPILE_DEBUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, vtg.control_program[stepnum][x]);	
	#endif
	
	ext: ;

	if (immed_exe && return_val==0)
		{
		// If the control program is running, the IMMED block will be initiated
		// by the control supervisor when it finishes running its current block.
		// Otherwise we'll initiate the block here since there should not be
		// any danger of recursion.
		if (!PROGRAM_RUNNING)
			{
			// Execute the block we just compiled. 
			update_block_exe((c_block)update_block, IMMED, 1); 
			#if UPDATE_COMPILE_DEBUG	
				printf("UPDATE compile, IMMED exe, finished calling update_block_exe()\n");
			#endif 
			}
		}
	
	return (return_val);
	}



// Execute update block
int update_block_exe(c_block control_block, int type_of_execution, int zone)
	{
	static int return_val, op1, op2, op2_present, temp_itu, axis_num;
	static int final_dac_voltage, initial_dac_voltage, current_dac_voltage, ramp;
	static int saved_execution_type, cond_result, first_loop, do_set_output;
	static unsigned int ramp_time;
	static ub *block;
	static c_block saved_control_block;
	volatile unsigned short int *halfword_ptr;
	//vp_pvu *pv1 = va_pv1;
	np_timer *timer2 = na_timer2;
	np_pio *misc_out_pio = na_misc_outs;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;

	if (zone == 1)
		{
		saved_execution_type = type_of_execution;
		saved_control_block = control_block;

		block = (ub *)control_block;
			
		cond_result = block_conditional_exe(control_block); 
		do_set_output=0;
		return_val=0;

		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			#if UPDATE_EXE_DEBUG
				printf("Update block exe-zone 1, executive error\n");
			#endif
			return_val=cond_result;	
			} 
		else if (cond_result < 20) // OK to proceed but there may be some conditions.
			{
			// This is the main body of the update block.
			#if UPDATE_EXE_DEBUG
				printf("Update block exe-zone 1, made it to the main body of the block, type of exe is %d\n", saved_execution_type);
			#endif
			
			// Initialize
			first_loop=1;		
			op2=0;
			op2_present=1;
			temp_itu=0;
			axis_num=1;	
			ramp=0;

			// Set/clr any outputs here
			if (SET_OUT_BEG_OR_DUR) SET_CLR_CONT_OUTPUT_NOW;
			
			// First get the 1st. operand.
			if (block->ub_indicator_bits & ub_op1_const_mask)
				{
				op1 = block->op1_df;	
				}
			else if (block->ub_indicator_bits & ub_op1_var_mask)
				{
				op1 = vtg.v[block->op1_df];
				}
			else if (block->ub_indicator_bits & ub_op1_adc_mask)
				{
				// op1 will be voltage X 1000.
				while (convert_ana_ch_from_dynamic(block->op1_df, VOLTAGE_X_1000, &op1)); // Keep trying until we get it
				}
			else if (block->ub_indicator_bits & ub_op1_dac_mask)
				{
				get_dac(block->op1_df, VOLTAGE_X_1000, &op1);
				}
			else if (block->ub_indicator_bits & ub_op1_pos_mask)
				{
				axis_num=block->op1_df;
				op1 = RD_PV_POS; 
				}	
			else if (block->ub_indicator_bits & ub_op1_vel_mask)
				{
				// Convert velocity to X1 pulses/sec	 
				axis_num=block->op1_df;
				op1 = axis[axis_num].vel_ctr_freq/RD_PV_VEL;
				}
			else 
				{
				#if UPDATE_EXE_DEBUG
					printf("Update block EXE, ERROR, no op1\n");
				#endif
				return_val = EXE_FAULT_IND_BITS;
				goto ext;
				}
					
			#if UPDATE_EXE_DEBUG
				printf("Update block op1=%d\n", op1);
			#endif

			// now get 2nd. operand
			if (block->ub_indicator_bits & ub_op2_const_mask)
				{
				op2 = block->op2_df;	
				}
			else if (block->ub_indicator_bits & ub_op2_var_mask)
				{
				op2 = vtg.v[block->op2_df];
				}
			else 
				op2_present = 0; // No op2

			if (op2_present) // Must have an operator.
				{
				if (block->ub_indicator_bits & ub_add_mask)
					temp_itu = op1+op2;
				else if (block->ub_indicator_bits & ub_sub_mask)
					temp_itu = op1-op2;
				else if (block->ub_indicator_bits & ub_mul_mask)
					temp_itu = op1*op2;
				else if (block->ub_indicator_bits & ub_div_mask)
					temp_itu = op1/op2;
				else if (block->ub_indicator_bits & ub_and_mask)
					temp_itu = op1&op2;
				else if (block->ub_indicator_bits & ub_or_mask)
					temp_itu = op1|op2;
				else if (block->ub_indicator_bits & ub_xor_mask)
					temp_itu = op1^op2;			
				else // Error, deal with here.
					{
					#if UPDATE_EXE_DEBUG
						printf("Update block EXE, ERROR, op2 without an operator\n");
					#endif
					return_val = EXE_FAULT_IND_BITS;
					goto ext;
					}
				#if UPDATE_EXE_DEBUG
					printf("Update block exe-zone1, op2=%d\n", op2);
				#endif	
				}
			
			if (block->ub_indicator_bits & ub_itu_var_mask)
				{
				if (op2_present) vtg.v[block->itu_df] = temp_itu;
				else vtg.v[block->itu_df] = op1;

				do_set_output=1;
				}
			else if (block->ub_indicator_bits & ub_itu_dac_out_mask)
				{
				// Check for ramp
				if (block->ub_indicator_bits & ub_vramp_var_mask)
					ramp = vtg.v[block->vramp_df];
				else if (block->ub_indicator_bits & ub_vramp_const_mask)
					ramp = block->vramp_df;
				// If neither const nor var, ramp will be 0.		
				
				if (op2_present) final_dac_voltage = temp_itu;	
					else final_dac_voltage = op1;
				
				if (final_dac_voltage > 10000)
					{
					//do_set_output=1; // ??? Not sure about this
					goto ext; // Skip entire block.
					}

				if (ramp) zone=2;
				else
					{	
					if (block->ub_indicator_bits & ub_dac_increment_mask)
						out_dac(block->itu_df, final_dac_voltage, VOLTAGE_X_1000, INCREMENTAL);
					else
						out_dac(block->itu_df, final_dac_voltage, VOLTAGE_X_1000, ABSOLUTE);

					do_set_output=1;
					}
				}
			else if (block->ub_indicator_bits & ub_itu_pos_mask)
				{
				axis_num=block->itu_df;
				if (op2_present) {SET_PV_PLOAD(temp_itu);}	
				else {SET_PV_PLOAD(op1);}
				do_set_output=1;
				}
			else if (block->ub_indicator_bits & ub_itu_command_mask)
				{	
				if (op2_present) {}
				else {}
				do_set_output=1;
				}
			else  // Gotta have something to update since this is an update block.
				{
				#if UPDATE_EXE_DEBUG
					printf("Update block EXE, ERROR, nothing to update!!!\n");
				#endif
				return_val = EXE_FAULT_IND_BITS;
				goto ext;
				}	
			}
		else if (cond_result == COND_NOT_MET); // Nothing more to do 
			
		else  // This should not happen - if it does, deal with it here.
			{
			return(BLOCK_EXECUTION_ERROR); // This will trigger a fatal error
			}
		}

	if (zone == 2) // Ramp control
		{
		#if UPDATE_EXE_DEBUG
			printf("update blk exe-zone 2, executing ramp\n");
		#endif

		if (first_loop)
			{
			first_loop = 0;

			// Get the initial DAC voltage.	
			get_dac(block->itu_df, VOLTAGE_X_1000, &initial_dac_voltage);	
			
			if (final_dac_voltage > initial_dac_voltage); // Positive
			else if (final_dac_voltage < initial_dac_voltage) // Negative
				ramp *= -1; // Make negative
			else ramp = 0; // No change in DAC value, reset ramp.	

			if (ramp) // Check again, ramp could now be zero.
				{
				// Now get ramp time.
				if (UPDATE_BLK_RAMP_TIME < 100) ramp_time = (CLK_FREQ/1000000)*100; // Default to 100uS.
				else ramp_time = (CLK_FREQ/1000000)*UPDATE_BLK_RAMP_TIME; // Crude 
		
				current_dac_voltage = initial_dac_voltage;
				halfword_ptr = (unsigned short int *)&ramp_time;
				#if BUILD_FOR_INTERRUPT_CONTROL
					ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 2);
				#else // Polled control
					ARM_TIMER2_FOR_GP_TIMING(halfword_ptr[0], halfword_ptr[1], CONTINUOUS);
				#endif
				}
			}
	
		if (ramp)
			{
		ub_accel_loop: ;	  
			current_dac_voltage += ramp;			
			out_dac(block->itu_df, current_dac_voltage, VOLTAGE_X_1000, ABSOLUTE);	
			vtg.timer2_timeout = 0;

			if (((ramp>0) && (current_dac_voltage<final_dac_voltage))
						|| ((ramp<0) && (current_dac_voltage>final_dac_voltage)))
				{
				if (block->ub_indicator_bits & ub_dac_increment_mask)
					out_dac(block->itu_df, final_dac_voltage, VOLTAGE_X_1000, INCREMENTAL);
				else
					out_dac(block->itu_df, final_dac_voltage, VOLTAGE_X_1000, ABSOLUTE);

				do_set_output=1;
				}
		   #if BUILD_FOR_INTERRUPT_CONTROL
			else return(REENTER_ON_INTERRUPT);
		   #else // Polled control
			else
				{
				while(!vtg.timer2_timeout)
					{

					}
				goto ub_accel_loop;
				}
		   #endif
			}
		}		
	
	if (zone == WHILE_CONDITIONAL_FAILED) 
		{
		#if UPDATE_EXE_DEBUG
			printf("update blk exe-WHILE_CONDITIONAL_FAILED, exiting block\n");
		#endif

		do_set_output=1; 
		}

	ext: ;
	
	#if UPDATE_EXE_DEBUG
		printf("Update block EXE, made it to ext: ;\n");
	#endif

	if (do_set_output)
		{
		if (SET_OUT_DUR) INVERT_CONT_OUTPUT_NOW; // Set output back to its original state.
		else if (SET_OUT_AT_END) SET_CLR_CONT_OUTPUT_NOW;
		}

	#if BUILD_FOR_INTERRUPT_CONTROL
		DISARM_TIMER2_CONT_INTERRUPT;
		DISARM_IO1_CONT_INTERRUPT; // Conditional checking may have armed this interrupt
	#else // Polled control
		STOP_TIMER2;
	#endif

	if (saved_execution_type==PROGRAM)
		{
		vtg.prog_ctr++; // Increment prog ctr.		
		}
	else // If immed. exe., set timer2 as timed out as some other block may be executing that
		 // is depending on the timer.  If we exit here with the timer2 stopped and
		 // vtg.timer2_timeout == 0, that block will hang.
		{
		vtg.timer2_timeout = 1;
		}
	
	return(return_val);
	}






// Use update block template for any block type calling this function.
// The conditional / set output section (words 7-9) for any block type 
// will be the same.
int block_conditional_compile(c_block control_block, char *condstr)
	{
	int return_val = 0, x;
	volatile char *ptr=0;
	ub *blkptr; // Use update block as a generic type

	ptr = condstr;
	blkptr = (ub *)control_block;

	#if COND_COMPILE_DEBUG
		printf("condstr is %s\n", ptr);	
	#endif	
	if (*ptr == 'I' || *ptr == 'W')
		{
		if (*ptr == 'I') blkptr->cond_indicator_bits |= if_mask; 
		else blkptr->cond_indicator_bits |= while_mask; 
		ptr++;
		if (*ptr == '1' || *ptr == '0') //Basic form conditional check
			{
			blkptr->cond_indicator_bits |= op1_discrete_io_mask;
			if (*ptr == '1') blkptr->cond_indicator_bits |= input_on_mask;
			else blkptr->cond_indicator_bits |= input_off_mask;	
			
			ptr += 2;
			if (*ptr == 'V')
			 	{
				ptr++;
				blkptr->cond_indicator_bits |= op1_var_mask;
				// If both discrete_io_indicator and var_indicators are
				// set, then the variable must contain a discrete I/O#, 
				x = atoi(ptr); if (x<0 || x>MAX_VAR) {return_val = 1; goto ext;} //Variable number out of range
				blkptr->cond_op1_df = (unsigned short int)x;
				}
			else 
				{
				// discrete_io_indicator already set
				x = atoi(ptr); if (x<0 || x>MAX_DISCRETE_IO_NUM) {return_val = 1; goto ext;} //I/O number out of range
				blkptr->cond_op1_df = (unsigned short int)x;	
				}
			}
		else //Advanced form conditional check
			{
			blkptr->cond_indicator_bits |= advanced_testing_mask;
						
			switch (*ptr) //1st. operand
				{
				case 'V': //Variable
					blkptr->cond_indicator_bits |= op1_var_mask;
					ptr++;
					x = atoi(ptr); 
					if (x <0 || x>MAX_VAR) {return_val = 1; goto ext;}
					blkptr->cond_op1_df = (unsigned short int)x;
	
					// Increment ptr to operator.
					if (x<10) ptr++;
					else if (x<100) ptr+=2;
					else ptr+=3; // Must be over 100.
					#if COND_COMPILE_DEBUG
						printf("1st. op was a variable, ptr now == %s\n", ptr);
					#endif	
					break;
					
				case 'A': //Axis position
					blkptr->cond_indicator_bits |= op1_pos_mask;
					ptr++;
					x = atoi(ptr);
					if (x<1 || x>NUM_OF_AXES) {return_val = 1; goto ext;} //Axis #
					blkptr->cond_op1_df = (unsigned short int)x;

					// Increment ptr to operator.
					ptr++;
					break;
					
				case 'N': //Analog input channel
					blkptr->cond_indicator_bits |= op1_adc_mask;
					ptr++;
					x = atoi(ptr);
									
					// Only channels 1 - 8 are allowed.  These are the 8 dynamic channels
					// selected by the analog mode, from 1 - 5 as defined in V429 - not
					// necessarily physical channels 1 - 8 unless analog mode is 1.
					//if (x<1 || x>MAX_ADC_CH) {return_val = 1; goto ext;}
					if (x<1 || x>8) {return_val = 1; goto ext;}

					blkptr->cond_op1_df = (unsigned short int)x;

					// Increment ptr to operator.
					if (x<10) ptr++;
					else ptr+=2;
					break;

				case 'D': //DAC output channel
					blkptr->cond_indicator_bits |= op1_dac_mask;
					ptr++;
					x = atoi(ptr);
					if (x<1 || x>MAX_DAC_CH) {return_val = 1; goto ext;}
					blkptr->cond_op1_df = (unsigned short int)x;

					// Increment ptr to operator.
					ptr++;
					break;

				case 'F': //Velocity
					blkptr->cond_indicator_bits |= op1_vel_mask;
					ptr++;
					x = atoi(ptr);
					if (x<1 || x>NUM_OF_AXES) {return_val = 1; goto ext;} //Axis #
					blkptr->cond_op1_df = (unsigned short int)x; 

					// Increment ptr to operator.
					ptr++;
					break;

				default: //Error
					return_val = 1;
					goto ext;
					break;
				}
				
			//Now look for an operator
			do
				{
				if (*ptr == NULL) {return_val = 1; goto ext; break;}
				else if (*ptr == '>')
					{
					ptr++; 
					if (*ptr == '=') // >= operator 
						{
						ptr++; //Move beyond the operator;
						blkptr->cond_indicator_bits |= gtr_than_eq_to_mask;
						break;
						}
					else {blkptr->cond_indicator_bits |= gtr_than_mask; break;} // > operator	
					} 
				else if (*ptr == '<')
					{
					ptr++; 
					if (*ptr == '=') // <= operator 
						{
						ptr++; //Move beyond the operator;
						blkptr->cond_indicator_bits |= less_than_eq_to_mask;
						break;
						}
					else {blkptr->cond_indicator_bits |= less_than_mask; break;} // < operator	
					} 
				else if (*ptr == '=') {blkptr->cond_indicator_bits |= eq_mask; ptr++; break;}
				else if (*ptr == '!') {blkptr->cond_indicator_bits |= neq_mask; ptr+=2; break;}
				//ptr++;
				} while (1);
			
			//Check if 2nd. operand a numeric constant.
			if (*ptr == '+' || *ptr == '-' || (*ptr >= '0' && *ptr <= '9')) 
				{
				blkptr->cond_indicator_bits |= op2_const_mask;
				blkptr->cond_op2_df = atoi(ptr);
				#if COND_COMPILE_DEBUG
					printf("Conditional 2nd. operand was a numeric constand %d\n", atoi(ptr)); 
				#endif	
				}
			else	
				{
				switch (*ptr) //2nd. operand
					{
					case 'h': //hex numeric constant
					case 'H':
						blkptr->cond_indicator_bits |= op2_const_mask;
						ptr++; //Get ptr past the "h" and convert hex ascii
						blkptr->cond_op2_df = htoi((char *)ptr);
						#if COND_COMPILE_DEBUG 
							printf("Conditional 2nd. operand was a hex constand %d\n", htoi((char *)ptr)); 
						#endif	
						break;

					case 'V': //Variable
						blkptr->cond_indicator_bits |= op2_var_mask;
						ptr++;
						x = atoi(ptr);
						if (x<0 || x>MAX_VAR) {return_val = 1; goto ext;}
						blkptr->cond_op2_df = x;
						#if COND_COMPILE_DEBUG
					 		printf("Conditional 2nd. operand was a variable, variable #%d\n", x); 
						#endif		
						break;
						
					default: // Syntax error
						return_val = 1;
						goto ext;
						break;	
					}
				}
			}
		// At this point, ptr should be poised to begin looking for any
		// set output items, lets see.
		#if COND_COMPILE_DEBUG
			printf("After parsing conditional stuff, ptr = %s\n", ptr);
		#endif		
		while (*ptr != NULL)
			{
			if (*ptr == 'B' || *ptr == 'D' || *ptr == 'E') break;
			else ptr++;	
			}
		} // If I or W
		
	
	// If there were not any conditional items to parse, ptr now points
	// to the beginning of the set output info if any exists.  If conditional items
	// were parsed, ptr will be incremented to the beginning of the set output
	// stuff if any exists.
	if (*ptr == 'B' || *ptr == 'D' || *ptr == 'E')
	   {
		if (*ptr == 'B') blkptr->cond_indicator_bits |= set_out_beginning_mask; 
		else if (*ptr == 'D') blkptr->cond_indicator_bits |= set_out_during_mask; 
		else if (*ptr == 'E') blkptr->cond_indicator_bits |= set_out_at_end_mask;
			
		ptr++;
		if (*ptr == '1') blkptr->cond_indicator_bits |= turn_on_turn_off_mask;
		else if (*ptr == '0'); //  turn_on_turn_off already off, nothing to do.
		else {return_val = 1; goto ext;} // Syntax error
		ptr+=2;
		if (*ptr >= '0' && *ptr <= '9')
			{
			x = atoi(ptr);
			if (x<0 || x>MAX_DISCRETE_IO_NUM) {return_val = 1; goto ext;}
			blkptr->set_out_df = (unsigned short int)x;	
			}
		else if (*ptr == 'V')
			{
			blkptr->cond_indicator_bits |= set_out_var_mask;	
			ptr++;
			x = atoi(ptr);
			if (x<0 || x>MAX_VAR) {return_val = 1; goto ext;}
			blkptr->set_out_df = (unsigned short int)x;	
			}
		else {return_val = 1; goto ext;} // Syntax error	
		}
	

	ext: ;
	
	return (return_val);
	}



// Execute conditional check
int block_conditional_exe(c_block control_block)
	{
	int cond_op1, cond_op2=0, condition=0, condition_met=0;
	int while_ = 0, axis_num=0;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	//vp_pvu *pv1 = va_pv1;
	ub *block;	// Update block used as a gerneric block type
	c_block saved_control_block;
		
	block = (ub *)control_block;
	saved_control_block = control_block; // Redundant but necessary for arming interrupts
										 // since they use "saved_control_block."
	// Check conditional.	
	if ((block->cond_indicator_bits & while_mask) ||
			(block->cond_indicator_bits & if_mask))
		{
		condition = 1;
		
		if (block->cond_indicator_bits & while_mask) while_ = 1;
			
		// Get 1st. operands.  We don't yet know what it is.  
		// The cond_indicator_bits will tell the story.
		if (block->cond_indicator_bits & op1_discrete_io_mask)
			{
			if (block->cond_indicator_bits & op1_var_mask) // discrete I/O can also be a variable.
				cond_op1 = (int)vtg.v[block->cond_op1_df];
			else		
				cond_op1 = (int)block->cond_op1_df;
			}
		else if (block->cond_indicator_bits & op1_var_mask)
			cond_op1 = (int)vtg.v[block->cond_op1_df];
		else if (block->cond_indicator_bits & op1_pos_mask) 
			{
			axis_num = block->cond_op1_df; 
			cond_op1 = RD_PV_POS; 
			}
		else if (block->cond_indicator_bits & op1_vel_mask)
			{
			// Convert velocity to X1 pulses/sec
			axis_num = block->cond_op1_df;	 
			cond_op1 = axis[axis_num].vel_ctr_freq/RD_PV_VEL;
			}
		else if (block->cond_indicator_bits & op1_dac_mask)	
			// cont_op1 will == DAC voltage X 1000.
			cond_op1 = ((vtg.dac_word[block->cond_op1_df]-8192)*10000)/8192;
		else if (block->cond_indicator_bits & op1_adc_mask)
			{
			// Since this function will not allow recursion, keep trying until we get it
			while (convert_ana_ch_from_dynamic(block->cond_op1_df, VOLTAGE_X_1000, &cond_op1));
			}
		
		// Now get 2nd. operand which can only be a constant or variable.
		if (block->cond_indicator_bits & op2_const_mask)
			cond_op2 = (int)block->cond_op2_df;
		else if (block->cond_indicator_bits & op2_var_mask)
			cond_op2 = (int)vtg.v[block->cond_op2_df];
		
		// Now that we have the operands, it will be easy to make comparisons
		// or check discrete I/O.	
					
		// First check for basic forms
		if (block->cond_indicator_bits & input_on_mask)
			{
			//if (ISW1 & (1<<(15+cond_op1))) condition_met=1;
			
			// Check if op1 is zero.  If so, this bypasses conditional
			// checking.  The block will be treated as though there were
			// not any conditions.
			if (cond_op1==0) condition=0;
			else chk_discrete_input(cond_op1, &condition_met); // condition_met will == 1 if input on
			} 
		else if (block->cond_indicator_bits & input_off_mask)
			{
			//if (!(ISW1 & (1<<(15+cond_op1)))) condition_met=1;
			
			if (cond_op1==0) condition=0; // Treat as though there are no conditions.
			else
				{
				chk_discrete_input(cond_op1, &condition_met);
				// Invert condition_met, if input was NOT on, our condition was met.
				condition_met = !condition_met;
				}
			} 
		// Now start looking at the more advanced stuff.
		else if (block->cond_indicator_bits & advanced_testing_mask)
			{
			if (block->cond_indicator_bits & eq_mask)
				{
				if (cond_op1 == cond_op2) condition_met=1; 
				}
			else if (block->cond_indicator_bits & neq_mask)
				{
				if (cond_op1 != cond_op2) condition_met=1; 
				}	
			else if (block->cond_indicator_bits & gtr_than_mask)
				{
				if (cond_op1 > cond_op2) condition_met=1; 
				}		
			else if (block->cond_indicator_bits & gtr_than_eq_to_mask)
				{
				if (cond_op1 >= cond_op2) condition_met=1; 
				}
			else if (block->cond_indicator_bits & less_than_mask)
				{
				if (cond_op1 < cond_op2) condition_met=1; 
				}
			else if (block->cond_indicator_bits & less_than_eq_to_mask)
				{
				if (cond_op1 <= cond_op2) condition_met=1; 
				}
			else
				return(EXE_FAULT_IND_BITS); // Indicator bits not properly set.							
			}
		else // Error?   
			{
			return(EXE_FAULT_IND_BITS); // Indicator bits not properly set.	
			}			
		}
	
	if (!condition) return(KEEP_GOING);
	else if (condition && condition_met)
		{
		if (while_)
			{
			#if COND_EXE_DEBUG	
				printf("while conditional, interrupt will be armed.\n");	
			#endif	
			
			// Arm the interrupt here
			if (block->cond_indicator_bits & input_on_mask) // While input on
				{
				#if BUILD_FOR_INTERRUPT_CONTROL
					ARM_IO1_CONT_INTERRUPT(cond_op1, OFF, WHILE_CONDITIONAL_FAILED); // Interrupt if input goes off
					#if COND_EXE_DEBUG	
						printf("while input on, interrupt has been armed\n");	
					#endif
				#else
					#if COND_EXE_DEBUG	
						printf("while input on\n");	
					#endif
				#endif
				} 
			else if (block->cond_indicator_bits & input_off_mask) // While input off
				{
				#if BUILD_FOR_INTERRUPT_CONTROL
					ARM_IO1_CONT_INTERRUPT(cond_op1, ON, WHILE_CONDITIONAL_FAILED); // Interrupt if input goes on
					#if COND_EXE_DEBUG	
						printf("while input off, interrupt has been armed\n");	
					#endif
				#else
					#if COND_EXE_DEBUG	
						printf("while input off\n");	
					#endif
				#endif
				}

			// A WHILE advanced will be treated as an IF.  There will be no checking during the block.
			#if BUILD_FOR_INTERRUPT_CONTROL
				return(KEEP_GOING_INTERRUPT_ARMED); 
			#else
				return(KEEP_GOING_CALL_BACK_DURING);
			#endif
			}
		else  // Must have been an IF, we're good to go, no interrupt to arm.
			{
			#if COND_EXE_DEBUG	
				printf("IF conditional, keep going.\n");	
			#endif	
			return(KEEP_GOING); // Otherwise tell caller to keep going, no need to call back.						
			}
		}
	else // Condition was not met.
		{
		#if COND_EXE_DEBUG	
			printf("Condition not met\n");	
		#endif
		return(COND_NOT_MET);	
		}		
	}




int push(unsigned int item)
{
int return_val=0;

if(vtg.stack_ptr<19)
	vtg.stack[vtg.stack_ptr++] = item;
else
	return_val=1;

return(return_val);
}


unsigned int pop(void)
{
return (vtg.stack[--vtg.stack_ptr]);
}








