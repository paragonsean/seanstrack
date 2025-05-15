// 				  control_program_blocks.h

//				Copyright 2001,2002 Visi-Trak Worldwide 


//enum control_blocks {VELOCITY, UPDATE, JUMP, DWELL, POSITION};
//enum actions {INTERPRET, EXECUTE, INTERPRET_AND_EXECUTE};

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

// Define conditional operators
#define GTR_THAN 1
#define GTR_THAN_EQU_TO 2
#define LESS_THAN 3
#define LESS_THAN_EQU_TO 4
#define EQUAL 5
#define NOT_EQUAL 6

//Define update block operators
#define PLUS 1
#define MINUS 2
#define SUB 3
#define DIV 4
#define AND 5
#define OR 6
#define XOR 7

#define COMPILE 0
#define EXECUTE 1




// Define bit fields for the conditional check / set output stuff.
// These fields will be the same for all block types.


typedef volatile struct
{
	unsigned if_indicator :1;
	unsigned while_indicator :1;
	unsigned input_on_indicator :1;
	unsigned input_off_indicator :1;
	unsigned advanced_testing_indicator :1;
	unsigned set_out_beginning :1;
	unsigned set_out_during :1;
	unsigned set_out_at_end :1;
	unsigned turn_on_turn_off :1;
	unsigned set_out_var_indicator :1;
	unsigned not_used1 :3;
	unsigned op1_var_indicator :1;
	unsigned op1_descrete_io_indicator :1;
	unsigned op1_adc_indicator :1;
	unsigned op1_dac_indicator :1;
	unsigned op1_pos_indicator :1;
	unsigned op1_vel_indicator :1;
	unsigned not_used2 :4;
	unsigned operator_ :5;
	unsigned op2_var_indicator :1;
	unsigned op2_const_indicator :1;
	unsigned not_used3 :2;
} cond_set_indicator_fields; //TestInputs_SetOutputs indicators

typedef volatile struct
{
	unsigned set_out_df :16;
	unsigned cond_op1_df :16;
} cond_set_data_fields;


//////////// Update block definitions /////////////

// Update block bit fields
typedef volatile struct
{
	unsigned blk_id :5;
	unsigned not_used1 :27;
} ub_word0_fields;

typedef volatile struct
{
	unsigned itu_var_indicator :1;
	unsigned itu_dac_out_indicator :1;
	unsigned itu_pos_indicator :1;
	unsigned itu_command :1;
	unsigned not_used1 :5;
	unsigned op1_var_indicator :1;
	unsigned op1_adc_indicator :1;
	unsigned op1_dac_indicator :1;
	unsigned op1_pos_indicator :1;
	unsigned op1_vel_indicator :1;
	unsigned op1_const_indicator :1;
	unsigned operator_ :5;
	unsigned op2_var_indicator :1;
	unsigned op2_const_indicator :1;
	unsigned not_used3 :8;
	unsigned vramp_var_indicator :1;
	unsigned vramp_const_indicator :1;
} ub_word1_fields; 


typedef volatile struct
{
	ub_word0_fields word0;					//Word0
	ub_word1_fields word1;					//Word1
	unsigned int itu_df;					//Word2
	unsigned int op1_df;   					//Word3
	unsigned int op2_df;  					//Word4
	unsigned int vramp_df;		  			//Word5
	unsigned int not_used1;					//Word6
	cond_set_indicator_fields word7;	 	//Word7
	cond_set_data_fields word8;	  			//Word8
	unsigned int cond_op2_df; 				//Word9
} ub; // Update block definition





//////////////// Dwell block definitions /////////////////

//Dwell block bit fields
typedef volatile struct
{
	unsigned blk_id :5;
	unsigned not_used1 :27;
} db_word0_fields;

typedef volatile struct
{
	unsigned dwell_var_indicator :1;
	unsigned dwell_const_indicator :1;
	unsigned not_used1 :30;
} db_word1_fields; //operand_specifier

