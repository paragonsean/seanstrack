//					control_blocks1.c
//
//			Copyright 2003, Visi-Trak Worldwide

//#include "nios.h"
//#include "control_program_blocks1.h"

#ifndef NULL
 #define NULL 0
#endif


int dwell_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v)
{
db *dwell_block=0;
int return_val = 0, stepnum, x, tempi;
volatile char *ptr=0;


if (action == COMPILE)
	{
	printf("string to compile = %s\n", str_to_compile);		
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
printf("Comment has been stripped - see %s\n", str_to_compile);
	if (str_to_compile[0] == 'D') //Command for immediate execution, first compile.
		{
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

printf("Block should be cleared - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	dwell_block->blk_id = DWELL;
	
printf("ID should be there now - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		// Always cast the update block structure when calling block conditional.
		// For all block types, the conditional, set output stuff will be the 
		// same.  The update block structure in this case is being used as a 
		// template that is good for all block types.
		return_val = block_conditional(COMPILE, (ub *)&control_program[stepnum][0], ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 
	
printf("Got past block_conditional()\n");

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
			if (*ptr == 'h' || *ptr == 'H') tempi = htoi(ptr+1);
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
	}
	
else if (action == EXECUTE) 
	{

	}

else return_val = 2; //Unknown action

clr_id: ;
if (return_val != 0) // Error of some kind, clear the block ID
	dwell_block->blk_id = NULL;
// Now print the block for debug
for (x=0; x<10; x++)
	printf("Word%d %x\n", x, control_program[stepnum][x]);	

ext: ;
return (return_val);		
}




int position_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v)
{
pb *position_block=0;
int return_val = 0, stepnum, x, tempi, item_being_scanned;
volatile char *ptr=0;


if (action == COMPILE)
	{
	printf("string to compile = %s\n", str_to_compile);		
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
printf("Comment has been stripped - see %s\n", str_to_compile);
	if (str_to_compile[0] == 'L') //Command for immediate execution, first compile.
		{
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

printf("Block should be cleared - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	position_block->blk_id = POSITION;
	
printf("ID should be there now - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		// Always cast the update block structure when calling block conditional.
		// For all block types, the conditional, set output stuff will be the 
		// same.  The update block structure in this case is being used as a 
		// template that is good for all block types.
		return_val = block_conditional(COMPILE, (ub *)&control_program[stepnum][0], ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 
	
printf("Got past block_conditional()\n");

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
	printf("item_being_scanned = %d\n", item_being_scanned);
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
			printf("Axis number is %c\n", *ptr);	
				ptr++; // Move past the axis specifier
				}
			printf("found a L %s\n", ptr-3);
			}
		else if (item_being_scanned==PB_VEL)
			{
			if ((ptr = (char *)strchr(str_to_compile, 'F')) == NULL) continue; // No accel to process, loop again 
			else ptr++;	
			printf("found a F %s\n", ptr-1);
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
				if (*ptr == 'h' || *ptr == 'H') tempi = htoi(ptr+1);
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
	
	}
else if (action == EXECUTE) 
	{

	}

else return_val = 2; //Unknown action

clr_id: ;
if (return_val != 0) // Error of some kind, clear the block ID
	position_block->blk_id = NULL;
// Now print the block for debug
for (x=0; x<10; x++)
	printf("Word%d %x\n", x, control_program[stepnum][x]);	

ext: ;
return (return_val);		
}





int velocity_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v)
{
vb *velocity_block=0;
int return_val = 0, stepnum, x, tempi, item_being_scanned;
volatile char *ptr=0;


if (action == COMPILE)
	{
	printf("string to compile = %s\n", str_to_compile);		
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
printf("Comment has been stripped - see %s\n", str_to_compile);
	if (str_to_compile[0] == 'V') //Command for immediate execution, first compile.
		{
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

printf("Block should be cleared - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	velocity_block->blk_id = VELOCITY;
	
printf("ID should be there now - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		// Always cast the update block structure when calling block conditional.
		// For all block types, the conditional, set output stuff will be the 
		// same.  The update block structure in this case is being used as a 
		// template that is good for all block types.
		return_val = block_conditional(COMPILE, (ub *)&control_program[stepnum][0], ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 
	
printf("Got past block_conditional()\n");

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
	printf("item_being_scanned = %d\n", item_being_scanned);
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
			printf("Axis number is %c\n", *ptr);	
				ptr++; // Move past the axis specifier
				}
			printf("found a V %s\n", ptr-3);
			}
		else if (item_being_scanned==VB_ACCEL)
			{
			if ((ptr = (char *)strstr(str_to_compile, "AC")) == NULL) continue; // No accel to process, loop again 
			else ptr+=2;	
			printf("found a A %s\n", ptr-2);
			}
		else if (item_being_scanned==VB_EP)
			{
			if ((ptr = (char *)strchr(str_to_compile, 'P')) == NULL) break; // No ending pos to process 
			else ptr++;	
			printf("found a P %s\n", ptr-1);
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
				if (*ptr == 'h' || *ptr == 'H') tempi = htoi(ptr+1);
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
	
	}
else if (action == EXECUTE) 
	{

	}

else return_val = 2; //Unknown action

clr_id: ;
if (return_val != 0) // Error of some kind, clear the block ID
	velocity_block->blk_id = NULL;
// Now print the block for debug
for (x=0; x<10; x++)
	printf("Word%d %x\n", x, control_program[stepnum][x]);	

ext: ;
return (return_val);		
}




int jump_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v)
{
jb *jump_block=0;
int return_val = 0, stepnum, x, tempi, rel=0;
volatile char *ptr=0;


if (action == COMPILE)
	{
	printf("string to compile = %s\n", str_to_compile);		
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
printf("Comment has been stripped - see %s\n", str_to_compile);
	if (str_to_compile[0] == 'J' || str_to_compile[0] == '-') //Command for immediate execution, first compile.
		{
		jump_block = (jb *)&control_program[stepnum=IMMED_EXECUTE_BLOCK][0]; 
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

printf("Block should be cleared - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	jump_block->blk_id = JUMP;
	
printf("ID should be there now - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x, ID = %d\n", x, control_program[stepnum][x], JUMP);

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		// Always cast the update block structure when calling block conditional.
		// For all block types, the conditional, set output stuff will be the 
		// same.  The update block structure in this case is being used as a 
		// template that is good for all block types.
		return_val = block_conditional(COMPILE, (ub *)&control_program[stepnum][0], ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 
	
printf("Got past block_conditional()\n");

	//At this point, any comment and conditional/set output items have been
	//removed.   
	if ((ptr = (char *)strchr(str_to_compile, 'J')) == NULL) {return_val = 1; goto clr_id;}	
	else
		{
		if (*(ptr-1) == '-') {jump_block->jb_indicator_bits |= jb_rel_abs_specifier_mask; rel = 1;}
	printf("Found rel jump %s\n", ptr-1);
		ptr++; // Now point to the jump type
		tempi = atoi(ptr);
		if (tempi < 0 || tempi > 2) {return_val = 1; goto clr_id;} // Jump type out of range
		else if (tempi == 0) jump_block->jb_indicator_bits |= jb_type_normal_mask;
		else if (tempi == 1) jump_block->jb_indicator_bits |= jb_type_sub_mask;
		else if (tempi == 2) jump_block->jb_indicator_bits |= jb_type_return_mask;
	printf("Jump type = %d\n", tempi);
		ptr+=2;
		if (rel)
			{
			if (*ptr == '-') ptr++; // jb_rel_dir_bit already zero.
			else if (*ptr == '+') {jump_block->jb_indicator_bits |= jb_rel_dir_mask; ptr++;}
			else jump_block->jb_indicator_bits |= jb_rel_dir_mask; // No sign, must be a forward jump, don't increment ptr
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
				
			default: //Error, valid data type not found
				return_val = 1;
				goto clr_id;
				break;

			}	
		}
	}
else if (action == EXECUTE) 
	{

	}

else return_val = 2; //Unknown action

clr_id: ;
if (return_val != 0) // Error of some kind, clear the block ID
	jump_block->blk_id = NULL;
// Now print the block for debug
for (x=0; x<10; x++)
	printf("Word%d %x\n", x, control_program[stepnum][x]);	

ext: ;
return (return_val);		
}




int update_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v)
{

ub *update_block=0;
int return_val = 0, stepnum, x, tempi, item_being_scanned;
volatile char *ptr=0;
 

if (action == COMPILE) 
	{
printf("string to compile = %s\n", str_to_compile);		
	//If a comment is present, terminate string at the comment delimiter.
	if ((ptr = (char *)strchr(str_to_compile, '#')) != NULL) *ptr = 0; 
printf("Comment has been stripped - see %s\n", str_to_compile);
	if (str_to_compile[0] == 'U') //Command for immediate execution, first compile.
		{
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

printf("Block should be cleared - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);
		
	// Set the block ID, after this, any aborts must goto clr_id
	// to clear the block ID before exiting.
	update_block->blk_id = UPDATE;
	
printf("Update ID should be there now - let's see\n");
for (x = 0; x<10; x++)
printf("word%d %x\n", x, control_program[stepnum][x]);

	//Now search for any conditional or set output items, if found, compile the
	//string and terminate the string at the occurance.
	if ((ptr = (char *)strchr(str_to_compile, '_')) == NULL); // None found
	else 
	   {
		return_val = block_conditional(COMPILE, (ub *)&control_program[stepnum][0], ptr+1);
		if (return_val != 0) goto clr_id; // Error in conditional string
		*ptr = 0; 
		} 
	
printf("Got past block_conditional()\n");

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
		else if (item_being_scanned==OP2);
			// ptr should already point 1st. char after the operator
			// if an operator was found.	
			
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
				if (item_being_scanned == ITU) {return_val = 1; goto clr_id;} // Item to update cannot be a constant	
				else // Only OP1 or OP2 can be constants
					{
					// First get integer and save to tempi
					if (*ptr == 'h' || *ptr == 'H') tempi = htoi(ptr+1);
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
						// Now check for accel
						ptr++; 
						if (*ptr == 'A') //Accel specified
							{
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
									update_block->vramp_df = htoi(ptr++);
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
	}

else if (action == EXECUTE) 
	{

	}

else return_val = 2; //Unknown action

clr_id: ;
if (return_val != 0) // Error of some kind, clear the block ID
	update_block->blk_id = NULL;
// Now print the block for debug
for (x=0; x<10; x++)
	printf("Word%d %x\n", x, control_program[stepnum][x]);	

	
ext: ;
return (return_val);
}






// Use update block template for any block type calling this function.
// The conditional / set output section (words 7-9) for any block type 
// will be the same.
//


int block_conditional(int action, ub *blkptr, char *condstr)
{
int return_val = 0, x;
volatile char *ptr=0;


if (action == COMPILE)
	{
	ptr = condstr;
printf("condstr is %s\n", ptr);	
	if (*ptr == 'I' || *ptr == 'W')
		{
		if (*ptr == 'I') blkptr->cond_indicator_bits |= if_mask; 
		else blkptr->cond_indicator_bits |= while_mask; 
		ptr++;
		if (*ptr == '1' || *ptr == '0') //Basic form conditional check
			{
			blkptr->cond_indicator_bits |= op1_descrete_io_mask;
			if (*ptr == '1') blkptr->cond_indicator_bits |= input_on_mask;
			else blkptr->cond_indicator_bits |= input_off_mask;	
			
			ptr += 2;
			if (*ptr == 'V')
			 	{
				ptr++;
				blkptr->cond_indicator_bits |= op1_var_mask;
				// If both descrete_io_indicator and var_indicators are
				// set, then the variable must contain a descrete I/O#, 
				x = atoi(ptr); if (x<0 || x>MAX_VAR) {return_val = 1; goto ext;} //Variable number out of range
				blkptr->cond_op1_df = (unsigned short int)x;
				}
			else 
				{
				// descrete_io_indicator already set
				x = atoi(ptr); if (x<0 || x>MAX_DESCRETE_IO_NUM) {return_val = 1; goto ext;} //I/O number out of range
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
				printf("1st. op was a variable, ptr now == %s\n", ptr);
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
			printf("Conditional 2nd. operand was a numeric constand %d\n", atoi(ptr)); 
				}
			else	
				{
				switch (*ptr) //2nd. operand
					{
					case 'h': //hex numeric constant
					case 'H':
						blkptr->cond_indicator_bits |= op2_const_mask;
						blkptr->cond_op2_df = htoi(ptr+1); //Get ptr past the "h" and convert hex ascii
					printf("Conditional 2nd. operand was a hex constand %d\n", htoi(ptr)); 
						break;

					case 'V': //Variable
						blkptr->cond_indicator_bits |= op2_var_mask;
						ptr++;
						x = atoi(ptr);
						if (x<0 || x>MAX_VAR) {return_val = 1; goto ext;}
						blkptr->cond_op2_df = x;
					 printf("Conditional 2nd. operand was a variable, variable #%d\n", x); 
						break;
						
					default: // Syntax error
						return_val = 1;
						goto ext;
						break;	
					}
				}
			}
		}
	// Now look for set output information, parse string from beginning.
	if ((ptr = (char *)strchr(condstr, '_')) != NULL) ptr++; // Move past the delimiter
	else ptr = condstr; // Must reload ptr since it is now NULL
	
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
			if (x<0 || x>MAX_DESCRETE_IO_NUM) {return_val = 1; goto ext;}
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
	}

else if (action == EXECUTE)
	{
	}

else return_val = 2; //Unknown action

ext: ;
	
return (return_val);
}


// Function to convert a ascii hex string to int.  No signs are dealt with
int htoi (char *str)
{
int x, return_val = 0;
char ch;

for (x = 0; x<=7; x++)
	{
	if (str[x] >= '0' && str[x] <= '9') ch = str[x] & 0x0f;
	else if (str[x] >= 'a' && str[x] <= 'f') ch = (str[x] & 0x0f)+9;
	else if (str[x] >= 'A' && str[x] <= 'F') ch = (str[x] & 0x0f)+9;
	else break; // Non hex digit encountered, exit 

	if (x == 0) //First time through
		{
		return_val = ch;
		}
	else
		{
		return_val <<= 4; return_val |= ch; 
		}
		
	}
return (return_val);
}




push(unsigned int item)
{
vtg.stack[vtg.stack_ptr++] = item;
}


unsigned int pop()
{
return (vtg.stack[--vtg.stack_ptr]);
}


#if 0

vel_control()
{
/* Program execution from high ram, block F000 to FEFE. */
/* Framework for the velocity control block

   *block_ptr + 2  == Velocity command.
   *block_ptr + 4  == Accel. in (vel. change / X1 count) * 1000
   *block_ptr + 6  == Ending position.
   *block_ptr + 8  == Low impact dac setting.
   *block_ptr + 10 == Low input input number.
   *block_ptr + 12 == Goose and wait dac command.

   *block_ptr + 14 to + 18 reserved for future use.

   *block_ptr + 20  == Variable indicator.
   *block_ptr + 22  == Input check indication.  999 - unconditional,
      xxxxxx d ccc bbbb aa
      |      | |   |    |
      |      | |   |    00 - If, 01 - While
      |      | |   |    10 - Undefined, 11 - Undefined
      |      | |   |
      |      | |   0000 - Input off, 0001 - Input on
      |      | |   0010 - ADC input voltage * 1000, ch.1
      |      | |   0011 - ADC input voltage * 1000, ch.2
      |      | |   0100 - ADC input voltage * 1000, ch.3
      |      | |   0101 - ADC input voltage * 1000, ch.4
      |      | |   0110 - Position in timer2 counts
      |      | |   0111 - Velocity in direct eng. units * 100
      |      | |   1000 - Page 1 variables (V0 - V63)
      |      | |   1001 - Page 2 variables (V64 - V127)
      |      | |   1010 - ADC input voltage * 1000, ch.5
      |      | |   1011 - ADC input voltage * 1000, ch.6
      |      | |   1100 - ADC input voltage * 1000, ch.7
      |      | |   1101 - ADC input voltage * 1000, ch.8
      |      | |   1110 - 1111 Undefined
      |      | |
      |      | 000 Undefined,  001 <=, 010 >=
      |      | 011 =, 100 !=, 101 <, 110 >
      |      | 111 Undefined
      |      |
      |      |
      |      Set when equal to 999d. No input checking.
      |
      Variable number, V0 - V63 page 1, V64 - V127 page 2.

   *block_ptr + 24  == Input to check, Analog val, Pos., Vel.  Ignored if previous is 999.
   *block_ptr + 26  == Output set or clear indication. 999 - no outputs to set.
		      0 - Beginning OFF, 1 - Beginning ON,
		      3 - During, 4 - At end OFF, 5 - At end ON.
   *block_ptr + 28  == Output to set / clear. Ignored if previous is 999.


   Order of precedence for executing block:

   1,  If conditional execution is required, check for condition. If
       condition is false, exit block.

   2,  If Beginning or during output is required, do it now.

   3,  Check for acceleration or goosing.

   4,  Close velocity loop.

   5,  Check for low impact, if required do now.

   6,  Take care of during or at end outputs now.
*/

/* Special goose values:

   -1 == Skip entire block
   -2 == Open loop block

*/

int goose, loop_num;
int out_dac(), low_imp_dac, pct, dac_increment;
int while_flag, check_conditional();
unsigned int input_indicator, input_number, position;
unsigned int output_indicator, output_number, variable_indicator;
unsigned int dither_time, dither_ampl, low_imp_input, servo_time, prev_servo_time;
unsigned char old_dp, while_on_flag, while_off_flag, while_physical_flag;
unsigned char vac_on, slow, fill_test, overflow, timeout, first_time_through;
unsigned char last_step;
unsigned int temp;
unsigned int ol_setting, accel_increments;
unsigned int max_block_time, vel_ctr, loop_time;
unsigned int newt2;
int temp_tim2;
long temp_dac_command, accel, long_dac_command, max_dv, min_dv, dv;
unsigned long act_vel, commanded_vel, int_vel_cmd;

/* parameter definitions, defined globally */
extern unsigned int bkgn1, bkgn2, bkgn3, vel_loop_gain;
extern unsigned int dither_time, dither_ampl;
extern unsigned long bk_vel1, bk_vel2, bk_vel3;
extern unsigned long vel_dividend;
extern unsigned int w_vel_check_zspeed; /* Raw velocity counts. */
extern int w_press_check_zspeed;  /* Raw ADC ch. 2 counts. */


old_dp = ioport1;
ioport1 &= 0xC6; /* Select RAM */
ioport1 |= 0x39;

/* Initialize variables */
while_physical_flag = 0, while_on_flag = 0;
while_off_flag = 0, while_flag = 0;

variable_indicator = *((unsigned int * )(block_ptr + 20));

if (variable_indicator & 0x0040)
   goose = *((unsigned int * )(*((unsigned int * )(block_ptr + 12))));
else  goose = *((unsigned int * )(block_ptr + 12));

/* Now check if goose == 0xffff.  This is a quick way to skip the
   entire block if necessary. */
if (goose == -1) goto ex;

if (*(unsigned int * )(block_ptr+30) != 3)
   last_step = 1;  /* Next step is not a vel. control step, therefore this must be the last step. */
else if ( *((unsigned int * )(*((unsigned int * )((block_ptr+30) + 12)))) == -1)
   last_step = 1;  /* Next step is not used, therefore this must be the last step. */
else last_step = 0; /* More steps after this one. */


max_block_time = tim1_ovfl_ctr + 115;  /* 10 seconds max time */

/* Now get commanded velocity */
if (variable_indicator & 0x0002)
   temp = *((unsigned int * )(*((unsigned int * )(block_ptr + 2))));
else
   temp = *((unsigned int * )(block_ptr + 2));

if (goose == -2)  /* Open loop block */
   ol_setting = temp;
else
   {
   commanded_vel = (unsigned long)temp * 100;
   commanded_vel = (commanded_vel * (unsigned long)vel_multiplier) / 100;
   /* Commanded velocity is now velocity in direct units * 10000 */
   }

if (variable_indicator & 0x0004) /* Accel in X1 count */
   accel_increments = *((unsigned int * )(*((unsigned int * )(block_ptr + 4))));
else
   accel_increments = *((unsigned int * )(block_ptr + 4));

if (variable_indicator & 0x0008)
   position = *((unsigned int * )(*((unsigned int * )(block_ptr + 6))));
else  position = *((unsigned int * )(block_ptr + 6));

if (variable_indicator & 0x0010)
   low_imp_dac = *((unsigned int * )(*((unsigned int * )(block_ptr + 8))));
else  low_imp_dac = *((unsigned int * )(block_ptr + 8));
if (low_imp_dac > 0)
   {
   if (low_imp_dac < 0) low_imp_dac = 1; /* Keep above zero, -10V!!! */
   }

if (variable_indicator & 0x0020)
   low_imp_input = *((unsigned int * )(*((unsigned int * )(block_ptr + 10))));
else  low_imp_input = *((unsigned int * )(block_ptr + 10));

input_indicator = *((unsigned int * )(block_ptr + 22));

if (variable_indicator & 0x1000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 24))));
else  input_number = *((unsigned int * )(block_ptr + 24));

output_indicator = *((unsigned int * )(block_ptr + 26));

if (variable_indicator & 0x4000)
   output_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC6;  /* select I/O */

/* Set vacuum flag if vacuum on */
vac_on = 0;
if (*RD_DIGITAL_IO & 0x0400) vac_on = 1;

/* Set fill_test if input present */
fill_test = 0;
if (*RD_DIGITAL_IO & 0x1000) fill_test = 1;


/* Now compute max allowable dv. */
max_dv = 0x7ffffffe / vel_loop_gain;
min_dv = 0 - max_dv;

/************** FIRST CHECK FOR CONDITIONAL EXECUTION **************/

if (input_indicator == 999);  /* Unconditional execution */
else
   {
   while_flag = check_conditional(input_indicator, input_number);
   if (!while_flag) goto ex; /* Condition not met */
   else if (while_flag == 1) while_flag = 0;  /* IF condition met, reset while_flag */
   else if (while_flag == 2) while_on_flag = 1;  /* WHILE INPUT ON conditional, also set while_on_flag */
   else if (while_flag == 3) while_off_flag = 1; /* WHILE INPUT OFF conditional, also set while_off_flag */
   else if (while_flag == 4) while_physical_flag = 1; /* WHILE conditional with a physical parameter */
   else goto ex;  /* Unknown return value, should never happen. */
   }

/********** NOW SEE IF ANY OUTPUTS NEED TO BE SET / CLEARED **********/

if (output_indicator == 999);

/* If Beginning ON or During (always turn on), turn on output. */
else if (output_indicator == 1 || output_indicator == 3) *WR_DIGITAL_IO = ~(temp_d_io |= output_number);

/* If Beginning OFF, turn off output. */
else if (output_indicator == 0) *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);



/************************** CHECK IF CLOSED LOOP ****************************/

if (goose != -2)  /* Closed loop */
   {
   /***** CHECK IF ACCELERATION CONTROL OR GOOSING OF VALVE IS NECESSARY ***/
   if (accel_increments) /* Closed loop acceleration */
      {
      /* Closed loop acceleration defined in units per X1 count */
      loop_num = 100;  /* For diagnostics */

      if ((vel_ctr = *VELOCITY) == 0) act_vel = 0;
      else  act_vel = vel_dividend / (unsigned long)vel_ctr;  /* Act vel X 10000 */

      int_vel_cmd = act_vel;

      /* First determine vel change per increment. */
      accel = ((long)(commanded_vel - act_vel)) / (long)accel_increments;
      for (pct = 1; pct <= accel_increments; pct++)
	 {
	 newt2 = timer2 + 4;
	 int_vel_cmd += accel;

	 if ((vel_ctr = *VELOCITY) == 0) act_vel = 0;
	 else  act_vel = vel_dividend / (unsigned long)vel_ctr;  /* Act vel X 10000 */

	 dv = (long)(int_vel_cmd - act_vel);
	 if (dv > max_dv)  dv = max_dv;
	 else if (dv < min_dv) dv = min_dv;

	 /* Temp_dac_command is the change in dac output before adding to
	    the existing dac output  */

	 /* Boost gain ONLY if accelerating when moving slowly. */
	 if (((vel_dividend / int_vel_cmd) > 7500) && (accel > 0))
	    {
	    if (((dv * 5) > max_dv) || ((dv * 5) < min_dv)) dv /= 5;
	    temp_dac_command = (dv * (long)(vel_loop_gain * 5)) / 10000000;
	    }
	 else
	    temp_dac_command = (dv * (long)vel_loop_gain) / 10000000;


	 /* Now check gain breaks. */
	 if ((act_vel >= bk_vel3) && (temp_tim2 > 240))
	    temp_dac_command = (temp_dac_command * (long)bkgn3) / 1000;

	 else if ((act_vel >= bk_vel2) && (temp_tim2 > 240))
	    temp_dac_command = (temp_dac_command * (long)bkgn2) / 1000;

	 else if ((act_vel >= bk_vel1) && (temp_tim2 > 240))
	    temp_dac_command = (temp_dac_command * (long)bkgn1) / 1000;

	 dac_command = (unsigned int)temp_dac_command + dac_command;

	 /* Put dither code here. */

	 out_dac(dac_command, CHECK);

	 /* Now write to the holding ram for testing */
	 if (hold_ctr < 2040)
	    {
	    ioport1 &= 0xC6; /* Select RAM */
	    ioport1 |= 0x39;

	    TEMP_HOLDING[hold_ctr++] = 600;
	    TEMP_HOLDING[hold_ctr++] = timer2;
	    TEMP_HOLDING[hold_ctr++] = (int)(act_vel / 10000);
	    TEMP_HOLDING[hold_ctr++] = (int)(int_vel_cmd / 10000);
	    TEMP_HOLDING[hold_ctr++] = dac_command;

	    ioport1 &= 0xC6;   /* select I/O */
	    }

	 do
	     {
	     /*1111111111111111111111111111111111111111111111111111111111111*/

	     #include <EXTLOOP3.H>

	     }while (timer2 < newt2);  /* Wait for next sample */
	 } /* for loop */

      /* Write for loop ending info */
      if (hold_ctr < 2040)
	 {
	 ioport1 &= 0xC6; /* Select RAM */
	 ioport1 |= 0x39;

	 TEMP_HOLDING[hold_ctr++] = 800;
	 TEMP_HOLDING[hold_ctr++] = pct;
	 TEMP_HOLDING[hold_ctr++] = newt2;
	 TEMP_HOLDING[hold_ctr++] = dac_command;
	 TEMP_HOLDING[hold_ctr++] = 800;

	 ioport1 &= 0xC6;   /* select I/O */
	 }
      }  /* Closed loop accel */

   else if (goose)  /* Legitiment goose command */
      {
      out_dac(dac_command = goose, CHECK);
      }


   /********************* CLOSE VELOCITY LOOP ********************/

   /* Determine loop time based on commanded velocity. */

   vel_ctr = *VELOCITY;
   act_vel = vel_dividend / (unsigned long)vel_ctr;  /* Act vel X 10000 */

   /* Fast loop if decelerating from above the slow point. */
   if ((act_vel > commanded_vel) && (vel_ctr < 12000))
      {
      loop_time = 1875;   /* 2.5mS */
      slow = 0;
      }
   else if ((vel_dividend / commanded_vel) > 12000)  /* Slow */
      {
      loop_time = 18750;  /* 25mS */
      slow = 1;
      }
   else
      {
      loop_time = 1875;   /* 2.5mS */
      slow = 0;
      }

   first_time_through = 1;

   loop_num = 300;  /* For diagnostics */

   while (1)  /* Loop until forced out. */
      {
      if (first_time_through)
	 {
	 first_time_through = 0;
	 prev_servo_time = timer1;
	 }

      disable(); /* */
      timer1_overflowed = 0;
      timeout = 0;
      /* prev_servo_time keeps a running total of timer1 counts in the
	 correct loop_time increments.  This makes the time between servo
	 samples almost exactly 2.5 or 25mS with no accumulating error buildup. */
      servo_time = prev_servo_time+loop_time;
      if (servo_time < prev_servo_time) overflow = 1; else overflow = 0;
      prev_servo_time = servo_time;
      enable();  /* */

      if ((vel_ctr = *VELOCITY) == 0)
	 act_vel = 0; /* Possibly make this a fault condition */
      else  act_vel = vel_dividend / (unsigned long)vel_ctr;   /* Vel X 10000 */

      dv = (long)(commanded_vel - act_vel);
      if (dv > max_dv)  dv = max_dv;
      else if (dv < min_dv) dv = min_dv;


      if (slow)  /* slow */
	 {
	 /* If slow, increase gain by a factor of 10. The loop time
	    has already been increased by a factor of 10.  This will
	    provide much improved slow speed velocity control. */
	 if (((dv * 10) > max_dv) || ((dv * 10) < min_dv)) dv /= 10;
	 temp_dac_command = (dv * ((long)vel_loop_gain * 10)) / 10000000;
	 }
      else
	 /* temp_dac_command is the change in dac output before adding to
	    the existing dac output */
	 temp_dac_command = (dv * (long)vel_loop_gain) / 10000000;


      /* Now check gain breaks. */
      temp_tim2 = timer2;  /* Freeze timer2 */

      if ((act_vel >= bk_vel3) && (temp_tim2 > 240))
	 temp_dac_command = (temp_dac_command * (long)bkgn3) / 1000;

      else if ((act_vel >= bk_vel2) && (temp_tim2 > 240))
	 temp_dac_command = (temp_dac_command * (long)bkgn2) / 1000;

      else if ((act_vel >= bk_vel1) && (temp_tim2 > 240))
	 temp_dac_command = (temp_dac_command * (long)bkgn1) / 1000;

      dac_command += (int)temp_dac_command;

      /* Since the 10 second block time out will not function here,
	 the following sanity check will terminate the velocity loop
	 if the cylinder is not moving for some reason. */
      if ((dac_command > 0x5000) || (dac_command < -0x1000)) /* Velocity error overflow. approx. +15 or -15 volts */
	 {
	 warning = 3;
	 SET_WARNING;
	 goto ex;   /* Abort servo control. */
	 }

      /* Put dither code here. */

      out_dac(dac_command, CHECK);

      /* Now write to the holding ram for testing */
      if (hold_ctr < 2040)
	 {
	 ioport1 &= 0xC6; /* Select RAM */
	 ioport1 |= 0x39;

	 TEMP_HOLDING[hold_ctr++] = timer1;
	 TEMP_HOLDING[hold_ctr++] = (int)(commanded_vel / 10000);
	 TEMP_HOLDING[hold_ctr++] = (int)(act_vel / 10000);
	 TEMP_HOLDING[hold_ctr++] = (int)temp_dac_command;
	 TEMP_HOLDING[hold_ctr++] = dac_command;

	 ioport1 &= 0xC6;   /* select I/O */
	 }

      /* Make waiting loop a do loop to ensure at least one pass through */
      do /* Wait for next sample */
	     {
	     /*222222222222222222222222222222222222222222222222222222*/

	     #include <EXTLOOP3.H>

	     if (overflow)
		{
		if (timer1_overflowed)
		   {
		   if (servo_time <= timer1)
		      timeout = 1;
		   }
		}
	     else if ((servo_time <= timer1) || timer1_overflowed) /* If timer1 overflows now, we have to be past the timeout point. */
		timeout = 1;
	     }while (!timeout);  /* Wait for next sample */
      }
   }


else if (goose == -2)  /* Open loop block */
   {
   if (accel_increments)
      {
      /* Open loop accel is the # of X4 (timer2) counts to
	 accelerate from the previous dac_command to the new one.
	 ol_setting is the final open loop DAC setting */

      /* Multiply dac increments by 100 to increase resolution.  This
	 is important during long ramps */
      dac_increment = (int)((((long)ol_setting * 100) - ((long)dac_command * 100)) / (long)accel_increments);

      /* Now check for correct bounds */
      if ((ol_setting > dac_command) && (dac_increment < 0))  /* overflow */
	 dac_increment = 0;
      else if ((ol_setting < dac_command) && (dac_increment > 0)) /* underflow */
	 dac_increment = 0;

      if (dac_increment == 0);  /* Don't bother controlling accel */
      else
	 {
	 long_dac_command = (long)dac_command * 100; /* better resolution */
	 loop_num = 400;  /* For diagnostics */
	 for (pct = 1; pct <= accel_increments; pct++) /* Use pct as a counter */
	    {
	    newt2 = timer2 + 4;
	    long_dac_command += (long)dac_increment; /* dac increment * 100 */
	    dac_command = (int)(long_dac_command / 100);
	    out_dac(dac_command, CHECK);

	    /* Now write to the holding ram for testing */
	    if (hold_ctr < 2040)
	       {
	       ioport1 &= 0xC6; /* Select RAM */
	       ioport1 |= 0x39;

	       TEMP_HOLDING[hold_ctr++] = 700;
	       TEMP_HOLDING[hold_ctr++] = newt2;
	       TEMP_HOLDING[hold_ctr++] = dac_increment;
	       TEMP_HOLDING[hold_ctr++] = dac_command;
	       TEMP_HOLDING[hold_ctr++] = 700;

	       ioport1 &= 0xC6;   /* select I/O */
	       }


	    do
	     {
	     /*333333333333333333333333333333333333333333333333333333333333*/

	     #include <EXTLOOP3.H>

	     }while (timer2 < newt2);  /* Wait for next sample */
	    }

	 /* Write for loop ending info */
	 if (hold_ctr < 2040)
	    {
	    ioport1 &= 0xC6; /* Select RAM */
	    ioport1 |= 0x39;

	    TEMP_HOLDING[hold_ctr++] = 900;
	    TEMP_HOLDING[hold_ctr++] = pct;
	    TEMP_HOLDING[hold_ctr++] = newt2;
	    TEMP_HOLDING[hold_ctr++] = dac_command;
	    TEMP_HOLDING[hold_ctr++] = 900;

	    ioport1 &= 0xC6;   /* select I/O */
	    }
	 }  /* End of controlled accel loop. */
      }

   /* ol_setting is the raw dac command */
   out_dac(dac_command = ol_setting, CHECK);
   loop_num = 500;  /* For diagnostics */

   do  /* Wait for something to terminate the block */
	     {
	     /*4444444444444444444444444444444444444444444444444444444444444*/

	     #include <EXTLOOP3.H>

	     }while (1);  /* Loop until something terminates the loop */
   }

end_of_vloop: ;


/************** OUTPUT LOW IMPACT COMMAND HERE IF NEEDED *****************/

if (low_imp_dac)  /* If low impact required */
   out_dac(dac_command = low_imp_dac, CHECK);



/**************** HANDLE DURING OR AT END OUTPUTS NOW *****************/
outputs: ;

/* Check if zspeed has been outputted here. */
if (!zspeed_outputted && last_step)  /* Must wait for zspeed. */
   {
   loop_num = 600;
   do
      {


	     temp_tim2 = timer2;   /* Freeze timer2 */

	     /* Abort if pump not on. */
	     if (!(*RD_DIGITAL_IO & 0x0001)) e_stop = TRUE;

	     if (*RD_DIGITAL_IO & 0x8000) e_stop = TRUE;  /* E stop input present */

	     if (e_stop)
		{
		/* Now write to the holding ram for testing */
		if (hold_ctr < 2040)
		   {
		   ioport1 &= 0xC6; /* Select RAM */
		   ioport1 |= 0x39;

		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;

		   ioport1 &= 0xC6;    /* select I/O */
		   }
		goto ex;
		}

	     if (temp_tim2 >= pos_check_zspeed)
		{
		if (ioport0 & 0x40)  /* True zspeed. */
		   {
		   if (!zspeed_outputted)
		      {
		      TURN_ON_ZSPEED;
		      zspeed_outputted = 1;
		      }

		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC6; /* Select RAM */
		      ioport1 |= 0x39;

		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;

		      ioport1 &= 0xC6;   /* select I/O */
		      }
		   goto outputs;
		   }

		/* Check if pressure and vel. are below the threshold for setting the zspeed output. */
		else if (!zspeed_outputted)
		   {
		   /* First get the pressure value from ana. ch.#2 */
		   *WR_A_TO_D = gain_array[2];
		   ioport1 &= 0xBF;           /* trig conversion */
		   ioport1 |= 0x40;

		   /* Now delay */
		   *WR_A_TO_D = gain_array[1];
		   *WR_A_TO_D = gain_array[1];
		   *WR_A_TO_D = gain_array[1];
		   *WR_A_TO_D = gain_array[1];

		   temp_ana[2] = *RD_A_TO_D & 0x0fff;
		   if (temp_ana[2] & 0x0800) temp_ana[2] |= 0xf000; /* Extend signed value to 16 bits. */

		   /* Now check if velocity is slower than the check velocity
		      and pressure is less than the check point. */
		   if ((*VELOCITY >= w_vel_check_zspeed) && (temp_ana[2] <= w_press_check_zspeed))
		      {
		      TURN_ON_ZSPEED;
		      zspeed_outputted = 1;

		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC6; /* Select RAM */
			 ioport1 |= 0x39;

			 TEMP_HOLDING[hold_ctr++] = loop_num+2;
			 TEMP_HOLDING[hold_ctr++] = loop_num+2;
			 TEMP_HOLDING[hold_ctr++] = loop_num+2;
			 TEMP_HOLDING[hold_ctr++] = loop_num+2;
			 TEMP_HOLDING[hold_ctr++] = loop_num+2;

			 ioport1 &= 0xC6;   /* select I/O */
			 }
		      }
		   }
		}

	     if (tim1_ovfl_ctr == max_block_time)
		{
		/* Now write to the holding ram for testing */
		if (hold_ctr < 2040)
		   {
		   ioport1 &= 0xC6; /* Select RAM */
		   ioport1 |= 0x39;

		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;

		   ioport1 &= 0xC6;  /* select I/O */
		   }
		warning = 2;
		SET_WARNING;
		goto ex;  /* Let's get out of here */
		}



      }while (!zspeed_outputted);
   }

if (e_stop) goto ex;

else
   {
   if (output_indicator == 999);

   /* If during output is on, turn it off now */
   else if (output_indicator == 3)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

   /* If At End  OFF output is required, do it now */
   else if (output_indicator == 4)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

   /* If At End  ON output is required, do it now */
   else if (output_indicator == 5)  *WR_DIGITAL_IO = ~(temp_d_io |= output_number);
   }

/************************** EXIT CONDITIONS *************************/

/* Now update the block_ptr to point to the correct next block to
   execute before exiting. */

ex: ;
block_ptr += 30;   /* Next block */
ioport1 = old_dp;
}





jump_block()
{
/* Framework for the jump block type. */

/* *block_ptr + 2   == Abs. or rel. indicator, 0 = absolute, 1 = rel+, 2 = rel-.
   *block_ptr + 4   == Block to jump to or # of relative blocks to move.
   *block_ptr + 6  == Jump type, 0 == normal, 1 == subroutine, 2 == return.

   *block_ptr + 8 to + 18 reserved for future use.

   *block_ptr + 20  == Variable indicator.
   *block_ptr + 22  == Input check indication.  999 - unconditional,
      xxxxxx d ccc bbbb aa
      |      | |   |    |
      |      | |   |    00 - If, 01 - While
      |      | |   |    10 - Undefined, 11 - Undefined
      |      | |   |
      |      | |   0000 - Input off, 0001 - Input on
      |      | |   0010 - ADC input voltage * 1000, ch.1
      |      | |   0011 - ADC input voltage * 1000, ch.2
      |      | |   0100 - ADC input voltage * 1000, ch.3
      |      | |   0101 - ADC input voltage * 1000, ch.4
      |      | |   0110 - Position in timer2 counts
      |      | |   0111 - Velocity in direct eng. units * 100
      |      | |   1000 - Page 1 variables (V0 - V63)
      |      | |   1001 - Page 2 variables (V64 - V127)
      |      | |   1010 - ADC input voltage * 1000, ch.5
      |      | |   1011 - ADC input voltage * 1000, ch.6
      |      | |   1100 - ADC input voltage * 1000, ch.7
      |      | |   1101 - ADC input voltage * 1000, ch.8
      |      | |   1110 - 1111 Undefined
      |      | |
      |      | 000 Undefined,  001 <=, 010 >=
      |      | 011 =, 100 !=, 101 <, 110 >
      |      | 111 Undefined
      |      |
      |      |
      |      Set when equal to 999d. No input checking.
      |
      Variable number, V0 - V63 page 1, V64 - V127 page 2.

   *block_ptr + 24  == Input to check. Ignored if previous is 999.
   *block_ptr + 26  == Output set or clear indication. 999 - no outputs to set.
		      0 - Beginning OFF, 1 - Beginning ON,
		      3 - During, 4 - At end OFF, 5 - At end ON.
   *block_ptr + 28  == Output to set / clear. Ignored if previous is 999.
*/

unsigned int rel_indicator, block_num, input_indicator, input_number;
unsigned int output_indicator, output_number, temp_ptr, type, pop();
unsigned int variable_indicator;
unsigned int temp;
unsigned char old_dp;
int push(), check_conditional();

old_dp = ioport1;
ioport1 &= 0xC6; /* Select RAM */
ioport1 |= 0x39;

variable_indicator = *((unsigned int * )(block_ptr + 20));

rel_indicator = *((unsigned int * )(block_ptr + 2));

if (variable_indicator & 0x0004)
   block_num = *((unsigned int * )(*((unsigned int * )(block_ptr + 4))));
else  block_num = *((unsigned int * )(block_ptr + 4));

type = *((unsigned int * )(block_ptr + 6));

input_indicator = *((unsigned int * )(block_ptr + 22));

if (variable_indicator & 0x1000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 24))));
else  input_number = *((unsigned int * )(block_ptr + 24));

output_indicator = *((unsigned int * )(block_ptr + 26));

if (variable_indicator & 0x4000)
   output_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC6;   /* select I/O */

/************** FIRST CHECK FOR CONDITIONAL EXECUTION **************/

if (input_indicator == 999);  /* Unconditional execution */
else
   {
   /* WHILE conditional treated like IF if present */
   if (check_conditional(input_indicator, input_number));  /* Condition met */
   else  {block_ptr += 30; goto ex;}
   }

/********** NOW SEE IF ANY OUTPUTS NEED TO BE SET / CLEARED **********/

if (output_indicator == 999);

/* If Beginning ON or During (always turn on), turn on output. */
else if (output_indicator == 1 || output_indicator == 3)
     *WR_DIGITAL_IO = ~(temp_d_io |= output_number);

/* If Beginning OFF, turn off output. */
else if (output_indicator == 0)
     *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

if (type == 2)  /* Return from subroutine */
   {
   /* If stack_ptr == 0, can't be a return from subroutine. */
   if (stack_ptr == 0) block_ptr += 30;  /* Move to next block */
   else  block_ptr = pop();
   }

else if (type == 0 || type == 1)
   {
   if (type == 0);
   else if (type == 1) push(block_ptr + 30);  /* Push next block upon retrun */

   if (!rel_indicator)  /* Absolute jump to block. */
      block_ptr = ((block_num * 30) - 30) + 0xf000;  /* 30 Bytes per block */

   else if (rel_indicator == 1) /* Relative jump forward */
      {
      temp_ptr = block_ptr + (block_num * 30);   /* 30 Bytes per block */
      if (temp_ptr >= 0xff00)  temp_ptr = 0xfefe;  /* Allow 128 blocks */
      block_ptr = temp_ptr;
      }

   else if (rel_indicator == 2)  /* Relative jump backward */
      {
      temp_ptr = block_ptr - (block_num * 30);   /* 30 Bytes per block */
      block_ptr = temp_ptr;
      }
   }

if (output_indicator == 999);

/* If during output is on, turn it off now */
else if (output_indicator == 3)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

/* If At End  OFF output is required, do it now */
else if (output_indicator == 4)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

/* If At End  ON output is required, do it now */
else if (output_indicator == 5)  *WR_DIGITAL_IO = ~(temp_d_io |= output_number);

ex: ;
ioport1 = old_dp;
}




setpos_block()
{
/* Framework for the set position block type */

/* *block_ptr + 2   == Position to set axis to in X4 counts.
   *block_ptr + 4 to + 18 reserved for future use.

   *block_ptr + 20  == Variable indicator.
   *block_ptr + 22  == Input check indication.  999 - unconditional,
      xxxxxx d ccc bbbb aa
      |      | |   |    |
      |      | |   |    00 - If, 01 - While
      |      | |   |    10 - Undefined, 11 - Undefined
      |      | |   |
      |      | |   0000 - Input off, 0001 - Input on
      |      | |   0010 - ADC input voltage * 1000, ch.1
      |      | |   0011 - ADC input voltage * 1000, ch.2
      |      | |   0100 - ADC input voltage * 1000, ch.3
      |      | |   0101 - ADC input voltage * 1000, ch.4
      |      | |   0110 - Position in timer2 counts
      |      | |   0111 - Velocity in direct eng. units * 100
      |      | |   1000 - Page 1 variables (V0 - V63)
      |      | |   1001 - Page 2 variables (V64 - V127)
      |      | |   1010 - ADC input voltage * 1000, ch.5
      |      | |   1011 - ADC input voltage * 1000, ch.6
      |      | |   1100 - ADC input voltage * 1000, ch.7
      |      | |   1101 - ADC input voltage * 1000, ch.8
      |      | |   1110 - 1111 Undefined
      |      | |
      |      | 000 Undefined,  001 <=, 010 >=
      |      | 011 =, 100 !=, 101 <, 110 >
      |      | 111 Undefined
      |      |
      |      |
      |      Set when equal to 999d. No input checking.
      |
      Variable number, V0 - V63 page 1, V64 - V127 page 2.

   *block_ptr + 24  == Input to check. Ignored if previous is 999.
   *block_ptr + 26  == Output set or clear indication. 999 - no outputs to set.
		      0 - Beginning OFF, 1 - Beginning ON,
		      3 - During, 4 - At end OFF, 5 - At end ON.
   *block_ptr + 28  == Output to set / clear. Ignored if previous is 999.
*/

unsigned int position, input_indicator, input_number;
unsigned int output_indicator, output_number, variable_indicator, x;
unsigned char old_dp;
int check_conditional();
extern unsigned int press_initial_setting;

old_dp = ioport1;
ioport1 &= 0xC6; /* Select RAM */
ioport1 |= 0x39;

variable_indicator = *((unsigned int * )(block_ptr + 20));

if (variable_indicator & 0x0002)
   position = *((unsigned int * )(*((unsigned int * )(block_ptr + 2))));
else  position = *((unsigned int * )(block_ptr + 2));

input_indicator = *((unsigned int * )(block_ptr + 22));

if (variable_indicator & 0x1000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 24))));
else  input_number = *((unsigned int * )(block_ptr + 24));

output_indicator = *((unsigned int * )(block_ptr + 26));

if (variable_indicator & 0x4000)
   output_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC6;   /* select I/O */

/************** FIRST CHECK FOR CONDITIONAL EXECUTION **************/

if (input_indicator == 999);  /* Unconditional execution */
else
   {
   /* WHILE conditional treated like IF if present */
   if (check_conditional(input_indicator, input_number));  /* Condition met */
   else  goto ex;
   }


/********** NOW SEE IF ANY OUTPUTS NEED TO BE SET / CLEARED **********/

if (output_indicator == 999);

/* If Beginning ON or During (always turn on), turn on output. */
else if (output_indicator == 1 || output_indicator == 3) *WR_DIGITAL_IO = ~(temp_d_io |= output_number);

/* If Beginning OFF, turn off output. */
else if (output_indicator == 0) *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);


/************************** MAIN PART OF BLOCK ************************/

timer2 = position;  /* Set position. */

/* Now clean the slate */
disable();
still_waiting = 0;
ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
e_stop = FALSE;
initialize_ls();
zspeed_outputted = 0;
follow_through = 0;
TURN_OFF_ZSPEED;

/* Load serial data but do not latch.  Latching will be done after passing
   the intensification ls setpoint. Do not update serial_dac_command
   since the command was not latch.  Update this variable when it is
   latched. */
if (press_initial_setting)
   out_serial_dac((unsigned long)press_initial_setting, NO_LATCH, NO_CHECK);

enable();

hold_ctr = 0;  /* Pointer for test purposes, saving cl info during shot. */

/* Now clear the TEMP_HOLDING ram used for saving cl info. */
ioport1 &= 0xC6; /* Select RAM */
ioport1 |= 0x39;

for (x = 0xC000; x <= 0xCFFE; x++)
    *((unsigned char * ) x) = 0;



/********** NOW SEE IF ANY OUTPUTS NEED TO BE SET / CLEARED **********/

ioport1 &= 0xC6;  /* select I/O */

if (output_indicator == 999);

/* If during output is on, turn it off now */
else if (output_indicator == 3)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

/* If At End  OFF output is required, do it now */
else if (output_indicator == 4)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

/* If At End  ON output is required, do it now */
else if (output_indicator == 5)  *WR_DIGITAL_IO = ~(temp_d_io |= output_number);


/************************** EXIT STUFF ******************************/

/* Now update the block_ptr to point to the correct next block to
   execute before exiting. */

ex: block_ptr += 30;
ioport1 = old_dp;
}





