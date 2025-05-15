// 				  control_program_blocks8.h

//				Copyright 2001, 2002, 2003 Visi-Trak Worldwide 


#define MAX_STEP 203
#define IMMED_EXECUTE_BLOCK 0
#define MAX_DAC_CH 4
#define MAX_ADC_CH 20
#define MAX_VAR 511
#define MAX_CMD 511
#define MAX_DISCRETE_IO_NUM 511
#define UPDATE 1
#define VELOCITY 2
#define JUMP 3
#define DWELL 4
#define POSITION 5
#define END_ 6
#define START_ 7

#define IMMED_EXE_BLK vtg.control_program[IMMED_EXECUTE_BLOCK][0]

typedef volatile unsigned int (*c_block)[10];  // Control block type, an array of 10 words
//typedef volatile unsigned int (c_block)[10];  // Control block type, an array of 10 words

// Control block execution function.
//typedef int (*control_block_function)(unsigned int (*control_program)[10], int type_of_execution, int zone);
typedef volatile int (*control_block_function)(c_block control_block, int type_of_execution, int zone);


// Define bit fields for the conditional check / set output stuff.
// These fields will be the same for all block types.

// cond_indicator_bits definitions.
enum
	{
	if_bit					= 0,
	while_bit				= 1,
	input_on_bit			= 2,
	input_off_bit			= 3,
	advanced_testing_bit	= 4,
	set_out_beginning_bit	= 5,
	set_out_during_bit		= 6,
	set_out_at_end_bit		= 7,
	turn_on_turn_off_bit	= 8,
	set_out_var_bit			= 9,
	op1_var_bit				= 10,
	op1_discrete_io_bit		= 11,
	op1_adc_bit				= 12,
	op1_dac_bit				= 13,
	op1_pos_bit				= 14,
	op1_vel_bit				= 15,
	op2_var_bit				= 16,
	op2_const_bit			= 17,
	// Operator bits	
	eq_bit					= 18, 
	neq_bit					= 19,
	gtr_than_bit			= 20,
	gtr_than_eq_to_bit		= 21,
	less_than_bit			= 22,
	less_than_eq_to_bit		= 23,
	
	// Masks
	if_mask					= (1<<0),
	while_mask				= (1<<1),
	input_on_mask			= (1<<2),
	input_off_mask			= (1<<3),
	advanced_testing_mask	= (1<<4),
	set_out_beginning_mask	= (1<<5),
	set_out_during_mask		= (1<<6),
	set_out_at_end_mask		= (1<<7),
	turn_on_turn_off_mask	= (1<<8),
	set_out_var_mask		= (1<<9),
	op1_var_mask			= (1<<10),
	op1_discrete_io_mask	= (1<<11), // Standard Opto 22 digital inputs
	op1_adc_mask			= (1<<12),
	op1_dac_mask			= (1<<13),
	op1_pos_mask			= (1<<14),
	op1_vel_mask			= (1<<15),
	op2_var_mask			= (1<<16),
	op2_const_mask			= (1<<17),	
	// Operator masks	
	eq_mask					= (1<<18), 
	neq_mask				= (1<<19),
	gtr_than_mask			= (1<<20),
	gtr_than_eq_to_mask		= (1<<21),
	less_than_mask			= (1<<22),
	less_than_eq_to_mask	= (1<<23),
	// Bits 24 - 31 not defined.
	};



//////////// Update block definitions /////////////

