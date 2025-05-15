// loop_exit_conditions3.h  For use with combined polled/interrupt control code


		// Open and closed loop exit code.  This code is inserted into
		// the various exit locations. 

		// Variables needed to declare in velocity_block()
		//
		// int temp_pos, vac_wait, vac_on, input_state, send_op_time
		// unsigned int initial_timer_mS

	    temp_pos = RD_PV_POS;   // Freeze current position

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
						printf("[loop_exit_conditions3] Exiting, vacuum abort zspeed\n");
					#endif		
					return_val = VAC_ABORT_ZSPEED; // This will trigger a warning
					goto ext;
					} 
				else 
					{
					#if EXT_LOOP_DEBUG
						printf("[loop_exit_conditions3] Exiting, zspeed, pos=%d, blk_no=%d\n", RD_PV_POS, vtg.prog_ctr);
					#endif	
					do_set_output=1;	
					goto ext;
					}
				}
			}
		
		#if BUILD_FOR_POLLED_CONTROL // Polled control, we'll check these items also
			// initial_timer_mS = nr_timer_milliseconds(); // Set this at start of block
			if ((nr_timer_milliseconds() - initial_timer_mS) > 10000) // Change back to 10000
				{
				#if EXT_LOOP_DEBUG
					printf("[loop_exit_conditions3] Exiting, block timeout\n");
				#endif		
				WARNING_WORD = BLOCK_TIMEOUT;
				goto ext; 	
				}

			// Abort if master enable is off.
			// If interrupt control, we'll sense ME dropout in the while loop in main().
			// Since polled control will stay in the block until completion, we have to
			// sense it here.
			chk_discrete_input(MASTER_ENABLE_INPUT, &input_state);
			if (input_state == OFF)
				{
				#if EXT_LOOP_DEBUG
					printf("[loop_exit_conditions3] Exiting, master enable dropped out\n");
				#endif		
				//return_val = MASTER_ENABLE_DROPPED_OUT; 
				goto ext;
				}

			if (cond_result == KEEP_GOING_CALL_BACK_DURING)
				{
				if (vac_wait && !vac_on); // Skip conditional checking
				else
					{ 
					if (block_conditional_exe(control_block) == KEEP_GOING_CALL_BACK_DURING);		
					else 
						{
						#if EXT_LOOP_DEBUG
							printf("[loop_exit_conditions3] Exiting, cond call back during failed\n");
						#endif		
						do_set_output=1;
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
							printf("[loop_exit_conditions3] Exiting, cyl. exceeded EP+240\n");
						#endif	
						return_val = VAC_ABORT_POS_TOO_LONG; // This will trigger a warning
						goto ext;
						}
					}
				else if (temp_pos >= position)
					{
					// If vac wait and fill_test, halt plunger movement.
					if (vac_wait && fill_test && !vac_on)
						{
						#if EXT_LOOP_DEBUG
							printf("[loop_exit_conditions3] Exiting, fill time abort\n");
						#endif		
						return_val = FILL_TEST_ABORT; // This will trigger a warning
						goto ext;
						}

					else
						{
						#if EXT_LOOP_DEBUG
							printf("[loop_exit_conditions3] Exiting, position met, actpos=%d, EP setpt = %d\n", RD_PV_POS, position);
						#endif	
						do_set_output=1;	
						goto ext;
						}
					}
				}
		#endif