update_block()
{
/* Framework for the update block

control_program[][0]
	
Low 16 bits reserved for block type identifier

High 16 bits = Variable bit indicator and operator.

cccccccc bbbb aaaa
|		 | 	  |
|		 |	  If setting / clearing individual variable bits, bit(s) to 
|		 |	  set / clear are indicated here.
|		 |
|		 Operator
|
|		 0000 No operator, no 2nd. operand
|		 0001 "+" add
|		 0010 "-" subtract
|		 0011 "*" multiply
|		 0100 "/" divide
|		 0101 "&" and
|		 0110 "|" or
|		 0111 "^" xor
|		 1000 - 1111 Available for future expansion
|
Unused, reserved

	 
control_program[][1] 
   
Low 16 bits == Item to update.

   h g f e d c b aaaaaaaaa 
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |
   | | | | | | | Data field
   | | | | | | |
   | | | | | | Extended variable indicator, 32 bit value
   | | | | | |
   | | | | | Set/clear individual variable bit.
   | | | | |
   | | | | Not used, keep cleared.
   | | | |
   | | | DAC output voltage x 1000, data field indicates channel number.
   | | |
   | | Position, X4 counts.  Data field indicates axis #.
   | |
   | Not used, keep cleared.
   |
   Command, data field indicates command number.

Note: When b, c, d, e, f, g and h are low, data field is assumed to be a variable number			  
				
				  
High 16 bits == Voltage ramp specification.

 aaaaaaaaaaaaaaaa
 |			  	
 | 
 If a ramp was specified in setting a DAC channel, value here represents
 mV/250uS.  If no ramp, these should be cleared.

			

control_program[][2]
			
Low 16 bits, 1st. operand.

   h g f e d c b aaaaaaaaa 
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |
   | | | | | | | Data field.
   | | | | | | |
   | | | | | | Extended variable indicator, 32 bit value.
   | | | | | |
   | | | | | Not used, keep cleared.
   | | | | |
   | | | | ADC input voltage x 1000, data field indicates channel number.
   | | | |
   | | | DAC output voltage x 1000, data field indicates channel number.
   | | |
   | | Position X4 counts, data field indicates axis number.
   | |
   | Velocity X1 counts/sec, data field indicates axis number.
   |
   Constant.


High 16 bits, 2nd. operand.

   h g f e d c b aaaaaaaaa 
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |
   | | | | | | | Data field
   | | | | | | |
   | | | | | | Extended variable indicator, 32 bit value
   | | | | | |
   | | | | | Not used, keep cleared
   | | | | |
   | | | | Not used, keep cleared
   | | | |
   | | | Not used, keep cleared
   | | |
   | | Not used, keep cleared
   | |
   | Not used, keep cleared
   |
   Constant

Note: When b, c, d, e, f, g and h are low, data field is assumed to be a variable number

  
control_program[][3] = 1st. operand constant if specified
	
control_program[][4] = 2nd. operand constant if specified

control_program[][5] = Reserved
control_program[][6] = Reserved

control_program[][7] == Test input/set output items.

jjjjj i h ggggggggg|f e  dddd cc bbbb aaaa
|	  |	| | 		| |	 |    |  |	  |	
|	  | | |			| |  |    |  |	  Conditional execution indicator:
|	  | | |         | |  |    |  |	  0000 - Unconditional execution
|	  | | |         | |  |    |  |	  XX01 - If 
|	  | | |         | |  |    |  |	  XX10 - While 	
|	  | | |         | |  |    |  |	  00XX - Input off
|	  | | |         | |  |    |  |	  01XX - Input on
|	  | | |         | |  |    |  |	  10XX - Advanced testing
|	  | | |         | |  |    |  |	  11XX - Reserved
|	  | | |         | |  |    |  |
|	  | | |			| |  |    |	 Operator:
|	  | | |			| |  |    |	 0000 No operator
|	  | | |			| |  |    |	 0001 >
|	  | | |			| |  |    |	 0010 >=
|	  | | |			| |  |    |	 0011 <
|	  | | |			| |  |    |	 0100 <=
|	  | | |			| |  |    |	 0101 =
|	  | | |			| |  |    |	 0110 !=
|	  | | |			| |  |    |	 0111 - 1111 Undefined
|	  | | |			| |  |    |								
|	  | | |			| |  |	  Set outputs:
|	  | | |			| |  |	  00 - No setting outputs
|	  | | |			| |  |	  01 - Beginning
|	  | | |			| |  |	  10 - During
|	  | | |			| |  |	  11 - At end
|	  | | |			| |  |			  
|	  | | |			| |  1st. operand variable bit identifier used when testing individual var bits.
|	  | | |			| |				  
|	  | | |			| Test 1st. operand individual variable bits.
|	  | | |			|
|	  | | |			Not used, reserved.
|	  | | |					  
|	  | | fffffffff = Set output data field.
|	  | |
|	  |	Unused at this time, reserved.
|	  |
|     Descrete output number indicator, if set, data field represents
|	  a descrete I/O number to set/clear.
|	
Unused at this time, reserved.
	
					  

program_control[][8] == Conditional check operands

Low 16 bits, 1st. operand, advanced conditional check

   h g f e d c b aaaaaaaaa 
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |
   | | | | | | | Data field.
   | | | | | | |
   | | | | | | Extended variable indicator, 32 bit value.
   | | | | | |
   | | | | | Descrete I/O number indicator, data field indicates I/O number.
   | | | | |
   | | | | ADC input voltage x 1000, data field indicates channel, 1 - 20.
   | | | |
   | | | DAC output voltage x 1000, data field indicates channel, 1 - 4.
   | | |
   | | Position in X4 counts, data field indicates axis number.
   | |
   | Velocity in X1 counts/sec, data field indicates axis number.
   |
   Not used, reserved

   
High 16 bits, 2nd. operand advanced conditional check

   h g f e d c b aaaaaaaaa 
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |           
   | | | | | | | |
   | | | | | | | Data field.
   | | | | | | |
   | | | | | | Extended variable indicator, 32 bit value.
   | | | | | |
   | | | | | Not used, reserved.
   | | | | |
   | | | | Not used, reserved.
   | | | |
   | | | Not used, reserved.
   | | |
   | | Not used, reserved.
   | |
   | Not used, reserved.
   |
   Constant.

Note: When b, c, d, e, f, g and h are low, data field is assumed to be a variable number


program_control[][9] == 2nd. operand constant.
*/

int out_dac(), out_serial_dac(), check_conditional();
unsigned int input_indicator, input_number;
unsigned int output_indicator, output_number, variable_indicator;
unsigned int temp_vel_count;
unsigned int operator, item_to_update;
unsigned char old_dp;
unsigned long act_vel;
int first_op, second_op, result, tempi;


/* Parameter definitions, defined globally */
extern unsigned long vel_dividend;
extern unsigned int vel_multiplier;


old_dp = ioport1;

ioport1 &= 0xC6;  /* select I/O */

temp_vel_count = *VELOCITY;


ioport1 &= 0xC6; /* Select RAM */
ioport1 |= 0x39;

/* Initialize variables */

variable_indicator = *((unsigned int * )(block_ptr + 20));

item_to_update = *((unsigned int * )(block_ptr + 2));

first_op = *((unsigned int * )(block_ptr + 4));
if (first_op == 200)
      first_op = *((unsigned int * )(block_ptr + 10));  /* Constant */
else if (first_op >= 0 && first_op <= 127)
      first_op = *((unsigned int * )(0xff00 + (2 * first_op))); /* Variable */
else if (first_op == 128) /* DAC voltage */
   {
   /* convert first_op to voltage * 1000 */
   first_op = (int)((((long)dac_command*20000)/16383)-10000);
   }
else if (first_op == 129) /* DAC voltage, ch. 2 */
   {
   /* convert first_op to voltage * 1000 */
   first_op = (int)(((serial_dac_command*20000)/65535)-10000);
   }
else if (first_op == 132) first_op = timer2;  /* Position */
else if (first_op == 133)  /* Velocity */
   {
   /* Convert first_op to velocity in direct units X 100 */
   first_op = (int)((vel_dividend / temp_vel_count) / vel_multiplier);
   }

else if (first_op >= 134 && first_op <= 141) /* Analog input voltage */
   {
   ioport1 &= 0xC6;   /* select I/O */

   a_to_d_loop();

   if (first_op == 134)
      tempi = (int)temp_ana[1];
   else if (first_op == 135)
      tempi = (int)temp_ana[2];
   else if (first_op == 136)
      tempi = (int)temp_ana[3];
   else if (first_op == 137)
      tempi = (int)temp_ana[4];
   else if (first_op == 138)
      tempi = (int)temp_ana[5];
   else if (first_op == 139)
      tempi = (int)temp_ana[6];
   else if (first_op == 140)
      tempi = (int)temp_ana[7];
   else if (first_op == 141)
      tempi = (int)temp_ana[8];

   /* Convert first_op to voltage * 1000. */
   first_op = (int)((((long)tempi*1000)*5)/2047);


   ioport1 &= 0xC6; /* Select RAM */
   ioport1 |= 0x39;
   }
else goto ex;  /* Unreconigzed operand type */

operator = *((unsigned int * )(block_ptr + 6));

second_op = *((unsigned int * )(block_ptr + 8));
if (second_op == 999);  /* No second operand */
else if (second_op == 200)
      second_op = *((unsigned int * )(block_ptr + 12));  /* Constant */
else if (second_op >= 0 && second_op <= 127)
      second_op = *((unsigned int * )(0xff00 + (2 * second_op))); /* Variable */
else if (second_op == 128) /* DAC voltage */
   {
   /* convert second_op to voltage * 1000 */
   second_op = (int)((((long)dac_command*20000)/16383)-10000);
   }
else if (second_op == 129) /* DAC voltage, ch. 2 */
   {
   /* convert second_op to voltage * 1000 */
   second_op = (int)(((serial_dac_command*20000)/65535)-10000);
   }
else if (second_op == 132) second_op = timer2;  /* Position */
else if (second_op == 133)  /* Velocity */
   {
   /* Convert second_op to velocity in direct units X 100 */
   second_op = (int)((vel_dividend / temp_vel_count) / vel_multiplier);
   }

else if (second_op >= 134 && first_op <= 141) /* Analog input voltage */
   {
   ioport1 &= 0xC6;  /* select I/O */

   a_to_d_loop();

   if (second_op == 134)
      tempi = (int)temp_ana[1];
   else if (second_op == 135)
      tempi = (int)temp_ana[2];
   else if (second_op == 136)
      tempi = (int)temp_ana[3];
   else if (second_op == 137)
      tempi = (int)temp_ana[4];
   else if (second_op == 138)
      tempi = (int)temp_ana[5];
   else if (second_op == 139)
      tempi = (int)temp_ana[6];
   else if (second_op == 140)
      tempi = (int)temp_ana[7];
   else if (second_op == 141)
      tempi = (int)temp_ana[8];


   /* Convert second_op to voltage * 1000. */
   second_op = (int)((((long)tempi*1000)*5)/2047);

   ioport1 &= 0xC6; /* Select RAM */
   ioport1 |= 0x39;
   }

else goto ex;  /* Unreconigzed operand type */


input_indicator = *((unsigned int * )(block_ptr + 22));

if (variable_indicator & 0x1000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 24))));
else  input_number = *((unsigned int * )(block_ptr + 24));