// ub_indicator_bits definition.
enum
	{
	ub_itu_var_bit			= 0,
	ub_itu_dac_out_bit		= 1,
	ub_itu_pos_bit			= 2,
	ub_itu_command_bit		= 3,
	ub_op1_var_bit			= 4,
	ub_op1_adc_bit			= 5,
	ub_op1_dac_bit			= 6,
	ub_op1_pos_bit			= 7,
	ub_op1_vel_bit			= 8,
	ub_op1_const_bit		= 9,
	ub_op2_var_bit			= 10,
	ub_op2_const_bit		= 11,
	ub_vramp_var_bit		= 12,
	ub_vramp_const_bit		= 13,
	// Operator bits
	ub_add_bit				= 14,
	ub_sub_bit				= 15,
	ub_mul_bit				= 16,
	ub_div_bit				= 17,
	ub_and_bit				= 18,
	ub_or_bit				= 19,
	ub_xor_bit				= 20,
	// Misc bits
	ub_dac_increment_bit	= 21,
	ub_dac_itu_is_axis_bit	= 22,
		
	// Masks
	ub_itu_var_mask			= (1<<0),
	ub_itu_dac_out_mask		= (1<<1),
	ub_itu_pos_mask			= (1<<2),
	ub_itu_command_mask		= (1<<3),
	ub_op1_var_mask			= (1<<4),
	ub_op1_adc_mask			= (1<<5),
	ub_op1_dac_mask			= (1<<6),
	ub_op1_pos_mask			= (1<<7),
	ub_op1_vel_mask			= (1<<8),
	ub_op1_const_mask		= (1<<9),
	ub_op2_var_mask			= (1<<10),
	ub_op2_const_mask		= (1<<11),
	ub_vramp_var_mask		= (1<<12),
	ub_vramp_const_mask		= (1<<13),
	// Operator masks
	ub_add_mask			= (1<<14),
	ub_sub_mask			= (1<<15),
	ub_mul_mask				= (1<<16),
	ub_div_mask				= (1<<17),
	ub_and_mask				= (1<<18),
	ub_or_mask				= (1<<19),
	ub_xor_mask				= (1<<20),
	// Misc masks
	ub_dac_increment_mask	= (1<<21),
	ub_dac_itu_is_axis_mask	= (1<<22),
	// Bits 23 - 31 not defined.
	}; 


typedef volatile struct
{
	//control_block_function blk_to_execute;			//Word0
	int blk_id;								//Word0
	unsigned int ub_indicator_bits;			//Word1
	unsigned int itu_df;					//Word2
	unsigned int op1_df;   					//Word3
	unsigned int op2_df;  					//Word4
	unsigned int vramp_df;		  			//Word5
	unsigned int not_used2;					//Word6
	unsigned int cond_indicator_bits;		//Word7
	unsigned short int set_out_df;			//Word8
	unsigned short int cond_op1_df;			// " "
	unsigned int cond_op2_df; 				//Word9
} ub; // Update block definition





//////////////// Dwell block definitions /////////////////

// db_indicator_bits definition.
enum
	{
	db_var_bit		= 0,
	db_const_bit	= 1,

	// Masks
	db_var_mask		= (1<<0),
	db_const_mask	= (1<<1),
	// Bits 2 - 31 not defined.
	}; 

typedef volatile struct
{
	//control_block_function blk_to_execute;			//Word0
	int blk_id;							//Word0
	unsigned int db_indicator_bits;		//Word1
	unsigned int dwell_df;				//Word2
	unsigned int not_used2;				//Word3
	unsigned int not_used3;				//Word4
	unsigned int not_used4;				//Word5
	unsigned int not_used5;				//Word6
	unsigned int cond_indicator_bits;	//Word7
	unsigned short int set_out_df;		//Word8
	unsigned short int cond_op1_df;		// " "
	unsigned int cond_op2_df; 			//Word9
} db; //Dwell block specification





///////////////// Position block definitions ///////////////////

// pb_indicator_bits definition.
enum
	{
	pb_pos_var_bit		= 0,
	pb_pos_const_bit	= 1,
	pb_axis1_bit		= 2,
	pb_axis2_bit		= 3,
	pb_axis3_bit		= 4,
	pb_axis4_bit		= 5,
	pb_vel_var_bit		= 6,
	pb_vel_const_bit	= 7,

	// Masks
	pb_pos_var_mask		= (1<<0),
	pb_pos_const_mask	= (1<<1),
	pb_axis1_mask		= (1<<2),
	pb_axis2_mask		= (1<<3),
	pb_axis3_mask		= (1<<4),
	pb_axis4_mask		= (1<<5),
	pb_vel_var_mask		= (1<<6),
	pb_vel_const_mask	= (1<<7),
	// Bits 8 - 31 not used.
	};

