//					combined_control_blocks11.c
//
//			Copyright 2008 Visi-Trak Worldwide

//#include "nios.h"
//#include "control_program_blocks8.h"
//#include "fixed_point_decimal.h"

/* 
7-15-03  control_blocks6.c  Supports 2 position axes.

11-22-04 combined_control_blocks2.c
	Changed compile routines so that if the control program is running, the memory locations for the
	control step will not be cleared first.  The program will be downloaded over top of what's already
	there, so that when the step that is currently being executed is loaded, it will happen seamlessly.

	Created a new fatal error, CONDITIONAL_EXECUTION_ERROR 107.  If an error is encountered during
	conditional checking this will be generated.  Also fixed a problem with BLOCK_EXECUTION_ERROR 104
	so that it now functions correctly.

4-12-05 combined_control_blocks3.c
	Added pressure control.

	7-14-05 In jump block exe, if absolute jump to block 1, stack ptr is reset.  This was done to fix a 
	stack overflow fatal error 101 due to a problem in the ftii_control_program.

	Quit declaring pointers for Nios peripherals.  Now just use the definitions in excalibur.h


2-14-07 combined_control_blocks4.c
	Added framework for position control.  A lot of work still needs to be done.

3-8-07 combined control blocks5.c
	Major change.  Elminated the compile routines.  The actual ASCII string is now saved in flash.  At run time, the
	string is parsed and executed.  This greatly simplifies adding new block types or making changes to existing blocks.

	For set output durings, added "R" in addition to "d."  For example: D1000_R1.33 will turn on output 33 during the
	execution of the block.  Found that the terminal program converts lower case to upper case when sending commands to 
	the board.  This allow for an upper case character that can be used if desired.

	Added macros for some of the items that are common to all block types.

	Functionally, everything else is the same.

3-8-07 combined control blocks6.c
	Implemented the PID loop.  Finalized on the recursive form of the equation.

    Changed gain break so that increases linearly from 1 to the final gain multiplier over the velocity specified.
	Now there is only one break multiplier and one gain velocity setting.  The velocity setting is not a break velocity
	as it was before, it's the velocity at which the max gain will be reached.  In most cases, this will be set to some
	level above the max. velocity for the shot.

	Added linear block.  Implemented position control in the update_pos_loop() function.  

	//////////// Framework for below is in place, however not implemented yet ///////////////////
	In velocity control, changed exit loop include to loop_exit_conditions4.h.  No longer using zspeed from the monitor to halt control.
	Now checking for a change in position that represents the EOS velocity to end velocity control.
	
1-11-08	combined control blocks8.c
		Goals:
		1,	Change PID and position loop update functions to use fixed point decimal numbers.
		2,	Implement pressure control with the PID function.
		
	Added support for the Prince binary valve.	
	
	11-5-08 Changed start block to not reenable the warning output if there is a warning. 
	
2-3-09 combined_control_blocks9.c
		Added debug levels for enhanced debugging.  The debug level is in vtg.control_debug_level.  Setting
		this to zero disables debugging.  When a debug level is set, messages will be sent out of the serial 
		port.
		
5-16-10 combined_control_blocks10.c
		Fixed pressure control blocks such that if a follow through signal is not sensed, switching to manual mode will kill the block.

		In the pressure control block, if the time were blank as it would be on the last pressure step without any time entered, the 
		time would be set to zero and there would be no 10 second timeout.  Changed the code so that if the time is seen as zero, it's
		changed to 10000 (10 seconds).	
		
7-12-10  V11, Elminated hires support		
*/



#ifndef NULL
 #define NULL 0
#endif

#define COND_EXE_DEBUG 0
#define DWELL_EXE_DEBUG 0
#define JUMP_EXE_DEBUG 0
#define UPDATE_EXE_DEBUG 0
#define VEL_EXE_DEBUG 0		// This could cause the WD timer to timeout resetting the board
#define LINEAR_EXE_DEBUG 0
#define PRESS_EXE_DEBUG 0
#define EXT_LOOP_DEBUG 0	// Ditto
#define PRESS_EXT_LOOP_DEBUG 0

#define BEGINNING 0
#define DURING 1
#define AT_END 2

// Operators
#define NO_OP 0
#define EQ_OP 1
#define NOT_EQ_OP 2
#define GR_THAN_OP 3
#define LESS_THAN_OP 4
#define GR_THAN_EQ_OP 5
#define LESS_THAN_EQ_OP 6
#define PLUS 7
#define MINUS 8
#define MUL 9
#define DIV 10
#define AND 11
#define OR 12
#define XOR 13

// Jump types
#define TYPE_NORMAL 0
#define TYPE_SUB 1
#define TYPE_RETURN 2
#define TYPE_REL 3


#define CFUNCTION_TRACE 0

// Macros
#if CFUNCTION_TRACE
	#define CPL \
				int zstr_ctr; \
				 \
				PRINT_CHAR('['); \
				PRINT_CHAR('C'); \
				PRINT_STR(TOSTRING(__LINE__)); \
				PRINT_CHAR(']');
#else
	#define CPL { }
#endif


#define SCAN_FOR_COND_SET_IO \
		cond_ptr=0; set_io_ptr=0; \
		while(*ptr) \
			{ \
			if(*ptr=='_') break; \
			ptr++; \
			} \
		if (*ptr=='_') /* Keep going, we found the test/set I/O delimiter. */ \
			{ \
			while(*ptr) \
				{ \
				if(*ptr=='B' || *ptr=='d' || *ptr=='E' || *ptr =='R') {set_io_ptr=ptr; break;} \
				if(!cond_ptr && (*ptr=='I' || *ptr=='W')) cond_ptr=ptr; /* Keep going if we find a conditional statement, there may still be a set I/O */ \
				ptr++; \
				} \
			}


#define SET_UP_SET_OUTPUTS \
	if (set_io_ptr) \
			{ \
			ptr = set_io_ptr; \
			if(*ptr=='B') \
				{ \
				ptr++; \
				turn_on_off = *ptr - '0'; \
				ptr+=2; \
				if(*ptr=='V') \
					{ \
					ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;} \
					set_beg = vtg.v[tempi]; \
					} \
				else set_beg = Atoi(ptr); \
				} \
			else if(*ptr=='d' || *ptr=='R') \
				{ \
				ptr++; \
				turn_on_off = *ptr - '0'; \
				ptr+=2; \
				if(*ptr=='V') \
					{ \
					ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;} \
					set_dur = vtg.v[tempi]; \
					} \
				else set_dur = Atoi(ptr); \
				} \
			else if(*ptr=='E') \
				{ \
				ptr++; \
				turn_on_off = *ptr - '0'; \
				ptr+=2; \
				if(*ptr=='V') \
					{ \
					ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;} \
					set_end = vtg.v[tempi]; \
					} \
				else set_end = Atoi(ptr); \
				} \
			}


#define SET_OUTPUTS_BEG_DUR \
		\
		SET_UP_SET_OUTPUTS; /* First set up the appropriate variables */ \
		\
		if (set_beg) \
			{ \
			set_clr_discrete_output(set_beg, (int)turn_on_off); /* If turn_on_off == zero, we'll turn off */ \
			} \
		else if (set_dur) \
			{ \
			set_clr_discrete_output(set_dur, (int)turn_on_off); \
			}

#define SET_OUTPUTS_DUR_END \
		if (do_set_output) \
			{ \
			if (set_dur) \
				{ \
				/* First invert turn_on_off since we want to do the opposite of what we did at the beginning of the block */ \
				if (turn_on_off==0) turn_on_off=1; \
				else turn_on_off=0; \
				set_clr_discrete_output(set_dur, (int)turn_on_off); \
				} \
			else if (set_end) \
				{ \
				set_clr_discrete_output(set_end, (int)turn_on_off); /* If turn_on == zero, we'll turn off */ \
				} \
			}

#define INITIALIZE_CONTROL_BLOCK \
		set_dur=0; set_beg=0; set_end=0; \
		return_val = 0; \
		saved_execution_type = type_of_execution; \
		do_set_output=0; \
		saved_control_block = control_block; \
		ptr = (char *)control_block; ptr++; /* Move past the block ID */ \
		/* Scan for conditional string and set I/O string */ \
		SCAN_FOR_COND_SET_IO; /* cond_ptr and set_io_ptr will be set here */ \
		if (cond_ptr) cond_result = block_conditional(control_block, cond_ptr); \
		else cond_result = KEEP_GOING_DONT_CALL_BACK; /* No conditions */




int end_block(c_block control_block, int type_of_execution, int zone) // ALL PASSED PARAMETERS ARE IGNORED!!!
	{
	int axis_num=1;
	CPL;

	STOP_CONTROL;
	return(0);
	//return (PROGRAM_STOPPED_BY_USER); // This will trigger a fatal error
	}