output_indicator = *((unsigned int * )(block_ptr + 26));

if (variable_indicator & 0x4000)
   output_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC6;   /* select I/O */


/************** FIRST CHECK FOR CONDITIONAL EXECUTION **************/

if (input_indicator == 999);  /* Unconditional execution */
else
   {
   /* WHILE conditional treated like IF if present */
   if (check_conditional(input_indicator, input_number));  /* Condition met */
   else  goto ex;
   }


/********** NOW SEE IF ANY OUTPUTS NEED TO BE SET / CLEARED **********/

if (output_indicator == 999);

/* If Beginning ON or During (always turn on), turn on output. */
else if (output_indicator == 1 || output_indicator == 3) *WR_DIGITAL_IO = ~(temp_d_io |= output_number);

/* If Beginning OFF, turn off output. */
else if (output_indicator == 0) *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);


/******************** MAIN PORTION OF BLOCK HERE **********************/

if (operator == 999) /* No operator, and therefore no second operator either. */
   {
   result = first_op; /* Can't be easier than this. */
   }

else if (operator != 999 && second_op != 999)
   {
   switch (operator)
      {
      case 0:  /* + */
	 result = first_op + second_op;
	 break;

      case 1:  /* - */
	 result = first_op - second_op;
	 break;

      case 2:  /* * */
	 result = first_op * second_op;
	 break;

      case 3:  /* / */
	 result = first_op / second_op;
	 break;

      default:  /* Unknown operator */
	 goto ex;  /* Exit block */
	 break;
      }
   }