typedef volatile struct
{
	//control_block_function blk_to_execute;			//Word0
	int blk_id;							//Word0
	unsigned int pb_indicator_bits;		//Word1
	unsigned int pos_df;				//Word2
	unsigned int vel_df; 				//Word3
	unsigned int not_used2;				//Word4
	unsigned int not_used3;				//Word5
	unsigned int not_used4;				//Word6
	unsigned int cond_indicator_bits;	//Word7
	unsigned short int set_out_df;		//Word8
	unsigned short int cond_op1_df;		// " "
	unsigned int cond_op2_df; 			//Word9
} pb; //Position block specification





////////////////// Velocity block definitions ///////////////////

// vb_indicator_bits definition.
enum
	{
	vb_vel_var_bit			= 0,
	vb_vel_const_bit		= 1,
	vb_axis1_bit			= 2,
	vb_axis2_bit			= 3,
	vb_axis3_bit			= 4,
	vb_axis4_bit			= 5,
	vb_accel_var_bit		= 6,
	vb_accel_const_bit		= 7,
	vb_endpos_var_bit		= 8,
	vb_endpos_const_bit		= 9,

	// Masks
	vb_vel_var_mask			= (1<<0),
	vb_vel_const_mask		= (1<<1),
	vb_axis1_mask			= (1<<2),
	vb_axis2_mask			= (1<<3),
	vb_axis3_mask			= (1<<4),
	vb_axis4_mask			= (1<<5),
	vb_accel_var_mask		= (1<<6),
	vb_accel_const_mask		= (1<<7),
	vb_endpos_var_mask		= (1<<8),
	vb_endpos_const_mask	= (1<<9),
	// Bits 10 - 31 undefined.
	};


typedef volatile struct
{
	//control_block_function blk_to_execute;		//Word0
	int blk_id;							//Word0
	unsigned int vb_indicator_bits;		//Word1
	unsigned int vel_df;				//Word2	 		
	unsigned int accel_df;				//Word3
	int endpos_df;		  				//Word4
	unsigned int not_used2;				//Word5
	unsigned int not_used3;				//Word6
	unsigned int cond_indicator_bits;	//Word7
	unsigned short int set_out_df;		//Word8
	unsigned short int cond_op1_df;		// " "
	unsigned int cond_op2_df; 			//Word9
} vb; //Velocity block specification




//////////////////// Jump block /////////////////////

// jb_indicator_bits definitions.
enum
	{
	jb_rel_abs_specifier_bit	= 0, // ==1 for rel, 0 for abs
	jb_type_normal_bit			= 1,
    jb_type_sub_bit				= 2,
	jb_type_return_bit			= 3,
	jb_rel_dir_bit				= 4, // 1 for forward, 0 for backwards
	jb_jump_var_bit				= 5,
	jb_jump_const_bit			= 6,
	
	// Masks
	jb_rel_abs_specifier_mask	= (1<<0), // ==1 for rel, 0 for abs
	jb_type_normal_mask			= (1<<1),
    jb_type_sub_mask			= (1<<2),
	jb_type_return_mask			= (1<<3),
	jb_rel_dir_mask				= (1<<4), // 1 for forward, 0 for backwards
	jb_jump_var_mask			= (1<<5),
	jb_jump_const_mask			= (1<<6),
	// Bits 8 - 31 undefined.
	};


