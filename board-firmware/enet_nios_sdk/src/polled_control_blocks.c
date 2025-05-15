//					polled_control_blocks.c
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
#define VEL_EXE_DEBUG 0
#define EXT_LOOP_DEBUG 0

#define BEGINNING 0
#define DURING 1
#define AT_END 2

// Return values from check_conditional().
#define VAL_OUT_OF_RANGE 1
#define INVALID_PARAMETER_PASSED 2
#define EXE_FAULT_IND_BITS 5
#define CANT_CONVERT_ANA_CH 6
#define PROG_CTR_OUT_OF_RANGE 8
#define GP_EXE_ERROR 9
#define KEEP_GOING_DONT_CALL_BACK 10
#define KEEP_GOING_CALL_BACK_DURING 11
#define KEEP_GOING_CALL_BACK_AT_END 12
#define COND_NOT_MET 20




#if 0
// Framework for executing the various block types.  Jump blocks will be somewhat different.
int ??????_block_exe(unsigned int (*control_program)[10], int type_of_execution)
	{
	int return_val=0, cond_result;
	?b *?????_block=0;
	np_timer *timer2 = na_timer2;
	
	
	if (type_of_execution==IMMED) 
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
		return_val=GP_EXE_ERROR;	
		
			
	ext: ;	
	if (type_of_execution==PROGRAM) vtg.prog_ctr++; // Increment prog ctr		
	
	return(return_val);
	}
#endif



int end_block_compile(char *str_to_compile, unsigned int (*control_program)[10])
	{
	ub *end_block=0; // Use update block for template
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
	end_block = (ub *)&control_program[stepnum][0];
	
	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		control_program[stepnum][x] = 0;
		}

#if END_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	end_block->blk_id = END_;

#if END_COMPILE_DEBUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
		printf("word%d %x\n", x, control_program[stepnum][x]);
#endif

	ext: ;
	return (return_val);		
	}


// There is not an end_block_exe().  the control supervisor will directly
// stop the control program if an END type block is encountered.


int dwell_block_compile(char *str_to_compile, unsigned int (*control_program)[10])
	{
	db *dwell_block=0;
	int return_val = 0, stepnum, x, tempi, immed_exe=0;
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
		immed_exe=1;	
		dwell_block = (db *)&control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		dwell_block = (db *)&control_program[stepnum][0];
		}

	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		control_program[stepnum][x] = 0;
		}

#if DWELL_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	dwell_block->blk_id = DWELL;

#if DWELL_COMPILE_DEBUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
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
		return_val = block_conditional_compile((ub *)&control_program[stepnum][0], (char *)ptr+1);
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
			goto clr_id;
			break;

		}	
	
	
	clr_id: ;
	if (return_val != 0) // Error of some kind, clear the block ID
		dwell_block->blk_id = NULL;
			
	// Now print the block for debug
	#if DWELL_COMPILE_DEBUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, control_program[stepnum][x]);	
	#endif
	
	
	ext: ;
	if (immed_exe && return_val==0)
		{
		// Execute the block we just compiled. 
		dwell_block_exe(control_program, IMMED); 
		}
					
	return (return_val);		
	}





// Execute dwell block
int dwell_block_exe(unsigned int (*control_program)[10], int type_of_execution)
	{
	int return_val=0, cond_result;
	int input_state; // For test
	unsigned int dwell_time_mS=0, timer_load_value;
	volatile unsigned short int *halfword_ptr;
	//unsigned int time_met_counts, current_raw_vel_ctr_val;
	long initial_timer_mS;
	db *dwell_block=0;
	np_timer *timer2 = na_timer2;
	np_pio *misc_ins_pio = na_misc_ins;
	

	if (type_of_execution==IMMED) 
		dwell_block = (db *)&control_program[IMMED_EXECUTE_BLOCK][0];
	else
		dwell_block = (db *)&control_program[vtg.prog_ctr][0];
		
	cond_result = block_conditional_exe((ub *)dwell_block, BEGINNING); 
	
	if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
		{
		#if DWELL_EXE_DEBUG
			printf("Dwell blk exe-Some sort of CONDITIONAL exe error\n");
		#endif	
		return_val=cond_result;	
		} 
	else if (cond_result < 20) // OK to proceed but there may be some conditions.
		{
		#if DWELL_EXE_DEBUG
			printf("Dwell blk exe-OK to proceed, blk#%d\n", vtg.prog_ctr);
		#endif	
		// This is the main body of the dwell block	
		if (dwell_block->db_indicator_bits & db_const_mask)
			{
			dwell_time_mS = dwell_block->dwell_df;
			}
		else if (dwell_block->db_indicator_bits & db_var_mask)
			{
			dwell_time_mS = vtg.v[dwell_block->dwell_df];
			}
		else  // Can't execute, invalid indicator bit settings.
			{
			return_val=EXE_FAULT_IND_BITS;
			}	
			
		
		if (dwell_time_mS > 0)
			{
			// Timing done using nr_timer_milliseconds().
			#if 0
			initial_timer_mS = nr_timer_milliseconds();
			while((nr_timer_milliseconds() - initial_timer_mS) < dwell_time_mS)
				{
				if (cond_result == KEEP_GOING_CALL_BACK_DURING)
					{ 
					if (block_conditional_exe((ub *)dwell_block, DURING) == KEEP_GOING_CALL_BACK_DURING);		
					else goto ext; // Condition was not met.  Any outputs to
								  // set_clr will have already been taken care
								 // of by block_conditional() therfore we jump
								// past the AT_END checking.
					}	
				}
			#endif
	
		
			// Timing done with a seperate timer, timer2.
			// Commands to start/stop timer
			// timer2->np_timercontrol = 7; // Start timer, continuous mode
			// timer2->np_timercontrol = 5; // Start timer, single mode
			// timer2->np_timercontrol = 8; // Stop timer
			timer2->np_timercontrol = 8; // Stop timer
			vtg.timer2_timeout = 0;
			//timer_load_value = dwell_time_mS*(CLK_FREQ/1000); // Clk. freq./1000 == counts/mS.

			// Could easily change dwell time spec to uS
			convert_uS_to_timer((dwell_time_mS*1000), &timer_load_value);
			halfword_ptr = (unsigned short int *)&timer_load_value;
			timer2->np_timerperiodl = halfword_ptr[0];
			timer2->np_timerperiodh = halfword_ptr[1];
			timer2->np_timercontrol = 5; // Start timer for one timeout.
			#if DWELL_EXE_DEBUG
				printf("Dwell blk exe-Timer has been started, entering WHILE loop\n");
			#endif
			while (!vtg.timer2_timeout)
				{
				if (cond_result == KEEP_GOING_CALL_BACK_DURING)
					{ 
					if (block_conditional_exe((ub *)dwell_block, DURING) == KEEP_GOING_CALL_BACK_DURING);		
					else goto ext; // Condition was not met.  Any outputs to
								  // set_clr will have already been taken care
								 // of by block_conditional() therfore we jump
								// past the AT_END checking.
					}
				// For test	
				//if (SW8) {vtg.warning = TEST_WARNING; goto ext;} 
				//if (SW9) {vtg.fatal_error = TEST_FATAL_ERROR; goto ext;} 	
				}
			#if DWELL_EXE_DEBUG
				printf("Dwell blk exe-Finished timing\n");
			#endif	
			}
		
		
		// Ending items here
		// If we got to this point, the block terminated normally as opposed to
		// a condition not being met.  Therefore even if we were requested to
		// call back during the block, there may be a set output item that needs to
		// be serviced here.   
		if (cond_result != KEEP_GOING_DONT_CALL_BACK) // May be something to set or clear
			block_conditional_exe((ub *)dwell_block, AT_END); // Checking back at end per block_conditional()'s request.	
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
		return_val=GP_EXE_ERROR;	
		}
		
			
	ext: ;	
	#if DWELL_EXE_DEBUG
		printf("Dwell blk exe-Made it to ext: ;\n");
	#endif
	
	if (type_of_execution==PROGRAM)
		{
		vtg.prog_ctr++; // Increment prog ctr.		
		}
	else // If immed. exe., set timer2 as timed out as some other block may be executing that
		 // is depending on the timer.  If we exit here with the timer2 stopped and
		 // vtg.timer2_timeout == 0, that block will hang.
		{
		timer2->np_timercontrol = 8; // Make sure timer is stopped.
		vtg.timer2_timeout = 1;
		}

	return(return_val);
	}