else goto ex;  /* If an operator is present, second_op must != 999 */

/* Now send the result to the right place */

ioport1 &= 0xC6; /* Select RAM */
ioport1 |= 0x39;

if (item_to_update >= 0 && item_to_update <= 127)  /* Variable */
    *((unsigned int *)(0xff00 + (2 * item_to_update))) = result;

else if (item_to_update == 128)  /* Dac word */
   {
   /* result is now voltage*1000.  Convert to a DAC word before sending. */
   if (result > 10000) result = 0x3fff;  /* > 10 volts */
   else if (result < -10000) result = 0; /* < 10 volts */
   else   /* Within range */
      {
      result = (int)((16383*((long)result+10000))/20000);
      }
   out_dac(dac_command = (unsigned int)result, NO_CHECK);
   }

else if (item_to_update == 129)  /* Dac word, ch. 2 */
   {
   /* result is now voltage*1000.  Convert to a DAC word before sending. */
   if (result > 10000) result = 10000;  /* > 10 volts */
   else if (result < -10000) result = -10000; /* < 10 volts */

   serial_dac_command = (65535*((long)result+10000))/20000;
   out_serial_dac(serial_dac_command, LATCH, NO_CHECK);
   }

else if (item_to_update == 132)  /* Position */
   timer2 = result;