typedef volatile struct
{
	db_word0_fields word0;				//Word0
	db_word1_fields word1;				//Word1
	unsigned int dwell_df;				//Word2
	unsigned int not_used1;				//Word3
	unsigned int not_used2;				//Word4
	unsigned int not_used3;				//Word5
	unsigned int not_used4;				//Word6
	cond_set_indicator_fields word7;	//Word7
	cond_set_data_fields word8;			//Word8
	unsigned int cond_op2_df; 			//Word9
} db; //Dwell block specification





///////////////// Position block definitions ///////////////////

//Position block fields
typedef volatile struct
{
	unsigned blk_id :5;
	unsigned not_used1 :27;
} pb_word0_fields;

typedef volatile struct
{
	unsigned pos_var_indicator :1;
	unsigned pos_const_indicator :1;
	unsigned axis1_indicator :1;
	unsigned axis2_indicator :1;
	unsigned axis3_indicator :1;
	unsigned axis4_indicator :1;
	unsigned vel_var_indicator :1;
	unsigned vel_const_indicator :1;
	unsigned not_used2 :24;
} pb_word1_fields; //pos_vel_operand_specifier

typedef volatile struct
{
	pb_word0_fields word0;						//Word0
	pb_word1_fields word1;						//Word1
	unsigned int pos_df;						//Word2
	unsigned int vel_df; 						//Word3
	unsigned int not_used1;						//Word4
	unsigned int not_used2;						//Word5
	unsigned int not_used3;						//Word6
	cond_set_indicator_fields word7;	 	//Word7
	cond_set_data_fields word8;				//Word8
	unsigned int cond_op2_df; 				//Word9
} pb; //Position block specification





////////////////// Velocity block definitions ///////////////////

//Velocity block fields
typedef volatile struct
{
	unsigned blk_id :5;
	unsigned not_used1 :27;
} vb_word0_fields;

typedef volatile struct
{
	unsigned vel_var_indicator :1;
	unsigned vel_const_indicator :1;
	unsigned axis1_indicator :1;
	unsigned axis2_indicator :1;
	unsigned axis3_indicator :1;
	unsigned axis4_indicator :1;
	unsigned not_used1 :4;
	unsigned accel_var_indicator :1;
	unsigned accel_const_indicator :1;
	unsigned not_used2 :8;
	unsigned endpos_var_indicator :1;
	unsigned endpos_const_indicator :1;
	unsigned not_used3 :10;
} vb_word1_fields; //vel_accel_operand_specifier


typedef volatile struct
{
	vb_word0_fields word0;				//Word0
	vb_word1_fields word1;				//Word1
	unsigned int vel_df;				//Word2	 		
	unsigned int accel_df;				//Word3
	int endpos_df;		  				//Word4
	unsigned int not_used1;				//Word5
	unsigned int not_used2;				//Word6
	cond_set_indicator_fields word7;	//Word7
	cond_set_data_fields word8;			//Word8
	unsigned int cond_op2_df; 			//Word9
} vb; //Velocity block specification




//////////////////// Jump block /////////////////////

//Jumb block fields
typedef volatile struct
{
	unsigned blk_id :5;
	unsigned not_used1 :27;
} jb_word0_fields;

typedef volatile struct
{
	unsigned rel_abs_specifier :1; // ==1 for rel, 0 for abs
	unsigned type_normal :1;
    unsigned type_sub	 :1;
	unsigned type_return :1;
	unsigned rel_dir	 :1; // 1 for forward, 0 for backwards
	unsigned not_used1 	 :25;
	unsigned jump_var_indicator :1;
	unsigned jump_const_indicator :1;
} jb_word1_fields; //jump_operand_specifier


typedef volatile struct
{
	jb_word0_fields word0;				//Word0
	jb_word1_fields word1;				//Word1
	unsigned int jump_df;				//Word2
	unsigned int not_used1;				//Word3
	unsigned int not_used2;				//Word4
	unsigned int not_used3;				//Word5
	unsigned int not_used4;				//Word6
	cond_set_indicator_fields word7;	//Word7
	cond_set_data_fields word8;			//Word8
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







