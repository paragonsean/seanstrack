// 				  control_program_blocks1.h

//				Copyright 2001, 2002, 2003 Visi-Trak Worldwide 


#define MAX_STEP 203
#define IMMED_EXECUTE_BLOCK 0
#define MAX_DAC_CH 4
#define MAX_ADC_CH 20
#define MAX_VAR 511
#define MAX_CMD 511
#define MAX_DESCRETE_IO_NUM 511
#define NUM_OF_AXES 2
#define UPDATE 1
#define VELOCITY 2
#define JUMP 3
#define DWELL 4
#define POSITION 5
#define END_ 6



#define COMPILE 0
#define EXECUTE 1




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
	op1_descrete_io_bit		= 11,
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
	op1_descrete_io_mask	= (1<<11),
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
	// Bits 24 to 31 not defined.
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
	// Bits 21 - 31 not defined.
	}; 


typedef volatile struct
{
	unsigned char blk_id;					//Word0
	unsigned char not_used;					// " "
	unsigned short int not_used1;			// " "
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
	unsigned char blk_id;				//Word0
	unsigned char not_used;				// " "
	unsigned short int not_used1;		// " "
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
	unsigned char blk_id;				//Word0
	unsigned char not_used;				// " "
	unsigned short int not_used1;		// " "
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
	unsigned char blk_id;				//Word0
	unsigned char not_used;				// " "
	unsigned short int not_used1;		// " "
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
	// Bits 7 - 31 undefined.
	};


typedef volatile struct
{
	unsigned char blk_id;				//Word0
	unsigned char not_used;				// " "
	unsigned short int not_used1;		// " "
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






//Prototypes
int block_conditional(int action, ub *blkptr, char *condstr);
int update_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v);
		
int dwell_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v);
						
int position_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v);
						
int velocity_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v);
						
int jump_block(int action, char *str_to_compile, 
				     unsigned int (*control_program)[10], 
					      unsigned int *prog_ctr, 
						      int *v);						
						
int htoi (char *str);