else if (item_to_update == 133)  /* Warning */
   {
   warning = result;
   SET_WARNING;
   }

else if (item_to_update == 134)  /* Fatal error */
   {
   fatal_error = result;
   SET_FATAL_ERR;
   }

else goto ex;  /* Unknown item to update */


/**************** HANDLE DURING OR AT END OUTPUTS NOW *****************/

ioport1 &= 0xC6;           /* select I/O */

if (output_indicator == 999);

/* If during output is on, turn it off now */
else if (output_indicator == 3)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

/* If At End  OFF output is required, do it now */
else if (output_indicator == 4)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

/* If At End  ON output is required, do it now */
else if (output_indicator == 5)  *WR_DIGITAL_IO = ~(temp_d_io |= output_number);

/************************** EXIT CONDITIONS *************************/

 /* Now update the block_ptr to point to the correct next block to
   execute before exiting. */

ex: ;
block_ptr += 30;   /* Next block */
ioport1 = old_dp;
}










/* Function check conditional will examine the input_indicator and
   input_number to see if a conditional execution is required.  A return
   value of 1 is returned if an "IF" condition is met or if unconditional
   execution is required.  A zero is returned if either an "IF" or "WHILE"
   condition was not met.  A return value of 2 is returned if a "WHILE ON"
   condition is met.  A return value of 3 is returned if a "WHILE OFF"
   condition is met.  A 4 is returned for all other "WHILE" conditions
   that are met.  This informs the calling program that this function
   must be periodically called to continue checking the "WHILE" condition.
   Variable input_number holds the input number if an input is to be checked
   as well as analog values, position or velocity if those items are to
   be tested. */