int position_block_compile(char *str_to_compile, unsigned int (*control_program)[10])
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
		position_block = (pb *)&control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		position_block = (pb *)&control_program[stepnum][0];
		}

	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		control_program[stepnum][x] = 0;
		}
#if POS_COMPILE_DEGUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
#endif	
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	position_block->blk_id = POSITION;
	
#if POS_COMPILE_DEGUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
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
		return_val = block_conditional_compile((ub *)&control_program[stepnum][0], (char *)ptr+1);
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
		position_block->blk_id = NULL;
			
	// Now print the block for debug
	#if POS_COMPILE_DEGUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, control_program[stepnum][x]);	
	#endif		

	ext: ;
	if (immed_exe && return_val==0)
		{
		// Execute the block we just compiled.  
		position_block_exe(control_program, IMMED); 
		}
	
	return (return_val);		
	}


// Execute position block
int position_block_exe(unsigned int (*control_program)[10], int type_of_execution)
	{

	}




int velocity_block_compile(char *str_to_compile, unsigned int (*control_program)[10])
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
		velocity_block = (vb *)&control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		velocity_block = (vb *)&control_program[stepnum][0];
		}

	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		control_program[stepnum][x] = 0;
		}
		
#if VEL_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	velocity_block->blk_id = VELOCITY;

#if VEL_COMPILE_DEBUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
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
		return_val = block_conditional_compile((ub *)&control_program[stepnum][0], (char *)ptr+1);
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
		velocity_block->blk_id = NULL;
		
	// Now print the block for debug	
	#if VEL_COMPILE_DEBUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, control_program[stepnum][x]);	
	#endif		

	ext: ;
	if (immed_exe && return_val==0)
		{
		// Execute the block we just compiled.  
		velocity_block_exe(control_program, IMMED); 
		}
		
	return (return_val);		
	}