typedef volatile struct
{
	//control_block_function blk_to_execute;		//Word0
	int blk_id;							//Word0
	unsigned int jb_indicator_bits;		//Word1
	unsigned int jump_df;				//Word2
	unsigned int not_used2;				//Word3
	unsigned int not_used3;				//Word4
	unsigned int not_used4;				//Word5
	unsigned int not_used5;				//Word6
	unsigned int cond_indicator_bits;	//Word7
	unsigned short int set_out_df;		//Word8
	unsigned short int cond_op1_df;		// " "
	unsigned int cond_op2_df; 			//Word9
} jb; //Jump block specification


//////////////////// End block /////////////////////

typedef volatile struct
{
	//control_block_function blk_to_execute;	//Word0
	int blk_id;						//Word0
	unsigned int not_used1;			//Word1
	unsigned int not_used2;			//Word2
	unsigned int not_used3;			//Word3
	unsigned int not_used4;			//Word4
	unsigned int not_used5;			//Word5
	unsigned int not_used6;			//Word6
	unsigned int not_used7;			//Word7
	unsigned int not_used8;			//Word8
	unsigned int not_used9;			//Word9
} eb; //End block specification



// Define type of execution
#define IMMED 1
#define PROGRAM 2

#define WHILE_CONDITIONAL_FAILED 20 //The last zone of any block, the one that terminates the block

// Define set output checking items
#define SET_OUT_BEG_OR_DUR ((block->cond_indicator_bits & set_out_beginning_mask) || (block->cond_indicator_bits & set_out_during_mask))
#define SET_OUT_DUR (block->cond_indicator_bits & set_out_during_mask)
#define SET_OUT_AT_END (block->cond_indicator_bits & set_out_at_end_mask)
#define SET_OUT_DUR_OR_AT_AND ((block->cond_indicator_bits & set_out_during_mask) || (block->cond_indicator_bits & set_out_at_end_mask))

#define SET_CLR_CONT_OUTPUT_NOW set_clr_discrete_output((int)block->set_out_df, (int)(block->cond_indicator_bits & turn_on_turn_off_mask))
#define INVERT_CONT_OUTPUT_NOW set_clr_discrete_output((int)block->set_out_df, (int)!(block->cond_indicator_bits & turn_on_turn_off_mask))		

typedef volatile struct
	{
	//control_block_function blk_to_execute; 
	int blk_id;
	int zone_to_goto;
	} what_to_do;

// An array of pointers to the various block functions.
control_block_function block_function_ptr[8];

// Initialize the above array.
#define INITIALIZE_CONTROL_BLOCKS \
								block_function_ptr[0]=NULL; \
								block_function_ptr[UPDATE]=(control_block_function)update_block_exe; \
								block_function_ptr[VELOCITY]=(control_block_function)velocity_block_exe; \
								block_function_ptr[JUMP]=(control_block_function)jump_block_exe; \
								block_function_ptr[DWELL]=(control_block_function)dwell_block_exe; \
								block_function_ptr[POSITION]=(control_block_function)position_block_exe; \
								block_function_ptr[END_]=(control_block_function)end_block_exe; \
								block_function_ptr[START_]=(control_block_function)start_block_exe

what_to_do when_cont_isr[9];
// These correspond to the control interrupt input bits.
// when_cont_isr[0] == Stop control program
// when_cont_isr[1] == Start control program
// when_cont_isr[2] == block timeout
// when_cont_isr[3] == GP pos1 (Block ending)
// when_cont_isr[4] == I/O 1 (Block ending)
// when_cont_isr[5] == GP pos2 (Non block ending)
// when_cont_isr[6] == I/O 2 (Non block ending)
// when_cont_isr[7] == X1 pulse, As position increments .0125" x 4, from pvu
// when_cont_isr[8] == timer2 interrupt

// control interrupt masks, 9 bit input pio
#define STOP_PROG_MASK (1<<0)
#define START_PROG_MASK (1<<1)
#define BLK_TIMEOUT_MASK (1<<2)
#define GP_POS1_INT_MASK (1<<3)
#define IO_1_MASK (1<<4)
#define GP_POS2_INT_MASK (1<<5)
#define IO_2_MASK (1<<6)
#define X1_INT_MASK (1<<7)
#define TIMER2_INT_MASK (1<<8)