int check_conditional (unsigned int input_indicator, unsigned int input_number)
   {
   int return_val;
   unsigned int temp_input_indicator, tempu;
   unsigned char old_dp, operator, condition_met;
   int tempi, physical_operand;

   /* Parameter definitions, defined globally */
   extern unsigned long vel_dividend;
   extern unsigned int vel_multiplier;

   old_dp = ioport1;
   return_val = 999;  /* Initialize with a high number */
   condition_met = 0;

   ioport1 &= 0xC6;  /* select I/O */

   if (input_indicator != 999) /* Conditional execution */
      {
      /* First get type of input to check */
      temp_input_indicator = input_indicator;
      temp_input_indicator &= 0x003c;  /* Mask */
      temp_input_indicator >>= 2;

      switch (temp_input_indicator)
	 {
	 case 0x0000:   /* IF or WHILE input off */
	    /* First check to see in there is an input number.  Setting
	       the input number to zero will bypass conditional checking
	       and the result will be as if we are dealing with an
	       unconditional step. */
	    if (input_number)
	       {
	       if (!(*RD_DIGITAL_IO & input_number))  /* Condition met */
		  {
		  condition_met = 1;
		  switch (input_indicator & 0x0003) /* Check for IF / WHILE */
		     {
		     case 0x0000:  /* IF */
			return_val = 1;
			break;
		     case 0x0001:  /* WHILE */
			return_val = 3;
			break;
		     default:  /* Undefined */
			return_val = 0; /* Treat as un-met condition */
			break;
		     }
		  }
	       }
	    else condition_met = 1; /* If !input_num, treat as unconditional */
	    break;

	 case 0x0001:   /* IF or WHILE input on */
	    /* First check to see in there is an input number.  Setting
	       the input number to zero will bypass conditional checking
	       and the result will be like an unconditional step. */
	    if (input_number)
	       {
	       if (*RD_DIGITAL_IO & input_number)  /* Condition met */
		  {
		  condition_met = 1;
		  switch (input_indicator & 0x0003) /* Check for IF / WHILE */
		     {
		     case 0x0000:  /* IF */
			return_val = 1;
			break;
		     case 0x0001:  /* WHILE */
			return_val = 2;
			break;
		     default:  /* Undefined */
			return_val = 0; /* Treat as un-met condition */
			break;
		     }
		  }
	       }
	    else condition_met = 1; /* If !input_num, treat as unconditional */
	    break;

	 case 0x0002:   /* Ana. ch. #1 */
	 case 0x0003:   /* Ana. ch. #2 */
	 case 0x0004:   /* Ana. ch. #3 */
	 case 0x0005:   /* Ana. ch. #4 */
	 case 0x000a:   /* Ana. ch. #5 */
	 case 0x000b:   /* Ana. ch. #6 */
	 case 0x000c:   /* Ana. ch. #7 */
	 case 0x000d:   /* Ana. ch. #8 */
	    a_to_d_loop();

	    if (temp_input_indicator == 0x0002)
	       tempi = (int)temp_ana[1];
	    if (temp_input_indicator == 0x0003)
	       tempi = (int)temp_ana[2];
	    if (temp_input_indicator == 0x0004)
	       tempi = (int)temp_ana[3];
	    if (temp_input_indicator == 0x0005)
	       tempi = (int)temp_ana[4];
	    if (temp_input_indicator == 0x000a)
	       tempi = (int)temp_ana[5];
	    if (temp_input_indicator == 0x000b)
	       tempi = (int)temp_ana[6];
	    if (temp_input_indicator == 0x000c)
	       tempi = (int)temp_ana[7];
	    if (temp_input_indicator == 0x000d)
	       tempi = (int)temp_ana[8];

	    /* Convert physical_operand to voltage * 1000. */
	    physical_operand = (int)((((long)tempi*1000)*5)/2047);
	    break;

	 case 0x0006:   /* Position */
	    physical_operand = (int)timer2;
	    break;

	 case 0x0007:   /* Velocity */
	    tempu = *VELOCITY;
	    physical_operand = (int)((vel_dividend / tempu) / vel_multiplier);
	    /* physical_operand is now velocity X 100 327.67 IPS max before becomming negative. */
	    break;

	 case 0x0008:  /* 1st. page Variable, V0 - V63 */
	    temp_input_indicator = input_indicator >> 10;

	    /* temp_input_indicator is now the variable number. */
	    ioport1 &= 0xC6; /* Select RAM */
	    ioport1 |= 0x39;

	    temp_input_indicator = (temp_input_indicator * 2) + 0xff00; /* Convert to variable's address. */
	    physical_operand = *((int *)temp_input_indicator);
	    break;

	 case 0x0009:  /* 2nd. page variable, V64 - V127 */
	    temp_input_indicator = (input_indicator >> 10) + 64;

	    /* temp_input_indicator is now the variable number. */
	    /* temp_input_indicator is now the variable number. */
	    ioport1 &= 0xC6; /* Select RAM */
	    ioport1 |= 0x39;

	    temp_input_indicator = (temp_input_indicator * 2) + 0xff00; /* Convert to variable's address. */
	    physical_operand = *((int *)temp_input_indicator);
	    break;

	 default:       /* 0x000e and 0x000f undefined */
	    return_val = 0;  /* Treat as unconditional if unsupported type. */
	    break;
	 }

      if (return_val == 999 && !condition_met)  /* Look for an operator */
	 {
	 /* Now extract operator */
	 temp_input_indicator = input_indicator;
	 temp_input_indicator &= 0x01c0;  /* Mask */
	 temp_input_indicator >>= 6;

	 switch (temp_input_indicator)
	    {
	    case 0x0000:   /* Undefined */
	       return_val = 0;  /* Treat as unmet */
	       break;

	    case 0x0001:  /* <= */
	       if (physical_operand <= (int)input_number)  /* Condition met */
		  condition_met = 1;
	       break;

	    case 0x0002:  /* >= */
	       if (physical_operand >= (int)input_number)
		  condition_met = 1;
	       break;

	    case 0x0003:  /* = */
	       if (physical_operand == (int)input_number)
		  condition_met = 1;
	       break;

	    case 0x0004:  /* != */
	       if (physical_operand != (int)input_number)
		  condition_met = 1;
	       break;

	    case 0x0005:  /* < */
	       if (physical_operand < (int)input_number)
		  condition_met = 1;
	       break;

	    case 0x0006:  /* > */
	       if (physical_operand > (int)input_number)
		  condition_met = 1;
	       break;

	    case 0x0007:  /* Undefined */
	       return_val = 0;  /* Treat as unmet */
	       break;

	    default:      /* Undefined operators */
	       return_val = 0;  /* Treat as unmet */
	       break;
	    }
	 }

      if (return_val != 999); /* Already set */
      else if (condition_met)  /* Set return_val */
	 {
	 switch (input_indicator & 0x0003) /* Check for IF / WHILE */
	    {
	    case 0x0000:  /* IF */
	       return_val = 1;
	       break;
	    case 0x0001:  /* WHILE PHYSICAL */
	       return_val = 4;
	       break;
	    default:  /* Undefined */
	       return_val = 0; /* Treat as un-met condition */
	       break;
	    }
	 }
      else return_val = 0;  /* Condition un-met. */
      }

   else return_val = 1;  /* Unconditional execution */

   ioport1 = old_dp;
   return (return_val);
   }

#endif