// This is used to start the control program running.  It is never executed as part of
// a control program.  All passed parameters are ignored.  This function is called from 
// within a control interrupt, therefore the function framework mimics a control block exe.
int start_block(c_block control_block, int type_of_execution, int zone)
		{
		int x, axis_num=1;
		//int zstr_ctr; // For trace
		CPL;
	
		// ALL PASSED PARAMETERS ARE IGNORED!!!

		// Also need to clear the watchdog output and the warning output here	
		SET_PROGRAM_RUNNING;
		CLR_FATAL_ERROR;
		CLR_WARNING;
		//OSW1 &= TURN_OFF_CONT_OUTPUTS_EXCEPT_LS_WARN_MASK; 
		//TURN_OFF_OUTPUTS_EXCEPT_LS_WARN;
		OSW1 = 0;
		OSW2 = 0;
		OSW3 = 0;
		WARNING_WORD=0;
		FATAL_ERROR_WORD=0;	
		
		//out_dac(CONT_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE);
		out_dac(AXIS1_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 1 to the retract setting
		axis_num=2;
		// If there is not a valid DAC channel specified for axis2, out_dac() will not do anything
		out_dac(AXIS2_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 2 to zero
		
		// Need to disable WD and warning output here.  If a warning presently exists, well leave it.
		if (vtg.saved_warning == 0) {set_clr_discrete_output(WARNING_OUTPUT, TURN_ON);} // Disable warning by turning on
		PULSE_WATCHDOG_RESET;
		update_control_parameters();
		//#if SCI_DEBUG
		//	print_again = nr_timer_milliseconds() + 1000;
		//#endif

		// From here we'll go down to the regular control supervisor and begin
		// executing blocks.
		return (0);
		}



// Execute dwell block
int dwell_block(c_block control_block, int type_of_execution, int zone)
	{
	static c_block saved_control_block;
	static int cond_result, return_val, saved_execution_type, do_set_output;
	//int input_state; // For test
	unsigned int dwell_time_mS=0, timer_load_value;
	volatile unsigned short int *halfword_ptr;
	int tempi;
	//
	static int set_dur, set_beg, set_end;
	static char *cond_ptr, *set_io_ptr, turn_on_off;
	char *ptr;
	CPL;

	if (zone == 1) // We're executing the block from the beginning.
		{
		INITIALIZE_CONTROL_BLOCK;
						
		#if DWELL_EXE_DEBUG
			printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
		#endif

		if (vtg.control_program_trace) printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);

		ptr = (char *)control_block; ptr++; // Move past the block ID

		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			#if DWELL_EXE_DEBUG
				printf("Dwell blk exe-Some sort of CONDITIONAL exe error\n");
			#endif	

			return_val = CONDITIONAL_EXECUTION_ERROR;
			} 
		else if (cond_result < 20) // OK to proceed, WHILE interrupt may have been armed if BUILD_FOR_INTERRUPT_CONTROL. 
			{
			#if DWELL_EXE_DEBUG
				printf("Dwell blk exe, zone 1-OK to proceed, blk#%d\n", vtg.prog_ctr);
			#endif	
			// This is the main body of the dwell block	

			// Set/clr any outputs here.  This will also initialize and set the corrrect variables.
			SET_OUTPUTS_BEG_DUR;
			ptr = (char *)control_block; ptr++; // Relocte ptr to point to the beginning of the block

			if (*ptr == 'V') 
				{
				ptr++; tempi = Atoi(ptr);
				if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				else dwell_time_mS = (unsigned int)vtg.v[tempi];
				}
			else dwell_time_mS = (unsigned int)Atoi(ptr);

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

			do_set_output=1; // D0 block, dwell with zero time.
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
				if (block_conditional(control_block, cond_ptr) == KEEP_GOING_CALL_BACK_DURING);
				else
					{
					zone = WHILE_CONDITIONAL_FAILED;
					break;    // Condition was not met.
					}
				}
			}
		#endif

		#if DWELL_EXE_DEBUG
			printf("Dwell blk exe-Zone 2, timeout has occured or while condition failed (polled mode).\n");
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
	//printf("DWELL set_end = %d and turn_on_off = %d\n", set_end, turn_on_off);
	SET_OUTPUTS_DUR_END;

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




// Execute velocity block
int velocity_block(c_block control_block, int type_of_execution, int zone)
	{
	static int accel, loop_num, starting_pos, delta_pos_zspeed, prev_pos, regular_dac_channel;
	static int saved_execution_type, vac_wait, vac_on, do_set_output, fill_test;
	static int axis_num, commanded_raw_dac, first_loop, dac_increment, accel_starting_pos;
	static int return_val, goose, cond_result, closed_loop, position, vloop_gain;
	static short int commanded_vel;
	static c_block saved_control_block;
	int temp_pos, input_state, temp_dac_command;
	int current_raw_dac, next_pos, tempi;
	long dac_output;
	// Fixed point decimal numbers
	fixed4 act_vel, dv, tempf4; 
	static fixed4 commanded_vel_fixed4, saved_commanded_vel, accel_fixed4;
	static fixed4 accel_increments, accel_starting_vel;
	//
	//unsigned int initial_timer_mS, send_op_time;
	unsigned int tempu, loop_time;
	unsigned int initial_timer_mS=0; // For polled control
	volatile unsigned short int *halfword_ptr;
	//
	static int set_dur, set_beg, set_end;
	static char *cond_ptr, *set_io_ptr, turn_on_off;
	char *ptr;
	CPL;

	if (zone == 1) // Initial entry of block
		{
		INITIALIZE_CONTROL_BLOCK;
		
		// Check if vac_on and fill test
		chk_discrete_input(VAC_ON_INPUT, &input_state);	
		vac_on = input_state; // 1 if on 0 if off
		chk_discrete_input(FILL_TEST_INPUT, &input_state);	
		fill_test = input_state;
		accel = 0;
		commanded_raw_dac = 0;
		first_loop=1;
		goose=0;
		closed_loop=1;
		vac_wait=0;
		tempu = 0;
		 		
		if (vtg.control_debug_level==2) printf("DBG V B%d Z%d %s\n", vtg.prog_ctr, zone, (char *)control_block);
		
		if (vtg.control_program_trace) printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
		
		ptr = (char *)control_block; ptr++; // Move past the block ID

		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			if (vtg.control_debug_level) printf("DBG V B%d Z%d COND_RESULT%d BLK EXE ERR!!!\n", vtg.prog_ctr, zone, cond_result); // Print this for any debug level
			
			return_val=CONDITIONAL_EXECUTION_ERROR;	
			} 
		else if (cond_result < 20) // OK to proceed.
			{
			if (vtg.control_debug_level==2) printf("DBG V B%d Z%d OK to preceed\n", vtg.prog_ctr, zone);
			
			// Set/clr any outputs here.  This will also initialize and set the corrrect variables.
			SET_OUTPUTS_BEG_DUR;
			ptr = (char *)control_block; ptr++; // Relocate ptr to point to the beginning of the block
			
			/////////////////// Initial setups ////////////////////
			//initial_timer_mS = nr_timer_milliseconds();
			//send_op_time = initial_timer_mS + 500; // Time to send first op data packet

			//print_again = initial_timer_mS+750; // For test
			
			// First get the axis_num
			ptr++;
			if (*ptr == '2') axis_num = 2;
			else axis_num = 1;
			if (vtg.control_debug_level==2) printf("DBG V B%d Z%d Velocity axis # is %d\n", vtg.prog_ctr, zone, axis_num);
			
			//regular_dac_channel = DAC_OUTPUT_CHANNEL(axis_num); // regular_dac_channel used for DAC override
			
			// Get the velocity setting
			ptr++;
			if (*ptr=='V')
				{
				ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;} 
				tempu = (unsigned int)vtg.v[tempi];
				}
			else if (*ptr=='H' || *ptr=='h') // Hex value
				{
				ptr++; tempu=htoi(ptr);  
				}
			else tempu = (unsigned int)Atoi(ptr);
			
			if (tempu & 0x80000000)
				{
				closed_loop=0; // Open loop
				if (vtg.control_debug_level==2) printf("DBG V B%d Z%d This is an open loop step\n", vtg.prog_ctr, zone);
				}

			if (tempu & 0x40000000)
				{
				vac_wait=1; // This is a vacuum wait step.
				if (vtg.control_debug_level==2) printf("DBG V B%d Z%d This is a vacuum wait step\n", vtg.prog_ctr, zone);
				}

			commanded_vel = (short int)tempu; // Velocity cmd is the lower half word, can be pos. or neg.
			if (commanded_vel == 0x7fff) // Skip entire block without any test inputs/set outputs.
				{
				if (vtg.control_debug_level==2) printf("DBG V B%d Z%d Found skip code, skipping entire vel blk\n", vtg.prog_ctr, zone);
				return_val = NO_PROBLEM;
				goto ext;
				} 
				
			// If open loop, convert commanded velocity from voltage to raw DAC value.
			if (!closed_loop)
				commanded_raw_dac = (unsigned int)(8192 + ((819 * (int)commanded_vel)/1000));
			
			tempu = 0; position = 0;
			// Parse for 'A' (accel) 'D' (DAC override) or 'P' ending position
			while (*ptr != NULL)
				{
				if (*ptr == '_') break; // Can't be a 'P' or a 'A' after this point
				if (*ptr == 'A') // An accel was found
					{
					ptr+=2;
					if (*ptr=='V') 
						{
						ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
						tempu = (unsigned int)vtg.v[tempi];
						}
					else if (*ptr=='H' || *ptr=='h') // Hex value
						{
						ptr++; tempu=htoi(ptr);  
						}	
					else tempu = (unsigned int)Atoi(ptr);
					}
				//if (*ptr=='D') // DAC channel override
				//	{
				//	regular_dac_channel = DAC_OUTPUT_CHANNEL(axis_num);
				//	ptr+=3;
				//	if (*ptr=='V')
				//		{
				//		ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				//		tempi = vtg.v[tempi];
				//		}
				//	else tempi = Atoi(ptr);
				//	
				//	if (tempi>0 && tempi<=NO_OF_DAC_CHANNELS) // Valid DAC override
				//		{
				//		DAC_OUTPUT_CHANNEL(axis_num) = tempi;
				//		}
				//	}
				if (*ptr == 'P')
					{
					ptr++;
					if (*ptr=='V') 
						{
						ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
						position = vtg.v[tempi];
						}
					else position = Atoi(ptr);

					if (vtg.control_debug_level==2) printf("DBG V B%d Z%d Ending pos is %d\n", vtg.prog_ctr, zone, position);

					break; // If we have the ending position, there's no more to do.
					}
					
				ptr++;
				} 
			
			if (tempu)
				{
				if (tempu & 0x80000000) 
					{
					if (vtg.control_debug_level==2) printf("DBG V B%d Z%d Goose detected\n", vtg.prog_ctr, zone);
   					goose = tempu & 0x0000ffff; // Goose value in volts X 1000, always assumed to be positive.
					}
				else
					{
					if (vtg.control_debug_level==2) printf("DBG V B%d Z%d Accel detected\n", vtg.prog_ctr, zone);
   					accel = tempu & 0x0000ffff; // Accel in X1 counts, always assumed to be positive.
					}
				}

			delta_pos_zspeed = (VEL_SET_EOS*4)/(1000000/SAMPLE_TIME_SETTING(axis_num)); // Change in X4 position counts @ EOS velocity
			if (delta_pos_zspeed<1) delta_pos_zspeed=1; // Force to at least 1
			prev_pos = RD_PV_POS(axis_num);

					
			
			///////////////////////// Reset/update the PID parameters ////////////////////////
			// These items are set in an update block.  These need to be set once before each shot.
			//PID_I_STATE(axis_num) = 0; 
			//SERVO_HERE(axis_num)=0; 	
			
			PID_LOOP_TYPE(axis_num) = VEL_LOOP;
			SERVO_HERE(axis_num) = 0; // If we have position control trying to hold a position, we'll release it here.  Redundant, but we'll do it again here.
			PID_D_STATE(axis_num) = FIXED4_DIV((axis[axis_num].vel_ctr_freq*10000LL), (RD_PV_VEL(axis_num)*10000LL)); // Set d state to the current velocity
			
			// Syncronize the PID's output with the current DAC output.  Do this at the beginning of each
			// velocity block as the previous block may have been an open loop step that did not update the PID.
			get_dac(DAC_OUTPUT_CHANNEL(axis_num), RAW_VAL, (int *)&tempi);
			//printf("[%d*%d*%d*%d]", tempi, vtg.dac_word[1], DAC_OUTPUT_CHANNEL(axis_num), axis_num);
			PID_OUTPUT(axis_num) = LONG_TO_FIXED4(tempi);
						
			// DAC range
			PID_O_MAX(axis_num) = LONG_TO_FIXED4(16383);  
			PID_O_MIN(axis_num) = 0;
			
			// Set the integrator limits to the O_MAX and O_MIN settings for now
			//PID_I_MAX(axis_num) = 5000; 
			//PID_I_MIN(axis_num) = -5000; 
			PID_I_MAX(axis_num) = LONG_TO_FIXED4(50000); 
			PID_I_MIN(axis_num) = LONG_TO_FIXED4(-50000); 

			// All gains fixed4 as is, no need to convert anything.
			// P gain
			PID_P_GAIN(axis_num) = VEL_LOOP_GAIN(axis_num);

			// I gain
			if (VEL_LOOP_INT_GAIN(axis_num) == 1) {PID_RECURSIVE_INTEGRAL(axis_num) = 1;} // Use recursive integral
			else {PID_RECURSIVE_INTEGRAL(axis_num) = 0; PID_I_GAIN(axis_num) = VEL_LOOP_INT_GAIN(axis_num);} 	

			// D gain
			PID_D_GAIN(axis_num) = VEL_LOOP_DERIV_GAIN(axis_num);
						
			// Calling the pid
			// update_pid(&PID_STRUCT(axis_num), (long)error, (long)CV);
			/////////////////////////////////////////////////////////////////////////////////	
			

			// Determine what zone to go to next here
			if (closed_loop)
				{
				if (goose) zone = 2;
				else if (accel) zone = 3; 
				else zone = 4;
				}
			else  // Open loop
				{
				// If we're open loop and no position is specified, this is probably a low impact step.
				// In that case we'll check to see if a binary valve has been selected, if so we'll generate
				// the necessary pulse.
				if (!position && VALVE_TYPE(axis_num)==BINARY_VALVE) 
					{
					zone=10; // Go to the binary valve section
					}
				else
					{
					if (accel) zone = 5;   
					else zone = 6;
					}
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
			
			if (vtg.control_debug_level==2) printf("DBG V B%d Z%d COND_RESULT%d Cond not met, exiting block\n", vtg.prog_ctr, zone, cond_result); 
			}
		else
			{
			// This should not happen - if it does, deal with it here.
			if (vtg.control_debug_level) printf("DBG V B%d Z%d BLK EXE ERR!!!\n", vtg.prog_ctr, zone); // Print this for any debug level
			return(BLOCK_EXECUTION_ERROR); // This will trigger a fatal error
			}
		} // zone 1 

	if (zone == 2) // Goose
		{
		#include "LOOP_EXIT_CONDITIONS4.H" // Check if we need to abort for some reason
      	//out_dac(CONT_DAC_CHANNEL, goose, VOLTAGE_X_1000, ABSOLUTE);
		out_dac(DAC_OUTPUT_CHANNEL(axis_num), goose, VOLTAGE_X_1000, ABSOLUTE); // Goose it here

		// Update the PID parameters based on our goose setting
		get_dac(DAC_OUTPUT_CHANNEL(axis_num), RAW_VAL, (int *)&tempi);
		PID_OUTPUT(axis_num) = LONG_TO_FIXED4(tempi);

		if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d Goose ended\n", vtg.prog_ctr, zone);
		zone=4; // Move to zone 4 and begin closing the velocity loop
		}	
	
	if (zone == 3) // Closed loop accel
		{
		// Major changed closed loop accel.  Accel is now time based, however the change in command is still
		// dependent on the position changing.  Every loop time, the pos at the start of the accel is subtracted from
		// the current position, and then divided by 4 to convert to X1 counts.  This is then added to the actual vel at 
		// the start of the accel to create our new commanded vel.  We then go to step 4 to close the loop.
		// Break gains for the accel are defined here in the first loop.

		if (first_loop)
			{
			if (vtg.control_debug_level==2) printf("DBG V B%d Z%d Closed loop accel, first loop\n", vtg.prog_ctr, zone);

			first_loop=0;
							
      		// Closed loop acceleration defined in X1 pps per X1 count
      		loop_num = 100;  // For diagnostics 
	
			commanded_vel_fixed4=LONG_TO_FIXED4(((fixed4)commanded_vel));
			accel_fixed4=LONG_TO_FIXED4(accel);
			accel_starting_pos=RD_PV_POS(axis_num);
			act_vel=FIXED4_DIV((axis[axis_num].vel_ctr_freq*10000LL), (RD_PV_VEL(axis_num)*10000LL));
			
      		// First determine vel change per increment. 
			// accel_increments = (commanded_vel_fixed4 - act_vel) / accel_fixed4
      		accel_increments = FIXED4_SUB(commanded_vel_fixed4, act_vel);  // Change in vel per X1 counts
			accel_increments = FIXED4_DIV(accel_increments, accel_fixed4);
			//if (accel_increments<0) printf("AI f4 = %d, AC F4=%d\n", accel_increments, accel_fixed4);
						
			///////////////////////// Old gain break code, enable this for old style gain break
			//
			// Now check gain breaks.  During an accel, gain break is applied to commanded velocity.
			// During  decel, it's applide to actual velocity.  In either case, the gain chosen here
			// remains in effect for the entire accel or decel.
			//vloop_gain = VEL_LOOP_GAIN(axis_num);
			//PID_D_GAIN(axis_num) = VEL_LOOP_DERIV_GAIN(axis_num);
			//if (accel_increments>0)
			//	{
			//	if (commanded_vel >= VEL_LOOP_BK_VEL3(axis_num))
			//		{vloop_gain = (vloop_gain * VEL_LOOP_BKGN3(axis_num))/1000; PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN3(axis_num))/1000;}
			//	else if (commanded_vel >= VEL_LOOP_BK_VEL2(axis_num))
			//		{vloop_gain = (vloop_gain * VEL_LOOP_BKGN2(axis_num))/1000;  PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN2(axis_num))/1000;}
			//	else if (commanded_vel >= VEL_LOOP_BK_VEL1(axis_num))
			//		{vloop_gain = (vloop_gain * VEL_LOOP_BKGN1(axis_num))/1000;  PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN1(axis_num))/1000;}
			//	}
			//else if (accel_increments<0)
			//	{
			//	if (act_vel >= VEL_LOOP_BK_VEL3(axis_num))
			//		{vloop_gain = (vloop_gain * VEL_LOOP_BKGN3(axis_num))/1000; PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN3(axis_num))/1000;}
			//	else if (act_vel >= VEL_LOOP_BK_VEL2(axis_num))
			//		{vloop_gain = (vloop_gain * VEL_LOOP_BKGN2(axis_num))/1000; PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN2(axis_num))/1000;}
			//	else if (act_vel >= VEL_LOOP_BK_VEL1(axis_num))
			//		{vloop_gain = (vloop_gain * VEL_LOOP_BKGN1(axis_num))/1000; PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN1(axis_num))/1000;}
			//	}
			//////////////////////////////////////////////////////////////////////////////////////

			saved_commanded_vel = commanded_vel_fixed4;
			accel_starting_vel = act_vel;
			
			// Now set up timer.  We'll still close the loop every loop time.
			loop_time = SAMPLE_TIME(axis_num);  
			halfword_ptr = (unsigned short int *)&loop_time;
			
			// Arm for time interrupt, reenter at zone 3
			if (accel_increments)
				{
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 3);
				
				return(REENTER_ON_INTERRUPT); // We'll wait for our first interrupt before beginning to control accel.
				}
			else
				{
				first_loop=1;
				commanded_vel_fixed4 = saved_commanded_vel;
				accel_fixed4=0; // Indicate to zone 4 that the accel has completed.
				goto end_of_z3;
				}
			}
		
		// We'll just keep updating the commanded velocity.  The main loop in zone 4 will close the loop.
		// Even in closed loop accel, we want to update the DAC on fixed time increments.
		temp_pos = (RD_PV_POS(axis_num) - accel_starting_pos) / 4; // Determine how many X1 increments we've moved.
		//if (temp_pos<1) temp_pos=1;  // Force to at least 1.  If this is a first loop, it could be zero.
		commanded_vel_fixed4=FIXED4_MUL(accel_increments, (temp_pos*10000)); // Temp. convert temp_pos to a fixed4
		commanded_vel_fixed4=FIXED4_ADD(commanded_vel_fixed4, accel_starting_vel);
		//printf("[%d] ", commanded_vel);
				
		//if (accel_increments<0) printf("[cvF4%d p%d]", commanded_vel_fixed4, RD_PV_POS(axis_num)); // For test
				
		if ((accel_increments > 0 && commanded_vel_fixed4 >= saved_commanded_vel) || (accel_increments < 0 && commanded_vel_fixed4 <= saved_commanded_vel)) // Finished accel, set up for closed loop velocity.  Zone 4 will execute first loop.
			{
			DISARM_TIMER2_CONT_INTERRUPT;
			first_loop=1;
			commanded_vel_fixed4 = saved_commanded_vel;
			if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d CL acc ended\n", vtg.prog_ctr, zone);
			}
		
		end_of_z3: ;
		zone = 4; // Proceed to close the loop
		}
		
	if (zone == 4) // Closed velocity loop
		{
		if (first_loop)
			{
			first_loop=0;
			loop_time = SAMPLE_TIME(axis_num);
			commanded_vel_fixed4=LONG_TO_FIXED4(((fixed4)commanded_vel));
			
			if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d LT%d Closing vel loop\n", vtg.prog_ctr, zone, loop_time);
			
   			loop_num = 300;  // For diagnostics 
			//printf("P_gn=%d, I_gn=%d, D_gn=%d, ST=%d\n", PID_P_GAIN(1), PID_I_GAIN(1), PID_D_GAIN(1), loop_time);	
				
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
		act_vel=FIXED4_DIV((axis[axis_num].vel_ctr_freq*10000LL), (RD_PV_VEL(axis_num)*10000LL)); // Vel in pps
		dv = commanded_vel_fixed4 - act_vel;
		
		//printf("cv=%d, av=%d, dv=%d\n", commanded_vel_fixed4, act_vel, dv);
		temp_pos = RD_PV_POS(axis_num); // Get current position

		///////////////// Old gain break code, enable this for old style gain break
		//if (!accel) // Otherwise gain is set in zone 3
		//	{
		//	// Now check gain breaks.
		//	vloop_gain = VEL_LOOP_GAIN(axis_num);
		//
		//	if (temp_pos > 240)
		//		{
		//		if (act_vel >= VEL_LOOP_BK_VEL3(axis_num))
		//			{vloop_gain = (vloop_gain * VEL_LOOP_BKGN3(axis_num))/1000; PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN3(axis_num))/1000;}
		//		else if (act_vel >= VEL_LOOP_BK_VEL2(axis_num))
		//			{vloop_gain = (vloop_gain * VEL_LOOP_BKGN2(axis_num))/1000; PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN2(axis_num))/1000;}
		//		else if (act_vel >= VEL_LOOP_BK_VEL1(axis_num))
		//			{vloop_gain = (vloop_gain * VEL_LOOP_BKGN1(axis_num))/1000; PID_D_GAIN(axis_num)=(VEL_LOOP_DERIV_GAIN(axis_num)*VEL_LOOP_BKGN1(axis_num))/1000;}
		//		}
		//	}
		//
		//PID_P_GAIN(axis_num) = vloop_gain;  
		////////////////////////////////////////////////////////////////////////////
		

		////////////// New gain break code
		// P gain initialize
		//vloop_gain = VEL_LOOP_GAIN(axis_num);
		PID_P_GAIN(axis_num)=VEL_LOOP_GAIN(axis_num);
		// D gain initialize
		PID_D_GAIN(axis_num)=VEL_LOOP_DERIV_GAIN(axis_num);
		// I gain initialize, if int gain set to 1, then we're using a recursive integral.
		if (VEL_LOOP_INT_GAIN(axis_num) != 1) {PID_I_GAIN(axis_num) = VEL_LOOP_INT_GAIN(axis_num);} 	

		// Determine a floating gain break that increases linearly with velocity.
		// Also increase the derivative linearly as the velocity increases
		if (VEL_LOOP_BKGN1(axis_num) > 1000) // Break gain > 1
			{
			// First subtract out a gain of 1 and convert to a fixed4 by multiplying by 10.
			tempf4=FIXED4_DIV(((VEL_LOOP_BKGN1(axis_num)-1000)*10), (VEL_LOOP_BK_VEL1(axis_num)*10000)); //First derive the multiplier to multiply the ct_vel by
			tempf4=FIXED4_MUL(tempf4, act_vel);
			tempf4=FIXED4_ADD(tempf4, 10000); // Add back in a gain of 1.0000.  Now tempi is the gain multiplier as a fixed4 number
			
			//vloop_gain=FIXED4_MUL(tempf4, vloop_gain);
			PID_P_GAIN(axis_num)=FIXED4_MUL(tempf4, VEL_LOOP_GAIN(axis_num));
			PID_D_GAIN(axis_num)=FIXED4_MUL(VEL_LOOP_DERIV_GAIN(axis_num), tempf4);
			if (VEL_LOOP_INT_GAIN(axis_num) != 1) {PID_I_GAIN(axis_num)=FIXED4_MUL(VEL_LOOP_INT_GAIN(axis_num), tempf4);}
			//printf("vlg=%d, d_term=%d\n", vloop_gain, PID_D_GAIN(axis_num));
			}		
		/////////////////////////////////
		
		
		// PID_P_GAIN(axis_num) = vloop_gain;

		update_pid(&PID_STRUCT(axis_num), (fixed4)dv, (fixed4)act_vel);  // This will update PID_OUTPUT(axis_num)
		dac_output=FIXED4_TO_LONG(PID_OUTPUT(axis_num));  // convert fixed4 to long
		
		// Put dither code here. 
		out_dac(DAC_OUTPUT_CHANNEL(axis_num), dac_output, RAW_VAL, ABSOLUTE);
		//printf("[%d, %d] ", temp_pos, dac_output);
		
		#if BUILD_FOR_INTERRUPT_CONTROL 
			#include "LOOP_EXIT_CONDITIONS4.H"
			return(REENTER_ON_INTERRUPT);
		#else 
			do // Wait for next sample 
	     		{
	     	
	     		#include "LOOP_EXIT_CONDITIONS4.H"
			
	     		}while (!vtg.timer2_timeout);  // Wait for next sample 

			goto vel_loop; // Loop until something in LOOP_EXIT_CONDITIONS4.H kicks us out.
		#endif
		}

	if (zone == 5) // Open loop accel
		{
		if (first_loop)
			{
			if (vtg.control_debug_level==2) printf("DBG V B%d Z%d Open loop accel, first loop\n", vtg.prog_ctr, zone);
			first_loop=0;
			starting_pos = RD_PV_POS(axis_num);
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
			get_dac(DAC_OUTPUT_CHANNEL(axis_num), RAW_VAL, &current_raw_dac); 
      		dac_increment = (commanded_raw_dac - current_raw_dac) / accel; // Remember, commanded_vel is an OL percentage here.

      		if (dac_increment == 0)  // Don't bother controlling accel 
				{
				if (vtg.control_debug_level==2) printf("DBG V B%d Z%d dac_increment=0, no accel\n", vtg.prog_ctr, zone);
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
		temp_pos = RD_PV_POS(axis_num);
		#if !BUILD_FOR_INTERRUPT_CONTROL 
			next_pos = temp_pos+4; // For polled control, X1 pulse
		#endif
		//#if VEL_EXE_DEBUG
		//	printf("dac_increment=%d, accel=%d, commanded_raw_dac=%d, cur_raw_dac=%d\n", dac_increment, accel, commanded_raw_dac, current_raw_dac);
		//#endif
		
				
		// Add dac_increment to current DAC voltage
		//out_dac(CONT_DAC_CHANNEL, dac_increment, VOLTAGE_X_1000, INCREMENTAL);
		//out_dac(CONT_DAC_CHANNEL, dac_increment, RAW_VAL, INCREMENTAL);
		out_dac(DAC_OUTPUT_CHANNEL(axis_num), dac_increment, RAW_VAL, INCREMENTAL);

		#if BUILD_FOR_INTERRUPT_CONTROL
			#include "LOOP_EXIT_CONDITIONS4.H"
		#else
			do
				{
				
				#include "LOOP_EXIT_CONDITIONS4.H"

				}while ((int)RD_PV_POS(axis_num) < next_pos);  // Wait for next sample 
		#endif

		if (--accel == 0) // Finished accel, set up for closed loop velocity
			{
			#if BUILD_FOR_INTERRUPT_CONTROL
				DISARM_X1_PULSE_CONT_INTERRUPT;
			#endif
			first_loop=1;
			zone=6;
			if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d OL acc ended\n", vtg.prog_ctr, zone);
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

			if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d DAC%d Open loop control\n", vtg.prog_ctr, zone, commanded_raw_dac);
					
			out_dac(DAC_OUTPUT_CHANNEL(axis_num), commanded_raw_dac, RAW_VAL, ABSOLUTE);
			}

		
		//#if VEL_EXE_DEBUG
		//	printf("vel blk exe-zone 6, ol step, blowing on through, we're in block = %d\n\n", vtg.prog_ctr);
		//#endif

		#if BUILD_FOR_INTERRUPT_CONTROL
			// Assuming a ZSPEED has not occured, we'll just exit here and wait for another interrupt
			vtg.timer2_timeout = 0; // Not using this for anything but should clear it anyway
			
			//printf("[%d] ", RD_PV_POS(axis_num)); // for test
			//printf("[$1 %d]", DAC_OUTPUT_CHANNEL(1));
			#include "LOOP_EXIT_CONDITIONS4.H"
			//printf("[$2 %d]", DAC_OUTPUT_CHANNEL(1));
			return(REENTER_ON_INTERRUPT);
		#else // Polled control
			do  // Wait for something to terminate the block 
	    		{
	     		
	     		#include "LOOP_EXIT_CONDITIONS4.H"

	     		}while (1);  // Loop until something terminates the loop 
		#endif
   		}

	///////////////////// Block ending zones, interrupt control only  ///////////////////////////////
	if (zone == 7) // 10 second block timeout.
		{
		if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d 10 sec timeout\n", vtg.prog_ctr, zone);
		return_val = BLOCK_TIMEOUT; // This will trigger a warning
		}

	if (zone == 8) // Ending position reached
		{
		// If vac wait and fill_test, halt plunger movement.
		if (vac_wait && fill_test && !vac_on)
			{
			if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d P%d Fill tst pos reached\n", vtg.prog_ctr, zone, RD_PV_POS(axis_num));	
			return_val = FILL_TEST_ABORT; // This will trigger a warning
			}
		else
			{
			// Block's done
			if (vtg.control_debug_level==1||vtg.control_debug_level==2)
				{
				act_vel=FIXED4_DIV((axis[axis_num].vel_ctr_freq*10000LL), (RD_PV_VEL(axis_num)*10000LL));
				act_vel /= 10000; // Convert vel to X1 pps
				printf("DBG V B%d Z%d P%d V%d C%d End pos reached\n", vtg.prog_ctr, zone, RD_PV_POS(axis_num), act_vel, vtg.dac_word[1]);
				}

			do_set_output=1;		
			}
		}

	if (zone == 9) // Vac abort, moved 3" past ending position.
		{
		if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d P%d Vac abrt - cyl moved 3\" past EP\n", vtg.prog_ctr, zone, RD_PV_POS(axis_num));

		return_val = VAC_ABORT_POS_TOO_LONG; // This will trigger a warning
		}

	if (zone == 10) // Binary valve pulse
		{
		if (first_loop==1)
			{
			BINARY_VALVE_ON;
			WRITE_OSW1;  // Write it here for speed.  We don't want the lag time by waiting for 1mS routine to write OSW1
			if (BINARY_VALVE_DEVENT_TIME > 1000) // At least 1 mS set, otherwise we'll ignore
				{
				convert_uS_to_timer(BINARY_VALVE_DEVENT_TIME, &tempu);	
				halfword_ptr = (unsigned short int *)&tempu;
				first_loop=2;
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], ONCE, 10);
				return(REENTER_ON_INTERRUPT);
				}
			else if (BINARY_VALVE_PULSE_TIME > 1000) // Also at least 1 mS
				{
				out_dac(DAC_OUTPUT_CHANNEL(axis_num), commanded_raw_dac, RAW_VAL, ABSOLUTE); // Forget about devent time, Cock proportional valve for low impact
				convert_uS_to_timer(BINARY_VALVE_PULSE_TIME, &tempu);	
				halfword_ptr = (unsigned short int *)&tempu;
				first_loop=0;
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], ONCE, 10);
				return(REENTER_ON_INTERRUPT);
				}
			else  // Niether an acceptable devent time or binary valve pulse time were entered
				{
				out_dac(DAC_OUTPUT_CHANNEL(axis_num), commanded_raw_dac, RAW_VAL, ABSOLUTE); // Output LI setting to proportional valve
				}
			}
		else if (first_loop == 2) // Devent timed out, output low impact setting to proportional valve
			{
			vtg.timer2_timeout = 0; // Not using this for anything but should clear it anyway
			first_loop = 0;
			out_dac(DAC_OUTPUT_CHANNEL(axis_num), commanded_raw_dac, RAW_VAL, ABSOLUTE); // Output LI setting to proportional valve
			
			// Set timer to the difference between the binary valve pulse time and the devent time.
			// If valid times have not been set, we'll turn of the binary valve pulse and exit.
			if ((BINARY_VALVE_PULSE_TIME-BINARY_VALVE_DEVENT_TIME) > 1000)
				{
				convert_uS_to_timer(BINARY_VALVE_PULSE_TIME-BINARY_VALVE_DEVENT_TIME, &tempu);	
				halfword_ptr = (unsigned short int *)&tempu;
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], ONCE, 10);
				return(REENTER_ON_INTERRUPT);
				}
			}
		
		// If we get here, we've finished pulsing the binary valve, turn off pulse output and exit
		if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("DBG V B%d Z%d Finished pulsing binary valve\n", vtg.prog_ctr, zone);
		
		vtg.timer2_timeout = 0; // Not using this for anything but should clear it anyway
		BINARY_VALVE_OFF;
		WRITE_OSW1; 
		do_set_output=1; // We'll exit here
		}	
		
	if (zone == WHILE_CONDITIONAL_FAILED) // Conditional WHILE interrupt
		{
		if (vtg.control_debug_level==2) printf("DBG V B%d Z%d WHILE condition failed\n", vtg.prog_ctr, zone);
		// If vac_wait && !vac_on, this interrupt has already been disarmed.
		// In the unlikely event that it triggers anyway, we'll ignore it here.
		if (vac_wait && !vac_on) // Skip conditional checking
			{
			if (vtg.control_debug_level==2) printf("DBG V B%d Z%d vel blk exe-zone WHILE_CONDITIONAL_FAILED, (vac_wait && !vac_on), skipping abort, will cont.\n", vtg.prog_ctr, zone);
			DISARM_IO1_CONT_INTERRUPT;
			return(REENTER_ON_INTERRUPT); // Wait for something else to interrupt us.
			}
		else  do_set_output=1; // We'll exit here
		}
	
	
	ext: ;

	// Handle any outputs to set/clr now.
	SET_OUTPUTS_DUR_END;
	//DAC_OUTPUT_CHANNEL(axis_num)=regular_dac_channel; // Restore DAC channel in case it was changed by a DAC override
	
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
	//printf("[END %d] ", DAC_OUTPUT_CHANNEL(1));
	//printf("End of vel, block # %d\n", vtg.prog_ctr-1);
	return(return_val);
	}


	