// control interrupt bits, 9 bit input pio. 
#define STOP_PROG_BIT	0
#define START_PROG_BIT  1
#define BLK_TIMEOUT_BIT 2
#define GP_POS1_INT_BIT 3
#define IO_1_BIT		4
#define GP_POS2_INT_BIT 5
#define IO_2_BIT		6
#define X1_INT_BIT		7
#define TIMER2_INT_BIT  8

// control interrupt output bit masks, 6 bit output pio
#define STOP_PROG_OUT_BIT_MASK (1<<0)
#define START_PROG_OUT_BIT_MASK (1<<1)
#define BLK_TIMEOUT_OUT_BIT_MASK (1<<2)
#define IO_1_OUT_BIT_MASK (1<<3)
#define IO_2_OUT_BIT_MASK (1<<4)
#define TIMER2_TIMEOUT_BIT_MASK (1<<5)

// Control output pio sets
#define SET_BLOCK_TIMEOUT_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status |= BLK_TIMEOUT_OUT_BIT_MASK)
#define SET_IO_1_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status |= IO_1_OUT_BIT_MASK)
#define SET_IO_2_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status |= IO_2_OUT_BIT_MASK)
#define SET_START_PROGRAM_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status |= START_PROG_OUT_BIT_MASK)
#define SET_STOP_PROGRAM_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status |= STOP_PROG_OUT_BIT_MASK)
#define SET_TIMER2_TIMEOUT_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status |= TIMER2_TIMEOUT_BIT_MASK)

#define CLR_ALL_CONTROL_INTERRUPTS  cont_int_output->np_piodata = (vtg.cont_int_out_status = 0)
#define CLR_BLOCK_TIMEOUT_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status &= ~BLK_TIMEOUT_OUT_BIT_MASK)
#define CLR_IO_1_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status &= ~IO_1_OUT_BIT_MASK)
#define CLR_IO_2_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status &= ~IO_2_OUT_BIT_MASK)
#define CLR_START_PROGRAM_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status &= ~START_PROG_OUT_BIT_MASK)
#define CLR_STOP_PROGRAM_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status &= ~STOP_PROG_OUT_BIT_MASK)
#define CLR_TIMER2_TIMEOUT_INTERRUPT cont_int_output->np_piodata = (vtg.cont_int_out_status &= ~TIMER2_TIMEOUT_BIT_MASK)

#define MASK_ALL_CONT_INTERRUPTS	control_int->np_piointerruptmask = 0
#define MASK_GP_POS1_INTERRUPT		control_int->np_piointerruptmask &= ~GP_POS1_INT_MASK
#define MASK_GP_POS2_INTERRUPT		control_int->np_piointerruptmask &= ~GP_POS2_INT_MASK
#define MASK_X1_INTERRUPT			control_int->np_piointerruptmask &= ~X1_INT_MASK
#define	MASK_BLK_TIMEOUT_INTERRUPT	control_int->np_piointerruptmask &= ~BLK_TIMEOUT_MASK
#define MASK_IO_1_INTERRUPT			control_int->np_piointerruptmask &= ~IO_1_MASK
#define MASK_IO_2_INTERRUPT			control_int->np_piointerruptmask &= ~IO_2_MASK
#define MASK_START_PROG_INTERRUPT	control_int->np_piointerruptmask &= ~START_PROG_MASK	
#define MASK_STOP_INTERRUPT			control_int->np_piointerruptmask &= ~STOP_PROG_MASK
#define MASK_TIMER2_INTERRUPT		control_int->np_piointerruptmask &= ~TIMER2_INT_MASK

