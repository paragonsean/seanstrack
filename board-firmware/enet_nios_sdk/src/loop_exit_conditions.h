
		// Open and closed loop exit code.  This code is inserted into
		// the various exit locations. 

		// Variables needed to declare in velocity_block()
		//
		// int temp_pos, vac_wait, vac_on, input_state, send_op_time
		// unsigned int initial_timer_mS

	    temp_pos = RD_PV1_POS;   // Freeze current position

		// Abort if master enable is off
		chk_discrete_input(MASTER_ENABLE_INPUT, &input_state);
		if (input_state == OFF)
			{
			#if EXT_LOOP_DEBUG
				printf("Exiting, master enable dropped out\n");
			#endif		
			vtg.warning = MASTER_ENABLE_DROPPED_OUT;
			goto ext;
			}

		if (temp_pos >= MIN_STROKE_LEN)
			{
			if (ZSPEED)
				{ 
				// Check if a vacuum shot step, if so must abort entire
				// shot to prevent filling vacuum system if not a legit
				// go fast shot signal (the only acceptable way of terminating
				// a vacuum step). 
				if (vac_wait && vac_on)
					{
					#if EXT_LOOP_DEBUG
						printf("Exiting, vacuum abort zspeed\n");
					#endif		
					vtg.warning = VAC_ABORT_ZSPEED;
					goto ext;
					} 
				else 
					{
					#if EXT_LOOP_DEBUG
						printf("Exiting, zspeed, pos=%d\n", RD_PV1_POS);
					#endif		
					goto outputs;
					}
				}
			}
			     
		// initial_timer_mS = nr_timer_milliseconds(); // Set this at start of block
		if ((nr_timer_milliseconds() - initial_timer_mS) > 10000) // Change back to 10000
			{
			#if EXT_LOOP_DEBUG
				printf("Exiting, block timeout\n");
			#endif		
			vtg.warning = BLOCK_TIMEOUT;
			goto ext; 	
			}

		#if EXT_LOOP_DEBUG
		if (nr_timer_milliseconds() > print_again)
			{
			print_again += 750;
			printf("\nact_vel = %d\n", act_vel);
			printf("position = %d\n", RD_PV1_POS);
			printf("commanded_vel = %d\n", commanded_vel);
			printf("dac_voltage = %d\n", (((vtg.dac_word[CONT_DAC_CHANNEL]-8192)*10000)/8192));
			}
		#endif		
				
		if (cond_result == KEEP_GOING_CALL_BACK_DURING)
			{
			if (vac_wait && !vac_on); // Skip conditional checking
			else
				{ 
				if (block_conditional_exe(velocity_block, DURING) == KEEP_GOING_CALL_BACK_DURING);		
				else 
					{
					#if EXT_LOOP_DEBUG
						printf("Exiting, cond call back during failed\n");
					#endif		
					goto ext; // Condition was not met.  Any outputs to
							  // set_clr will have already been taken care
							 // of by block_conditional() therfore we jump
							// past the AT_END checking.
					}		
				}
			}
		
	    if (position)
			{
			if (vac_wait && vac_on)  // Vacuum shot 
				{
				// If cylinder pos. exceeds ending position by more
				// than 3"/120mm (240 counts), abort shot.  This is an
				// additional vacuum safty. 
				if (temp_pos > position+240)
					{
					#if EXT_LOOP_DEBUG
						printf("Exiting, cyl. exceeded EP+240\n");
					#endif	
					vtg.warning = VAC_ABORT_POS_TOO_LONG;
					goto ext;
					}
				}
			else if (temp_pos >= position)
				{
				// If vac wait and fill_test, halt plunger movement.
				if (vac_wait && fill_test && !vac_on)
					{
					#if EXT_LOOP_DEBUG
						printf("Exiting, fill time abort\n");
					#endif		
					vtg.warning = FILL_TEST_ABORT;
					goto ext;
					}

				else
					{
					#if EXT_LOOP_DEBUG
						printf("Exiting, position met, actpos=%d, EP setpt = %d\n", RD_PV1_POS, position);
					#endif		
					goto end_of_vloop;
					}
				}
			}