// Execute pressure block
int pressure_block(c_block control_block, int type_of_execution, int zone)
	{
	static int ramp, loop_num, starting_time;
	static int saved_execution_type, do_set_output, ploop_divisor, current_raw_dac;
	static int commanded_raw_dac, first_loop, dac_increment, axis_num;
	static int return_val, goose, cond_result, soft_cl, open_loop, time, going_higher;
	static short int commanded_press;
	static c_block saved_control_block;
	static fixed4 soft_cl_press_cmd, dp, int_press_cmd, ramp_increments;
	
	int temp_time, input_state, act_press, temp_dac_command;
	int next_time, tempi, skip=0;
	//unsigned int initial_timer_mS, send_op_time;
	unsigned int tempu, loop_time, temp_pressure_setting;
	unsigned int initial_timer_mS=0; // For polled control
	volatile unsigned short int *halfword_ptr;
	//vp_pvu *pv1 = va_pv1;
	//vp_pvu *pv2 = va_pv2;
	//int print_again; // For test
	//
	static int set_dur, set_beg, set_end;
	static char *cond_ptr, *set_io_ptr, turn_on_off;
	char *ptr;
	CPL;

	//printf("[%d]", zone);
	if (zone == 1) // Initial entry of block
		{
		INITIALIZE_CONTROL_BLOCK;
		
		ramp = 0;
		commanded_raw_dac = 0;
		first_loop=1;
		goose=0;
		soft_cl=1;
		open_loop=0;
		ploop_divisor = 100; // Might have to increase this to lower overall gain if control is too sensitive.
		going_higher=1;
		tempu=0;

		if (vtg.control_debug_level==3)	printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
				
		if (vtg.control_program_trace) printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
			
		ptr = (char *)control_block; ptr++; // Move past the block ID

		if (PRESS_CONTROL_ENABLED)
			{
			if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
				{
				return_val=CONDITIONAL_EXECUTION_ERROR;	
				} 
			else if (cond_result < 20) // OK to proceed.
				{
				#if PRESS_EXE_DEBUG
					printf("\n\n\n****************************************************************\n");
					printf("press blk exe-zone 1, OK to proceed, block #%d\n", vtg.prog_ctr);
				#endif
				if (vtg.control_debug_level==3) printf("press blk exe-zone 1, OK to proceed, block #%d\n", vtg.prog_ctr);
				
				// Set/clr any outputs here.  This will also initialize and set the corrrect variables.
				SET_OUTPUTS_BEG_DUR;
				ptr = (char *)control_block; ptr++; // Relocte ptr to point to the beginning of the block

				/////////////////// Initial setups ////////////////////
				//initial_timer_mS = nr_timer_milliseconds();
				//send_op_time = initial_timer_mS + 500; // Time to send first op data packet

				//print_again = initial_timer_mS+750; // For test
				
				ptr++; // Now point to the axis num
				if (*ptr == '1') axis_num = 1; else axis_num = 2; // Possible future expansion to 3 and 4
				
				ptr++; // Now point to the pressure setting
				if (*ptr == 'V')
					{
					ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
					temp_pressure_setting = vtg.v[tempi];
					}
				else temp_pressure_setting = (unsigned int)Atoi(ptr);
				
				if (temp_pressure_setting & 0x40000000) // Open loop, pressure entered as a %
					{
					soft_cl=0; // Full open loop
					open_loop=1;
					
					if (vtg.control_debug_level==3) printf("This is a full open loop pressure setting %d.\n", temp_pressure_setting & 0x0000ffff);
					}
				else if (temp_pressure_setting & 0x80000000)
					{
					soft_cl=0; // Hardware closed loop
					
					if (vtg.control_debug_level==3) printf("This is a hardware closed loop pressure setting %d.\n", temp_pressure_setting & 0x0000ffff);
					}
				else
					{
					if (vtg.control_debug_level==3) printf("This is a closed loop pressure setting %d.\n", temp_pressure_setting & 0x0000ffff);
					}				
					
				commanded_press = (short int)temp_pressure_setting; // Pressure cmd is the lower half word, can be pos. or neg.
				if (commanded_press == 0x7fff) {skip=1, zone=8;}  //{return_val = NO_PROBLEM; goto ext;} // Skip entire block without any test inputs/set outputs.
				if (!open_loop)
					{
					convert_ana_ch_from_dynamic(1, VOLTAGE_X_1000, &act_press); // Pressure in volts x 1000
					if (act_press <= (int)commanded_press) going_higher = 1;
					else going_higher = 0;
					
					if (vtg.control_debug_level==3) printf("going higher = %d\n", going_higher);
					}
				
				// If hardware closed loop, convert commanded pressure from voltage to raw DAC value.
				if (!soft_cl)
					commanded_raw_dac = (unsigned int)(8192 + ((819 * (int)commanded_press)/1000));
				
				tempu=0; time=10000; // Default ending time to 10 seconds
				// Parse for 'R' (Ramp command) or 'T' ending time
				while (*ptr != NULL)
					{
					if (*ptr == '_') break; // Can't be an 'R' or a 'T' after this point
					if (*ptr == 'R') // A ramp was found
						{
						ptr++;
						if (*ptr=='V')
							{
							ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
							tempu = (unsigned int)vtg.v[tempi];
							}
						else tempu = (unsigned int)Atoi(ptr);
						}
					if (*ptr == 'T')
						{
						ptr++;
						if (*ptr=='V') 
							{
							ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
							time = vtg.v[tempi];
							}
						else time = Atoi(ptr);

						// If time == zero, time was left blank which is only realistic for the last pressure step.
						// If we see this, force time to 10000 (10 secs) which will be a block timeout time.
						if (time == 0) time = 10000; 
						if (vtg.control_debug_level==3) printf("Ending time = %d, 0x%x\n", time, time);
						
						break; // If we have the ending position, there's no more to do.
						}
					ptr++;
					} 
				
				// Set ramp_increments / goose
				if (tempu)
					{
					if (tempu & 0x80000000) 
						{
						if (tempu & 0x40000000)
							{
							open_loop=1; // An open loop percentage was entered, open loop_may already be set
							if (vtg.control_debug_level==3) printf("Full open loop pressure goose detected %d\n", tempu & 0x0000ffff);
							}
						else
							{
							if (vtg.control_debug_level==3) printf("Regular pressure goose detected %d\n", tempu & 0x0000ffff);
							}						
							
						goose = tempu & 0x0000ffff; // Goose value in volts X 1000, always assumed to be positive.
						}	
					else
						{
						ramp = tempu & 0x0000ffff; // Ramp in milliseconds, always assumed to be positive.
						
						if (vtg.control_debug_level==3) if (ramp) printf("Ramp detected %d\n", tempu & 0x0000ffff);
						}
					}
				//////////////////////////////////////////////////////////////////////////////////	
				
				///////////////////////// Reset/update the PID parameters ////////////////////////
				if (soft_cl)
					{
					// These items are set in an update block.  These need to be set once before each shot.
					//PRESS_I_STATE = 0; 
								
					PID_LOOP_TYPE(axis_num) = PRESS_LOOP;
					PRESS_PID_D_STATE = act_press*10; // Set d state to the current press.  Convert act press (volts X 1000) to fixed4 (Volts X 10000).
				
					// Syncronize the PID's output with the current DAC setting. 
					get_dac(DAC_OUTPUT_CHANNEL(axis_num), RAW_VAL, (int *)&tempi);
					PRESS_PID_OUTPUT = LONG_TO_FIXED4(tempi); // D_STATE is the current pressure
							
					// Set DAC output range
					if (((PRESSURE_CONT_STATUS_WORD>>4) & 0x00000007) == 0) // +/-2.5V range
						{
						PRESS_PID_O_MAX = LONG_TO_FIXED4(10239);  
						PRESS_PID_O_MIN = LONG_TO_FIXED4(6145);
						}
					else if (((PRESSURE_CONT_STATUS_WORD>>4) & 0x00000007) == 1) // +/-5V range
						{
						PRESS_PID_O_MAX = LONG_TO_FIXED4(12287);  
						PRESS_PID_O_MIN = LONG_TO_FIXED4(4096);
						}
					else   // +/-10V range
						{
						PRESS_PID_O_MAX = LONG_TO_FIXED4(16383);  
						PRESS_PID_O_MIN = 0;
						}
										
					// Don't allow command to go negative if KEEP_PRESS_CMD_POSITIVE.
					if (KEEP_PRESS_CMD_POSITIVE) {PRESS_PID_O_MIN = LONG_TO_FIXED4(8192);} 
						
					// Set the integrator limits to the O_MAX and O_MIN settings for now
					PRESS_PID_I_MAX = LONG_TO_FIXED4(50000); 
					PRESS_PID_I_MIN = LONG_TO_FIXED4(-50000); 

					// All gains fixed4 as is, no need to convert anything.
					// P gain
					PRESS_PID_P_GAIN = PRESS_LOOP_GAIN;

					// I gain
					if (PRESS_I_GAIN_SETTING == 1) {PRESS_PID_RECURSIVE_INTEGRAL = 1;} // Use recursive integral
					else {PRESS_PID_RECURSIVE_INTEGRAL = 0; PRESS_PID_I_GAIN = PRESS_I_GAIN_SETTING;} 	

					// D gain
					PRESS_PID_D_GAIN = PRESS_D_GAIN_SETTING;
							
					// Calling the pid
					// update_pid(&PRESS_STRUCT, (long)error, (long)CV);
					}
				/////////////////////////////////////////////////////////////////////////////////	
				
				//zone = 2; // Go to zone 2 next and begin looking for pressure level to begin control

				if (soft_cl) // Pressure loop closed in software
					{
					if (goose) zone = 3;
					else if (ramp) zone = 4; 
					else zone = 5;
					}
				else  // Hardware closed loop or open loop
					{
					if (goose) zone = 3;
					else if (ramp) zone = 6;
					else zone = 7;
					}

				// Set up ending time or 10 block timeout
				// interrupts here.  Either of thse will end the block.
				ARM_BLK_TIMEOUT_CONT_INTERRUPT(time, 8);
				ARM_IO2_CONT_INTERRUPT(MANUAL_INPUT, ON, 8); // Manual mode will kill pressure control
				//ARM_IO2_CONT_INTERRUPT(FOLLOW_THROUGH_INPUT, ON, 8); // Follow through checked in a while condition
				// For test
				//printf("time=%d\n", time);
				//printf("ONE_MS_COUNTER=%d\n", ONE_MS_COUNTER);	
				//printf("vtg.blk_timeout_time=%d\n", vtg.blk_timeout_time);
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
			}
		else  // Pressure control not enabled, skip entire block.
			{
			return_val = NO_PROBLEM; goto ext;
			}
		} // zone 1
		
	if (zone == 2) // Goose
		{

		}

	if (zone == 3) // Goose 
		{
		if (open_loop)
			{
			// If open loop, we'll just spit out the goose value here and move on.  It really doesn't mean
			// much as it will be overridden very quickly by the open loop pressure command.  
			out_dac(DAC_OUTPUT_CHANNEL(axis_num), goose, PRESS_CMD, ABSOLUTE); // Goose it here
			zone=7;
			}
		else
			{
			//if ((!going_higher && (goose >= (int)commanded_press)) || (going_higher && (goose <= (int)commanded_press)))
			//if (!going_higher || (going_higher && (goose <= (int)commanded_press)))
			if (!going_higher) // Goose only if we're increasing the pressure
				{
				if (vtg.control_debug_level==3) 	printf("Commanding a lower pressure!!!, ignoring goose\n");
				
				if (soft_cl)
					{
					zone = 5;
					}
				else  // Hardware closed loop
					{
					zone = 7;
					}
				}
			else  // Legit. goose command, we'll proceed
				{
				if (first_loop)
					{
					servo_amp_interface(OPEN_PRESSURE_FEEDBACK2);
					out_dac(DAC_OUTPUT_CHANNEL(axis_num), goose, PRESS_CMD, ABSOLUTE); // Goose it here

					if (vtg.control_debug_level==3) printf("press blk exe-zone 3, Goose, waiting for pressure to rise\n");
					first_loop=0;
					
					convert_uS_to_timer(1000, &loop_time); // 1 mS
					halfword_ptr = (unsigned short int *)&loop_time;
					
					// Set up for return on interrupt to zone 4 here
					ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 3);
					}

			chk_pressure_goose: ;
				vtg.timer2_timeout = 0;
				convert_ana_ch_from_dynamic(1, VOLTAGE_X_1000, &act_press); // Pressure in volts x 1000
				//printf("[A%d, C%d, %d ]", act_press, commanded_press, going_higher);

				//if (act_press <= (int)commanded_press) printf("That's right\n"); // For test
				//else printf("[A%d, C%d, %d ]", act_press, commanded_press, going_higher);

				if ((going_higher && (act_press >= (int)commanded_press)) || (!going_higher && (act_press <= (int)commanded_press)))
					{
					// For test
					//if (!going_higher)
					//	{
					//	printf("act_press=%d, commanded_press=%d\n", act_press, commanded_press);
					//	}

					// Make sure timers are stopped
					DISARM_TIMER2_CONT_INTERRUPT;
					
					first_loop = 1;
					// Commanded pressure reached, determine what zone to go to next
					if (soft_cl)
						{
						if (vtg.control_debug_level==3)	printf("Goose exit condition met, moving on to software closed loop zone 5\n");
						zone = 5;
						}
					else  // Hardware closed loop
						{
						if (vtg.control_debug_level==3) printf("Goose exit condition met, moving on to hardware software closed loop zone 7\n");
						zone = 7;
						}
					}
				else
					{
					return(REENTER_ON_INTERRUPT);
					}
				}
			}	
		}	

	if (zone == 4) // Soft closed loop ramp
		{
		if (first_loop)
			{
			if (vtg.control_debug_level==3) printf("press blk exe-zone 4, Closed loop ramp, first time through\n");
			
			servo_amp_interface(OPEN_PRESSURE_FEEDBACK2);
			first_loop=0;
							
      		// Closed loop ramp defined in mS.
      		loop_num = 100;  // For diagnostics 
	
			starting_time = 0;
			convert_ana_ch_from_dynamic(1, VOLTAGE_X_1000, &act_press); // Pressure in volts x 1000
			act_press*=10; // Convert t a fixed4 number (volts x 10000)
			soft_cl_press_cmd = (fixed4)commanded_press * 10; // Convert to a fixed4 number
			
			// First determine pressure change per increment. 
			ramp_increments = FIXED4_SUB(soft_cl_press_cmd, act_press);
			ramp_increments = FIXED4_DIV(ramp_increments, (fixed4)(ramp*10000));
			
			int_press_cmd = act_press;

      		convert_uS_to_timer(1000, &loop_time); // 1 mS
			halfword_ptr = (unsigned short int *)&loop_time;
			
			// Set up for return on interrupt to zone 4 here
			ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 4);
			}

		vtg.timer2_timeout = 0;
		int_press_cmd = FIXED4_ADD(int_press_cmd, ramp_increments);
		convert_ana_ch_from_dynamic(1, VOLTAGE_X_1000, &act_press); // Pressure in volts x 1000
		act_press*=10; // Convert to a fixed4 number
		dp = FIXED4_SUB(int_press_cmd, act_press);
		
		update_pid(&PRESS_PID_STRUCT, dp, (fixed4)act_press);  // This will update PID_OUTPUT(axis_num)
		temp_dac_command=FIXED4_TO_LONG(PRESS_PID_OUTPUT);  // convert fixed4 to long
		out_dac(DAC_OUTPUT_CHANNEL(axis_num), temp_dac_command, RAW_VAL, ABSOLUTE);
		
		if (--ramp == 0) // Finished ramp, set up for software closed loop pressure
			{
			DISARM_TIMER2_CONT_INTERRUPT;
			
			first_loop=1;
			zone=5;
			}
		else 
			{
			return(REENTER_ON_INTERRUPT);
			}
		}
		
	if (zone == 5) // Soft closed pressure loop
		{
		if (first_loop)
			{
			if (vtg.control_debug_level==3) printf("press blk exe, zone 5, Closing pressure loop, first time through\n");
			
			servo_amp_interface(OPEN_PRESSURE_FEEDBACK2);
			first_loop=0;
			loop_num = 300;  // For diagnostics 
			soft_cl_press_cmd = (fixed4)commanded_press * 10; // Convert to a fixed4 number
		
			convert_uS_to_timer(PRESS_LOOP_TIME, &loop_time); 
			halfword_ptr = (unsigned short int *)&loop_time;
			
			// Set up for return on interrupt to zone 5 here
			ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 5);
			}
		
		vtg.timer2_timeout = 0; 
		convert_ana_ch_from_dynamic(1, VOLTAGE_X_1000, &act_press); // Pressure in volts x 1000
		act_press *=10; // Convert to a fixed4 number
		dp = FIXED4_SUB(soft_cl_press_cmd, (fixed4)act_press);
	//printf("cp=%d, ap=%d, dp=%d\n", soft_cl_press_cmd, act_press, dp);	
		update_pid(&PRESS_PID_STRUCT, dp, (fixed4)act_press);  // This will update PID_OUTPUT(axis_num)
		temp_dac_command=FIXED4_TO_LONG(PRESS_PID_OUTPUT);  // convert fixed4 to long
		out_dac(DAC_OUTPUT_CHANNEL(axis_num), temp_dac_command, RAW_VAL, ABSOLUTE);
		
      	return(REENTER_ON_INTERRUPT); // We'll keep comming back until the block times out or follow through is detected.
		}


	if (zone == 6) // Hardware closed loop ramp
		{
		if (first_loop)
			{
			if (vtg.control_debug_level==3)	printf("press blk exe-zone 6, Hardware closed loop ramp, first time through\n");
			
			servo_amp_interface(CLOSE_PRESSURE_FEEDBACK2);
			first_loop=0;
			
			get_dac(DAC_OUTPUT_CHANNEL(axis_num), RAW_VAL, &current_raw_dac); 
			current_raw_dac <<= 10; // Multiply by 1024
			dac_increment = ((commanded_raw_dac<<10) - current_raw_dac) / ramp; 
			//printf("cmd_rd=%d cur_rd=%d ramp=%d, di=%d\n", commanded_raw_dac, current_raw_dac, ramp, dac_increment);

      		if (dac_increment == 0)  // Don't bother controlling ramp 
				{
				if (vtg.control_debug_level==3) printf("dac_increment=0, no ramp\n");
				}
			else
				{
				loop_num = 400;  // For diagnostics
				
				convert_uS_to_timer(1000, &loop_time); // 1 mS
				halfword_ptr = (unsigned short int *)&loop_time;
				
				// Set up for return on interrupt to zone 4 here
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 6);
				}
			}

		vtg.timer2_timeout = 0;
		//out_dac(DAC_OUTPUT_CHANNEL(axis_num), dac_increment, PRESS_RAW, INCREMENTAL);
		current_raw_dac += dac_increment;
		out_dac(DAC_OUTPUT_CHANNEL(axis_num), current_raw_dac>>10, PRESS_RAW, ABSOLUTE);
		//printf("%d\n", current_raw_dac/1000);
		
		if (--ramp == 0) // Finished ramp, set up for hardware software closed loop pressure
			{
			DISARM_TIMER2_CONT_INTERRUPT;
			
			first_loop=1;
			zone=7;
			}
		else
			{
			return(REENTER_ON_INTERRUPT);
			}
		}

	if (zone == 7) // Hardware closed loop control / open loop
		{
		if (vtg.control_debug_level==3) printf("press blk exe-zone 7, Outputting hardware software closed loop DAC setting = %d\n\n", commanded_raw_dac);
		
		if (!open_loop)	servo_amp_interface(CLOSE_PRESSURE_FEEDBACK2); // Hardware CL? close pressure feedback
		out_dac(DAC_OUTPUT_CHANNEL(axis_num), commanded_raw_dac, PRESS_RAW, ABSOLUTE);
		return(REENTER_ON_INTERRUPT); // We'll wait for end time or follow through to end the block.	
		}

	///////////////////// Block ending zones, interrupt control only  ///////////////////////////////
	if (zone == 8) // Ending time reached or we're skipping the entire block.
		{
		if (vtg.control_debug_level==3) printf("press blk exe-zone 8, ending time reached\n");
				
		// If this is the last pressure step, or if we're skipping which means the last step was the last pressure control step and
		// we've timed out (which must be the case if we're here), open the pressure feedback and set the pressure command to the park setting. 
		// It's logical to assume that if pressure control times out, we would want to quit controlling pressure at that point.  The other option
		// is that the user did not enter a end time for the last step.  At that point pressure control will remain active until follow through.
		if (vtg.prog_ctr == (unsigned int)vtg.v[86] || skip) // Last pressure control block?  Open pressure feedback
			{
			servo_amp_interface(OPEN_PRESSURE_FEEDBACK2);
			out_dac(DAC_OUTPUT_CHANNEL(axis_num), vtg.v[85], PRESS_CMD, ABSOLUTE); // Park pressure control valve
			}

		return_val=NO_PROBLEM;
		do_set_output=1;
		}

	if (zone == WHILE_CONDITIONAL_FAILED) // Conditional WHILE interrupt
		{
		if (vtg.control_debug_level==3) printf("press blk exe-zone WHILE_CONDITIONAL_FAILED\n");
				
		servo_amp_interface(OPEN_PRESSURE_FEEDBACK2);
		out_dac(DAC_OUTPUT_CHANNEL(axis_num), vtg.v[85], PRESS_CMD, ABSOLUTE); // Park pressure control valve
		return_val=NO_PROBLEM;
		do_set_output=1; 
		}
		
	ext: ;

	// Handle any outputs to set/clr now.
	SET_OUTPUTS_DUR_END;

	// Disarm any potentially pending interrupts we may have used executing this block.
	DISARM_BLK_TIMEOUT_CONT_INTERRUPT;
	DISARM_TIMER2_CONT_INTERRUPT;
	DISARM_IO1_CONT_INTERRUPT; // Conditional checking may have armed this interrupt
	DISARM_IO2_CONT_INTERRUPT;
	
	if (saved_execution_type==PROGRAM)
		{
		vtg.prog_ctr++; // Increment prog ctr.	
		//printf("Incrementing prog ctr, prog_ctr = %d\n", vtg.prog_ctr);
		}

	return(return_val);
	}
	
	

	