#define UNMASK_ALL_CONT_INTERRUPTS		control_int->np_piointerruptmask = 0x1ff
#define UNMASK_GP_POS1_INTERRUPT		control_int->np_piointerruptmask |= GP_POS1_INT_MASK
#define UNMASK_GP_POS2_INTERRUPT		control_int->np_piointerruptmask |= GP_POS2_INT_MASK
#define UNMASK_X1_INTERRUPT				control_int->np_piointerruptmask |= X1_INT_MASK
#define	UNMASK_BLK_TIMEOUT_INTERRUPT	control_int->np_piointerruptmask |= BLK_TIMEOUT_MASK
#define UNMASK_IO_1_INTERRUPT			control_int->np_piointerruptmask |= IO_1_MASK
#define UNMASK_IO_2_INTERRUPT			control_int->np_piointerruptmask |= IO_2_MASK
#define UNMASK_START_PROG_INTERRUPT		control_int->np_piointerruptmask |= START_PROG_MASK	
#define UNMASK_STOP_INTERRUPT			control_int->np_piointerruptmask |= STOP_PROG_MASK
#define UNMASK_TIMER2_INTERRUPT			control_int->np_piointerruptmask |= TIMER2_INT_MASK

#define INITIALIZE_CONTROL \
							MASK_ALL_CONT_INTERRUPTS; \
							CLR_ALL_CONTROL_INTERRUPTS; \
							vtg.blk_timeout_time = 0; \
							vtg.immed_in_process = 0; \
							IMMED_EXE_BLK = NULL; \
							vtg.io_int1 = 0; \
							vtg.io_int2 = 0; \
							for (x=0; x<9; x++) \
								{ \
								/*when_cont_isr[x].blk_to_execute = 0;*/ \
								when_cont_isr[x].blk_id = 0; \
								when_cont_isr[x].zone_to_goto = 0; \
								} \
							/* Clear out the IMMED block location. */ \
							for (x=0; x<10; x++) vtg.control_program[IMMED_EXECUTE_BLOCK][x] = 0; \
							/*when_cont_isr[0].blk_to_execute = (control_block_function)end_block_exe;*/ \
							/*when_cont_isr[1].blk_to_execute = (control_block_function)start_block_exe;*/ \
							when_cont_isr[0].blk_id = END_; \
							when_cont_isr[1].blk_id = START_; \
							vtg.prog_ctr = 1; \
							vtg.stack_ptr = 0; \
							timer2->np_timercontrol = 8; \
							vtg.timer2_timeout = 0; \
							UNMASK_START_PROG_INTERRUPT; \
							UNMASK_STOP_INTERRUPT


// Macros to arm and disarm control interrupts.  These should only be used within control
// blocks.  Zone is the block_exe zone to go to after returning upon interrupt.
#define ARM_BLK_TIMEOUT_CONT_INTERRUPT(time_mS, zone) \
								vtg.blk_timeout_time = ONE_MS_COUNTER + time_mS; \
								/*when_cont_isr[BLK_TIMEOUT_BIT].blk_to_execute=(control_block_function)*saved_control_block[0];*/ \
								when_cont_isr[BLK_TIMEOUT_BIT].blk_id=(int)*saved_control_block[0]; \
								when_cont_isr[BLK_TIMEOUT_BIT].zone_to_goto=zone; \
								CLR_BLOCK_TIMEOUT_INTERRUPT; \
								UNMASK_BLK_TIMEOUT_INTERRUPT
								
#define ARM_GP_POS1_CONT_INTERRUPT(pos, zone) \
								WR_PV_GP_POS1 = pos; \
								/*when_cont_isr[GP_POS1_INT_BIT].blk_to_execute=(control_block_function)*saved_control_block[0];*/ \
								when_cont_isr[GP_POS1_INT_BIT].blk_id=(int)*saved_control_block[0]; \
								when_cont_isr[GP_POS1_INT_BIT].zone_to_goto=zone; \
								UNMASK_GP_POS1_INTERRUPT


