// pressure_loop_exit_conditions.h  For use with combined polled control code


		// Open and closed loop exit code.  This code is inserted into
		// the various exit locations. 

		// Variables needed to declare in pressure_block()
		//
				
		//#if BUILD_FOR_POLLED_CONTROL // Polled control, we'll check these items also
			// initial_timer_mS = nr_timer_milliseconds(); // Set this at start of block
			// Unlike velocity control, this is not considered a "fault."  The normal
			// block ending time uses this timer as well.  Even if it goes to the full 10
			// seconds, that's OK.
			if ((nr_timer_milliseconds() - initial_timer_mS) > time) 
				{
				#if PRESS_EXT_LOOP_DEBUG
					printf("[pressure_loop_exit_conditions] Exiting, block timeout\n");
				#endif	
					
				do_set_output=1;
				goto ext;; 	
				}

			// Abort if master enable is off.
			// If interrupt control, we'll sense ME dropout in the while loop in main().
			// Since polled control will stay in the block until completion, we have to
			// sense it here.
			chk_discrete_input(MASTER_ENABLE_INPUT, &input_state);
			if (input_state == OFF)
				{
				#if PRESS_EXT_LOOP_DEBUG
					printf("[pressure_loop_exit_conditions] Exiting, master enable dropped out\n");
				#endif		
				//return_val = MASTER_ENABLE_DROPPED_OUT; 
				goto ext;
				}

			// Check if follow through present.
			chk_discrete_input(FOLLOW_THROUGH_INPUT, &input_state);
			if (input_state == ON)
				{
				#if PRESS_EXT_LOOP_DEBUG
					printf("[pressure_loop_exit_conditions] Exiting, follow through sensed\n");
				#endif	
					
				do_set_output=1; 
				goto ext;
				}

			if (cond_result == KEEP_GOING_CALL_BACK_DURING)
				{
				if (block_conditional_exe(control_block) == KEEP_GOING_CALL_BACK_DURING);		
				else 
					{
					#if PRESS_EXT_LOOP_DEBUG
						printf("[pressure_loop_exit_conditions3] Exiting, cond call back during failed\n");
					#endif		
					do_set_output=1;
					goto ext; // Condition was not met.  Any outputs to
							  // set_clr will have already been taken care
							 // of by block_conditional() therfore we jump
							// past the AT_END checking.
					}		
				}
		//#endif