// Execute jump block
int jump_block(c_block control_block, int type_of_execution, int zone)
	{
	static int saved_execution_type, dont_increment_prog_ctr, return_val, cond_result;
	static int do_set_output, type;
	static int where_to_jump;
	static c_block saved_control_block;
	int tempi;
	//
	static int set_dur, set_beg, set_end;
	static char *cond_ptr, *set_io_ptr, turn_on_off;
	char *ptr;
	CPL;

	if (zone == 1)
		{
		INITIALIZE_CONTROL_BLOCK;

		dont_increment_prog_ctr=0;
		where_to_jump=0;

		#if JUMP_EXE_DEBUG
			printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
		#endif
		
		if (vtg.control_program_trace) printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
			
		ptr = (char *)control_block; ptr++; // Move past the block ID

		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			#if JUMP_EXE_DEBUG
				printf("Jump blk - zone 1, executive error %d\n", cond_result);
			#endif

			if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
			else {} // vtg.prog_ctr will be incremented. 	
			return_val=CONDITIONAL_EXECUTION_ERROR;	
			} 
		else if (cond_result < 20) // OK to proceed but there may be some conditions.
			{
			// This is the main body of the block	
			
			#if JUMP_EXE_DEBUG
				printf("Jump blk exe-zone 1, Made it to main body of jump block\n");
			#endif
			
			// Set/clr any outputs here.  This will also initialize and set the corrrect variables.
			SET_OUTPUTS_BEG_DUR;
			ptr = (char *)control_block; ptr++; // Relocte ptr to point to the beginning of the block
			
			// Get the jump type
			type = *ptr-'0';
			if ((unsigned int)type > TYPE_REL) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}	

			ptr +=2; // Scan past the "."

			// Now get where we're jumping to
			if (*ptr == 'V') 
				{
				ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}	
				where_to_jump = vtg.v[tempi];
				}
			else where_to_jump = Atoi(ptr);

			if (type == TYPE_NORMAL)
				{
				if (vtg.prog_ctr == (unsigned int)where_to_jump) // We're jumping nowhere
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
						while (block_conditional(control_block, cond_ptr) == KEEP_GOING_CALL_BACK_DURING)
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
					vtg.prog_ctr = (unsigned int)where_to_jump;	
					dont_increment_prog_ctr=1;
					if (where_to_jump == 1) vtg.stack_ptr=0;
					#if JUMP_EXE_DEBUG
						printf("Abs jump to %d\n", vtg.prog_ctr);
					#endif
					}
				}
			else if (type == TYPE_SUB) // Jump to subroutine 
				{
				if (vtg.prog_ctr == (unsigned int)where_to_jump) // Can't jump to sub that is the current block!
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
					vtg.prog_ctr = (unsigned int)where_to_jump;
					dont_increment_prog_ctr=1;
					#if JUMP_EXE_DEBUG
						printf("jmpin to blk %d\n", vtg.prog_ctr);
					#endif	
					}
				}
			else if (type == TYPE_RETURN) // Return from subroutine
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
			else if (type == TYPE_REL) // Relative jump forward
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
						while (block_conditional(control_block, cond_ptr) == KEEP_GOING_CALL_BACK_DURING)
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
				else // We're actually relative jumping 
					{	
					vtg.prog_ctr += where_to_jump;	
					dont_increment_prog_ctr=1;	
					}
				}

   			do_set_output=1;
			}
		else if (cond_result == COND_NOT_MET) // Nothing more to do 
			{
			#if JUMP_EXE_DEBUG
				printf("Jump blk - condition not met\n");
			#endif

			if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
			else {} // vtg.prog_ctr will be incremented. 
			}
			
		else  // This should not happen - if it does, deal with it here.
			{
			#if JUMP_EXE_DEBUG
				printf("Jump blk - Error!!! we shouldn't be here\n");
			#endif

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

		// Condition no longer met.  
		if (saved_execution_type==IMMED) dont_increment_prog_ctr=1; // vtg.prog_ctr will be unaffected
		else {} // vtg.prog_ctr will be incremented.

		do_set_output=1; 
		}

	ext: ;

	// Handle any outputs to set/clr now.
	SET_OUTPUTS_DUR_END;

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