// Execute velocity block
int velocity_block_exe(unsigned int (*control_program)[10], int type_of_execution)
	{
	int return_val=0, cond_result, temp_pos, goose=0, accel_increments=0;
	int vac_wait=0, closed_loop=1, input_state, dv, loop_num=0, vac_on, axis_num = 1;
	int accel=0, act_vel, int_vel_cmd, x, starting_pos, next_pos;
	int current_dac_voltage, temp_dac_command, slow, dac_increment;
	int position, fill_test, commanded_raw_dac=0, current_raw_dac;
	short int commanded_vel;
	unsigned int initial_timer_mS, tempu=0, send_op_time, loop_time;
	volatile unsigned short int *halfword_ptr;
	vb *velocity_block=0;
	np_timer *timer2 = na_timer2;
	//vp_pvu *pv1 = va_pv1;
	//vp_pvu *pv2 = va_pv2;
	int print_again; // For test

	if (type_of_execution==IMMED) 
		velocity_block = (vb *)&control_program[IMMED_EXECUTE_BLOCK][0];
	else
		velocity_block = (vb *)&control_program[vtg.prog_ctr][0];
		
	/////////////////// Initial setups ////////////////////
	initial_timer_mS = nr_timer_milliseconds();
	send_op_time = initial_timer_mS + 500; // Time to send first op data packet

	print_again = initial_timer_mS+750; // For test
	
	// Check if vac_on and fill test
	chk_discrete_input(VAC_ON_INPUT, &input_state);	
	vac_on = input_state; // 1 if on 0 if off
	chk_discrete_input(FILL_TEST_INPUT, &input_state);	
	fill_test = input_state;
	
	// Check if closed loop or open loop
	if (velocity_block->vb_indicator_bits & vb_vel_var_mask)
		tempu = vtg.v[velocity_block->vel_df];
	else if (velocity_block->vb_indicator_bits & vb_vel_const_mask)
		tempu = velocity_block->vel_df;
	else tempu=0x7fff; // Skip code
		
	if (tempu & 0x80000000)
		{
		closed_loop=0; // Open loop
		#if VEL_EXE_DEBUG
			printf("tempu & 0x80000000==TRUE_this is an open loop step\n");
		#endif
		}

	if (tempu & 0x40000000) vac_wait=1; // This is a vacuum wait step.
	commanded_vel = (short int)tempu; // Velocity cmd is the lower half word, can be pos. or neg.
	if (commanded_vel == 0x7fff) goto ext; // Skip entire block without any test inputs/set outputs.
	// If open loop, convert commanded velocity from voltage to raw DAC value.
	if (!closed_loop)
		commanded_raw_dac = (unsigned int)(8192 + ((819 * (int)commanded_vel)/1000));
			
		
	// Set accel_increments / goose
	if (velocity_block->vb_indicator_bits & vb_accel_var_mask)
		tempu = vtg.v[velocity_block->accel_df];
	else if (velocity_block->vb_indicator_bits & vb_accel_const_mask)
		tempu = velocity_block->accel_df;
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
	if (velocity_block->vb_indicator_bits & vb_endpos_var_mask)
		tempu = vtg.v[velocity_block->endpos_df];
	else if (velocity_block->vb_indicator_bits & vb_endpos_const_mask)
		tempu = velocity_block->endpos_df;
	else tempu=0; // No position
	position = tempu;

	// Get axis number, if not 2, we're already defaulted to 1.
	if (velocity_block->vb_indicator_bits & vb_axis2_mask) axis_num = 2;
	#if VEL_EXE_DEBUG
		printf("Velocity axis# is %d\n", axis_num);
	#endif
		
	///////////////////////////////////////////////////////	
		
	cond_result = block_conditional_exe((ub *)velocity_block, BEGINNING); 
	
	if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
		{
		return_val=cond_result;	
		} 
	else if (cond_result < 20) // OK to proceed but there may be some conditions.
		{
		// This is the main body of the block
			
		////////////////////// VELOCITY CONTROL //////////////////////
		if (closed_loop)  
   			{
   			// Check if accel control or goosing is required.
   			if (accel) // Closed loop acceleration in X1 counts
      			{
				#if VEL_EXE_DEBUG
					printf("Closed loop accel\n");
				#endif
      			// Closed loop acceleration defined in X1 pps per X1 count
      			loop_num = 100;  // For diagnostics 
			
				starting_pos = RD_PV_POS;
      			act_vel = axis[axis_num].vel_ctr_freq/RD_PV_VEL; // Vel in pps

      			int_vel_cmd = act_vel;

      			// First determine vel change per increment. 
      			accel_increments = ((int)commanded_vel - act_vel) / accel;
      			for (x = 1; x <= accel; x++)
					{
					temp_pos = RD_PV_POS;	
					next_pos=starting_pos+(4*x);	
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

					do
						{
						//1111111111111111111111111111111111111111111111111111111111111

						#include "LOOP_EXIT_CONDITIONS1.H"

						}while ((int)RD_PV_POS < next_pos);  // Wait for next sample 
					} // for loop 
	  			}  // Closed loop accel 

   			else if (goose)   
      			{
				#if VEL_EXE_DEBUG
					printf("Goose\n");
				#endif
      			//out_dac(CONT_DAC_CHANNEL, goose, VOLTAGE_X_1000, ABSOLUTE);
				out_dac(DAC_OUTPUT_CHANNEL, goose, VOLTAGE_X_1000, ABSOLUTE);
      			}


   			//********************* CLOSE VELOCITY LOOP ********************

			#if VEL_EXE_DEBUG
				printf("Closing velocity loop\n");
			#endif
	
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

   			// Timing done with a seperate timer, timer2.
			// Commands to start/stop timer
			// timer2->np_timercontrol = 7; // Start timer, continuous mode
			// timer2->np_timercontrol = 5; // Start timer, single mode
			// timer2->np_timercontrol = 8; // Stop timer
			timer2->np_timercontrol = 8; // Stop timer
			vtg.timer2_timeout = 0;
			halfword_ptr = (unsigned short int *)&loop_time;
			timer2->np_timerperiodl = halfword_ptr[0];
			timer2->np_timerperiodh = halfword_ptr[1];
			timer2->np_timercontrol = 7; // Start timer continuously.

   			loop_num = 300;  // For diagnostics 

   			while (1)  // Loop until forced out.
      			{
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
				//out_dac(CONT_DAC_CHANNEL, temp_dac_command, VOLTAGE_X_1000, INCREMENTAL);
			//printf("temp_dac_command = %d, dac voltage = %d\n", temp_dac_command, (((vtg.dac_word[CONT_DAC_CHANNEL]-8192)*10000)/8192));	
		//printf("temp_dac_command = %d, dac voltage = %d\n", temp_dac_command, vtg.dac_word[CONT_DAC_CHANNEL]);	
				// Make waiting loop a do loop to ensure at least one pass through 
      			do // Wait for next sample 
	     			{
	     			//222222222222222222222222222222222222222222222222222222

	     			#include "LOOP_EXIT_CONDITIONS1.H"

	     			}while (!vtg.timer2_timeout);  // Wait for next sample 
      			}
   			}
		else // Open loop
   			{
			#if VEL_EXE_DEBUG
				printf("ol block\n"); 
			#endif
   			if (accel)
      			{
				#if VEL_EXE_DEBUG
					printf("Open loop accel\n");
				#endif

				starting_pos = RD_PV_POS;
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
					#if VEL_EXE_DEBUG
						printf("dac_increment=%d, accel=%d, commanded_raw_dac=%d, cur_raw_dac=%d\n", dac_increment, accel, commanded_raw_dac, current_raw_dac);
					#endif
					for (x = 1; x <= accel; x++) // Use x as a counter 
						{
						temp_pos = RD_PV_POS;	
						next_pos=starting_pos+(4*x);
						
						// Add dac_increment to current DAC voltage
						//out_dac(CONT_DAC_CHANNEL, dac_increment, VOLTAGE_X_1000, INCREMENTAL);
						//out_dac(CONT_DAC_CHANNEL, dac_increment, RAW_VAL, INCREMENTAL);
						out_dac(DAC_OUTPUT_CHANNEL, dac_increment, RAW_VAL, INCREMENTAL);
						
	       				do
							{
							//333333333333333333333333333333333333333333333333333333333333

							#include "LOOP_EXIT_CONDITIONS1.H"

							}while ((int)RD_PV_POS < next_pos);  // Wait for next sample   
						}
					}  
      			}

   			// commanded_vel is volts X 1000.
			#if VEL_EXE_DEBUG
				printf("Outputting open loop DAC setting\n\n");
			#endif 
   			//out_dac(CONT_DAC_CHANNEL, (int)commanded_vel, VOLTAGE_X_1000, ABSOLUTE);
			//out_dac(CONT_DAC_CHANNEL, commanded_raw_dac, RAW_VAL, ABSOLUTE);
			out_dac(DAC_OUTPUT_CHANNEL, commanded_raw_dac, RAW_VAL, ABSOLUTE);
			#if VEL_EXE_DEBUG
				printf("OL cmd__commanded_raw_dac = %d\n", commanded_raw_dac);	// For test
			#endif

   			loop_num = 500;  // For diagnostics 

   			do  // Wait for something to terminate the block 
	    		{
	     		//4444444444444444444444444444444444444444444444444444444444444

	     		#include "LOOP_EXIT_CONDITIONS1.H"

	     		}while (1);  // Loop until something terminates the loop 
   			}
		
		end_of_vloop: ;
		//////////////////////////////////////////////////////////////


		outputs: ;
		// If we got to this point, the block terminated normally as opposed to
		// a condition not being met.  Therefore even if we were requested to
		// call back during the block, there may be a set output item that needs to
		// be serviced here.   
		if (cond_result != KEEP_GOING_DONT_CALL_BACK) // May be something to set or clear
			block_conditional_exe((ub *)velocity_block, AT_END); // Checking back at end per block_conditional()'s request.	
		}
	else if (cond_result == COND_NOT_MET); // Nothing more to do 
		
	else  // This should not happen - if it does, deal with it here.
		return_val=GP_EXE_ERROR;	
		
			
	ext: ;	
	if (type_of_execution==PROGRAM)
		{
		vtg.prog_ctr++; // Increment prog ctr.		
		}
	else // If immed. exe., set timer2 as timed out as some other block may be executing that
		 // is depending on the timer.  If we exit here with the timer2 stopped and
		 // vtg.timer2_timeout == 0, that block will hang.
		{
		timer2->np_timercontrol = 8; // Make sure timer is stopped.
		vtg.timer2_timeout = 1;
		}

	return(return_val);
	}