#define ARM_GP_POS2_CONT_INTERRUPT(pos, zone) \
								WR_PV_GP_POS2 = pos; \
								/*when_cont_isr[GP_POS2_INT_BIT].blk_to_execute=(control_block_function)*saved_control_block[0];*/ \
								when_cont_isr[GP_POS2_INT_BIT].blk_id=(int)*saved_control_block[0]; \
								when_cont_isr[GP_POS2_INT_BIT].zone_to_goto=zone; \
								UNMASK_GP_POS2_INTERRUPT

#define ARM_IO1_CONT_INTERRUPT(io_num, on_off, zone) \
								vtg.on_off1 = on_off; \
								vtg.io_int1 = io_num; \
								/*when_cont_isr[IO_1_BIT].blk_to_execute=(control_block_function)*saved_control_block[0];*/ \
								when_cont_isr[IO_1_BIT].blk_id=(int)*saved_control_block[0]; \
								when_cont_isr[IO_1_BIT].zone_to_goto=zone; \
								CLR_IO_1_INTERRUPT; \
								UNMASK_IO_1_INTERRUPT

#define ARM_IO2_CONT_INTERRUPT(io_num, on_off, zone) \
								vtg.on_off2 = on_off; \
								vtg.io_int2 = io_num; \
								/*when_cont_isr[IO_2_BIT].blk_to_execute=(control_block_function)*saved_control_block[0];*/ \
								when_cont_isr[IO_2_BIT].blk_id=(int)*saved_control_block[0]; \
								when_cont_isr[IO_2_BIT].zone_to_goto=zone; \
								CLR_IO_2_INTERRUPT; \
								UNMASK_IO_2_INTERRUPT

#define ARM_X1_PULSE_CONT_INTERRUPT(zone) \
								/*when_cont_isr[X1_INT_BIT].blk_to_execute=(control_block_function)*saved_control_block[0];*/ \
								when_cont_isr[X1_INT_BIT].blk_id=(int)*saved_control_block[0]; \
								when_cont_isr[X1_INT_BIT].zone_to_goto=zone; \
								UNMASK_X1_INTERRUPT

#define ARM_TIMER2_CONT_INTERRUPT(timl, timh, cont, zone) \
								timer2->np_timercontrol = 8; /* Stop timer */ \
								vtg.timer2_timeout = 0; /* We don't really use this */ \
								/*when_cont_isr[TIMER2_INT_BIT].blk_to_execute=(control_block_function)*saved_control_block[0];*/ \
								when_cont_isr[TIMER2_INT_BIT].blk_id=(int)*saved_control_block[0]; \
								when_cont_isr[TIMER2_INT_BIT].zone_to_goto=zone; \
								timer2->np_timerperiodl = timl; \
								timer2->np_timerperiodh = timh; \
								if (cont) timer2->np_timercontrol = 7; /* run continously */ \
								else timer2->np_timercontrol = 5; /* run one time */ \
								CLR_TIMER2_TIMEOUT_INTERRUPT; \
								UNMASK_TIMER2_INTERRUPT



#define DISARM_BLK_TIMEOUT_CONT_INTERRUPT \
								vtg.blk_timeout_time = 0; \
								/*when_cont_isr[BLK_TIMEOUT_BIT].blk_to_execute=NULL;*/ \
								when_cont_isr[BLK_TIMEOUT_BIT].blk_id=NULL; \
								when_cont_isr[BLK_TIMEOUT_BIT].zone_to_goto=NULL; \
								MASK_BLK_TIMEOUT_INTERRUPT; \
								CLR_BLOCK_TIMEOUT_INTERRUPT; \
								control_int->np_pioedgecapture &= ~BLK_TIMEOUT_MASK
								
#define DISARM_GP_POS1_CONT_INTERRUPT \
								WR_PV_GP_POS1 = 0x7fffff; \
								/*when_cont_isr[GP_POS1_INT_BIT].blk_to_execute=NULL;*/ \
								when_cont_isr[GP_POS1_INT_BIT].blk_id=NULL; \
								when_cont_isr[GP_POS1_INT_BIT].zone_to_goto=NULL; \
								MASK_GP_POS1_INTERRUPT; \
								control_int->np_pioedgecapture &= ~GP_POS1_INT_MASK