// Execute update block
int update_block(c_block control_block, int type_of_execution, int zone)
	{
	static int return_val, op1, op2, temp_itu, axis_num;
	static int final_dac_voltage, initial_dac_voltage, current_dac_voltage, ramp;
	static int saved_execution_type, cond_result, first_loop, do_set_output, dac_channel, inc_dac_update;
	static unsigned int ramp_time;
	static c_block saved_control_block;
	volatile unsigned short int *halfword_ptr;
	int tempi, operator_=0, ctr;
	//
	static int set_dur, set_beg, set_end;
	static char *cond_ptr, *set_io_ptr, turn_on_off;
	char *ptr;
	//int zstr_ctr; // For trace
	CPL;

	if (zone == 1)
		{
		INITIALIZE_CONTROL_BLOCK;
		
		// Initialize
		first_loop=1;		
		op2=0;
		temp_itu=0;
		axis_num=1;	
		ramp=0;
		inc_dac_update=0;
				
		#if UPDATE_EXE_DEBUG
			printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
		#endif
		
		if (vtg.control_program_trace) printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
			
		ptr = (char *)control_block; ptr++; // Move past the block ID

		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			#if UPDATE_EXE_DEBUG
				printf("Update block exe-zone 1, executive error\n");
			#endif
			return_val=CONDITIONAL_EXECUTION_ERROR;	
			} 
		else if (cond_result < 20) // OK to proceed but there may be some conditions.
			{
			// This is the main body of the update block.
			#if UPDATE_EXE_DEBUG
				printf("Update block exe-zone 1, made it to the main body of the block, type of exe is %d\n", saved_execution_type);
			#endif

			// Set/clr any outputs here.  This will also initialize and set the corrrect variables.
			SET_OUTPUTS_BEG_DUR;
			ptr = (char *)control_block; ptr++; // Relocte ptr to point to the beginning of the block
			
			while (*ptr != '=' && *ptr != NULL) ptr++;
			if (*ptr == NULL) {return_val = BLOCK_EXECUTION_ERROR; goto ext;} // Must have an equal sign
			
			// We found an equal sign, now we'll get the first operand
			#if UPDATE_EXE_DEBUG
				printf("Update block - found an equal sign\n");
			#endif

			ptr++;
			if (*ptr == '@') {ptr++; inc_dac_update = 1;} // Incremental DAC update.  ITU can only be a DAC update

			if (*ptr == 'V')
				{
				ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				op1 = vtg.v[tempi];
				}
			else if (*ptr == 'N')
				{
				ptr++;
				tempi = *ptr - '0';
				if (tempi<1 || tempi>8) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				// op1 will be voltage X 1000.
				while (convert_ana_ch_from_dynamic(tempi, VOLTAGE_X_1000, &op1)); // Keep trying until we get it
				}
			else if (*ptr == 'D')
				{
				ptr++;
				tempi = *ptr - '0';
				if (tempi<1 || tempi>8) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				get_dac(tempi, VOLTAGE_X_1000, &op1);
				}
			else if (*ptr == 'P')
				{
				ptr++;
				axis_num=*ptr-'0';
				if (axis_num<1 || axis_num>2) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				if (SENSOR_TYPE(axis_num) == ENCODER) op1 = RD_PV_POS(axis_num); 
				}
			else if (*ptr == 'F')
				{
				ptr++;
				axis_num=*ptr-'0';
				if (axis_num<1 || axis_num>2) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				// Convert velocity to X1 pulses/sec	 
				op1 = axis[axis_num].vel_ctr_freq/RD_PV_VEL(axis_num);
				}
			else  // Assume constant
				{
				if (*ptr == 'H' || *ptr == 'h') {ptr++; op1 = htoi(ptr);}
				else op1 = Atoi(ptr);
				// If op1 is a signed constant, the sign will fool the operator check below.  We'll increment once to move past the sign.
				if (*ptr == '-' || *ptr == '+') ptr++;
				}

			#if UPDATE_EXE_DEBUG
				printf("Update block op1=%d\n", op1);
			#endif

			
			// Now get the operator
			while (*ptr != '_' && *ptr != NULL)
				{
				if (*ptr=='+') {operator_=PLUS; break;}
				else if (*ptr=='-') {operator_=MINUS; break;}
				else if (*ptr=='*') {operator_=MUL; break;}
				else if (*ptr=='/') {operator_=DIV; break;}
				else if (*ptr=='&') {operator_=AND; break;}
				else if (*ptr=='|') {operator_=OR; break;}
				else if (*ptr=='^') {operator_=XOR; break;}
				ptr++;
				}
							
			if (operator_) // We must have a second operand
				{
				#if UPDATE_EXE_DEBUG
					printf("Update block %d operator found\n", operator_);
				#endif

				// now get 2nd. operand
				ptr++;
				if (*ptr == 'V')
					{
					ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
					op2 = vtg.v[tempi];
					}
				else  // Assume constant
					{
					if (*ptr == 'H' || *ptr == 'h') {ptr++; op2 = htoi(ptr);}
					else op2 = Atoi(ptr);
					}

				#if UPDATE_EXE_DEBUG
					printf("Update block exe-zone1, op2=%d\n", op2);
				#endif

				if (operator_ == PLUS)
					{temp_itu = op1+op2;}
				else if (operator_ == MINUS)
					{temp_itu = op1-op2;}
				else if (operator_ == MUL)
					{temp_itu = op1*op2;}
				else if (operator_ == DIV)
					{temp_itu = op1/op2;}
				else if (operator_ == AND)
					{temp_itu = op1&op2;}
				else if (operator_ == OR)
					{temp_itu = op1|op2;}
				else if (operator_ == XOR)
					{temp_itu = op1^op2;}			
				else {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				}
			else 
				{
				#if UPDATE_EXE_DEBUG
					printf("Update block NO operator, NO op2\n");
				#endif

				temp_itu = op1; // No operator and no op2
				
				#if UPDATE_EXE_DEBUG
					printf("Update block temp_itu=%d\n", temp_itu);
				#endif
				}
			
			// Now update the itu
			ptr = (char *)control_block; ptr++; // We now point to the itu
						
			if (*ptr == 'V') // Update variable
				{
				ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				vtg.v[tempi] = temp_itu;

				#if UPDATE_EXE_DEBUG
					printf("Item to update is V%d\n", tempi);
				#endif
				do_set_output=1;
				}
			else if (*ptr == 'D') // Update DAC channel
				{
				ptr++;
				dac_channel = *ptr - '0';
				if (dac_channel<1 || dac_channel>4) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
				
				// Check for ramp
				ptr++;
				if (*ptr == 'A')
					{
					ptr+=2;
					if (*ptr == 'V')
						{
						ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
						ramp = vtg.v[tempi];
						}
					else ramp = Atoi(ptr);
					}

				final_dac_voltage = temp_itu;	
							
				if (final_dac_voltage > 10000)
					{
					//do_set_output=1; // ??? Not sure about this
					return_val = NO_PROBLEM; goto ext; // Skip entire block.
					}

				if (ramp) zone=2;
				else
					{
					if (inc_dac_update)
						out_dac(dac_channel, final_dac_voltage, VOLTAGE_X_1000, INCREMENTAL); 
					else
						out_dac(dac_channel, final_dac_voltage, VOLTAGE_X_1000, ABSOLUTE); 

					do_set_output=1;
					}
				}
			else if (*ptr == 'A') // Update position
				{
				ptr++;
				axis_num=*ptr-'0';
				if (axis_num<1 || axis_num>2) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}

				SET_PV_PLOAD(axis_num, temp_itu);
				
				//  Reset the PID I_STATE, D_STATE and servo_here  
				PID_I_STATE(axis_num) = 0;
				PRESS_PID_I_STATE = 0;
				SERVO_HERE(axis_num)=0; // If we have position control trying to hold a position, we'll release it here.
				do_set_output=1;
				}
			else if (*ptr == 'C') // Update command, not implemented
				{	
				do_set_output=1;
				}
			else  // Gotta have something to update since this is an update block.
				{
				#if UPDATE_EXE_DEBUG
					printf("Update block EXE, ERROR, nothing to update!!!\n");
				#endif
				return_val = BLOCK_EXECUTION_ERROR;
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
			get_dac(dac_channel, VOLTAGE_X_1000, &initial_dac_voltage);	
			
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
			out_dac(dac_channel, current_dac_voltage, VOLTAGE_X_1000, ABSOLUTE);	
			vtg.timer2_timeout = 0;

			if (((ramp>0) && (current_dac_voltage<final_dac_voltage))
						|| ((ramp<0) && (current_dac_voltage>final_dac_voltage)))
				{
				if (inc_dac_update)
					out_dac(dac_channel, final_dac_voltage, VOLTAGE_X_1000, INCREMENTAL);
				else
					out_dac(dac_channel, final_dac_voltage, VOLTAGE_X_1000, ABSOLUTE);

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

	// Handle any outputs to set/clr now.
	SET_OUTPUTS_DUR_END;

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
	else // If immed. exe., set na_timer2 as timed out as some other block may be executing that
		 // is depending on the timer.  If we exit here with the na_timer2 stopped and
		 // vtg.timer2_timeout == 0, that block will hang.
		{
		vtg.timer2_timeout = 1;
		}
	
	return(return_val);
	}




// Linear block derived from a velocity block
int linear_block(c_block control_block, int type_of_execution, int zone)
	{
	static int feedrate, prev_feedrate, const_vel, prev_pid_output;
	static int saved_execution_type, do_set_output, accel_setting, decel_setting;
	static int axis_num, first_loop, vac_wait, vac_on, finished;
	static int return_val, cond_result, following_error, target_pos, loop_time;
	static int relative, stopped, trapezoidal, dir;
	static c_block saved_control_block;
	int act_pos=0, input_state, temp_dac_command, torque_limit;
	int current_raw_dac, tempi, zstr_ctr;
	long long templl;
	unsigned int tempu;
	volatile unsigned short int *halfword_ptr;
	//
	static int set_dur, set_beg, set_end;
	static char *cond_ptr, *set_io_ptr, turn_on_off;
	char *ptr;
	char temp_str[81]; // For test
	CPL;

	if (zone == 1) // Initial entry of block
		{
		INITIALIZE_CONTROL_BLOCK;
		
		first_loop=1;
		stopped = 0;
		finished = 0;
		//following_error=0;
		const_vel=0;
		trapezoidal=0;
		decel_setting=0;
		prev_feedrate=feedrate; // Assign the last feedrate setting from the previous block to prev_feedrate
		
		#if LINEAR_EXE_DEBUG
			printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
		#endif
		
		if (vtg.control_program_trace) printf(".%d %s \n", vtg.prog_ctr, (char *)control_block);
		
		ptr = (char *)control_block; ptr++; // Move past the block ID

		if (cond_result < 10 && cond_result > 0) // Some sort of executive error took place.
			{
			return_val=CONDITIONAL_EXECUTION_ERROR;	
			} 
		else if (cond_result < 20) // OK to proceed.
			{
			#if LINEAR_EXE_DEBUG
				printf("\n\n\n****************************************************************\n");
				printf("vel blk exe-zone 1, OK to proceed, block #%d\n", vtg.prog_ctr);
			#endif
			
			// Set/clr any outputs here.  This will also initialize and set the corrrect variables.
			SET_OUTPUTS_BEG_DUR;
			ptr = (char *)control_block; ptr++; // Relocate ptr to point to the beginning of the block
			
			/////////////////// Initial setups ////////////////////
			//initial_timer_mS = nr_timer_milliseconds();
			//send_op_time = initial_timer_mS + 500; // Time to send first op data packet

			//print_again = initial_timer_mS+750; // For test
			
			// First get the axis_num
			ptr++;
			if (*ptr == '2') axis_num = 2;
			else axis_num = 1;
			#if LINEAR_EXE_DEBUG
				printf("Velocity axis# is %d\n", axis_num);
			#endif

			// Get the position setting
			ptr++;
			if (*ptr == '*') {relative = 1; ptr++;} else relative = 0; 

			if (*ptr=='V')
				{
				ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;} 
				target_pos = (unsigned int)vtg.v[tempi];
				}
			else if (*ptr=='H' || *ptr=='h') // Hex value
				{
				ptr++; target_pos = htoi(ptr);  
				}
			else if (*ptr=='F') // Feedrate being preset.  We won't execute a motion block here, we'll only set the feedrate and leave
				{
				ptr++;
				if (*ptr=='V')
					{
					ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;} 
					feedrate = (unsigned int)vtg.v[tempi];
					}
				else feedrate = (unsigned int)Atoi(ptr);
				return_val=NO_PROBLEM;
				SERVO_HERE(axis_num)=0;
				PID_I_STATE(axis_num) = 0; 
				//out_dac(DAC_OUTPUT_CHANNEL(axis_num), 0, VOLTAGE_X_1000, ABSOLUTE);	
				//printf("Turning off servo here\n");
				goto ext;
				}
			else target_pos = (unsigned int)Atoi(ptr);

			// Parse for 'A' (accel), 'D' (decel) or 'F' (feedrate)
			while (*ptr != NULL)
				{
				if (*ptr == '_') break; // Can't be an 'A', 'D' or 'F' after this point
				if (*ptr == 'A') // An accel was found
					{
					ptr+=2;
					if (*ptr=='V') 
						{
						ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
						accel_setting = vtg.v[tempi]; // Accel in X4 pulses
						}
					else accel_setting = Atoi(ptr);
					trapezoidal = 1;
					}
				if (*ptr == 'D') // A decel was found, completely ignored if no 'A' (accel)
					{
					ptr+=2;
					if (*ptr=='V') 
						{
						ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
						decel_setting = vtg.v[tempi]; // Decel in X4 pulses
						}
					else decel_setting = Atoi(ptr);
					}
				if (*ptr == 'F')
					{
					ptr++;
					if (*ptr=='V') 
						{
						ptr++; tempi = Atoi(ptr); if ((unsigned int)tempi > MAX_VAR) {return_val = BLOCK_EXECUTION_ERROR; goto ext;}
						feedrate = vtg.v[tempi]; // Feedrate in X4 pulses/sec
						}
					else feedrate = Atoi(ptr);

					#if LINEAR_EXE_DEBUG
						printf("Feedrate = %d\n", feedrate);
					#endif

					break; // If we have the feedrate, there's no more to do.
					}
				ptr++;
				} 
			
			// Test for feedrate = 0x7fff for testing.  This can be removed later
			if (target_pos == (int)0x7fffffff || (feedrate&0x0000ffff) == 0x7fff) {feedrate=0; return_val = NO_PROBLEM; goto ext;} // Skip entire block without any test inputs/set outputs.
			if (trapezoidal && !decel_setting) decel_setting = accel_setting; // Trapezoidal move must have both accel and decel.  If decel not defined, we'll set it to the accel setting.
			//if (!trapezoidal)
			//	{
			//	printf("blk%d - feed=%d, pfeed=%d, tp=%d\n", vtg.prog_ctr, feedrate, prev_feedrate, target_pos);
			//	}	

			///////////////////////// Reset/update the PID parameters ////////////////////////
			PID_LOOP_TYPE(axis_num) = POS_LOOP;

			if (SENSOR_TYPE(axis_num) == ENCODER) PID_D_STATE(axis_num) = RD_PV_POS(axis_num); // Set d state to the current position
						
			PID_RECURSIVE_INTEGRAL(axis_num) = 0; // We want seperate integral control for position control.
			
			torque_limit = (819 * TORQUE_LIMIT(axis_num)) / 1000; // TORQUE_LIMIT in volts * 1000
			PID_O_MAX(axis_num) = 8192+torque_limit;
			PID_O_MIN(axis_num) = 8192-torque_limit;
			
			// Set the integrator limits to the O_MAX and O_MIN settings for now
			PID_I_MAX(axis_num) = 5000; 
			PID_I_MIN(axis_num) = -5000; 
			
			PID_I_GAIN(axis_num) = INT_GAIN(axis_num);   
			PID_P_GAIN(axis_num) = PROP_GAIN(axis_num);
			PID_D_GAIN(axis_num) = DERIV_GAIN(axis_num);
			///////////////////////////////////////////////////////	
			
			///////////////////////// Update / reset the position loop parameters ////////////////////
			// Position loop moves always consist of a start pos, end pos, start vel and an end vel.
			// A trapezoidal block will consist of 3 moves, the accel phase, the const. vel phase and the decel phase
			// to a stop.  Trapezoidal moves are always assumed to begin from a stop and end at a stop.  There's no
			// changes allowed on the fly.  If on the fly changes are needed, define a point to point move.  
			if (SENSOR_TYPE(axis_num) == ENCODER) act_pos = RD_PV_POS(axis_num); 
			
			if (relative) target_pos = act_pos+target_pos;
			if (target_pos < act_pos) dir=-1; else dir=1;

			SAMPLE_TIME_uS(axis_num) = SAMPLE_TIME_SETTING(axis_num);
			STARTING_POS(axis_num) = act_pos;
			if (trapezoidal) 
				{
				COMMANDED_POS(axis_num) = act_pos; // Initialize the pos command from the position loop
				if (dir==1) ENDING_POS(axis_num) = STARTING_POS(axis_num) + accel_setting;
				else ENDING_POS(axis_num) = STARTING_POS(axis_num) - accel_setting;
				STARTING_VEL(axis_num) = 0;
				}
			else
				{
				if (prev_feedrate==0) // Beginning of a profile
					{
					COMMANDED_POS(axis_num) = act_pos; // Otherwise we're probably in the middle of a profile, we'll leave the command alone
					}
				else
					{
					//STARTING_POS(axis_num) = act_pos+following_error; // This can elmin the blip when accellerating
					//STARTING_POS(axis_num) = ENDING_POS(axis_num);	
					}
				ENDING_POS(axis_num) = target_pos;
				STARTING_VEL(axis_num) = prev_feedrate;
				}
			ENDING_VEL(axis_num) = feedrate;  // If a feedrate wasn't specified in this blk, the last specified feedrate will still be in force. 
			update_pos_loop(&PLOOP_STRUCT(axis_num), STARTING_POS(axis_num), 1); // Initialize position loop for the initial accel.
			//////////////////////////////////////////////////////////////////////////////////////////

			loop_time = SAMPLE_TIME(axis_num); // SAMPLE_TIME(axis_num) already in timer counts
			//printf("loop_time = %d\n", loop_time);
			halfword_ptr = (unsigned short int *)&loop_time;
			if (trapezoidal)
				{
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 2);
				//if (SENSOR_TYPE(axis_num) == ENCODER) {ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 2);}
				}
			else  // Do point to point motion at zone 3
				{
				ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 3);
				//if (SENSOR_TYPE(axis_num) == ENCODER) {ARM_TIMER2_CONT_INTERRUPT(halfword_ptr[0], halfword_ptr[1], CONTINUOUS, 3);}
				}
			
			SERVO_HERE(axis_num)=0; // At this point we're no longer going to hold a position at rest, so we'll release SERVO_HERE.

			// Set up ending position and 10 block timeout
			// interrupts here.  Either of thse will end the block.
			ARM_BLK_TIMEOUT_CONT_INTERRUPT(10000, 7);
			return(REENTER_ON_INTERRUPT);
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

	if (zone == 2)  // Execute a trapezoidal move beginning at zero speed and ending at zero speed with controlled accel / decel
		{
		if (SENSOR_TYPE(axis_num) == ENCODER) act_pos = RD_PV_POS(axis_num); 
		vtg.timer2_timeout = 0; 

	// For test
	//sprintf(temp_str, "\nZone2 pos = 0x%x\n", act_pos);
	//PRINT_STR_UART2(temp_str);	

		tempi = update_pos_loop(&PLOOP_STRUCT(axis_num), act_pos, NULL); // Execute the position loop to determine our position command
		following_error = COMMANDED_POS(axis_num)-act_pos;  // Calculate the following error
		update_pid(&PID_STRUCT(axis_num), (long)following_error, (long)act_pos);  // Run the error through the PID loop and get the DAC setting. PID_OUTPUT(axis_num) will be updated

		if (accel_setting) // Must always have an accel_setting if a trapezoidal move.
			{
			if (tempi==ACT_POS_EQ_FINAL || tempi==ACC_DCC_FINISHED) // Either the actual pos == ENDING_POS(axis_num) or the accel loop has completed.
				{
				// Set up for the steady speed portion of the move.
				STARTING_POS(axis_num) = act_pos;   
				if (dir==1) ENDING_POS(axis_num) = target_pos - decel_setting;	
				else ENDING_POS(axis_num) = target_pos + decel_setting;
				STARTING_VEL(axis_num) = feedrate; 
				ENDING_VEL(axis_num) = feedrate; 
				update_pos_loop(&PLOOP_STRUCT(axis_num), act_pos, 1);
				accel_setting=0; // Flag that we're done with accel.
				const_vel=1;
				}
			}
		else if (const_vel)
			{
			if (tempi==ACT_POS_EQ_FINAL) // The actual pos == ENDING_POS(axis_num).
				{
				// Set up for the decel portion of the move.
				if (dir==1) STARTING_POS(axis_num) = target_pos-decel_setting;   
				else STARTING_POS(axis_num) = target_pos+decel_setting;
				ENDING_POS(axis_num) = target_pos;	
				STARTING_VEL(axis_num) = feedrate; 
				ENDING_VEL(axis_num) = 0; 
				update_pos_loop(&PLOOP_STRUCT(axis_num), act_pos, 1);
				const_vel=0; // Flag that we're done with accel.
				}
			}
		else if (decel_setting)  // Must also have a decel setting.
			{
			if (ENDING_VEL(axis_num) == 0)
				{
				if (tempi==MOTION_STOPPED)
					{
					stopped=1; // A return value of 4 indicates we stopped.
					decel_setting=0;
					//printf("\nstopped 4\n");
					}
				}
			else
				{
				if (tempi==ACC_DCC_FINISHED)
					{
					finished=1; // The decel loop has finished.
					decel_setting=0;
					//printf("\nstopped 3\n");
					}
				}
			}
		
		if (!stopped && !finished)
			{
			out_dac(DAC_OUTPUT_CHANNEL(axis_num), PID_OUTPUT(axis_num), RAW_VAL, ABSOLUTE); // Update the DAC.  
					
			//printf("cp=%d, act_pos=%d, following_error=%d, output=%d\n", commanded_pos, act_pos, following_error, PID_OUTPUT(axis_num));
			return(REENTER_ON_INTERRUPT);
			}
		else
			{
			feedrate=0; // Set feedrate to zero so that the next block computes its move correctly (if point to point).  Trapezoidal moves always end with the feedrate set to zero.
			if (stopped) SERVO_HERE(axis_num)=1;
			//printf(", set SERVO_HERE, axis_num=%d\n", axis_num);
			return_val = NO_PROBLEM; goto ext; // Block is finished
			}
		}

		
	if (zone == 3) // Point to point profiling.  This allows "on the fly changes" to the profile in progress.
		{
		if (SENSOR_TYPE(axis_num) == ENCODER) act_pos = RD_PV_POS(axis_num); 
		vtg.timer2_timeout = 0; 

	// For test
	//sprintf(temp_str, "\nZone3 pos = 0x%x\n", act_pos);
	//PRINT_STR_UART2(temp_str);	
	
		prev_pid_output = PID_OUTPUT(axis_num); // Save output from last update
		tempi = update_pos_loop(&PLOOP_STRUCT(axis_num), act_pos, NULL); // Execute the position loop to determine our position command
		following_error = COMMANDED_POS(axis_num)-act_pos;  // Calculate the following error
		update_pid(&PID_STRUCT(axis_num), (long)following_error, (long)act_pos);  // Run the error through the PID loop and get the DAC setting. PID_OUTPUT(axis_num) will be updated
	//printf("[cp=%d, ap=%d, fe=%d, tempi=%d] ", COMMANDED_POS(axis_num), act_pos, following_error, tempi); // For test

		if (STARTING_VEL(axis_num) == ENDING_VEL(axis_num)) // Moving at a constant speed. The actual pos == ENDING_POS(axis_num)
			{
			if (tempi==ACT_POS_EQ_FINAL) finished = 1; // Check for tempi == 2 to end based on actual position
			} 
		else if (ENDING_VEL(axis_num) > STARTING_VEL(axis_num)) // Accel 
			{
			if (tempi==ACT_POS_EQ_FINAL||tempi==ACC_DCC_FINISHED) finished = 1;
			} 
		else // Decel
			{
			if (ENDING_VEL(axis_num) == 0) {if (tempi==MOTION_STOPPED) stopped=1;} // We're going all the way to a stop
			else {if (tempi==ACC_DCC_FINISHED) finished=1;}
			}
		
		if (!stopped && !finished)
			{
			out_dac(DAC_OUTPUT_CHANNEL(axis_num), PID_OUTPUT(axis_num), RAW_VAL, ABSOLUTE); // Update the DAC.  
					
			//printf("cp=%d, act_pos=%d, following_error=%d, output=%d\n", commanded_pos, act_pos, following_error, PID_OUTPUT(axis_num));
			//printf("PID_OUTPUT=%d\n", PID_OUTPUT(axis_num));
			return(REENTER_ON_INTERRUPT);
			}
		else
			{
			if (stopped) SERVO_HERE(axis_num) = 1;
			return_val = NO_PROBLEM; goto ext; // Block is finished
			}
		}

	if (zone == 4) // Maintain current position (servo here).
		{
		//printf("*"); // For test
		if (SERVO_HERE(1)) // Maintain position on axis 1
			{
			// We don't want to alter any of the static or other variables that may be in use from somewhere else.
			// We'll treat zone 4 as completely private.  We won't mess with anything that could cause problems.
			// We won't alter axis_num, therefore we'll have call everything out longhand without using the defines.
			if (SENSOR_TYPE(1) == ENCODER) act_pos = RD_PV_POS(1); 
			
			PID_I_GAIN(axis_num) = INT_GAIN(axis_num)*POS_BK_GN(axis_num);   // For test
			PID_P_GAIN(axis_num) = PROP_GAIN(axis_num)*POS_BK_GN(axis_num);
			PID_D_GAIN(axis_num) = DERIV_GAIN(axis_num)*POS_BK_GN(axis_num);

			update_pid(&PID_STRUCT(1), (long)(COMMANDED_POS(1)-act_pos), (long)act_pos);
			out_dac(DAC_OUTPUT_CHANNEL(1), PID_OUTPUT(1), RAW_VAL, ABSOLUTE);
			//printf("SH, CP=%d AP=%d PID out=%d, pid_i_state=%d, err=%d\n", COMMANDED_POS(1), act_pos, PID_OUTPUT(1), PID_I_STATE(1), PID_EQUATION(1)); // For test
			//printf("[%d] ", PID_OUTPUT(1));
			//printf("SH, pid_i_state=%d\n", PID_I_STATE(1)); // For test	
			return(NO_PROBLEM);
			}
		else if (SERVO_HERE(2)) // Maintain position on axis 2
			{
			if (SENSOR_TYPE(2) == ENCODER) act_pos = RD_PV_POS(2); 
			
			update_pid(&PID_STRUCT(2), (long)(COMMANDED_POS(2) - act_pos), (long)act_pos);
			out_dac(DAC_OUTPUT_CHANNEL(2), PID_OUTPUT(2), RAW_VAL, ABSOLUTE);
			return(NO_PROBLEM);
			}
		}

	///////////////////// Block ending zones, interrupt control only  ///////////////////////////////
	if (zone == 7) // 10 second block timeout.
		{
		#if LINEAR_EXE_DEBUG
			printf("vel blk exe-zone 7, 10 second block timeout\n");
		#endif 
		return_val = BLOCK_TIMEOUT; // This will trigger a warning
		}

	if (zone == WHILE_CONDITIONAL_FAILED) // Conditional WHILE interrupt
		{
		#if LINEAR_EXE_DEBUG
			printf("vel blk exe-zone WHILE_CONDITIONAL_FAILED\n");
		#endif 
		// If vac_wait && !vac_on, this interrupt has already been disarmed.
		// In the unlikely event that it triggers anyway, we'll ignore it here.
		if (vac_wait && !vac_on) // Skip conditional checking
			{
			#if LINEAR_EXE_DEBUG
				printf("vel blk exe-zone WHILE_CONDITIONAL_FAILED, (vac_wait && !vac_on), skipping abort, will cont.\n");
			#endif

			DISARM_IO1_CONT_INTERRUPT;
			return(REENTER_ON_INTERRUPT); // Wait for something else to interrupt us.
			}
		else  do_set_output=1; // We'll exit here
		}
	
	
	ext: ;

	// Handle any outputs to set/clr now.
	SET_OUTPUTS_DUR_END;

	#if BUILD_FOR_INTERRUPT_CONTROL
		// Disarm any potentially pending interrupts we may have used executing this block.
		DISARM_BLK_TIMEOUT_CONT_INTERRUPT;
		DISARM_TIMER2_CONT_INTERRUPT;
		DISARM_IO1_CONT_INTERRUPT; // Conditional checking may have armed this interrupt
	#else // Polled control
		STOP_TIMER2;
	#endif

	if (saved_execution_type==PROGRAM)
		{
		vtg.prog_ctr++; // Increment prog ctr.		
		}

	//printf("End of linear, block # %d\n", vtg.prog_ctr-1);
	return(return_val);
	}




// This function will determine the commanded position for a position control loop each time it is called.  This 
// function should be called once each sample time.  This loop will not determine following error, only the 
// position command for the current loop.  The calling routine must subtract the actual position from the commanded
// position generated here and determine following error.  Than the calling routine will call the PID loop function.
// The commanded position will be written to ploop->commanded_position.
//
// Initially this is called with 1 passed to new_loop.  After that, this is called once each sample time with 
// with NULL passed to new_loop.
//
// Four things will inform the calling routine that we may be finished.  This function will return a 1 if the
// commanded position is >= the actual position.  The commanded position will lead the actual position by the 
// following error.  The caller may wish to take action at that point.
//
// If the caller chooses to ignore the return value of 1, this function will at some time later return a 2
// when the actual position is >= ploop->ending_pos.  At that point the caller will certainly want to take action.
//
// When in accel or decel, the accel/decel loops will return a valve of 3 when the loop is finished.  The caller
// may wish to take action before then if it receives back a 1 or 2 before the accel/decel loops finish.  That's OK.
//
// When in decel, if the final vel is zero, we'll enter a "soft landing" mode.  This function will return a 4 
// when it detects motion has ended when in soft landing mode. 
//
// If none of these conditions are met, this function will return a zero.  The caller will want to keep calling
// every sample time when the return value is zero.

#define VO ploop->starting_vel // Original velocity
#define VF ploop->ending_vel   // Final velocity

int update_pos_loop(vs_ploop *ploop, int act_pos, int new_loop)
	{
	int rval=PLOOP_KEEP_GOING, a, t, tempi;
	long long templl;
	static int d, ac, dc, dir, ctr, update_ctr, prev_pos, soft_landing;
	static int servo_updates_dur_ac_dc, pos_change_per_ac_dc_update, ac_dc_servo_updates_per_pos_cmd_change;
	static int position_change_per_update_at_vf, position_change_per_update_at_vo;

	if (new_loop) //////////////// Set up a new loop ////////////////////
		{
		// ploop->commanded_pos is not initialized here.  It must be initialized by the calling routine at the
		// beginning of a move or once at the beginning of a series of moves.

		d = ploop->ending_pos - ploop->starting_pos; // Displacement
		if (d<0) {dir=-1; d=0-d;} else dir=1;
		ac=0; dc=0; // Default the accel and decel flags to zero
		position_change_per_update_at_vf = ((VF * ploop->sample_time_uS) / 1000000)*dir;
		position_change_per_update_at_vo = ((VO * ploop->sample_time_uS) / 1000000)*dir;
		soft_landing=0;
		prev_pos = act_pos;
		ctr=0; update_ctr=1;
		//printf("d=%d, dir=%d\n", d, dir);


		if (VF != VO) // An accel or decel is called for
			{
			/* Motion based on the following formulas

			Accelleration in X4 cps/sec
			a = ((vf*vf)-(vo*vo))/(2*d)	where:

			vf == the final velocity and vo == the starting velocity in X4 cps and d == distance in X4 counts over which we'll accellerate.

			Time in uS to execute the accelleration
			t = (vf-v0)*1000000/a	

			Displacement in X4 counts during accelerated motion 
			d = (vo*t)/1^6 + (1/2*a*(t*t))/1^12 where t is time in uS
			
			Velocity resolution in X4 cps = 1/sample time.  If for example the sample time is 2.5mS then
			1 / 2.5mS = 400 cps which is 5 IPS if we assume a 20P rod.  Rather poor, however the velcoity control at
			speeds divisible by 5 will be very tight.  As sample time increases, so does the velocity resolution, 
			however overall performance will begin to degrade.  Increasing the counts from the sensor/encoder will
			increase velocity resolution while also allowing short sample times.

			commanded_pos will lead actual position by the following error.  commanded_pos can be thought of as a
			kind of "computed position."
			*/

			templl = (((long long)VF*(long long)VF)-((long long)VO*(long long)VO))/(2*d); // accel in X4 cps/sec
			a = (int)templl; // Accel in cps/sec
			if (a<0) dc=1; // If a<0 then we're going to decel, set the decel flag and convert to a positive number
			else ac=1;   // Otherwise we're going to accel, set the accel flag
			
			templl = (((long long)VF-(long long)VO)*1000000)/a;
			t = (int)templl; // Accel/decel time in uS, t will always be a positive value.
			
			servo_updates_dur_ac_dc = t / ploop->sample_time_uS;
			//printf("a=%d, d=%d, t=%d\n", a, d, t);
			
			// Accel/decel is the position_change_per_update over the accel/decel distance.  Now we can either
			// have multiple position changes per sample or (more likely) multiple samples per change in position cmd.
			// First we'll decide which case applies here.
			if (servo_updates_dur_ac_dc) // An accel or decel is specified
				{
				if (ac) pos_change_per_ac_dc_update = (position_change_per_update_at_vf - position_change_per_update_at_vo) / servo_updates_dur_ac_dc; // Accel
				else pos_change_per_ac_dc_update = (position_change_per_update_at_vo - position_change_per_update_at_vf) / servo_updates_dur_ac_dc;  // Decel
				
				if (pos_change_per_ac_dc_update) // Some change in position each sample during accel/decel.
					{
					ac_dc_servo_updates_per_pos_cmd_change = 1; // A change in position every sample time during accel.
					}
				else  // Multiple samples before a change in position during accel.
					{
					if (ac) ac_dc_servo_updates_per_pos_cmd_change = servo_updates_dur_ac_dc / (position_change_per_update_at_vf - position_change_per_update_at_vo);
					else ac_dc_servo_updates_per_pos_cmd_change = servo_updates_dur_ac_dc / (position_change_per_update_at_vo - position_change_per_update_at_vf);
					pos_change_per_ac_dc_update = dir; // We'll increment the pos. cmd. by 1 or -1 each ac_dc_servo_updates_per_pos_cmd_change depending on direction.
					}
				}
			else  // Can't have an accel or decel, we'll move at the final velocity vf.
				{
				ac=0; dc=0; 
				}
			//printf("a=%d, t=%d, dir=%d\n", a, t, dir);
			//printf("servo_updates_dur_ac_dc=%d\n", servo_updates_dur_ac_dc);
			//printf("pos_change_per_ac_dc_update=%d\n", pos_change_per_ac_dc_update);
			//printf("ac_dc_servo_updates_per_pos_cmd_change=%d\n", ac_dc_servo_updates_per_pos_cmd_change);
			//printf("position_change_per_update_at_vo=%d\n", position_change_per_update_at_vo);
			//printf("position_change_per_update_at_vf=%d\n\n", position_change_per_update_at_vf);	
			}
		}
	else  /////////////// Service the loop we've already set up //////////////////
		{
		if (ac) // Accel
			{
			//printf("Accel ");
			if (++ctr == ac_dc_servo_updates_per_pos_cmd_change)
				{
				ctr=0;
				update_ctr++;
				}
			ploop->commanded_pos += (pos_change_per_ac_dc_update*update_ctr);
			//printf("[cp=%d, ap=%d, FE=%d, pcu=%d] ", ploop->commanded_pos, act_pos, ploop->commanded_pos-act_pos, pos_change_per_ac_dc_update);
			if ((pos_change_per_ac_dc_update*update_ctr) >= position_change_per_update_at_vf) return(ACC_DCC_FINISHED); // Accel is finished
			}
		else if (dc) // Decel
			{
			//printf("Decel ");
			if (!soft_landing)
				{
				if (++ctr == ac_dc_servo_updates_per_pos_cmd_change)
					{
					ctr=0;
					update_ctr++;
					}

				// If decelleration to a stop, we can't allow the change in command to go to zero, otherwise we'll
				// begin to reverse.
				tempi = (position_change_per_update_at_vo - (pos_change_per_ac_dc_update*update_ctr));
				if (tempi==0) tempi=1*dir; // Force it to some movement 
				ploop->commanded_pos += tempi;
				
				//ploop->commanded_pos += (position_change_per_update_at_vo - (pos_change_per_ac_dc_update*update_ctr));

				//printf("[cp=%d, ap=%d, FE=%d, pcu=%d] ", ploop->commanded_pos, act_pos, ploop->commanded_pos-act_pos, pos_change_per_ac_dc_update);
				if (position_change_per_update_at_vf==0) // VF==0
					{
					if ((dir==1 && (ploop->commanded_pos>=ploop->ending_pos)) || (dir==-1 && (ploop->commanded_pos<=ploop->ending_pos)))
						{soft_landing=1; ploop->commanded_pos = ploop->ending_pos;} 
					}
				else
					{
					if ((pos_change_per_ac_dc_update*update_ctr) <= position_change_per_update_at_vf) return(ACC_DCC_FINISHED); // Decel is finished
					}

				}
			else  // soft_landing, the ending_pos has been commanded.  We'll now wait for the cylinder to stop
				{
				if (dir==1) // Moving forward
					{
					if (act_pos>=ploop->ending_pos || act_pos<=prev_pos) return(MOTION_STOPPED); // Stopped
					}
				else  // Moving backward
					{
					if (act_pos<=ploop->ending_pos || act_pos>=prev_pos) return(MOTION_STOPPED); // Ditto
					}
				}
			}
		else  // Moving at a constant velocity
			{
			ploop->commanded_pos = ploop->commanded_pos + position_change_per_update_at_vf;
			//printf("[cp=%d, ap=%d, FE=%d] ", ploop->commanded_pos, act_pos, ploop->commanded_pos-act_pos);
			}

		// Commanded position will reach the ending position first as commanded position leads the actual by the 
		// following error.  Therefore we'll check it first and set rval to 1.  If the actual position has reached
		// the ending position, we'll overwrite the rval and set it to 2.  If neither of these conditions has occured,
		// We'll default rval to 0.
		if (dir==1 && (ploop->commanded_pos>=ploop->ending_pos) || (dir==-1 && (ploop->commanded_pos<=ploop->ending_pos))) rval=CMD_POS_EQ_ACT; 
		if ((dir==1 && (act_pos>=ploop->ending_pos)) || (dir==-1 && (act_pos<=ploop->ending_pos))) rval=ACT_POS_EQ_FINAL; // Will overwrite rval if true

		prev_pos = act_pos;
		}

	return(rval); 
	}

#if 0
// New version using fixed4 numbers
//	
int update_pos_loop(vs_ploop *ploop, int act_pos, int new_loop)
	{
	int rval=PLOOP_KEEP_GOING, tempi;
	long long templl;
	fixed4ll a, t, f1, f2;
	long long vf2, vo2;
	static fixed4ll prev_pos, fixed4 position_change_per_update_at_vf, position_change_per_update_at_vo;
	static fixed4ll servo_updates_dur_ac_dc, pos_change_per_ac_dc_update, sample_time, update_ctr;
	static int d, ac, dc, dir, ctr, soft_landing;
	static int ac_dc_servo_updates_per_pos_cmd_change;
		
	if (new_loop) //////////////// Set up a new loop ////////////////////
		{
		// ploop->commanded_pos is not initialized here.  It must be initialized by the calling routine at the
		// beginning of a move or once at the beginning of a series of moves.

		d = ploop->ending_pos - ploop->starting_pos; // Displacement
		if (d<0) {dir=-1; d=0-d;} else dir=1;
		sample_time = ploop->sample_time_uS/100; // Convert sample time in uS to seconds in fixed4 format
		ac=0; dc=0; // Default the accel and decel flags to zero
		position_change_per_update_at_vf = FIXED4_MUL((VF*10000), sample_time);
		position_change_per_update_at_vf *= dir; // No need to use the macro for this
		position_change_per_update_at_vo = FIXED4_MUL((VO*10000), sample_time);
		position_change_per_update_at_vo *= dir; // No need to use the macro here
		soft_landing=0;
		prev_pos = act_pos;
		ctr=0; update_ctr=0; 
		//printf("d=%d, dir=%d\n", d, dir);

		if (VF != VO) // An accel or decel is called for
			{
			/* Motion based on the following formulas

			Accelleration in X4 cps/sec
			a = ((vf*vf)-(vo*vo))/(2*d)	where:

			vf == the final velocity and vo == the starting velocity in X4 cps and d == distance in X4 counts over which we'll accellerate.

			Time in uS to execute the accelleration
			t = (vf-v0)*1000000/a	

			Displacement in X4 counts during accelerated motion 
			d = (vo*t)/1^6 + (1/2*a*(t*t))/1^12 where t is time in uS
			
			Velocity resolution in X4 cps = 1/sample time.  If for example the sample time is 2.5mS then
			1 / 2.5mS = 400 cps which is 5 IPS if we assume a 20P rod.  Rather poor, however the velcoity control at
			speeds divisible by 5 will be very tight.  As sample time increases, so does the velocity resolution, 
			however overall performance will begin to degrade.  Increasing the counts from the sensor/encoder will
			increase velocity resolution while also allowing short sample times.

			commanded_pos will lead actual position by the following error.  commanded_pos can be thought of as a
			kind of "computed position."
			*/

			// a = (VF^2 - VO^2) / (2d)
			vf2=(long long)VF*(long long)VF; // We might overrange if we converted to a fixed4ll before squaring.
			vo2=(long long)VO*(long long)VO; // Nothing to be gained by converting at this point anyway.
			f1=vf2-vo2;
			f1 = CONVERT_TO_FIXED4LL(f1); // VF2 - VO2 as a fixed4ll number
			
			f2 = CONVERT_TO_FIXED4LL(d);    // Convert d t a fixed4ll number
			f2 = FIXED4_MUL(f2, 20000); // Multiply d by 2.0000 so that d now equals 2d.
			
			a = FIXED4_DIV(f1, f2); // a in x4 cps/sec
			
			if (a<0) dc=1; // If a<0 then we're going to decel, set the decel flag and convert to a positive number
			else ac=1;   // Otherwise we're going to accel, set the accel flag
			
			// t in sec = (vf-v0)/a Where the difference between vf and vo is converted to a fixed4ll after the subtraction.
			f1=VF-VO;
			f1 = CONVERT_TO_FIXED4LL(f1);
			t = FIXED4_DIV(f1, a); // t in seconds, always positive
			
			// Check if an a controled acced/decel is needed
			servo_updates_dur_ac_dc = FIXED4_DIV(t, sample_time); 
			//printf("a=%d, d=%d, t=%d\n", a, d, t);
			
			if (servo_updates_dur_ac_dc>=10000) // >= 1.0000, an accel or decel is specified
				{
				if (ac) // Accel
					{
					//pos_change_per_ac_dc_update = (position_change_per_update_at_vf - position_change_per_update_at_vo) / servo_updates_dur_ac_dc
					pos_change_per_ac_dc_update = FIXED4_SUB(position_change_per_update_at_vf, position_change_per_update_at_vo);
					pos_change_per_ac_dc_update = FIXED4_DIV(pos_change_per_ac_dc_update, servo_updates_dur_ac_dc);
					}
				else // Decel
					{
					//pos_change_per_ac_dc_update = (position_change_per_update_at_vo - position_change_per_update_at_vf) / servo_updates_dur_ac_dc
					pos_change_per_ac_dc_update = FIXED4_SUB(position_change_per_update_at_vo, position_change_per_update_at_vf);
					pos_change_per_ac_dc_update = FIXED4_DIV(pos_change_per_ac_dc_update, servo_updates_dur_ac_dc);
					}
				}
			else  // Can't have an accel or decel, we'll move at the final velocity vf.
				{
				ac=0; dc=0; 
				}
			//printf("a=%d, t=%d, dir=%d\n", a, t, dir);
			//printf("servo_updates_dur_ac_dc=%d\n", servo_updates_dur_ac_dc);
			//printf("pos_change_per_ac_dc_update=%d\n", pos_change_per_ac_dc_update);
			//printf("ac_dc_servo_updates_per_pos_cmd_change=%d\n", ac_dc_servo_updates_per_pos_cmd_change);
			//printf("position_change_per_update_at_vo=%d\n", position_change_per_update_at_vo);
			//printf("position_change_per_update_at_vf=%d\n\n", position_change_per_update_at_vf);	
			}
		}
	else  /////////////// Service the loop we've already set up //////////////////
		{
		if (ac) // Accel
			{
			//printf("Accel ");
			update_ctr+=10000; // Increment by 1.0000
			// ploop->commanded_pos += (pos_change_per_ac_dc_update*update_ctr)
			f1 = FIXED4_MUL(pos_change_per_ac_dc_update, update_ctr);
			ploop->commanded_pos += f1;
			//printf("[cp=%d, ap=%d, FE=%d, pcu=%d] ", ploop->commanded_pos, act_pos, ploop->commanded_pos-act_pos, pos_change_per_ac_dc_update);
			if (f1 >= position_change_per_update_at_vf) return(ACC_DCC_FINISHED); // Accel is finished
			}
		else if (dc) // Decel
			{
			//printf("Decel ");
			if (!soft_landing)
				{
				update_ctr+=10000; // Increment by 1.0000
				
				// If decelleration to a stop, we can't allow the change in command to go to zero, otherwise we'll
				// begin to reverse.
				tempi = (position_change_per_update_at_vo - (pos_change_per_ac_dc_update*update_ctr));
				if (tempi==0) tempi=1*dir; // Force it to some movement 
				ploop->commanded_pos += tempi;
				
				//ploop->commanded_pos += (position_change_per_update_at_vo - (pos_change_per_ac_dc_update*update_ctr));

				//printf("[cp=%d, ap=%d, FE=%d, pcu=%d] ", ploop->commanded_pos, act_pos, ploop->commanded_pos-act_pos, pos_change_per_ac_dc_update);
				if (position_change_per_update_at_vf==0) // VF==0
					{
					if ((dir==1 && (ploop->commanded_pos>=ploop->ending_pos)) || (dir==-1 && (ploop->commanded_pos<=ploop->ending_pos)))
						{soft_landing=1; ploop->commanded_pos = ploop->ending_pos;} 
					}
				else
					{
					if ((pos_change_per_ac_dc_update*update_ctr) <= position_change_per_update_at_vf) return(ACC_DCC_FINISHED); // Decel is finished
					}

				}
			else  // soft_landing, the ending_pos has been commanded.  We'll now wait for the cylinder to stop
				{
				if (dir==1) // Moving forward
					{
					if (act_pos>=ploop->ending_pos || act_pos<=prev_pos) return(MOTION_STOPPED); // Stopped
					}
				else  // Moving backward
					{
					if (act_pos<=ploop->ending_pos || act_pos>=prev_pos) return(MOTION_STOPPED); // Ditto
					}
				}
			}
		else  // Moving at a constant velocity
			{
			ploop->commanded_pos = ploop->commanded_pos + position_change_per_update_at_vf;
			//printf("[cp=%d, ap=%d, FE=%d] ", ploop->commanded_pos, act_pos, ploop->commanded_pos-act_pos);
			}

		// Commanded position will reach the ending position first as commanded position leads the actual by the 
		// following error.  Therefore we'll check it first and set rval to 1.  If the actual position has reached
		// the ending position, we'll overwrite the rval and set it to 2.  If neither of these conditions has occured,
		// We'll default rval to 0.
		if (dir==1 && (ploop->commanded_pos>=ploop->ending_pos) || (dir==-1 && (ploop->commanded_pos<=ploop->ending_pos))) rval=CMD_POS_EQ_ACT; 
		if ((dir==1 && (act_pos>=ploop->ending_pos)) || (dir==-1 && (act_pos<=ploop->ending_pos))) rval=ACT_POS_EQ_FINAL; // Will overwrite rval if true

		prev_pos = act_pos;
		}

	return(rval); 
	}	
#endif	
	
	

// PID loop, 4 place fixed point decimal math is used.  All gains are gain * 10000 (fixed4) as entered.
// fixed4 numbers are longs * 10000 such that 4 decimal places are represented.  we need to use 
// fixed point decimal macros to perform operations on fixed4 numbers.
//
// Fixed point decimal is a good compromise between floating point without hardware acceleration (too slow)
// and floating with an FPU (too much logic).  All math is done on integers, so it's fast.  Four decimal
// places are assumed in all fixed4 numbers.
fixed4 update_pid(vs_pid_fixed4 *pid, fixed4 error, fixed4 CV)
	{
	fixed4 p_term, d_term, i_term, output;
	
	// calculate the proportional term, pid->p_gain is a fixed4 number.
	// p_term = pid->p_gain * error
	p_term=FIXED4_MUL(pid->p_gain, error);
	
	// calculate the integral state with appropriate limiting, pid->i_gain is a fixed4 number.
	//pid->i_state += error
	pid->i_state=FIXED4_ADD(pid->i_state, error);
	if (pid->i_state > pid->i_max) pid->i_state = pid->i_max;
	else if (pid->i_state < pid->i_min) pid->i_state = pid->i_min;
	// i_term = pid->i_gain * pid->i_state
	i_term =FIXED4_MUL(pid->i_gain, pid->i_state);
	
	// Calculate the derivitive, d_gain is a fixed4 number.
	// d_term = pid->d_gain * (CV - pid->d_state)
	d_term=FIXED4_SUB(CV, pid->d_state);
	d_term=FIXED4_MUL(d_term, pid->d_gain);
	pid->d_state = CV;

	// Sum all of the terms.  
	if (pid->recursive_integral) // Useful for velocity control
		{
		//output = p_term - d_term
		output=FIXED4_SUB(p_term, d_term);
		output += pid->output; // Current computation is added to the previous output.
		}
	else  // Better for position control
		{
		//output = (p_term + i_term - d_term) + 8192.0000
		output=FIXED4_ADD(p_term, i_term);
		output=FIXED4_SUB(output, d_term);
		output=FIXED4_ADD(output, 81920000); // Since we have a seperate i_term, the output is always added to 8192.0000 (zero volts).
		}
		
	// Do a final sanity check
	if (output > pid->o_max) output = pid->o_max;
	else if (output < pid->o_min) output = pid->o_min;

	pid->output=output;
	return (pid->output);
	}

	
// Floating point version
#if 0
float update_pid(vs_pid_float *pid, float error, float CV)
	{
	float p_term, d_term, i_term, output;
	
	// calculate the proportional term, pid->p_gain is a fixed4 number.
	p_term = pid->p_gain * error;
		
	// calculate the integral state with appropriate limiting, pid->i_gain is a fixed4 number.
	pid->i_state += error;
	if (pid->i_state > pid->i_max) pid->i_state = pid->i_max;
	else if (pid->i_state < pid->i_min) pid->i_state = pid->i_min;
	i_term = pid->i_gain * pid->i_state;
		
	// Calculate the derivitive, d_gain is a fixed4 number.
	d_term = pid->d_gain * (CV - pid->d_state);
	pid->d_state = CV;

	// Sum all of the terms.  
	if (pid->recursive_integral) // Useful for velocity control
		{
		output = p_term - d_term;
		output += pid->output; // Current computation is added to the previous output.
		}
	else  // Better for position control
		{
		// Since we have a seperate i_term, the output is always added to 8192.0000 (zero volts).
		output = ((p_term + i_term) - d_term) + 8192.0;
		}
		
	// Do a final sanity check
	if (output > pid->o_max) output = pid->o_max;
	else if (output < pid->o_min) output = pid->o_min;

	pid->output=output;
	return (pid->output);
	}
#endif


// Execute conditional check
int block_conditional(c_block control_block, char *str)
	{
	int cond_op1, cond_op2=0, condition_met=0, operator_, tempi;
	int while_, axis_num=0, input_on_off=0, channel_num, advanced=0;
	c_block saved_control_block;
	CPL;
	
	saved_control_block = control_block; // Redundant but necessary for arming interrupts since they use "saved_control_block."

	// Check conditional.  If we're here, *str must point to an 'I' or a 'W', we'll check to make sure.	
	if (*str=='W') 
		{
		while_ = 1; 

		#if COND_EXE_DEBUG	
			printf("Check conditional - WHILE ");	
		#endif
		}
	else if (*str=='I')
		{
		while_ = 0; 

		#if COND_EXE_DEBUG	
			printf("Check conditional - IF ");	
		#endif
		}
	else 
		{
		#if COND_EXE_DEBUG	
			printf("Check conditional - Error!!! Must be an I or a W\n");	
		#endif
		return(EXE_FAULT_IND_BITS); 
		}
	

	// Get 1st. operands.
	str++;
	if (*str=='1' || *str=='0') // Basic form
		{
		if (*str=='1') input_on_off=1; // else it's defaulted to zero.

		#if COND_EXE_DEBUG
			if (*str=='1') printf("input ON ");
			else printf("input OFF ");
		#endif
		
		str+=2;
		if (*str=='V') // discrete I/O can also be a variable.
			{
			str++; tempi = Atoi(str); if ((unsigned int)tempi > MAX_VAR) {return(EXE_FAULT_IND_BITS);}
			cond_op1 = vtg.v[tempi]; // cond_op1 will contain the I/O number to test.
			}
		else		
			cond_op1 = Atoi(str);

		#if COND_EXE_DEBUG
			printf("I/O# %d\n", cond_op1);
		#endif

		if (cond_op1)
			{
			chk_discrete_input(cond_op1, &condition_met);
			if (!input_on_off) condition_met = !condition_met; // If testing for input off, invert condition_met.
			}
		else return(KEEP_GOING);  //condition_met=1; // If I/O number to test is zero, treat as unconditional
		}
	else  // Advanced form
		{
		advanced = 1;

		#if COND_EXE_DEBUG	
			printf("ADVANCED\n");	
		#endif
				
		// First get cond_op1
		if (*str=='V')		// Variable
			{
			str++; tempi = Atoi(str); if ((unsigned int)tempi > MAX_VAR) {return(EXE_FAULT_IND_BITS);}
			cond_op1 = vtg.v[tempi];
			} 
		else if (*str=='A') // Position
			{
			str++;
			axis_num = (int)*str-'0';
			if (axis_num < 1 || axis_num > 2) return(EXE_FAULT_IND_BITS); 
			if (SENSOR_TYPE(axis_num) == ENCODER) cond_op1 = RD_PV_POS(axis_num); 
			}
		else if (*str=='F') // Velocity
			{
			str++;
			axis_num = (int)*str-'0';
			if (axis_num < 1 || axis_num > 2) return(EXE_FAULT_IND_BITS); 
			cond_op1 = axis[axis_num].vel_ctr_freq/RD_PV_VEL(axis_num);
			}
		else if (*str=='D') // Dac channel
			{
			str++;
			channel_num = (int)*str-'0';
			if (channel_num < 1 || channel_num > 4) return(EXE_FAULT_IND_BITS); 
			// cont_op1 will == DAC voltage X 1000.
			cond_op1 = ((vtg.dac_word[channel_num]-8192)*10000)/8192;
			}
		else if (*str=='N') // ADC channel
			{
			str++;
			channel_num = Atoi(str);
			if (channel_num < 1 || channel_num > 8) return(EXE_FAULT_IND_BITS); 
			// Since this function will not allow recursion, keep trying until we get it
			while (convert_ana_ch_from_dynamic(channel_num, VOLTAGE_X_1000, &cond_op1));
			}
		else return(EXE_FAULT_IND_BITS); 	
		
		#if COND_EXE_DEBUG	
			printf("cond_op1 = %d\n", cond_op1);	
		#endif

		// Now get operator
		while (*str && (*str != '=' && *str != '!' && *str != '>' && *str != '<')) {str++;}
		if (*str == NULL) return(EXE_FAULT_IND_BITS); // No operator found?  indicate a fault.
		
		if (*str == '=') {str++; operator_ = EQ_OP;}
		else if (*str == '!') {str+=2; operator_ = NOT_EQ_OP;}
		else if (*str == '>')
			{
			str++;
			if (*str == '=') {str++; operator_ = GR_THAN_EQ_OP;}
			else operator_ = GR_THAN_OP;
			}
		else if (*str == '<')
			{
			str++;
			if (*str == '=') {str++; operator_ = LESS_THAN_EQ_OP;}
			else operator_ = LESS_THAN_OP;
			}
		else return(EXE_FAULT_IND_BITS); // This should never happen.
			

		// Now get 2nd. operand which can only be a constant or variable. str should now point to cond_op2.
		if (*str=='V') // Variable 
			{
			str++; tempi = Atoi(str); if ((unsigned int)tempi > MAX_VAR) {return(EXE_FAULT_IND_BITS);}
			cond_op2 = vtg.v[tempi];
			}
		else if (*str == 'h' || *str == 'H') {str++; cond_op2 = htoi(str);} // Hex constant
		else cond_op2 = Atoi(str); // Constant

		#if COND_EXE_DEBUG	
			printf("cond_op2 = %d\n", cond_op2);	
		#endif
		
		// Now that we have the operands, it will be easy to make comparisons.
		if (operator_ == EQ_OP)
			{
			if (cond_op1 == cond_op2) condition_met=1; 
			}
		else if (operator_ == NOT_EQ_OP)
			{
			if (cond_op1 != cond_op2) condition_met=1; 
			}	
		else if (operator_ == GR_THAN_OP)
			{
			if (cond_op1 > cond_op2) condition_met=1;
			}		
		else if (operator_ == GR_THAN_EQ_OP)
			{
			if (cond_op1 >= cond_op2) condition_met=1; 
			}
		else if (operator_ == LESS_THAN_OP)
			{
			if (cond_op1 < cond_op2) condition_met=1;
			}
		else if (operator_ == LESS_THAN_EQ_OP)
			{
			if (cond_op1 <= cond_op2) condition_met=1; 
			}
		else
			return(EXE_FAULT_IND_BITS); // This should never happen							
		}
	
	if (condition_met)
		{
		if (while_)
			{
			#if COND_EXE_DEBUG	
				printf("while condition met!!!\n");	
			#endif	
						
			if (advanced)
				{
				// A WHILE advanced will be treated as an IF.  There will be no checking during the block.
				#if BUILD_FOR_INTERRUPT_CONTROL
					return(KEEP_GOING); 
				#else
					return(KEEP_GOING_CALL_BACK_DURING);
				#endif
				}
			else  // Basic form
				{
				// Arm the interrupt here
				if (input_on_off) // While input on
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
				else // While input off
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

				#if BUILD_FOR_INTERRUPT_CONTROL
					return(KEEP_GOING_INTERRUPT_ARMED); 
				#else
					return(KEEP_GOING_CALL_BACK_DURING);
				#endif
				}
			}
		else  // Must have been an IF, we're good to go, no interrupt to arm.
			{
			#if COND_EXE_DEBUG	
				printf("IF condition met!!!, keep going.\n");	
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
CPL;

if(vtg.stack_ptr<19)
	vtg.stack[vtg.stack_ptr++] = item;
else
	return_val=1;

return(return_val);
}


unsigned int pop(void)
{
CPL;

return (vtg.stack[--vtg.stack_ptr]);
}








