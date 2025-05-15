
		// Open and closed loop exit code.  This code is inserted into
		// the various exit locations. 

		// Variables needed to declare in velocity_block()
		//
		// int temp_pos, vac_wait, vac_on, input_state
		// unsigned int initial_timer_mS

	    temp_pos = RD_PV1_POS;   // Freeze current position

		// Abort if master enable is off
		chk_discrete_input(300, &input_state);
		if (input_state == OFF)
			{
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
					vtg.warning = VAC_ABORT_ZSPEED;
					goto ext;
					} 
				else goto outputs;
				}
			}
			     
		// initial_timer_mS = nr_timer_milliseconds(); // Set this at start of block
		if ((nr_timer_milliseconds() - initial_timer_mS) > 10000)
			{
			vtg.warning = BLOCK_TIMEOUT;
			goto ext; 	
			}

		if (cond_result == KEEP_GOING_CALL_BACK_DURING)
			{
			if(vac_wait and !vac_on); // Skip conditional checking
			else
				{ 
				if (block_conditional_exe(velocity_block, DURING) == KEEP_GOING_CALL_BACK_DURING);		
				else goto ext; // Condition was not met.  Any outputs to
							  // set_clr will have already been taken care
							 // of by block_conditional() therfore we jump
							// past the AT_END checking.
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
					vtg.warning = VAC_ABORT_POS_TOO_LONG;
					goto ext;
					}
				}
			else if (temp_pos >= position)
				{
				// If vac wait and fill_test, halt plunger movement.
				if (vac_wait && fill_test && !vac_on)
					{
					vtg.warning = FILL_TEST_ABORT;
					goto ext;
					}

				else
					goto end_of_vloop;
				}
			}