#define DISARM_GP_POS2_CONT_INTERRUPT \
								WR_PV_GP_POS2 = 0x7fffff; \
								/*when_cont_isr[GP_POS2_INT_BIT].blk_to_execute=NULL;*/ \
								when_cont_isr[GP_POS2_INT_BIT].blk_id=NULL; \
								when_cont_isr[GP_POS2_INT_BIT].zone_to_goto=NULL; \
								MASK_GP_POS2_INTERRUPT; \
								control_int->np_pioedgecapture &= ~GP_POS2_INT_MASK

#define DISARM_IO1_CONT_INTERRUPT \
								vtg.io_int1 = 0; \
								/*when_cont_isr[IO_1_BIT].blk_to_execute=NULL;*/ \
								when_cont_isr[IO_1_BIT].blk_id=NULL; \
								when_cont_isr[IO_1_BIT].zone_to_goto=NULL; \
								MASK_IO_1_INTERRUPT; \
								CLR_IO_1_INTERRUPT; \
								control_int->np_pioedgecapture &= ~IO_1_MASK

#define DISARM_IO2_CONT_INTERRUPT \
								vtg.io_int2 = 0; \
								/*when_cont_isr[IO_2_BIT].blk_to_execute=NULL;*/ \
								when_cont_isr[IO_2_BIT].blk_id=NULL; \
								when_cont_isr[IO_2_BIT].zone_to_goto=NULL; \
								MASK_IO_2_INTERRUPT; \
								CLR_IO_2_INTERRUPT; \
								control_int->np_pioedgecapture &= ~IO_2_MASK

#define DISARM_X1_PULSE_CONT_INTERRUPT \
								/*when_cont_isr[X1_INT_BIT].blk_to_execute=NULL;*/ \
								when_cont_isr[X1_INT_BIT].blk_id=NULL; \
								when_cont_isr[X1_INT_BIT].zone_to_goto=NULL; \
								MASK_X1_INTERRUPT; \
								control_int->np_pioedgecapture &= ~X1_INT_MASK

#define DISARM_TIMER2_CONT_INTERRUPT \
								timer2->np_timercontrol = 8; /* Stop timer */ \
								vtg.timer2_timeout = 0; /* We don't really use this */ \
								/*when_cont_isr[TIMER2_INT_BIT].blk_to_execute=NULL;*/ \
								when_cont_isr[TIMER2_INT_BIT].blk_id=NULL; \
								when_cont_isr[TIMER2_INT_BIT].zone_to_goto=NULL; \
								MASK_TIMER2_INTERRUPT; \
								CLR_TIMER2_TIMEOUT_INTERRUPT; \
								control_int->np_pioedgecapture &= ~TIMER2_INT_MASK







//Prototypes
int block_conditional_compile(c_block control_block, char *condstr);
int update_block_compile(char *str_to_compile);
int dwell_block_compile(char *str_to_compile);
int position_block_compile(char *str_to_compile);
int velocity_block_compile(char *str_to_compile);
int jump_block_compile(char *str_to_compile);	
int end_block_compile(char *str_to_compile);	
int block_conditional_exe(c_block control_block);
int update_block_exe(c_block control_block, int type_of_execution, int zone);
int dwell_block_exe(c_block control_block, int type_of_execution, int zone);
int position_block_exe(c_block control_block, int type_of_execution, int zone);
int velocity_block_exe(c_block control_block, int type_of_execution, int zone);
int jump_block_exe(c_block control_block, int type_of_execution, int zone);	
int end_block_exe(c_block control_block, int type_of_execution, int zone);
int start_block_exe(c_block control_block, int type_of_execution, int zone);
void push(unsigned int item);
unsigned int pop(void);