int jump_block_compile(char *str_to_compile, unsigned int (*control_program)[10])
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
		jump_block = (jb *)&control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		immed_exe = 1;
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		jump_block = (jb *)&control_program[stepnum][0];
		}

	//Clear all 10 block locations
	for (x = 0; x<=9; x++)
		{
		control_program[stepnum][x] = 0;
		}

#if JUMP_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	jump_block->blk_id = JUMP;
	
#if JUMP_COMPILE_DEBUG	
	printf("ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x, ID = %d\n", x, control_program[stepnum][x], JUMP);
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
		return_val = block_conditional_compile((ub *)&control_program[stepnum][0], (char *)ptr+1);
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
		jump_block->blk_id = NULL;
		
	// Now print the block for debug
	#if JUMP_COMPILE_DEBUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, control_program[stepnum][x]);	
	#endif		

	ext: ;
	if (immed_exe && return_val==0)
		{
		// Execute the block we just compiled.  
		jump_block_exe(control_program, IMMED); 
		}
	
	return (return_val);		
	}




// Execute jump block
int jump_block_exe(unsigned int (*control_program)[10], int type_of_execution)
	{
	int return_val=0, cond_result, dont_increment_prog_ctr=0;
	unsigned int where_to_jump=0;
	jb *jump_block=0;
	
	if (type_of_execution == IMMED)  
		jump_block = (jb *)&control_program[IMMED_EXECUTE_BLOCK][0];
	else // Must be PROGRAM
		jump_block = (jb *)&control_program[vtg.prog_ctr][0];
		
	cond_result = block_conditional_exe((ub *)jump_block, BEGINNING);
	#if JUMP_EXE_DEBUG
		printf("Jump block, cond_result = %d\n", cond_result);
	#endif
	
	if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
		{
		if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
		else {} // vtg.prog_ctr will be incremented. 	
		return_val=cond_result;	
		} 
	else if (cond_result < 20) // OK to proceed but there may be some conditions.
		{
		// This is the main body of the block	
		
		#if JUMP_EXE_DEBUG
			printf("Made it to main body of jump block\n");
		#endif
		if (jump_block->jb_indicator_bits & jb_type_return_mask) // Return from subroutine
   			{
   			// If vtg.stack_ptr == 0, can't be a return from subroutine.
   			if (vtg.stack_ptr == 0) 
				{
				if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
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
			if (jump_block->jb_indicator_bits & jb_jump_var_mask)
				where_to_jump = vtg.v[jump_block->jump_df];
			else // Assume constant
				where_to_jump = jump_block->jump_df;
				
	
   			if (jump_block->jb_indicator_bits & jb_type_normal_mask)
				{
				if (jump_block->jb_indicator_bits & jb_rel_abs_specifier_mask) // Relative jump forward
      				{
					if (where_to_jump==0) // Jump to current block
						{
						// Since we're staying at the same block, it is
						// possible we have a while condition.
						#if JUMP_EXE_DEBUG
							printf("We're jumping to the current block (type relative)\n");
						#endif
						if (cond_result == KEEP_GOING_CALL_BACK_DURING)
							{ 
							while (block_conditional_exe((ub *)jump_block, DURING) == KEEP_GOING_CALL_BACK_DURING)
								{
								// Could do something here later	
								}		
							   // Condition no longer met.  Any outputs to
							  // set_clr will have already been taken care
							 // of by block_conditional() therfore we jump
							// past the AT_END checking.
							if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
							else {} // vtg.prog_ctr will be incremented. 
							goto ext;	
							}
						// Otherwise a jump to the current block without any conditions
						// would be a forever loop, we won't let it happen.
						// Increment the prog_ctr and move on.
						if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
						else {} // vtg.prog_ctr will be incremented 	
						}
					else // We're actually relative jumping somewhere
						{	
						if (jump_block->jb_indicator_bits & jb_rel_dir_mask) // Forward relative jump
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
						if (cond_result == KEEP_GOING_CALL_BACK_DURING)
							{ 
							while (block_conditional_exe((ub *)jump_block, DURING) == KEEP_GOING_CALL_BACK_DURING)
								{
								// Could do something here later	
								}		
							   // Condition no longer met.  Any outputs to
							  // set_clr will have already been taken care
							 // of by block_conditional() therfore we jump
							// past the AT_END checking.
							if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
							else {} // vtg.prog_ctr will be incremented.  
							goto ext;	
							}
						// Otherwise a jump to the current block without any conditions
						// would be a forever loop, we won't let it happen.
						// Increment the prog_ctr and move on.	
						if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
						else {} // vtg.prog_ctr will be incremented. 		
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
			else if (jump_block->jb_indicator_bits & jb_type_sub_mask) // Jump to subroutine 
				{
				if (vtg.prog_ctr == where_to_jump) // Can't jump to sub that is the current block!
					if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
					else {} // Otherwise vtg.prog_ctr will be incremented
				else
					{
					if (type_of_execution==IMMED)
						{		
						// If IMMED, push the current block since that is where
						// we want to come back to in this case.	
						push(vtg.prog_ctr);
						#if JUMP_EXE_DEBUG
							printf("Blk %d jmp to sub IMMED, pushing pc=%d, ", vtg.prog_ctr, vtg.prog_ctr);
						#endif
						}
					else
						{
						// Otherwise, if executing the control program, 
						// pust the next block.
						push(vtg.prog_ctr+1); 
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
		
		// If we got to this point, the block terminated normally as opposed to
		// a condition not being met.  Therefore even if we were requested to
		// call back during the block, there may be a set output item that needs to
		// be serviced here.   
		if (cond_result != KEEP_GOING_DONT_CALL_BACK) // May be something to set or clear
			block_conditional_exe((ub *)jump_block, AT_END); // Checking back at end per block_conditional()'s request.	
		}
	else if (cond_result == COND_NOT_MET) // Nothing more to do 
		{
		if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
		else {} // vtg.prog_ctr will be incremented. 	
		}
		
	else  // This should not happen - if it does, deal with it here.
		{
		if (type_of_execution==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
		else {} // vtg.prog_ctr will be incremented. 
		return_val=GP_EXE_ERROR;	
		}
			
	ext: ;
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





int update_block_compile(char *str_to_compile, unsigned int (*control_program)[10])
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
		update_block = (ub *)&control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
		}
	else 
		//Compile and save step to SRAM, get step number
		{
		//The first digits will be the step number
		stepnum = atoi(str_to_compile); 
		if (stepnum > MAX_STEP || stepnum < 1) {return_val = 1; goto ext;} //Error 
		update_block = (ub *)&control_program[stepnum][0];
		}

	//Clear all 10 update block locations
	for (x = 0; x<=9; x++)
		{
		control_program[stepnum][x] = 0;
		}

#if UPDATE_COMPILE_DEBUG
	printf("Block should be cleared - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
#endif
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	update_block->blk_id = UPDATE;
	
#if UPDATE_COMPILE_DEBUG	
	printf("Update ID should be there now - let's see\n");
	for (x = 0; x<10; x++)
	printf("word%d %x\n", x, control_program[stepnum][x]);
#endif

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		return_val = block_conditional_compile((ub *)&control_program[stepnum][0], (char *)ptr+1);
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
					if (tempi<1 || tempi>MAX_ADC_CH) {return_val = 1; goto clr_id;}
				
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
		update_block->blk_id = NULL;
		
	// Now print the block for debug
	#if UPDATE_COMPILE_DEBUG
		for (x=0; x<10; x++)
			printf("Word%d %x\n", x, control_program[stepnum][x]);	
	#endif
	
	ext: ;

	if (immed_exe && return_val==0)
		{
		// Execute the block we just compiled.  Send a NULL ptr for the prog_ctr
		// to indicate this is an immed. exe. block.
		update_block_exe(control_program, IMMED); 
		#if UPDATE_COMPILE_DEBUG	
			printf("UPDATE compile, IMMED exe, finished calling update_block_exe()\n");
		#endif
		}

	return (return_val);
	}



// Execute update block
int update_block_exe(unsigned int (*control_program)[10], int type_of_execution)
	{
	int return_val=0, cond_result, op1, op2=0, ctr=0, op2_present=1, temp_itu=0, axis_num=1;
	int final_dac_voltage, initial_dac_voltage, current_dac_voltage, ramp=0;
	unsigned int ramp_time;
	volatile unsigned short int *halfword_ptr;
	ub *update_block=0;
	//vp_pvu *pv1 = va_pv1;
	np_timer *timer2 = na_timer2;
	np_pio *misc_out_pio = na_misc_outs;

	if (type_of_execution==IMMED) // an immed exe block.  prog_ctr will not be incremented if immed exe.
		update_block = (ub *)&control_program[IMMED_EXECUTE_BLOCK][0];
	else
		update_block = (ub *)&control_program[vtg.prog_ctr][0];
		
	cond_result = block_conditional_exe((ub *)update_block, BEGINNING); 
	
	if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
		{
		#if UPDATE_EXE_DEBUG
			printf("Update block EXE, executive error\n");
		#endif
		return_val=cond_result;	
		} 
	else if (cond_result < 20) // OK to proceed but there may be some conditions.
		{
		// This is the main body of the update block.
		#if UPDATE_EXE_DEBUG
			printf("Update block EXE, made it to the main body of the block, type of exe is %d\n", type_of_execution);
		#endif
		
		// First get the 1st. operand.
		if (update_block->ub_indicator_bits & ub_op1_const_mask)
			{
			op1 = update_block->op1_df;	
			}
		else if (update_block->ub_indicator_bits & ub_op1_var_mask)
			{
			op1 = vtg.v[update_block->op1_df];
			}
		else if (update_block->ub_indicator_bits & ub_op1_adc_mask)
			{
			// op1 will be voltage X 1000.
			while (convert_ana_ch(update_block->op1_df, VOLTAGE_X_1000, &op1)); // Keep trying until we get it
			}
		else if (update_block->ub_indicator_bits & ub_op1_dac_mask)
			{
			get_dac(update_block->op1_df, VOLTAGE_X_1000, &op1);
			}
		else if (update_block->ub_indicator_bits & ub_op1_pos_mask)
			{
			axis_num=update_block->op1_df;
			op1 = RD_PV_POS; 
			}	
		else if (update_block->ub_indicator_bits & ub_op1_vel_mask)
			{
			// Convert velocity to X1 pulses/sec	 
			axis_num=update_block->op1_df;
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
		if (update_block->ub_indicator_bits & ub_op2_const_mask)
			{
			op2 = update_block->op2_df;	
			}
		else if (update_block->ub_indicator_bits & ub_op2_var_mask)
			{
			op2 = vtg.v[update_block->op2_df];
			}
		else 
			op2_present = 0; // No op2

		if (op2_present) // Must have an operator.
			{
			if (update_block->ub_indicator_bits & ub_add_mask)
				temp_itu = op1+op2;
			else if (update_block->ub_indicator_bits & ub_sub_mask)
				temp_itu = op1-op2;
			else if (update_block->ub_indicator_bits & ub_mul_mask)
				temp_itu = op1*op2;
			else if (update_block->ub_indicator_bits & ub_div_mask)
				temp_itu = op1/op2;
			else if (update_block->ub_indicator_bits & ub_and_mask)
				temp_itu = op1&op2;
			else if (update_block->ub_indicator_bits & ub_or_mask)
				temp_itu = op1|op2;
			else if (update_block->ub_indicator_bits & ub_xor_mask)
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
				printf("Update block EXE, op2=%d\n", op2);
			#endif	
			}
		
		if (update_block->ub_indicator_bits & ub_itu_var_mask)
			{
			if (op2_present) vtg.v[update_block->itu_df] = temp_itu;
			else vtg.v[update_block->itu_df] = op1;
			}
		else if (update_block->ub_indicator_bits & ub_itu_dac_out_mask)
			{
			// Check for ramp
			if (update_block->ub_indicator_bits & ub_vramp_var_mask)
				ramp = vtg.v[update_block->vramp_df];
			else if (update_block->ub_indicator_bits & ub_vramp_const_mask)
				ramp = update_block->vramp_df;
			// If neither const nor var, ramp will be 0.		
			
			if (op2_present) final_dac_voltage = temp_itu;	
				else final_dac_voltage = op1;
			
			if (final_dac_voltage > 10000) goto ext; // Skip entire block.
			
			if (ramp)
				{
				// Get the initial DAC voltage.	
				get_dac(update_block->itu_df, VOLTAGE_X_1000, &initial_dac_voltage);	
					
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
					// Start timer
					// Commands to start/stop timer
					// timer2->np_timercontrol = 7; // Start timer, continuous mode
					// timer2->np_timercontrol = 5; // Start timer, single mode
					// timer2->np_timercontrol = 8; // Stop timer
					timer2->np_timercontrol = 8; // Stop timer
					vtg.timer2_timeout = 0;
					halfword_ptr = (unsigned short int *)&ramp_time;
					timer2->np_timerperiodl = halfword_ptr[0];
					timer2->np_timerperiodh = halfword_ptr[1];
					timer2->np_timercontrol = 7; // Start timer continously
					do
						{
						while (!vtg.timer2_timeout)
							{
							if (cond_result == KEEP_GOING_CALL_BACK_DURING)
								{ 
								if (block_conditional_exe((ub *)update_block, DURING) == KEEP_GOING_CALL_BACK_DURING);		
								else goto ext; // Condition was not met.  Any outputs to
											  // set_clr will have already been taken care
											 // of by block_conditional() therfore we jump
											// past the AT_END checking.
								}
							}
						current_dac_voltage += ramp;			
						out_dac(update_block->itu_df, current_dac_voltage, VOLTAGE_X_1000, ABSOLUTE);	
						vtg.timer2_timeout = 0;
						}while (((ramp>0) && (current_dac_voltage<final_dac_voltage))
								|| ((ramp<0) && (current_dac_voltage>final_dac_voltage)));
					}			
				}
			
			if (update_block->ub_indicator_bits & ub_dac_increment_mask)
				out_dac(update_block->itu_df, final_dac_voltage, VOLTAGE_X_1000, INCREMENTAL);
			else
				out_dac(update_block->itu_df, final_dac_voltage, VOLTAGE_X_1000, ABSOLUTE);
			timer2->np_timercontrol = 8; // Stop timer
			}
		else if (update_block->ub_indicator_bits & ub_itu_pos_mask)
			{
			axis_num=update_block->itu_df;
			if (op2_present) {SET_PV_PLOAD(temp_itu);}	
			else {SET_PV_PLOAD(op1);}
			}
		else if (update_block->ub_indicator_bits & ub_itu_command_mask)
			{	
			if (op2_present) {}
			else {}
			}
		else  // Gotta have something to update since this is an update block.
			{
			#if UPDATE_EXE_DEBUG
				printf("Update block EXE, ERROR, nothing to update!!!\n");
			#endif
			return_val = EXE_FAULT_IND_BITS;
			goto ext;
			}	
			
		// Will have to check if conditional callback DURING is necessary here.

		// If we got to this point, the block terminated normally as opposed to
		// a condition not being met.  Therefore even if we were requested to
		// call back during the block, there may be a set output item that needs to
		// be serviced here.   
		if (cond_result != KEEP_GOING_DONT_CALL_BACK) // May be something to set or clear
			block_conditional_exe((ub *)update_block, AT_END); // Checking back at end per block_conditional()'s request.	
		}
	else if (cond_result == COND_NOT_MET); // Nothing more to do 
		
	else  // This should not happen - if it does, deal with it here.
		return_val=GP_EXE_ERROR;	
		
			
	ext: ;
	
	#if UPDATE_EXE_DEBUG
		printf("Update block EXE, Made to to ext: ;\n");
	#endif

	timer2->np_timercontrol = 8; // Make sure timer is stopped.
	if (type_of_execution==PROGRAM)
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
int block_conditional_compile(ub *blkptr, char *condstr)
	{
	int return_val = 0, x;
	volatile char *ptr=0;

	ptr = condstr;
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
					if (x<1 || x>MAX_ADC_CH) {return_val = 1; goto ext;}
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
//
// This function uses a lot of in-line code.  Code size was traded for execution
// speed.  It is easier to read this way also.

/* Steps to execute this function, BEGINNING, DURING and AT_END refer to when
   the calling block calls this function.

		If AT_END // No conditional checking here, only set/clear outputs
			Handle during and at_end set/clr outputs here.
				
		Else // Must be BEGINNING or DURING.
   			If IF/While.  If conditional checking:
				Convert the operands now.
				Set condition flag.
    	 		Check for condition.
					Condition met:
						Set condition_met flag.
																
					Condition not met:
						Clr condition_met flag.
						 
			Else
				Clr condition flag. // No conditional checking.			
						
			If BEGINNING
				If !condition 
					If BEGINNING or DURING set/clr outputs, deal with them now.
					Set return value.
				
				Else if condition && condition met
					If BEGINNING or DURING set/clr outputs, deal with them now.
					If WHILE
						Set return value.
					Else // Must be IF
						Set return value.	
				
				Else // Condition not met
					Set return value.
			
			
			Else if DURING
				If condition && condition_met // Should only be a while condition
					Set return value.
				
				Else
					Deal with any DURING or AT_END set/clr outputs here.
					Set return value.
					
		Return				 			
*/
 


// Execute conditional checking / set_clr outputs.
int block_conditional_exe(ub *blkptr, int when_called)
	{
	int cond_op1, cond_op2=0, ctr=0, condition=0, condition_met=0;
	int while_ = 0, axis_num=0;
	unsigned short int set_out_io_num;
	//vp_pvu *pv1 = va_pv1;
		
	// First get the set output I/O number just in case we need it.	
	if (blkptr->cond_indicator_bits & set_out_var_mask) // Output to set is a variable
			set_out_io_num = (unsigned short int)vtg.v[blkptr->set_out_df];
	else set_out_io_num = blkptr->set_out_df;	
				
	// If called AT_END, no need to check conditional, check if any
	// SET_OUTPUTS AT_END or DURING and deal with them now.
	if (when_called == AT_END)
		{
		#if COND_EXE_DEBUG	
			printf("AT_END ");	
		#endif	
		// Conditions to turn an output on	
		if (((blkptr->cond_indicator_bits & set_out_during_mask) &&
		         !(blkptr->cond_indicator_bits & turn_on_turn_off_mask))   ||
						((blkptr->cond_indicator_bits & set_out_at_end_mask) &&
							(blkptr->cond_indicator_bits & turn_on_turn_off_mask)))
			{
			#if COND_EXE_DEBUG	
				printf("-turning on an output\n");	
			#endif	
			//OSW1 |= 1<<(15+set_out_io_num);				
			set_clr_discrete_output(set_out_io_num, TURN_ON);
			}
		// Conditions to turn an output off		
		else if (((blkptr->cond_indicator_bits & set_out_during_mask) &&
		          (blkptr->cond_indicator_bits & turn_on_turn_off_mask))   ||
						((blkptr->cond_indicator_bits & set_out_at_end_mask) &&
							!(blkptr->cond_indicator_bits & turn_on_turn_off_mask)))
			{
			#if COND_EXE_DEBUG	
				printf("-turning off an output\n");	
			#endif	
			//OSW1 &= ~(1<<(15+set_out_io_num));	
			set_clr_discrete_output(set_out_io_num, TURN_OFF);
			}
	#if COND_EXE_DEBUG
		else printf("-no outputs to set_clr\n");
	#endif
					
		// else nothing to do
		return(0);	
		}
	
	else // Must be BEGINNING or DURING. 
		{
		// Check conditional.	
		if ((blkptr->cond_indicator_bits & while_mask) ||
				(blkptr->cond_indicator_bits & if_mask))
			{
			condition = 1;
			if (blkptr->cond_indicator_bits & while_mask) while_ = 1;
				
			// Get 1st. operands.  We don't yet know what it is.  
			// The cond_indicator_bits will tell the story.
			if (blkptr->cond_indicator_bits & op1_discrete_io_mask)
				{
				if (blkptr->cond_indicator_bits & op1_var_mask) // discrete I/O can also be a variable.
					cond_op1 = (int)vtg.v[blkptr->cond_op1_df];
				else		
					cond_op1 = (int)blkptr->cond_op1_df;
				}
			else if (blkptr->cond_indicator_bits & op1_var_mask)
				cond_op1 = (int)vtg.v[blkptr->cond_op1_df];
			else if (blkptr->cond_indicator_bits & op1_pos_mask) 
				{
				axis_num = blkptr->cond_op1_df; 
				cond_op1 = RD_PV_POS; 
				}
			else if (blkptr->cond_indicator_bits & op1_vel_mask)
				{
				// Convert velocity to X1 pulses/sec
				axis_num = blkptr->cond_op1_df;	 
				cond_op1 = axis[axis_num].vel_ctr_freq/RD_PV_VEL;
				}
			else if (blkptr->cond_indicator_bits & op1_dac_mask)	
				// cont_op1 will == DAC voltage X 1000.
				cond_op1 = ((vtg.dac_word[blkptr->cond_op1_df]-8192)*10000)/8192;
			else if (blkptr->cond_indicator_bits & op1_adc_mask)
				{
				// Since this function will not allow recursion, keep trying until we get it
				while (convert_ana_ch(blkptr->cond_op1_df, VOLTAGE_X_1000, &cond_op1));
				}
			
			// Now get 2nd. operand which can only be a constant or variable.
			if (blkptr->cond_indicator_bits & op2_const_mask)
				cond_op2 = (int)blkptr->cond_op2_df;
			else if (blkptr->cond_indicator_bits & op2_var_mask)
				cond_op2 = (int)vtg.v[blkptr->cond_op2_df];
			
			// Now that we have the operands, it will be easy to make comparisons
			// or check discrete I/O.	
						
			// First check for basic forms
			if (blkptr->cond_indicator_bits & input_on_mask)
				{
				//if (ISW1 & (1<<(15+cond_op1))) condition_met=1;
				
				// Check if op1 is zero.  If so, this bypasses conditional
				// checking.  The block will be treated as though there were
				// not any conditions.
				if (cond_op1==0) condition=0;
				else chk_discrete_input(cond_op1, &condition_met); // condition_met will == 1 if input on
				} 
			else if (blkptr->cond_indicator_bits & input_off_mask)
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
			else if (blkptr->cond_indicator_bits & advanced_testing_mask)
				{
				if (blkptr->cond_indicator_bits & eq_mask)
					{
					if (cond_op1 == cond_op2) condition_met=1; 
					}
				else if (blkptr->cond_indicator_bits & neq_mask)
					{
					if (cond_op1 != cond_op2) condition_met=1; 
					}	
				else if (blkptr->cond_indicator_bits & gtr_than_mask)
					{
					if (cond_op1 > cond_op2) condition_met=1; 
					}		
				else if (blkptr->cond_indicator_bits & gtr_than_eq_to_mask)
					{
					if (cond_op1 >= cond_op2) condition_met=1; 
					}
				else if (blkptr->cond_indicator_bits & less_than_mask)
					{
					if (cond_op1 < cond_op2) condition_met=1; 
					}
				else if (blkptr->cond_indicator_bits & less_than_eq_to_mask)
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
		
		if (when_called == BEGINNING)
			{
			#if COND_EXE_DEBUG
				printf("BEGINNING ");	
			#endif	
			if (!condition)
				{
				#if COND_EXE_DEBUG	
					printf("-no condition- ");	
				#endif	
				if (blkptr->cond_indicator_bits & set_out_beginning_mask)
					{
					if (blkptr->cond_indicator_bits & turn_on_turn_off_mask)	
						//OSW1 |= 1<<(15+set_out_io_num);
						set_clr_discrete_output(set_out_io_num, TURN_ON);
					else
						//OSW1 &= ~(1<<(15+set_out_io_num));
						set_clr_discrete_output(set_out_io_num, TURN_OFF);
					#if COND_EXE_DEBUG	
						printf("keep going, set_clr at beginning-no call back\n");
					#endif
						
					return(KEEP_GOING_DONT_CALL_BACK); // Tell caller to keep going, no need to call back,	
					}
				else if (blkptr->cond_indicator_bits & set_out_during_mask)
					{
					if (blkptr->cond_indicator_bits & turn_on_turn_off_mask)	
						//OSW1 |= 1<<(15+set_out_io_num);
						set_clr_discrete_output(set_out_io_num, TURN_ON);
					else
						//OSW1 &= ~(1<<(15+set_out_io_num));
						set_clr_discrete_output(set_out_io_num, TURN_OFF);
					#if COND_EXE_DEBUG	
						printf("keep going, set_clr during-call back at end\n");	
					#endif
						
					return(KEEP_GOING_CALL_BACK_AT_END); // Tell caller to keep going, call back at end.					
					}	
				else if (blkptr->cond_indicator_bits & set_out_at_end_mask)
					{
					#if COND_EXE_DEBUG	
						printf("keep going, set_clr at end - call back at end\n");	
					#endif	
					// Nothing to set/clr at this time, call back at end.	
					return(KEEP_GOING_CALL_BACK_AT_END); // Tell caller to call back at end.		
					}
				else // Else nothing to set/clr.			
					{
					#if COND_EXE_DEBUG	
						printf("Keep going, nothing to set_clr - don't call back\n");	
					#endif	
					return(KEEP_GOING_DONT_CALL_BACK); 	
					}
				}
			else if (condition && condition_met)
				{
				#if COND_EXE_DEBUG	
					printf("-condition met-");	
				#endif	
				if (blkptr->cond_indicator_bits & set_out_beginning_mask)
					{
					if (blkptr->cond_indicator_bits & turn_on_turn_off_mask)	
						//OSW1 |= 1<<(15+set_out_io_num);
						set_clr_discrete_output(set_out_io_num, TURN_ON);
					else
						//OSW1 &= ~(1<<(15+set_out_io_num));
						set_clr_discrete_output(set_out_io_num, TURN_OFF);
						
					if (while_)
						{
						#if COND_EXE_DEBUG	
							printf("set_clr at beginning - while conditional found, keep going, call back during\n");	
						#endif	
						return(KEEP_GOING_CALL_BACK_DURING); // Tell caller to check back during block execution.	
						}
					else
						{
						#if COND_EXE_DEBUG	
							printf("set_clr at beginning, keep going, don't call back\n");	
						#endif	
						return(KEEP_GOING_DONT_CALL_BACK); // Otherwise tell caller to keep going, no need to call back.						
						}
					}		
				else if (blkptr->cond_indicator_bits & set_out_during_mask)
					{
					if (blkptr->cond_indicator_bits & turn_on_turn_off_mask)	
						//OSW1 |= 1<<(15+set_out_io_num);
						set_clr_discrete_output(set_out_io_num, TURN_ON);
					else
						//OSW1 &= ~(1<<(15+set_out_io_num));
						set_clr_discrete_output(set_out_io_num, TURN_OFF);
						
					if (while_)
						{
						#if COND_EXE_DEBUG	
							printf("set_clr during - while conditional found, keep going, call back during\n");		
						#endif	
						return(KEEP_GOING_CALL_BACK_DURING); // Tell caller to check back during block execution.	
						}
					else
						{
						#if COND_EXE_DEBUG	
							printf("set_clr during, keep going, don't call back\n");		
						#endif	
						return(KEEP_GOING_CALL_BACK_AT_END); // Otherwise tell caller to keep going, call back at end of block.						
						}
					}
				else if (blkptr->cond_indicator_bits & set_out_at_end_mask)
					{
					if (while_)
						{
						#if COND_EXE_DEBUG	
							printf("set_clr at end - while conditional found, keep going, call back during\n");			
						#endif	
						return(KEEP_GOING_CALL_BACK_DURING); // Tell caller to check back during block execution.	
						}
					else
						{
						#if COND_EXE_DEBUG	
							printf("set_clr at end, keep going, call back at end\n");	
						#endif	
						return(KEEP_GOING_CALL_BACK_AT_END); // Otherwise tell caller to keep going, call back at end of block.						
						}
					}
				else // No outputs to set/clr.
					{
					if (while_)
						{
						#if COND_EXE_DEBUG	
							printf("nothing to set_clr - while conditional found, keep going, call back during\n");				
						#endif	
						return(KEEP_GOING_CALL_BACK_DURING); // Tell caller to check back during block execution.	
						}
					else
						{
						#if COND_EXE_DEBUG	
							printf("nothing to set_clr - don't call back\n");	
						#endif	
						return(KEEP_GOING_DONT_CALL_BACK); // Otherwise tell caller to keep going, no need to call back.							
						}
					}						
				}
			else // Condition was not met.
				{
				#if COND_EXE_DEBUG	
					printf("-condition not met\n");	
				#endif	
				return(COND_NOT_MET);	
				}		
			}
		else if (when_called == DURING)
			{
			// DURING always a WHILE condition, otherwise this would not be called.	
			#if COND_EXE_DEBUG
				printf("DURING ");
			#endif		
			if (condition && condition_met) // Should only be a WHILE.
				{
				#if COND_EXE_DEBUG	
					printf("-condition met, keep going\n");	
				#endif	
				// Never deal with set/clr outputs during	
				return(KEEP_GOING_CALL_BACK_DURING); // Tell caller to keep going.	
				}
			else 
				{
				// Condition not met, since this will be the end of the block
				// check for any set/clr outputs here.	
					
				// Conditions to turn an output on	
				if (((blkptr->cond_indicator_bits & set_out_during_mask) &&
		        		 !(blkptr->cond_indicator_bits & turn_on_turn_off_mask))   ||
								((blkptr->cond_indicator_bits & set_out_at_end_mask) &&
									(blkptr->cond_indicator_bits & turn_on_turn_off_mask)))
					{
					#if COND_EXE_DEBUG	
						printf("-condition not met, turning something on before exiting\n");	
					#endif	
					//OSW1 |= 1<<(15+set_out_io_num);				
					set_clr_discrete_output(set_out_io_num, TURN_ON);
					}
				// Conditions to turn an output off		
				else if (((blkptr->cond_indicator_bits & set_out_during_mask) &&
		          		(blkptr->cond_indicator_bits & turn_on_turn_off_mask))   ||
								((blkptr->cond_indicator_bits & set_out_at_end_mask) &&
									!(blkptr->cond_indicator_bits & turn_on_turn_off_mask)))
					{
					#if COND_EXE_DEBUG	
						printf("-condition not met, turning something off before exiting\n");		
					#endif	
					//OSW1 &= ~(1<<(15+set_out_io_num));	
					set_clr_discrete_output(set_out_io_num, TURN_OFF);
					}
				// else nothing to do
			#if COND_EXE_DEBUG	
				else printf("-condition not met, no outputs to set/clr\n");
			#endif
					
				return(COND_NOT_MET); // Condition was not met.	
				}		
			}
		else // Error, when_called not set to a correct value
			return(INVALID_PARAMETER_PASSED);				
		}
	}





void push(unsigned int item)
{
vtg.stack[vtg.stack_ptr++] = item;
}


unsigned int pop(void)
{
return (vtg.stack[--vtg.stack_ptr]);
}








