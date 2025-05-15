/*************************** cont30.c ***************************/

/*
1-6-94  cont15 -- Removed the DAC error offset calculation and variable.
Future controller boards will have a pot for adjusting the DAC offset error.

1-24-94 cont16 -- Added the null_offset to all open loop command in both
the vel_control and val_step blocks.  Things like goose and low_imp_dac
both have the offset added to them as well.

Special goose values 0xffff (-1, skip entire block) and
0xfffe (-2, open loop) were changed to -1 and -2 respectively
since goose is declared as an int.

2-2-94 cont17 -- Added fatal error and warning framework to the control
supervisor.  As of now, no fatal errors or warnings have been defined.
The only warning forseeable at this time would be a quadrature error.

2-16-94 cont18 -- Made the Jog Shot, Follow Through and Retract valve
settings machine dependant parameters insted of part dependant profile
settings.

added set and clear commands for the program_downloaded flag.

3-2-94 cont19 -- Fixed problem with the while_on, while_off conditional
execution in the vel_control and valve step blocks.

3-3-94 cont20 -- Increased all velocity related variables to units * 10000
and made long ints.  This was done to increase the velocity calculation
resoultion, especially in closed loop.  Fixed problem with goosing.

3-4-94 cont21 --  If counts_per_unit is <= 10, probably a metric setup.
In this case, SCI will send the accel in units * 10000.  If counts_per_units
is > 10, accel will be units * 100.

3-10-94 cont22 -- Allowed for setting both a vacuum wait and an ending
position within the same step.  If vacuum is on, the vacuum wait will
take effect. If vacuum is off the block will end at the ending position.

Added lvdt_offset all open loop commands.

3-11-94 cont23 -- Modified the out_dac_w_check function to do a sanity
check and to add the null_offset and lvdt_offset to the dac_command.  This
will centralize these functions.

Added seperate commands for centering valve and nulling valve.

3-30-94 cont25 -- If the velocity counter is > 12000, loop time is 25mS
and the gain is increased by a factor of 10.  Less than 12000 and the loop
time is 2.5mS.

Temp_dac_command is divided by 10000000 before checking gain breaks.  This
allows gain settings to be reasonably large.

Gain break calculation is made after the temp_dac_command has been divided
to reduce problems from dealing with very large numbers.

Writing closed loop variables to ram is being done for testing purposes.
After a shot, the values can be accessed with the hi ram block in service
tools.

In closed loop acceleration, if the velocity counter is > 7500, the gain
is multiplied by 5.  This was emperically found to give good accel control.

5-25-94 cont26 -- Increased the closed loop diagnostics.  More info is
written to the test ram bank.

corrected the problem with the ram_test() function.  ram_test() will not
alter the operating programs.  High ram upper half and lower half are
swapped during testing and then restored.  Also the ram_clear()
function was altered to cast pointers to unsigned ints when clearing.  this
is the same type of change that was made to the ram_test() function.

5-31-94 -- cont26a  Made the velocity dividend an unsigned long to allow for
larger values (more resoultion).  Also the following variables were changed
to type unsigned: int_vel_cmd, act_vel, commanded_vel, prev_vel_cmd,
bk_vel1, bk_vel2 and bk_vel3, since they will never be negitive.

5-31-94 -- cont26b  underflow checking has been incorporated in both the
vel_control and val_step block functions.  This should elminate the potential
problem of bloacks being skipped in the event of a an underflow.  Also, the
timer2 contents are saved (frozen) before any checking is done.  It has been
found that operating directly on timer2 can incorrect logic as the contents
can change in the middle of an operation.

6-1-94 -- cont26c  Added a new parameter, vel_multiplier.  The purpose of
this is to allow setting the vel_dividend higher (better resoultion) and
then multiply the shot velocities, accels and break velocities by this
factor so that the end result, will be in the exact engineering units
specified.

The use of the HSO was elminated for all position based events (accel control)
in the vel_control() block.  It was found that there were still HSO position
based interrupts pending, possibly resulting in skewed acceleration contol,
when moving from block to block.  This was especially noticable in the
3humpol shot where the peaks would flatten out at various points.

initilaize_ls() was changed to save the old data page (ioport1) and then
restore.

6-3-94 -- cont27  Major revision.  Closed loop accel now computes the
velocity change.  Only the X1 accel distance is sent. This is now the case for
both open loop and closed loop accel.  This gives much greater flexibility
in mixing and matching closed loop / open loop blocks with accels.  A new
version of sci is now required to work with this version. prev_vel_cmd has
been elminated.

The dac_increment for open loop accels is now multiplied by 100 for greater
resolution.

Added code to set fatal error if a vacuum step is
terminated by anything but an OK FOR FS signal or e_stop.  This is to
prevent filling the vacuum valve if a vacuum block terminates prematurely.

Renumbered diagnositic codes to keep everything in order.

Made warning and fatal_error unsigned ints.  Added a new command to read
the warning value for future when warning is supported.  Reading the fatal
error code is already supported.

Changed the fatal_error code in control_supervisor() to set the error output
if a fatal error is detected.

Expanded checking for a low impact from external signal to encompass open
loop as well as closed loop accel.

Modularized all of the loop exit code.  A master is at the bottom of
this file.

Changed the looking for max_block_time logic from >= to ==.  When
max_block_time is computed, if tim1_ovfl_ctr and the max_block_time value
cross the 0xffff - 0 boundary, the logic is wrong, resulting in the step
ending prematurly.  This would have been a definite problem with the vacuum
systems.

Changed control2.h to elmin block 0x8000 in low ram.  Also ram_clear() and
ram_test() were changed to leave this ram block alone.  Appearently there
is a conflict of some sort and altering this block causes the board
to crash.  Further investigation is needed to determine why.

6-14-94 -- cont28  Added code to establish max dv based on the vel_loop_gain.
This will prevent an uncontrolled shot if dv very high.

6-21-94 -- cont29  Added fill test checking.  If a vacuum wait step and
fill_test is true, the shot will be halted at the ending position setting.
This is done to check for the best start of fast shot.  Metal is poured,
and the cylinder is halted at the position.

7-25-94 -- cont30  Added commands set and clear individual opto bits.
Allowed for setting the SHOT_IN_PROGRESS status bit.
*/


#include <80C196.H>
/*#include <FPAL96.H>*/
#include <STDIO.H>
#include <CONTROL2.H>
#pragma interrupt(cmd_interp=29, soft_timer_isr=5, non_mask_int=31)
#pragma interrupt(tim1_overflow=0, hso_isr=3)
#define UNDER_FLOW_VAL 57000   /* Amount of underflow allowed */
#define TRUE 1
#define FALSE 0
#define VER_NUM 30
#define VALVE_CENTER 0x2000 + null_offset + lvdt_offset
#define VALVE_NULL 0x2000 + null_offset


	/* Global register definitions */
	register int dac_command;
	register unsigned int x, rst, temp_d_io;
	register unsigned int temp_ana[5];
	register unsigned int status, block_ptr;
	/* hso timer1 times, make global so the command can be reversed
	   if necessary */
	unsigned int one_ms_tim1;
	unsigned int ls1, ls2, ls3, ls4, ls_turn_off;
	unsigned int tls1, tls2, tls3, tls4, tls_turn_off;
	unsigned char temp_int_mask, temp_imask1, temp_ioc2, temp_misc;
	unsigned char gn_ch1, gn_ch2, gn_ch3, gn_ch4;
	unsigned char int_flag, reg_tst_flg, all_ls_off_flag;
	unsigned char program_downloaded;
	/* make new_hso_tim2 global so that the position based hso
	   command can be reversed in case of a true zero speed
	   being sensed. main() needs to know the time that was previously
	   commanded. */
	register unsigned int new_hso_tim2;
	volatile register unsigned int tim1_ovfl_ctr;
	unsigned char control_active, dwell_time_flag, e_stop;
	unsigned char position_reached_flag;
	unsigned int stack[20], fatal_error, warning;
	unsigned char stack_ptr;
	int hold_ctr;


	/* Parameter definitions */
	unsigned int vel_loop_gain, bkgn1, bkgn2, bkgn3;
	unsigned long vel_dividend, bk_vel1, bk_vel2, bk_vel3;
	unsigned int dither_time, dither_ampl, p_check_z_speed;
	unsigned int counts_per_unit, vel_multiplier;
	int null_offset, lvdt_offset;
	unsigned int js_setting, retr_setting, ft_setting;



	main()
	{
	unsigned char newdp();
	void en_int(), dis_int(), reset(), change_channel();
	void cmd_interp(), vel_loop(), non_mask_int(), tim1_overflow();
	void chk_lim_sws();
	int out_dac(), out_dac_w_check(), initialize_ls();
	void a_to_d_loop();

	/* now initialize variables / set default settings */

	/*fpinit();       /* Initialize floating point */
	ioport1 = 0xA0; /* select I/O page directly */
	*WR_STATUS = 0x8000;
	dis_int();
	temp_imask1 = 0x20;
	temp_ioc2 = 0x06;
	temp_int_mask = 0x29;
	wsr = 0;
	ipend1 = 0, int_pending = 0;
	int_mask = temp_int_mask, imask1 = temp_imask1;
	timer2 = 0, all_ls_off_flag = 0;
	ioc0 = 0x08, ioc2 = temp_ioc2, sp_con = 0;
	ioc1 = 0x94;
	int_flag = FALSE, reg_tst_flg = FALSE;
	stack_ptr = 0; fatal_error = 0; warning = 0;

	if (rst != 0xAAAA) /* power up */
	   {
	   temp_d_io = 0; *WR_DIGITAL_IO = temp_d_io;

	   /* Initialize global parameters. */
	   gn_ch1 = 0, gn_ch2 = 0x01, gn_ch3 = 0x02, gn_ch4 = 0x03;
	   ls1 = 160, ls2 = 320, ls3 = 480, ls4 = 640;
	   ls_turn_off = 800;

	   /* Offsets are parameters but initialize now */
	   null_offset = 0, lvdt_offset = 0, x = 0;

	   temp_misc = 0x70; /* enable PC-AT interrupt */
	   newdp((unsigned char)IO_SEL);
	   *MISC = temp_misc;
	   status = 0, int_flag = FALSE;
	   SET_POWER_UP;
	   rst = 0xAAAA;
	   program_downloaded = FALSE;  /* Possibly use in future */
	   }

	if (status & 0x0200)  /* power up */
	    {
	    CLEAR_STATUS;
	    SET_POWER_UP;
	    }
	else
	    {
	    CLEAR_STATUS;
	    }
	newdp((unsigned char)IO_SEL);
	temp_misc |= 0x40;
	*MISC = temp_misc;  /* enable PC AT interrupt */
	temp_d_io = 0; *WR_DIGITAL_IO = temp_d_io;

	WRITE_STATUS;
	en_int();

	if (program_downloaded) control_active = 1;
	else control_active = 0;


begin:  ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
	imask1 = (temp_imask1 |= 0x20); /* allow command interrupts */

	if (!program_downloaded) control_active = 0;
	newdp((unsigned char)IO_SEL);
	/* *WR_DIGITAL_IO = temp_d_io &= 0xfffe;*/
	if (dac_command < 0) dac_command = 0;
	out_dac(dac_command = (VALVE_CENTER));  /* Center valve. */
	/* *WR_DIGITAL_IO = (temp_d_io = 0);*/
	*WR_A_TO_D = gn_ch1;
	ioport1 |= 0xC0; /* set at int trig. and a to d start conv. */


	if (int_flag)
	   at_int_trig();

	initialize_ls();  /* Initialize the limit switches. */

	if (!control_active)
	   {
	   /* Any error bits will be set here */
	   CLR_CONTROL_ACTIVE;
	   WRITE_STATUS;
	   }

	while (!control_active);  /* Wait until control active */

	SET_CONTROL_ACTIVE;
	CLEAR_ERRORS;
	WRITE_STATUS;
	control_supervisor();
	goto begin;
	}





void soft_timer_isr()  /* Higher priority than hso_isr below. */
{
register unsigned int temp_ios1, z;
unsigned char old_dp, newdp();
int out_dac();
unsigned char something_happened, ctr;

temp_ios1 = ios1;
temp_ios1 &= 0x0f;

old_dp = newdp((unsigned char)IO_SEL);
something_happened = FALSE;

if (temp_ios1)
   {
   /* Use software timer 0 to check for an emergency stop input. */
   if (temp_ios1 & 0x01)          /* Software timer 0 */
      {
      if ((*RD_DIGITAL_IO & 0x8000) && control_active && !e_stop)  /* E stop input present */
	 e_stop = TRUE;

      if (control_active && !e_stop)  /* Reactivate software timer 0 */
	 {
	 hso_command = 0x18;
	 hso_time = timer1 + 3750;  /*timer1 + 3750;  /* 5ms */
	 }
      something_happened = TRUE;
      }

   if (temp_ios1 & 0x02)          /* Software timer 1 */
      {
      position_reached_flag = 1;
      }

   if (temp_ios1 & 0x04)          /* Software timer 2 */
      {
      }

   if (temp_ios1 & 0x08)          /* Software timer 3 */
      {
      }

#if 0
   if (!something_happened)  /* isr triggered but event that caused it not detected. */
      {
      ctr = 0;
      do
	{
	*WR_DIGITAL_IO = (temp_d_io &= 0);
	*WR_DIGITAL_IO = (temp_d_io |= 0x000f);
	for (z = 0; z <= 30000; z++);
	*WR_DIGITAL_IO = (temp_d_io &= 0);
	*WR_DIGITAL_IO = (temp_d_io |= 0xf000);
	for (z = 0; z <= 30000; z++);
	ctr++;
	} while (ctr <= 15);
      }
#endif

   }

ioport1 = old_dp;
}




/***************************************************************************
Routine hso_isr() is an isr which uses the HSO pin interrupt vector.
The physical outputs on the HSO pins are not used for anything.
*/

void hso_isr()  /* Lower priority than software timer isr above */
{
register unsigned int temp_ios2, z;
unsigned char old_dp, newdp(), something_happened, ctr;

temp_ios2 = ios2;
temp_ios2 &= 0x3f;

old_dp = newdp((unsigned char)IO_SEL);
something_happened = FALSE;

if (temp_ios2)
   {
   if (temp_ios2 & 0x01)          /* HSO 0, Limit switch #1 */
      {
      if (!(temp_d_io & 0x1000))
	 {
	 LS1_ON;
	 hso_command = 0x50;
	 hso_time = (tls1 = ls1 - 1);
	 }
      else
	 {
	 LS1_OFF;
	 hso_command = 0x50;
	 hso_time = (tls1 = ls1);
	 }
      something_happened = TRUE;
      }


   if (temp_ios2 & 0x02)          /* HSO 1, Limit switch #2 */
      {
      if (!(temp_d_io & 0x2000))
	 {
	 LS2_ON;
	 hso_command = 0x51;
	 hso_time = (tls2 = ls2 - 1);
	 }
      else
	 {
	 LS2_OFF;
	 hso_command = 0x51;
	 hso_time = (tls2 = ls2);
	 }
      something_happened = TRUE;
      }


   if (temp_ios2 & 0x04)          /* HSO 2, Limit switch #3 */
      {
      if (!(temp_d_io & 0x4000))
	 {
	 LS3_ON;
	 hso_command = 0x52;
	 hso_time = (tls3 = ls3 - 1);
	 }
      else
	 {
	 LS3_OFF;
	 hso_command = 0x52;
	 hso_time = (tls3 = ls3);
	 }
      something_happened = TRUE;
      }


   if (temp_ios2 & 0x08)          /* HSO 3, Limit switch #4 */
      {
      if (!(temp_d_io & 0x8000))
	 {
	 LS4_ON;
	 hso_command = 0x53;
	 hso_time = (tls4 = ls4 - 1);
	 }
      else
	 {
	 LS4_OFF;
	 hso_command = 0x53;
	 hso_time = (tls4 = ls4);
	 }
      something_happened = TRUE;
      }


#if 0
   if (temp_ios2 & 0x10)          /* HSO 4, Limit switch master cutoff */
      {
      all_ls_off_flag = 1;
      LS1_OFF;
      LS2_OFF;
      LS3_OFF;
      LS4_OFF;
      /* ALL_LS_OFF;*/
      something_happened = TRUE;
      }
#endif

   if (temp_ios2 & 0x20)          /* HSO 5, General purpose timer */
      {
      something_happened = TRUE;
      dwell_time_flag = 1;
      }

#if 0
   if (!something_happened)  /* isr triggered but event that caused it not detedted. */
      {
      ctr = 0;
      do
	{
	*WR_DIGITAL_IO = (temp_d_io &= 0);
	*WR_DIGITAL_IO = (temp_d_io |= 0x0f00);
	for (z = 0; z <= 30000; z++);
	*WR_DIGITAL_IO = (temp_d_io &= 0);
	*WR_DIGITAL_IO = (temp_d_io |= 0x00f0);
	for (z = 0; z <= 30000; z++);
	ctr++;
	} while (ctr <= 15);
      }
#endif

   }

ioport1 = old_dp;
}





void a_to_d_loop()
/* IO page must be selected before calling this function */
{
unsigned int y; /* possibly make static for speed */
extern unsigned char gn_ch1, gn_ch2, gn_ch3, gn_ch4;

ioport1 &= 0xBF;           /* trig conversion */
ioport1 |= 0x40;

*WR_A_TO_D = gn_ch2;
/*for (y = 0; y <= 10; y++);  /* dummy assignment for delay */
y = 5; y = 3; y = 2; y = 1; /* */
temp_ana[1] = *RD_A_TO_D & 0x0fff;
if (temp_ana[1] & 0x0800) temp_ana[1] |= 0xf000;

ioport1 &= 0xBF;           /* trig conversion */
ioport1 |= 0x40;

*WR_A_TO_D = gn_ch3;
/*for (y = 0; y <= 10; y++);  /* dummy assignment for delay */
y = 5; y = 3; y = 2; y = 1;  /* */
temp_ana[2] = *RD_A_TO_D & 0x0fff;
if (temp_ana[2] & 0x0800) temp_ana[2] |= 0xf000;

ioport1 &= 0xBF;           /* trig conversion */
ioport1 |= 0x40;

*WR_A_TO_D = gn_ch4;
/*for (y = 0; y <= 10; y++);  /* dummy assignment for delay */
y = 5; y = 3; y = 2; y = 1;  /* */
temp_ana[3] = *RD_A_TO_D & 0x0fff;
if (temp_ana[3] & 0x0800) temp_ana[3] |= 0xf000;

ioport1 &= 0xBF;           /* trig conversion */
ioport1 |= 0x40;

*WR_A_TO_D = gn_ch1;
/*for (y = 0; y <= 10; y++);  /* dummy assignment for delay */
y = 5; y = 3; y = 2; y = 1;  /* */
temp_ana[4] = *RD_A_TO_D & 0x0fff;
if (temp_ana[4] & 0x0800) temp_ana[4] |= 0xf000;
}




       void cmd_interp()
       {
       unsigned int command, temp_command, b, temp_data;
       unsigned char newdp(), old_page, no_int;
       unsigned int get_count();
       void ram_test(), test_dio();
       void change_channel(), en_int();
       int out_dac();

       old_page = newdp((unsigned char)IO_SEL); /* select IO page */
       SET_BUSY;
       CLR_POWER_UP;
       WRITE_STATUS;

       /* now get command */
       command = *RD_COMMAND;
       temp_command = command; /* work on temp_command, leave original command unmodified for future testing */


       newdp((unsigned char)IO_SEL);
       no_int = FALSE;

       if (temp_command == 0x8000); /* command to set BUSY */
       else if (reg_tst_flg)
	  {
	  *WR_STATUS = temp_command;
	  *DATA_OUT = temp_command;
	  }
       else if (!(temp_command & 0x8000))
	  {
	  /* msb cleared, must be a data access */
	  temp_command <<= 1;
	  if (!(temp_command & 0x8000))
	     {
	     /* this is a ram lo access and therefore 0x8000 must be added
		to the command value to obtain the correct address to fetch
		data from */
	     temp_command += 0x8000;
	     newdp((unsigned char)RAM_LO_SEL);
	     b = *((unsigned int * ) temp_command);
	     newdp((unsigned char)IO_SEL);
	     *DATA_OUT = b;
	     no_int = TRUE;
	     }
	  else
	     {
	     /* this is a ram hi access and the command value is already
		equal to the address to fetch data from */
	     newdp((unsigned char)RAM_HI_SEL);
	     b = *((unsigned int * ) temp_command);
	     newdp((unsigned char)IO_SEL);
	     *DATA_OUT = b;
	     no_int = TRUE;
	     }
	  }
       else if (temp_command & 0x4000)
	  {
	  /* must be a ram high write for downloading control program. */
	  temp_command <<= 1; /* command is the address after shifting */

	  /* If a program block is being written, disable control. */
	  /*if (temp_command >= 0xf000 || temp_command <= 0xfefe)
	     control_active = FALSE; */

	  newdp((unsigned char)IO_SEL);
	  temp_data = *AT_DATA;
	  newdp((unsigned char)RAM_HI_SEL);
	  *((unsigned int * ) temp_command) = temp_data;
	  no_int = TRUE;
	  }
       else
	  {
	  /* bit 14 cleared, must be a do something command */
	  newdp((unsigned char)IO_SEL);
	  switch (temp_command)
		 {
		 /* case 0x8000 defined above */

		 case 0x8001:
		      /* enable AT interrupt */
		      temp_misc |= 0x40;
		      *MISC = temp_misc;
		      CLR_AT_INT_DIS;
		      no_int = TRUE;  /* flag so we don't int. AT */
		      break;
		 case 0x8002:
		      /* initiate self test, code to come later */
		      self_test();
		      no_int = TRUE;
		      break;
		 case 0x8003:
		      /* disable AT interrupt */
		      temp_misc &= 0xBF;
		      *MISC = temp_misc;
		      SET_AT_INT_DIS;
		      break; /* no need to flag for no interrupt */
		 case 0x8004:
		      /* clear all errors */
		      CLEAR_ERRORS;
		      no_int = TRUE;
		      break;
		 case 0x8005:
		      /* write to the DAC */
		      out_dac(dac_command = *AT_DATA);
		      no_int = TRUE;
		      break;
		 case 0x8006:
		      /* test digital I/O */
		      test_dio();
		      no_int = TRUE;
		      break;
		 case 0x8007:
		      /* Set the register test flag. When this flag is
			 set all commands other than command zero and
			 set command mode (NMI) will be echoed to both
			 the status register and data out register for
			 test purposes. */
		      reg_tst_flg = TRUE;
		      no_int = TRUE;
		      break;
		 case 0x8008:
		      /* ram test command */
		      if (!control_active) ram_test();
		      no_int = TRUE;
		      break;
		 case 0x8009:
		      /* read current position */
		      *DATA_OUT = timer2;
		      no_int = TRUE;
		      break;
		 case 0x800a:
		      /* read current velocity */
		      *DATA_OUT = *VELOCITY;
		      no_int = TRUE;
		      break;
		 case 0x800b:
		      /* read current analog ch. 1 */
		      a_to_d_loop();
		      *DATA_OUT = temp_ana[1];
		      no_int = TRUE;
		      break;
		 case 0x800c:
		      /* read current analog ch. 2 */
		      a_to_d_loop();
		      *DATA_OUT = temp_ana[2];
		      no_int = TRUE;
		      break;
		 case 0x800d:
		      /* read current analog ch. 3 */
		      a_to_d_loop();
		      *DATA_OUT = temp_ana[3];
		      no_int = TRUE;
		      break;
		 case 0x800e:
		      /* read current analog ch. 4 */
		      a_to_d_loop();
		      *DATA_OUT = temp_ana[4];
		      no_int = TRUE;
		      break;
		 case 0x800f:
		      /* Data register test. This command will take the
			 value in the data input register and send it to
			 both the status register and data output register
			 for test purposes */
		      *WR_STATUS = *AT_DATA;
		      *DATA_OUT = *AT_DATA;
		      no_int = TRUE;
		      break;
		 case 0x8010:
		      /* Restore sataus register. After executing the above
			 command it will be necessary to restore the status
			 register. This is a dummy command which accomplishes
			 this in effect. */
		      no_int = TRUE;
		      break;
		 case 0x8011:
		      /* Send interrupt to PC.  Can be used for test purposes
			 to check out the interrupt signal */
		      no_int = FALSE;
		      break;
		 case 0x8012: /* get version number */
		      *DATA_OUT = VER_NUM;
		      no_int = TRUE;
		      break;
		 case 0x8013: /* Set control mode */
		      /*load_sample_prog();*/
		      control_active = TRUE;
		      no_int = TRUE;
		      break;
		 case 0x8014:  /* Clear control mode */
		      control_active = FALSE;
		      no_int = TRUE;
		      break;
		 case 0x8015:  /* Update parameters */
		      set_parameters();
		      no_int = TRUE;
		      break;

		 /* Setting limit switch values will disable control. */
		 case 0x8016:  /* Set ls1 */
		      ls1 = *AT_DATA;
		  #if 0
		      control_active = FALSE;
		      ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
		      initialize_ls();
		  #endif
		      no_int = TRUE;
		      break;
		 case 0x8017:  /* Set ls2 */
		      ls2 = *AT_DATA;
		  #if 0
		      control_active = FALSE;
		      ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
		      initialize_ls();
		  #endif
		      no_int = TRUE;
		      break;
		 case 0x8018:  /* Set ls3 */
		      ls3 = *AT_DATA;
		  #if 0
		      control_active = FALSE;
		      ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
		      initialize_ls();
		  #endif
		      no_int = TRUE;
		      break;
		 case 0x8019:  /* Set ls4 */
		      ls4 = *AT_DATA;
		  #if 0
		      control_active = FALSE;
		      ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
		      initialize_ls();
		  #endif
		      no_int = TRUE;
		      break;
		 case 0x801a:  /* Set ls turn off */
		      /* This feature is not totally usuable
			 at this time. */
		      ls_turn_off = *AT_DATA;
		  #if 0
		      control_active = FALSE;
		      ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
		      initialize_ls();
		  #endif
		      no_int = TRUE;
		      break;
		 case 0x801b:  /* Read back ls1 */
		      *DATA_OUT = ls1;
		      no_int = TRUE;
		      break;
		 case 0x801c:  /* Read back ls2 */
		      *DATA_OUT = ls2;
		      no_int = TRUE;
		      break;
		 case 0x801d:  /* Read back ls3 */
		      *DATA_OUT = ls3;
		      no_int = TRUE;
		      break;
		 case 0x801e:  /* Read back ls4 */
		      *DATA_OUT = ls4;
		      no_int = TRUE;
		      break;
		 case 0x801f:  /* Read back ls turn off */
		      *DATA_OUT = ls_turn_off;
		      no_int = TRUE;
		      break;
		 case 0x8020:  /* Read current control block */
		      *DATA_OUT = block_ptr;  /* Raw unconverted block pointer */
		      no_int = TRUE;
		      break;
		 case 0x8021:  /* Read current DAC word */
		      *DATA_OUT = dac_command;  /* Raw Dac command */
		      no_int = TRUE;
		      break;
		 case 0x8022:  /* Read current input word */
		      temp_data = *RD_DIGITAL_IO;
		      *DATA_OUT = temp_data;  /* Input word */
		      no_int = TRUE;
		      break;
		 case 0x8023:  /* Read current output word */
		      *DATA_OUT = temp_d_io;  /* Output word */
		      no_int = TRUE;
		      break;
		 case 0x8024:  /* Read contoller error word */
		      *DATA_OUT = fatal_error;  /* Error status word */
		      no_int = TRUE;
		      break;
		 case 0x8025:  /* Execute one valve test cycle */
		      if (!control_active)
			 {
			 /* First establish the imasks */
			 int_mask = (temp_int_mask &= 0x7f);
			 int_mask = (temp_int_mask |= 0x29);

			 en_int();
			 valve_test();
			 dis_int();
			 /*imask1 = temp_imask1;*/
			 }
		      no_int = TRUE;
		      break;
		 case 0x8026:  /* Write directly to opto */
		      temp_data = *AT_DATA & 0x00f0; /* bits 4 - 7 only */
		      temp_d_io &= 0xff0f;
		      temp_d_io |= temp_data;
		      *WR_DIGITAL_IO = temp_d_io;
		      no_int = TRUE;
		      break;
		 case 0x8027:  /* Set individual Opto bits */
		      temp_data = *AT_DATA & 0xf03f; /* Only allowable bits */
		      temp_d_io |= temp_data;
		      *WR_DIGITAL_IO = temp_d_io;
		      no_int = TRUE;
		      break;
		 case 0x8028:  /* Clear individual Opto bits */
		      temp_data = *AT_DATA & 0xf03f; /* Only allowable bits */
		      temp_d_io &= ~temp_data;
		      *WR_DIGITAL_IO = temp_d_io;
		      no_int = TRUE;
		      break;
		 case 0x8029: /* Zero the dac */
		      out_dac(dac_command = 0x2000);
		      no_int = TRUE;
		      break;
		 case 0x802a:  /* Null the valve */
		      out_dac(dac_command = (VALVE_NULL));
		      no_int = TRUE;
		      break;
		 case 0x802b:  /* Position transducer test */
		      /* First establish the imasks */
		      int_mask = (temp_int_mask &= 0x7f);
		      int_mask = (temp_int_mask |= 0x29);

		      en_int();
		      get_pos_frame();
		      dis_int();
		      no_int = TRUE;
		      break;
		 case 0x802c:  /* Set the program downloaded flag */
		      program_downloaded = TRUE;
		      no_int = TRUE;
		      break;
		 case 0x802d:  /* Clear the program downloaded flag */
		      program_downloaded = FALSE;
		      no_int = TRUE;
		      break;
		 case 0x802e:  /* Read the program_downloaded status */
		      *DATA_OUT = (unsigned int)program_downloaded;
		      no_int = TRUE;
		      break;
		 case 0x802f:  /* Center the valve */
		      out_dac(dac_command = (VALVE_CENTER));
		      no_int = TRUE;
		      break;
		 case 0x8030:  /* Read the warning code */
		      *DATA_OUT = warning;
		      no_int = TRUE;
		      break;
		 default:
		      /* this is an invalid command */
		      SET_INVALID_CMD;
		      no_int = TRUE;
		      break;
		 }
	  }

       if (!reg_tst_flg)
	  {
	  /*if ((status & 0x2001) != 0 || command == 0x8000);*/
	  if (command == 0x8000);  /* Set busy only */
	  /* if data collection mode, scan mode or set busy command,
	     keep busy set */

	  else if (no_int)
	     CLR_BUSY;  /* no interrupt signal */
	  else
	     {
	     CLR_BUSY;
	     int_flag = TRUE;
	     /* at_int_trig(); /* interrupt AT if not data collection mode */
	     }

	  /* don't update status if register test or data reg. test */
	  if (command != 0x800f)
	     {
	     newdp((unsigned char)IO_SEL);
	     WRITE_STATUS;
	     }
	  }
       ioport1 = old_page;
       }




       void non_mask_int()
       {
       reset();
       }



       void tim1_overflow()
       {
       tim1_ovfl_ctr++;
       }




       at_int_trig()
       {
       int_flag = FALSE;
       ioport1 &= 0x7F; /* send low */
       ioport1 |= 0x80; /* send high */
       }



       ram_clear()
       {
       unsigned int x;
       unsigned char old_dp, newdp();

       /* first initialize ram lo bank */
       old_dp = newdp((unsigned char)RAM_LO_SEL);
       for (x = 0x9000; x <= 0xFFFE, x >= 0x9000; x+=2)
	     *((unsigned int * ) x) = 0;

       /* now do ram hi bank */
       newdp((unsigned char)RAM_HI_SEL);
       for (x = 0x8000; x <= 0xEFFE; x+=2) /* Leave F bank alone */
	    *((unsigned int * ) x) = 0;
       ioport1 = old_dp;   /* restore old inst./data page */
       }


       void ram_test()
       {
       unsigned char old_dp, newdp(), errflg;
       unsigned int actr, lctr, hctr, y;


       errflg = FALSE;
       old_dp = newdp((unsigned char)RAM_LO_SEL);
       for (actr = 0x9000; actr <= 0xFFFE, actr >= 0x9000; actr+=2)
	   *((unsigned int * ) actr) = 0;
       for (actr = 0x9000; actr <= 0xFFFE, actr >= 0x9000; actr+=2)
	   {
	   y = *((unsigned int * ) actr);
	   if (y != 0)
	      {
	      errflg = TRUE;
	      break;
	      }
	   }

       if (!errflg)
	  {
	  for (actr = 0x9000; actr <= 0xFFFE, actr >= 0x9000; actr+=2)
	      *((unsigned int * ) actr) = 0xFFFF;
	  for (actr = 0x9000; actr <= 0xFFFE, actr >= 0x9000; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0xFFFF)
		 {
		 errflg = TRUE;
		 break;
		 }
	      }
	   }

       if (!errflg)
	  {
	  for (actr = 0x9000; actr <= 0xFFFE, actr >= 0x9000; actr+=2)
	      *((unsigned int * ) actr) = 0x5555;
	  for (actr = 0x9000; actr <= 0xFFFE, actr >= 0x9000; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0x5555)
		 {
		 errflg = TRUE;
		 break;
		 }
	      }
	   }

       if (!errflg)
	  {
	  for (actr = 0x9000; actr <= 0xFFFE, actr >= 0x9000; actr+=2)
	      *((unsigned int * ) actr) = 0xAAAA;
	  for (actr = 0x9000; actr <= 0xFFFE, actr >= 0x9000; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0xAAAA)
		 {
		 errflg = TRUE;
		 break;
		 }
	      }
	   }

       if (!errflg)
	  {
	  newdp((unsigned char)RAM_HI_SEL);
	  /* First test lower half. */

	  for (actr = 0x8000; actr <= 0xBFFE; actr+=2)
	      *((unsigned int * ) actr) = 0;
	  for (actr = 0x8000; actr <= 0xBFFE; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0)
		 {
		 errflg = 2;
		 break;
		 }
	      }
	  }

       if (!errflg)
	  {
	  for (actr = 0x8000; actr <= 0xBFFE; actr+=2)
	      *((unsigned int * ) actr) = 0xFFFF;
	  for (actr = 0x8000; actr <= 0xBFFE; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0xFFFF)
		 {
		 errflg = 2;
		 break;
		 }
	      }
	   }

       if (!errflg)
	  {
	  for (actr = 0x8000; actr <= 0xBFFE; actr+=2)
	      *((unsigned int * ) actr) = 0x5555;
	  for (actr = 0x8000; actr <= 0xBFFE; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0x5555)
		 {
		 errflg = 2;
		 break;
		 }
	      }
	   }

       if (!errflg)
	  {
	  for (actr = 0x8000; actr <= 0xBFFE; actr+=2)
	      *((unsigned int * ) actr) = 0xAAAA;
	  for (actr = 0x8000; actr <= 0xBFFE; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0xAAAA)
		 {
		 errflg = 2;
		 break;
		 }
	      }
	   }

       /* Now shuffle high half to the low half to preserve the programs. */
       if (!errflg)
	  {
	  for (lctr = 0x8000, hctr = 0xC000; lctr <= 0xBFFE; lctr+=2, hctr+=2)
	      *((unsigned int * ) lctr) = *((unsigned int * ) hctr);
	  }

       /* Now check high half */
       if (!errflg)
	  {
	  for (actr = 0xC000; actr <= 0xFFFE, actr >= 0xC000; actr+=2)
	      *((unsigned int * ) actr) = 0;
	  for (actr = 0xC000; actr <= 0xFFFE, actr >= 0xC000; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0)
		 {
		 errflg = 2;
		 break;
		 }
	      }
	  }

       if (!errflg)
	  {
	  for (actr = 0xC000; actr <= 0xFFFE, actr >= 0xC000; actr+=2)
	      *((unsigned int * ) actr) = 0xFFFF;
	  for (actr = 0xC000; actr <= 0xFFFE, actr >= 0xC000; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0xFFFF)
		 {
		 errflg = 2;
		 break;
		 }
	      }
	   }

       if (!errflg)
	  {
	  for (actr = 0xC000; actr <= 0xFFFE, actr >= 0xC000; actr+=2)
	      *((unsigned int * ) actr) = 0x5555;
	  for (actr = 0xC000; actr <= 0xFFFE, actr >= 0xC000; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0x5555)
		 {
		 errflg = 2;
		 break;
		 }
	      }
	   }

       if (!errflg)
	  {
	  for (actr = 0xC000; actr <= 0xFFFE, actr >= 0xC000; actr+=2)
	      *((unsigned int * ) actr) = 0xAAAA;
	  for (actr = 0xC000; actr <= 0xFFFE, actr >= 0xC000; actr+=2)
	      {
	      y = *((unsigned int * ) actr);
	      if (y != 0xAAAA)
		 {
		 errflg = 2;
		 break;
		 }
	      }
	   }

       /* Now shuffle low half back to the high half to restore the programs. */
       if (!errflg)
	  {
	  for (lctr = 0x8000, hctr = 0xC000; lctr <= 0xBFFE; lctr+=2, hctr+=2)
	      *((unsigned int * ) hctr) = *((unsigned int * ) lctr);
	  }


       newdp ((unsigned char) IO_SEL);
       if (errflg == 1) /* Low ram error */
	  {
	  SET_MEMORY_ERR;
	  *DATA_OUT = actr;
	  }
       else if (errflg == 2)
	  {
	  SET_MEMORY_ERR;
	  *DATA_OUT = actr + 1;  /* make odd to indicate ram hi error */
	  }
       else
	  {
	  *DATA_OUT = 0;
	  }

       ioport1 = old_dp ;
       }


       void test_dio()
       {
       unsigned char old_dp, newdp(), err;
       unsigned int c;

       err = FALSE;
       old_dp = newdp((unsigned char) IO_SEL);

       *WR_DIGITAL_IO = 0;
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0)
	  err = TRUE;

       *WR_DIGITAL_IO = 0x5555;
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0x5555)
	  err = TRUE;

       *WR_DIGITAL_IO = 0xAAAA;
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0xAAAA)
	  err = TRUE;

       *WR_DIGITAL_IO = 0x6666;
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0x6666)
	  err = TRUE;

       *WR_DIGITAL_IO = 0x9999;
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0x9999)
	  err = TRUE;

       *WR_DIGITAL_IO = 0xFFFF;
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0xFFFF)
	  err = TRUE;

       *WR_DIGITAL_IO = 0;
       *DATA_OUT = err;
       if (err)
	  SET_INTERNAL_ERR;
       ioport1 = old_dp;
       }


       /* The out_dac() function writes a value to the DAC */

       out_dac(unsigned int value)
       {
       unsigned char old_dp, newdp();
       unsigned int z;

       old_dp = newdp((unsigned char)WR_DAC);
       *RD_WR_DAC = value; /* write value to low ram */
       z = *RD_WR_DAC; /* dummy read which actually writes value */
       z = *LDAC; /* another dummy read which loads the DAC register */
       ioport1 = old_dp; /* restore old data page */
       }



       /* The out_dac_w_check() function writes a value to the DAC
	  if the control_active flag is set and the e_stop flag is false.
	  The null_offset and lvdt_offset are added to the value to send.
	  Also a sanity check is done. */

       out_dac_w_check(int value)
       {
       unsigned char old_dp, newdp();
       unsigned int z;

       if (control_active && !e_stop)
	  {
	  value = value + null_offset + lvdt_offset;

	  /* Now do a sanity check */
	  if (value < 0) value = 0;
	  if (value > 0x3fff) value = 0x3fff;

	  old_dp = ioport1;

	  ioport1 &= 0xC7;           /* select wr_dac, do directly insted    */
	  ioport1 |= 0x28;           /* of calling the newdp() function      */
				     /* to save time.                        */

	  *RD_WR_DAC = (unsigned int)value; /* write value to low ram */
	  z = *RD_WR_DAC; /* dummy read which actually writes value */
	  z = *LDAC; /* another dummy read which loads the DAC register */
	  ioport1 = old_dp; /* restore old data page */
	  }
       }


       initialize_ls()
       {
       unsigned char old_dp;
       /* Initialize the limit switches */
       /* timer2 = 0;               /* clear timer2 for lim sws */

       old_dp = ioport1;

       ioport1 &= 0xC7;           /* select IO, do directly insted      */
       ioport1 |= 0x20;           /* of calling the newdp() function    */
				  /* to save time.                      */

       dis_int();
       ALL_LS_OFF;

       hso_command = 0x50, hso_time = (tls1 = ls1);
       hso_command = 0x51, hso_time = (tls2 = ls2);
       hso_command = 0x52, hso_time = (tls3 = ls3);
       hso_command = 0x53, hso_time = (tls4 = ls4);
       hso_command = 0x54, hso_time = (tls_turn_off = ls_turn_off);
       en_int();

       ioport1 = old_dp;
       }


       self_test()
       {
       }




/*************************** CONTROL SECTION ********************************/


control_supervisor()
{
unsigned char old_dp;

/* block types for *block_ptr:

   0  ==  Valve step
   1  ==  Jump
   2  ==  Set position
   3  ==  Velocity control

*/

old_dp = ioport1;
block_ptr = 0xf000;

ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function      */
			   /* to save time.                        */
e_stop = FALSE;
fatal_error = FALSE;
warning = FALSE;

dis_int();
/* Now set up software timer 0 for E stop checking. */
hso_command = 0x18;
hso_time = timer1 + 3750;  /* 5ms */
en_int();


set_parameters();  /* Set closed loop velocity parameters. */

while ((*(unsigned int * )block_ptr != 999) && control_active)
    {
    if (e_stop || stack_ptr > 20)  /* Reset control program if e_stop is detected. */
       {
       dis_int();
       ioport1 &= 0xC7;           /* select IO, do directly insted      */
       ioport1 |= 0x20;           /* of calling the newdp() function    */
				  /* to save time.                      */

       ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
       e_stop = FALSE;
       block_ptr = 0xf000;
       stack_ptr = 0;
       out_dac(dac_command = (VALVE_CENTER));
       *WR_DIGITAL_IO = (temp_d_io &= 0xf000);  /* Don't turn off limit sws */
       initialize_ls(); /*   */
       /* Now set up software timer 0 for E stop checking. */
       hso_command = 0x18;
       hso_time = timer1 + 3750;  /* 5ms */
       ioport1 &= 0xC7;           /* select ram high, do directly insted  */
       ioport1 |= 0x38;           /* of calling the newdp() function      */
				  /* to save time.                        */

       en_int();
       continue;
       }

    if (fatal_error)
       {
       ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
       ioport1 &= 0xC7;           /* select IO, do directly insted      */
       ioport1 |= 0x20;           /* of calling the newdp() function    */
				  /* to save time.                      */
       *WR_DIGITAL_IO = (temp_d_io &= 0xf000);  /* Don't turn off limit sws */
       *WR_DIGITAL_IO = (temp_d_io |= 0x0800);  /* Turn on fault output. */
       out_dac(dac_command = (VALVE_CENTER));
       block_ptr = 0xf000;
       stack_ptr = 0;
       break;
       /* Status bits will be updated in main loop after begin: */
       }

    if (warning)
       {
       /* Code here to handle warning detection. */
       }

    if (!program_downloaded) break;

    if (*(unsigned int * )block_ptr == 3) {vel_control(); continue;}
    if (*(unsigned int * )block_ptr == 0) {val_step(); continue;}
    if (*(unsigned int * )block_ptr == 1) {jump_block(); continue;}
    if (*(unsigned int * )block_ptr == 2) {setpos_block(); continue;}
    }

control_active = 0;
ioport1 = old_dp;
}




val_step()
{
/* Program execution from high ram, block F000 to FEFE. */
/* Framework for the valve step block

   *block_ptr + 2  == Output to DAC from 0 to 0x3fff. -1 will leave unchanged.
   *block_ptr + 4  == Accel in DAC counts per 5 mS. 0 == no accel.
   *block_ptr + 6  == Position to make change, 0 == don't wait for position.
   *block_ptr + 8  == Long dwell time, 87.3813 mS increments.
   *block_ptr + 10 == Short dwell time, 1.33 uS increments.

   *block_ptr + 12 to + 16 reserved for future use.

   *block_ptr + 18  == Conditional analog check value.
   *block_ptr + 20  == Variable indicator.
   *block_ptr + 22  == Input check indication.  999 - unconditional,
	      xxxxxx yyy ccc bb aa
	      |      |   |   |  |
	      |      |   |   |  00 - If input off, 01 - If input on
	      |      |   |   |  10 - While input off, 11 - While input on
	      |      |   |   |
	      |      |   |   00 - Analog channel 1,  01 - Analog channel 2
	      |      |   |   10 - Analog channel 3,  11 - Analog channel 4
	      |      |   |
	      |      |   000 - No analog checking, 001 - If <=, 010 - If >=
	      |      |   011 - If =, 100 - If !=
	      |      |   101 - 111 Undefined
	      |      |
	      |      000 - 110 Undefined but usable for future
	      |      111 - Unconditional block.  Set when 999d
	      |
	      Unused, set to all zeros.

   *block_ptr + 24  == Input to check. Ignored if previous is 999.
   *block_ptr + 26  == Output set or clear indication. 999 - no outputs to set.
		      0 - Beginning OFF, 1 - Beginning ON,
		      3 - During, 4 - At end OFF, 5 - At end ON.
   *block_ptr + 28  == Output to set / clear. Ignored if previous is 999.


   SPECIAL DAC COMMAND SETTINGS

   1,   -1 == Leave dac output unchanged.
   2,   -2 == Skip entire block.
   3,   -3 == Null valve -- zero output + null_offset.
   4,   -4 == Center valve -- zero output + null_offset + lvdt_offset.
   5,   -5 == Jog shot setting (parameter based).
   6,   -6 == Retract valve setting (parameter based).
   7,   -7 == Follow through valve setting (parameter based).
   8,   -8 == Set shot in progress status bit.
   9,   -9 == Clear shot in progress status bit.

   Order of precedence for executing block:

   1,  If conditional execution is required, check for condition. If
       condition is false, exit block.

   2,  If Beginning or during output is required, do it now.

   3,  If command is immediate, check if accel is necessary. Output command
       to the DAC.

   4,  Else if command is position based, wait until position is reached,
       then check if accel is necessary and output command to DAC.

   5,  Else dwell the necessary amount of time, and output
       command to the DAC with accel if necessary.

   6,  If during output is present, remove it now.

   7,  If at end output is required, do it now.
*/

int final_dac_command;
unsigned int accel, position, l_dwell_time, s_dwell_time;
unsigned int input_indicator, input_number;
unsigned int output_indicator, output_number, temp_time, variable_indicator;
unsigned char old_dp, decel_flag, while_on_flag, while_off_flag;
unsigned int temp, analog_val, ana_check_val;
unsigned int max_block_time;
int out_dac_w_check();

old_dp = ioport1;
ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function */
			   /* to save time.                   */

while_on_flag = 0;  while_off_flag = 0;
max_block_time = tim1_ovfl_ctr + 115;   /* 10 seconds max block time */

variable_indicator = *((unsigned int * )(block_ptr + 20));

if (variable_indicator & 0x0002)
   final_dac_command = *((unsigned int * )(*((unsigned int * )(block_ptr + 2))));
else  final_dac_command = *((unsigned int * )(block_ptr + 2));

/* Now check if final_dac_command == -2.  This is a quick way to skip the
   entire block if necessary. */
if (final_dac_command == -2) goto ex;

if (variable_indicator & 0x0004)
   accel = *((unsigned int * )(*((unsigned int * )(block_ptr + 4))));
else  accel = *((unsigned int * )(block_ptr + 4));

if (variable_indicator & 0x0008)
   position = *((unsigned int * )(*((unsigned int * )(block_ptr + 6))));
else  position = *((unsigned int * )(block_ptr + 6));

if (variable_indicator & 0x0010)
   l_dwell_time = *((unsigned int * )(*((unsigned int * )(block_ptr + 8))));
else  l_dwell_time = *((unsigned int * )(block_ptr + 8));

if (variable_indicator & 0x0020)
   s_dwell_time = *((unsigned int * )(*((unsigned int * )(block_ptr + 10))));
else  s_dwell_time = *((unsigned int * )(block_ptr + 10));

if (variable_indicator & 0x0200)
   ana_check_val = *((unsigned int * )(*((unsigned int * )(block_ptr + 18))));
else  ana_check_val = *((unsigned int * )(block_ptr + 18));

input_indicator = *((unsigned int * )(block_ptr + 22));

if (variable_indicator & 0x1000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 24))));
else  input_number = *((unsigned int * )(block_ptr + 24));

output_indicator = *((unsigned int * )(block_ptr + 26));

if (variable_indicator & 0x4000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));



ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */


/************** FIRST CHECK FOR CONDITIONAL EXECUTION **************/

if (input_indicator == 999);  /* Unconditional execution */

else if (input_indicator < 16)  /* Conditional If / While */
   {
   if (input_indicator == 1)  /* IF input on conditional execution. */
      {
      if (!(*RD_DIGITAL_IO & input_number)) goto ex;
      }

   else if (input_indicator == 0) /* IF input off conditional execution. */
      {
      if (*RD_DIGITAL_IO & input_number) goto ex;
      }


   else if (input_indicator == 3)  /* WHILE input on conditional.  */
      {
      if (!(*RD_DIGITAL_IO & input_number)) goto ex;
      while_on_flag = 1;
      }

   else if (input_indicator == 2)  /* WHILE input off conditional. */
      {
      if (*RD_DIGITAL_IO & input_number) goto ex;
      while_off_flag = 1;
      }

   }

else    /* Check analog conditional */
   {
   a_to_d_loop();

   temp = input_indicator; temp >>= 2; temp &= 0x0003; /* Extract channel */
   switch (temp)
       {
       case 0:  /* analog channel 1 */
	 analog_val = temp_ana[1];
	 break;

       case 1:  /* analog channel 2 */
	 analog_val = temp_ana[2];
	 break;

       case 2:  /* analog channel 3 */
	 analog_val = temp_ana[3];
	 break;

       case 3:  /* analog channel 4 */
	  analog_val = temp_ana[4];
	  break;
       }

   /* Now extract operator */
   temp = input_indicator; temp >>= 4; temp &= 0x0007;
   switch (temp)
      {
      case 0:  /* Indicates no analog checking. */
	break;

      case 1:  /* <= */
	if (analog_val <= ana_check_val)  break;
	else  goto ex;
	break;

      case 2:  /* >= */
	if (analog_val >= ana_check_val)  break;
	else  goto ex;
	break;

      case 3:  /* = */
	if (analog_val == ana_check_val)  break;
	else  goto ex;
	break;

      case 4:  /* != */
	if (analog_val != ana_check_val)  break;
	else  goto ex;
	break;

      case 5:
      case 6:
      case 7:
	break;   /* Undefined operators */
      }
   }

/********* NOW CHECK IF BEGINNING OR DURING OUTPUTS ARE REQUIRED ***********/

if (output_indicator == 999);

/* If Beginning ON or During (always turn on), turn on output. */
else if (output_indicator == 1 || output_indicator == 3) *WR_DIGITAL_IO = (temp_d_io |= output_number);

/* If Beginning OFF, turn off output. */
else if (output_indicator == 0) *WR_DIGITAL_IO = (temp_d_io &= ~output_number);


/***** IF A POSITION IS SET, WAIT UNTIL REACHED BEFORE CONTINUING ******/

if (position > 0)
   {
   do  /* wait for position */
      {
      temp = timer2;  /* Freeze timer2 */

      if (e_stop) goto ex;
      if (!while_on_flag && !while_off_flag)
	 {
	 if (tim1_ovfl_ctr == max_block_time)  goto outputs;
	 }
      /* If while conditional, check condition. */
      if (while_on_flag)
	 {
	 if (!(*RD_DIGITAL_IO & input_number))
	    {
	    goto end_of_dac_control;
	    }
	 }
      else if (while_off_flag)
	 {
	 if (*RD_DIGITAL_IO & input_number)
	    {
	    goto end_of_dac_control;
	    }
	 }
      }while ((temp < position) || (temp >= UNDER_FLOW_VAL));  /* wait for position */
   }


/******** IF LONG DWELL, SHORT DWELL OR BOTH ARE NEEDED, DO NOW *******/

/* For delays > 87.3813 mS to 5726 seconds (1.59 hours). */
if (l_dwell_time == 0);
else
   {
   temp_time = tim1_ovfl_ctr + l_dwell_time;

   do
      {
      if (e_stop)  goto ex;
      if (!while_on_flag && !while_off_flag)
	 {
	 if (tim1_ovfl_ctr == max_block_time)  goto outputs;
	 }

      /* If while conditional, check condition. */
      if (while_on_flag)
	 {
	 if (!(*RD_DIGITAL_IO & input_number))
	    {
	    goto end_of_dac_control;
	    }
	 }
      else if (while_off_flag)
	 {
	 if (*RD_DIGITAL_IO & input_number)
	    {
	    goto end_of_dac_control;
	    }
	 }
      }while (tim1_ovfl_ctr != temp_time);

   }

/* For delays < 87.3813 mS to 1.3333 uS */
if (s_dwell_time == 0);
else
   {
   dis_int();
   dwell_time_flag = 0;

   hso_command = 0x15;
   hso_time = timer1 + s_dwell_time; /*  timer1 + s_dwell_time;*/

   en_int();

   /* Don't check input while conditions during short dwells. */
   do
      {
      if(e_stop) goto ex;
      if (!while_on_flag && !while_off_flag)
	 {
	 if (tim1_ovfl_ctr == max_block_time)  goto outputs;
	 }
      }while (!dwell_time_flag);

   }


/* Now check for special dac commands. */

if (final_dac_command < 0)
   {
   if(final_dac_command == -1);       /* Leave unchanged */

   else if (final_dac_command == -3)  /* Null */
      /* Since the out_dac_w_check will add both the null and lvdt offsets,
	 the out_dac() function must be used. */
      out_dac(dac_command = (VALVE_NULL));

   else if (final_dac_command == -4)  /* Center */
      out_dac_w_check(dac_command = 0x2000);

   else if (final_dac_command == -5)  /* Jog shot valve setting */
      out_dac_w_check(dac_command = js_setting);

   else if (final_dac_command == -6)  /* Retract valve setting */
      out_dac_w_check(dac_command = retr_setting);

   else if (final_dac_command == -7)  /* Follow through valve setting */
      out_dac_w_check(dac_command = ft_setting);

   else if (final_dac_command == -8)  /* Set SHOT_IN_PROGRESS status bit */
      {
      SET_SHOT_IN_PROGRESS;
      WRITE_STATUS;
      }

   else if (final_dac_command == -9)  /* Clear SHOT_IN_PROGRESS status bit */
      {
      CLR_SHOT_IN_PROGRESS;
      WRITE_STATUS;
      }
   }


/***** NOW OUTPUT TO DAC, EITHER ALL OUT OR CONTROLLED ACCELERATION *****/

else if (accel == 0)  /* All out acceleration */
   out_dac_w_check(dac_command = final_dac_command);

else   /* Stepped output is required */
   {
   if (final_dac_command < dac_command) decel_flag = 1;  else decel_flag = 0;

   /* POSITIVE GOING COMMAND */
   /* If accel increment > current step, write command and move on */
   if (!decel_flag)
      {
      if (accel > (final_dac_command - dac_command))
	 out_dac_w_check(dac_command = final_dac_command);
      else
	 {
	 out_dac_w_check(dac_command += accel);
	 while (dac_command < final_dac_command)
	     {
	     dis_int();
	     dwell_time_flag = 0;
	     hso_command = 0x15;
	     hso_time = timer1 + 3750;  /* 5 mS */
	     en_int();

	     do     /* Wait for time out */
		 {
		 if (e_stop) goto ex;
		 if (!while_on_flag && !while_off_flag)
		    {
		    if (tim1_ovfl_ctr == max_block_time)  goto outputs;
		    }

		 /* If while conditional, check condition. */
		 if (while_on_flag)
		    {
		    if (!(*RD_DIGITAL_IO & input_number))
		       {
		       goto end_of_dac_control;
		       }
		    }
		 else if (while_off_flag)
		    {
		    if (*RD_DIGITAL_IO & input_number)
		       {
		       goto end_of_dac_control;
		       }
		    }
		 }while (!dwell_time_flag);  /* Wait for time out */

	     out_dac_w_check(dac_command += accel);
	     }
	 out_dac_w_check(dac_command = final_dac_command);  /* Write final value to DAC */
	 }
      }

   /* NEGATIVE GOING COMMAND */
   /* If decel decrement > current step, write cammand and move on */
   if (decel_flag)
      {
      if (accel > (dac_command - final_dac_command))
	 out_dac_w_check(dac_command = final_dac_command);
      else
	 {
	 out_dac_w_check(dac_command -= accel);
	 while (dac_command > (final_dac_command + accel))
	     {
	     dis_int();
	     dwell_time_flag = 0;
	     hso_command = 0x15;
	     hso_time = timer1 + 3750;  /* 5 mS */
	     en_int();

	     do  /* Wait for time out */
		 {
		 if (e_stop) goto ex;
		 if (!while_on_flag && !while_off_flag)
		    {
		    if (tim1_ovfl_ctr == max_block_time)  goto outputs;
		    }

		 /* If while conditional, check condition. */
		 if (while_on_flag)
		    {
		    if (!(*RD_DIGITAL_IO & input_number))
		       {
		       goto end_of_dac_control;
		       }
		    }
		 else if (while_off_flag)
		    {
		    if (*RD_DIGITAL_IO & input_number)
		       {
		       goto end_of_dac_control;
		       }
		    }
		 }while (!dwell_time_flag);  /* Wait for time out */

	     out_dac_w_check(dac_command -= accel);
	     }
	 out_dac_w_check(dac_command = final_dac_command);  /* Write final value to DAC */
	 }
      }
   }

end_of_dac_control: ;


/*********** IF WHILE CONDITION STILL TRUE, WAIT UNTIL FALSE *************/

if (e_stop) goto ex;

else
   {
   if (while_on_flag)
      {
      do
	 {
	 if (e_stop) goto ex;
	 if (!while_on_flag && !while_off_flag)
	    {
	    if (tim1_ovfl_ctr == max_block_time)  goto outputs;
	    }
	 }while (*RD_DIGITAL_IO & input_number);
      }

   else if (while_off_flag)
      {
      do
	 {
	 if (e_stop) goto ex;
	 if (!while_on_flag && !while_off_flag)
	    {
	    if (tim1_ovfl_ctr == max_block_time)  goto outputs;
	    }
	 }while (!(*RD_DIGITAL_IO & input_number));
      }


/**************** CHECK FOR DURING OR AT END OUTPUTS NOW ****************/
outputs: ;

   if (output_indicator == 999);

   /* If during output is on, turn it off now */
   else if (output_indicator == 3)  *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

   /* If At End  OFF output is required, do it now */
   else if (output_indicator == 4)  *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

   /* If At End  ON output is required, do it now */
   else if (output_indicator == 5)  *WR_DIGITAL_IO = (temp_d_io |= output_number);
   }

/******************* UPDATE BLOCK POINTER AND EXIT *****************/

/* Now update the block_ptr to point to the correct next block to
   execute before exiting. */
ex: ;
block_ptr += 30;   /* Next block */
ioport1 = old_dp;
}





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

   *block_ptr + 14 to + 16 reserved for future use.

   *block_ptr + 18  == Conditional analog check value.
   *block_ptr + 20  == Variable indicator.
   *block_ptr + 22  == Input check indication.  999 - unconditional,
	      xxxxxx yyy ccc bb aa
	      |      |   |   |  |
	      |      |   |   |  00 - If input off, 01 - If input on
	      |      |   |   |  10 - While input off, 11 - While input on
	      |      |   |   |
	      |      |   |   00 - Analog channel 1,  01 - Analog channel 2
	      |      |   |   10 - Analog channel 3,  11 - Analog channel 4
	      |      |   |
	      |      |   000 - No analog checking, 001 - If <=, 010 - If >=
	      |      |   011 - If =, 100 - If !=
	      |      |   101 - 111 Undefined
	      |      |
	      |      000 - 110 Undefined but usable for future
	      |      111 - Unconditional block.  Set when 999d
	      |
	      Unused, set to all zeros.

   *block_ptr + 24  == Input to check. Ignored if previous is 999.
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
int out_dac_w_check(), low_imp_dac, pct, dac_increment, loop_time;
unsigned int input_indicator, input_number, position;
unsigned int output_indicator, output_number, variable_indicator;
unsigned int dither_time, dither_ampl, low_imp_input;
unsigned char old_dp, while_on_flag, while_off_flag, vac_on, slow, fill_test;
unsigned int temp, analog_val, ana_check_val;
unsigned int ol_setting, accel_increments;
unsigned int max_block_time, vel_ctr;
unsigned int temp_tim2, newt2;
long temp_dac_command, accel, long_dac_command, max_dv, min_dv, dv;
unsigned long act_vel, commanded_vel, int_vel_cmd;

/* Parameter definitions, defined globally */
extern unsigned int bkgn1, bkgn2, bkgn3, vel_loop_gain;
extern unsigned int dither_time, dither_ampl;
extern unsigned long bk_vel1, bk_vel2, bk_vel3;
extern unsigned long vel_dividend;


old_dp = ioport1;
ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function      */
			   /* to save time.                        */


/* Initialize variables */
while_on_flag = 0;
while_off_flag = 0;

variable_indicator = *((unsigned int * )(block_ptr + 20));

if (variable_indicator & 0x0040)
   goose = *((unsigned int * )(*((unsigned int * )(block_ptr + 12))));
else  goose = *((unsigned int * )(block_ptr + 12));

/* Now check if goose == 0xffff.  This is a quick way to skip the
   entire block if necessary. */
if (goose == -1) goto ex;

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

if (variable_indicator & 0x0200)
   ana_check_val = *((unsigned int * )(*((unsigned int * )(block_ptr + 18))));
else  ana_check_val = *((unsigned int * )(block_ptr + 18));

input_indicator = *((unsigned int * )(block_ptr + 22));

if (variable_indicator & 0x1000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 24))));
else  input_number = *((unsigned int * )(block_ptr + 24));

output_indicator = *((unsigned int * )(block_ptr + 26));

if (variable_indicator & 0x4000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */

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

else if (input_indicator < 16 && input_number) /* Conditional execution */
   /* Setting the input_number to 0 will disable conditional checking */
   {
   if (input_indicator == 1)  /* IF input on conditional execution. */
      {
      if (!(*RD_DIGITAL_IO & input_number)) goto ex;
      }

   else if (input_indicator == 0) /* IF input off conditional execution. */
      {
      if (*RD_DIGITAL_IO & input_number) goto ex;
      }

   else if (input_indicator == 3)
      /* WHILE input on conditional.  */
      {
      if (!(*RD_DIGITAL_IO & input_number)) goto ex;
      while_on_flag = 1;
      }

   else if (input_indicator == 2)
      /* WHILE input off conditional. */
      {
      if (*RD_DIGITAL_IO & input_number) goto ex;
      while_off_flag = 1;
      }
   }

else    /* Check analog conditional */
   {
   a_to_d_loop();

   temp = input_indicator; temp >>= 2; temp &= 0x0003; /* Extract channel */
   switch (temp)
       {
       case 0:  /* analog channel 1 */
	 analog_val = temp_ana[1];
	 break;

       case 1:  /* analog channel 2 */
	 analog_val = temp_ana[2];
	 break;

       case 2:  /* analog channel 3 */
	 analog_val = temp_ana[3];
	 break;

       case 3:  /* analog channel 4 */
	  analog_val = temp_ana[4];
	  break;
       }

   temp = input_indicator; temp >>= 4; temp &= 0x0007;  /* Extract operator */
   switch (temp)
      {
      case 0:   /* Indicates no analog checking. */
	break;

      case 1:  /* <= */
	if (analog_val <= ana_check_val)  break;
	else  goto ex;
	break;

      case 2:  /* >= */
	if (analog_val >= ana_check_val)  break;
	else  goto ex;
	break;

      case 3:  /* = */
	if (analog_val == ana_check_val)  break;
	else  goto ex;
	break;

      case 4:  /* != */
	if (analog_val != ana_check_val)  break;
	else  goto ex;
	break;

      case 5:
      case 6:
      case 7:
	break;   /* Undefined operators */
      }
   }

/********** NOW SEE IF ANY OUTPUTS NEED TO BE SET / CLEARED **********/

if (output_indicator == 999);

/* If Beginning ON or During (always turn on), turn on output. */
else if (output_indicator == 1 || output_indicator == 3) *WR_DIGITAL_IO = (temp_d_io |= output_number);

/* If Beginning OFF, turn off output. */
else if (output_indicator == 0) *WR_DIGITAL_IO = (temp_d_io &= ~output_number);



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
		if ((vel_dividend / int_vel_cmd) > 7500)
			{
			if (((dv * 5) > max_dv) || ((dv * 5) < min_dv)) dv /= 5;
			temp_dac_command = (dv * (long)(vel_loop_gain * 5)) / 10000000;
			}
		else
			temp_dac_command = (dv * (long)vel_loop_gain) / 10000000;


		/* Boost code here */

		/* Now check gain breaks. */
		if ((act_vel >= bk_vel3) && (temp_tim2 > 240) && (temp_tim2 < UNDER_FLOW_VAL))
			temp_dac_command = (temp_dac_command * (long)bkgn3) / 1000;

		else if ((act_vel >= bk_vel2) && (temp_tim2 > 240) && (temp_tim2 < UNDER_FLOW_VAL))
			temp_dac_command = (temp_dac_command * (long)bkgn2) / 1000;

		else if ((act_vel >= bk_vel1) && (temp_tim2 > 240) && (temp_tim2 < UNDER_FLOW_VAL))
			temp_dac_command = (temp_dac_command * (long)bkgn1) / 1000;

		dac_command = (unsigned int)temp_dac_command + dac_command;

		/* Put dither code here. */

		out_dac_w_check(dac_command);

		do
			{
			/*1111111111111111111111111111111111111111111111111111111111111*/
			temp_tim2 = timer2;  /* Freeze timer2 */

			if (e_stop)
				goto ex;
				
			if ((ioport0 & 0x40) && (temp_tim2 >= p_check_z_speed))
				{
				if (temp_tim2 >= UNDER_FLOW_VAL);

				/* Check if a vacuum shot step, if so must abort entire
				shot to prevent filling vacuum system if not a legit
				go fast shot signal (the only acceptable way of terminating
				a vacuum step). */
				else if (vac_on && while_off_flag)
					{
					fatal_error = 1;
					SET_FATAL_ERR;
					goto ex;  /* Let's get out of here */
					}

				else
					goto outputs;  /* True zspeed, end v_loop */
				}


			if (tim1_ovfl_ctr == max_block_time)
				{
				fatal_error = 2;
				SET_FATAL_ERR;
				goto ex;  /* Let's get out of here */
				}


			if (!while_on_flag && !while_off_flag && !low_imp_input);
			else
				{
				/* Termininating conditions of the v_loop that don't care about
				the position setting.  Zspeed will terminate block. */

				/* If while conditional, check condition. */
				if (while_on_flag && !(*RD_DIGITAL_IO & input_number))
					goto outputs;
					
				else if (while_off_flag && (*RD_DIGITAL_IO & input_number))
					{
					if (!vac_on);
					else
						goto outputs;
					}

			else if (low_imp_input && low_imp_dac)  /* Input based low impact */
				{
				/* Check if a vacuum shot step, if so must abort entire
					shot to prevent filling vacuum system if not a legit
					go fast shot signal (the only acceptable way of terminating
					a vacuum step). */
				if (vac_on && while_off_flag)
					{
					fatal_error = 3;
					SET_FATAL_ERR;
					goto ex;  /* Let's get out of here */
					}

			else if (*RD_DIGITAL_IO & low_imp_input)
				/* clear hso cam */
				/*ioc2 = (temp_ioc2 | 0x80);*/
				goto end_of_vloop;
		      
		  
			if (position)
				{
				if (vac_on && while_off_flag);
				else if (temp_tim2 >= position)
					{
					if (temp_tim2 >= UNDER_FLOW_VAL);
					/* If vac wait and fill_test, halt plunger movement. */
					else if (while_off_flag && fill_test && !vac_on)
						e_stop = TRUE;
					else
						goto end_of_vloop;
		      
				}	   
		    }while (timer2 < newt2);  /* Wait for next sample */
	    } /* for loop */
      }  /* Closed loop accel */

   else if (goose)  /* Legitiment goose command */
      {
      out_dac_w_check(dac_command = goose);
      act_vel = vel_dividend / (unsigned long)*VELOCITY;  /* Vel X 10000 */
      loop_num = 200;  /* For diagnostics */

      /* Wait until speed is within 90% of commanded. */
      while (act_vel <= ((commanded_vel / 111) * 100)) /* approx. 90% */
	     {
	     /*2222222222222222222222222222222222222222222222222222222222222*/
	     temp_tim2 = timer2;  /* Freeze timer2 */

	     if (e_stop)
			goto ex;
		
	     if ((ioport0 & 0x40) && (temp_tim2 >= p_check_z_speed))
			{
			if (temp_tim2 >= UNDER_FLOW_VAL);

			/* Check if a vacuum shot step, if so must abort entire
				shot to prevent filling vacuum system if not a legit
				go fast shot signal (the only acceptable way of terminating
				a vacuum step). */
			else if (vac_on && while_off_flag)
				{
			    fatal_error = 1;
				SET_FATAL_ERR;
				goto ex;  /* Let's get out of here */
				}

			else
				goto outputs;  /* True zspeed, end v_loop */
			
			}


	     if (tim1_ovfl_ctr == max_block_time)
			{
			fatal_error = 2;
			SET_FATAL_ERR;
			goto ex;  /* Let's get out of here */
			}

	     if (!while_on_flag && !while_off_flag && !low_imp_input);
	     else
			{
			/* Termininating conditions of the v_loop that don't care about
				the position setting.  Zspeed will terminate block. */
	
			/* If while conditional, check condition. */
			if (while_on_flag && !(*RD_DIGITAL_IO & input_number))
			   goto outputs;
		   
			else if (while_off_flag && (*RD_DIGITAL_IO & input_number))
				{
				if (!vac_on);
				else
					goto outputs;
				}

			else if (low_imp_input && low_imp_dac)  /* Input based low impact */
				{
				/* Check if a vacuum shot step, if so must abort entire
					shot to prevent filling vacuum system if not a legit
					go fast shot signal (the only acceptable way of terminating
					a vacuum step). */
				if (vac_on && while_off_flag)
					{
					fatal_error = 3;
					SET_FATAL_ERR;
					goto ex;  /* Let's get out of here */
					}

				else if (*RD_DIGITAL_IO & low_imp_input)
					/* clear hso cam */
					/*ioc2 = (temp_ioc2 | 0x80);*/
					goto end_of_vloop;
				}
			}

	     if (position)
			{
			if (vac_on && while_off_flag);
			else if (temp_tim2 >= position)
				{
				if (temp_tim2 >= UNDER_FLOW_VAL);
				/* If vac wait and fill_test, halt plunger movement. */
				else if (while_off_flag && fill_test && !vac_on)
					{
					e_stop = TRUE;
					goto ex;
					}
				else
					{
					/* clear hso cam */
					/*ioc2 = (temp_ioc2 | 0x80);*/
					goto end_of_vloop;
					}
				}
			}

	     act_vel = vel_dividend / (unsigned long)*VELOCITY;  /* Vel X 10000 */
	     }
      }

   /********************* CLOSE VELOCITY LOOP ********************/

   /* Determine loop time based on commanded velocity. */
   if ((vel_dividend / commanded_vel) > 12000)  /* Slow */
      {
      loop_time = 18750;  /* 25mS */
      slow = 1;
      }
   else
      {
      loop_time = 1875;   /* 2.5mS */
      slow = 0;
      }
   loop_num = 300;  /* For diagnostics */

   while (1)  /* Loop until forced out by code in loop */
      {
      dis_int();
      dwell_time_flag = 0;
      hso_command = 0x15;
      hso_time = timer1 + loop_time;
      en_int();

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
		temp_dac_command = (dv * (long)(vel_loop_gain * 10)) / 10000000;
		}
      else
		/* temp_dac_command is the change in dac output before adding to
			the existing dac output */
		temp_dac_command = (dv * (long)vel_loop_gain) / 10000000;


      /* Boost code here */

      /* Now check gain breaks. */
      temp_tim2 = timer2;  /* Freeze timer2 */

      if ((act_vel >= bk_vel3) && (temp_tim2 > 240) && (temp_tim2 < UNDER_FLOW_VAL))
		temp_dac_command = (temp_dac_command * (long)bkgn3) / 1000;

      else if ((act_vel >= bk_vel2) && (temp_tim2 > 240) && (temp_tim2 < UNDER_FLOW_VAL))
		temp_dac_command = (temp_dac_command * (long)bkgn2) / 1000;

      else if ((act_vel >= bk_vel1) && (temp_tim2 > 240) && (temp_tim2 < UNDER_FLOW_VAL))
		temp_dac_command = (temp_dac_command * (long)bkgn1) / 1000;

      dac_command = (unsigned int)temp_dac_command + dac_command;


      /* Put dither code here. */

      out_dac_w_check(dac_command);



      /* Make waiting loop a do loop to ensure at least one pass through */
      do /* Wait for next sample */
	     {
	     /*3333333333333333333333333333333333333333333333333333333333333*/
	     temp_tim2 = timer2;  /* Freeze timer2 */

	     if (e_stop)
			goto ex;
		
	     if ((ioport0 & 0x40) && (temp_tim2 >= p_check_z_speed))
			{
			if (temp_tim2 >= UNDER_FLOW_VAL);

			/* Check if a vacuum shot step, if so must abort entire
				shot to prevent filling vacuum system if not a legit
				go fast shot signal (the only acceptable way of terminating
				a vacuum step). */
			else if (vac_on && while_off_flag)
				{
				fatal_error = 1;
				SET_FATAL_ERR;
				goto ex;  /* Let's get out of here */
				}

			else
				goto outputs;  /* True zspeed, end v_loop */
			}


	     if (tim1_ovfl_ctr == max_block_time)
			{
			fatal_error = 2;
			SET_FATAL_ERR;
			goto ex;  /* Let's get out of here */
			}


	     if (!while_on_flag && !while_off_flag && !low_imp_input);
	     else
			{
			/* Termininating conditions of the v_loop that don't care about
				the position setting.  Zspeed will terminate block. */

			/* If while conditional, check condition. */
			if (while_on_flag && !(*RD_DIGITAL_IO & input_number))
				goto outputs;
		    
			else if (while_off_flag && (*RD_DIGITAL_IO & input_number))
				{
				if (!vac_on);
				else
					goto outputs;
		        }

			else if (low_imp_input && low_imp_dac)  /* Input based low impact */
				{
				/* Check if a vacuum shot step, if so must abort entire
					shot to prevent filling vacuum system if not a legit
					go fast shot signal (the only acceptable way of terminating
					a vacuum step). */
				if (vac_on && while_off_flag)
					{
					fatal_error = 3;
					SET_FATAL_ERR;
					goto ex;  /* Let's get out of here */
					}

			else if (*RD_DIGITAL_IO & low_imp_input)
				/* clear hso cam */
				/*ioc2 = (temp_ioc2 | 0x80);*/
				/* Now write to the holding ram for testing */
				goto end_of_vloop;
		   }
		

	     if (position)
			{
			if (vac_on && while_off_flag);
			else if (temp_tim2 >= position)
				{
				if (temp_tim2 >= UNDER_FLOW_VAL);
				/* If vac wait and fill_test, halt plunger movement. */
				else if (while_off_flag && fill_test && !vac_on)
					e_stop = TRUE;
		      
				goto ex;
				}

		   else
		      /* clear hso cam */
		      /*ioc2 = (temp_ioc2 | 0x80);*/
		      /* Now write to the holding ram for testing */
		      goto end_of_vloop;
		      
		   }
		
		 }while (!dwell_time_flag);  /* Wait for next sample */
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
	    out_dac_w_check(dac_command);

	    /* Now write to the holding ram for testing */
	    if (hold_ctr < 2040)
	       {
	       ioport1 &= 0xC7;           /* select ram high, do directly insted  */
	       ioport1 |= 0x38;           /* of calling the newdp() function.     */
	       TEMP_HOLDING[hold_ctr++] = 700;
	       TEMP_HOLDING[hold_ctr++] = newt2;
	       TEMP_HOLDING[hold_ctr++] = dac_increment;
	       TEMP_HOLDING[hold_ctr++] = dac_command;
	       TEMP_HOLDING[hold_ctr++] = 700;
	       ioport1 &= 0xC7;           /* select I/O, do directly insted  */
	       ioport1 |= 0x20;           /* of calling the newdp() function */
	       }


	    do
	     {
	     /*4444444444444444444444444444444444444444444444444444444444444*/
	     temp_tim2 = timer2;  /* Freeze timer2 */

	     if (e_stop)
		{
		/* Now write to the holding ram for testing */
		if (hold_ctr < 2040)
		   {
		   ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		   ioport1 |= 0x38;           /* of calling the newdp() function.     */
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		   ioport1 |= 0x20;           /* of calling the newdp() function */
		   }
		goto ex;
		}

	     if ((ioport0 & 0x40) && (temp_tim2 >= p_check_z_speed))
		{
		if (temp_tim2 >= UNDER_FLOW_VAL);

		/* Check if a vacuum shot step, if so must abort entire
		   shot to prevent filling vacuum system if not a legit
		   go fast shot signal (the only acceptable way of terminating
		   a vacuum step). */
		else if (vac_on && while_off_flag)
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   fatal_error = 1;
		   SET_FATAL_ERR;
		   goto ex;  /* Let's get out of here */
		   }

		else
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   goto outputs;  /* True zspeed, end v_loop */
		   }
		}


	     if (tim1_ovfl_ctr == max_block_time)
		{
		/* Now write to the holding ram for testing */
		if (hold_ctr < 2040)
		   {
		   ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		   ioport1 |= 0x38;           /* of calling the newdp() function.     */
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		   ioport1 |= 0x20;           /* of calling the newdp() function */
		   }
		fatal_error = 2;
		SET_FATAL_ERR;
		goto ex;  /* Let's get out of here */
		}


	     if (!while_on_flag && !while_off_flag && !low_imp_input);
	     else
		{
		/* Termininating conditions of the v_loop that don't care about
		   the position setting.  Zspeed will terminate block. */

		/* If while conditional, check condition. */
		if (while_on_flag && !(*RD_DIGITAL_IO & input_number))
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   goto outputs;
		   }

		else if (while_off_flag && (*RD_DIGITAL_IO & input_number))
		   {
		   if (!vac_on);
		   else
		      {
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto outputs;
		      }
		   }

		else if (low_imp_input && low_imp_dac)  /* Input based low impact */
		   {
		   /* Check if a vacuum shot step, if so must abort entire
		      shot to prevent filling vacuum system if not a legit
		      go fast shot signal (the only acceptable way of terminating
		      a vacuum step). */
		   if (vac_on && while_off_flag)
		      {
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      fatal_error = 3;
		      SET_FATAL_ERR;
		      goto ex;  /* Let's get out of here */
		      }

		   else if (*RD_DIGITAL_IO & low_imp_input)
		      {
		      /* clear hso cam */
		      /*ioc2 = (temp_ioc2 | 0x80);*/
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto end_of_vloop;
		      }
		   }
		}

	     if (position)
		{
		if (vac_on && while_off_flag);
		else if (temp_tim2 >= position)
		   {
		   if (temp_tim2 >= UNDER_FLOW_VAL);
		   /* If vac wait and fill_test, halt plunger movement. */
		   else if (while_off_flag && fill_test && !vac_on)
		      {
		      e_stop = TRUE;
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto ex;
		      }

		   else
		      {
		      /* clear hso cam */
		      /*ioc2 = (temp_ioc2 | 0x80);*/
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto end_of_vloop;
		      }
		   }
		}

	     }while (timer2 < newt2);  /* Wait for next sample */
	    }

	 /* Write for loop ending info */
	 if (hold_ctr < 2040)
	    {
	    ioport1 &= 0xC7;           /* select ram high, do directly insted  */
	    ioport1 |= 0x38;           /* of calling the newdp() function.     */
	    TEMP_HOLDING[hold_ctr++] = 900;
	    TEMP_HOLDING[hold_ctr++] = pct;
	    TEMP_HOLDING[hold_ctr++] = newt2;
	    TEMP_HOLDING[hold_ctr++] = dac_command;
	    TEMP_HOLDING[hold_ctr++] = 900;
	    ioport1 &= 0xC7;           /* select I/O, do directly insted  */
	    ioport1 |= 0x20;           /* of calling the newdp() function */
	    }
	 }  /* End of controlled accel loop. */
      }

   /* ol_setting is the raw dac command */
   out_dac_w_check(dac_command = ol_setting);
   loop_num = 500;  /* For diagnostics */

   do  /* Wait for something to terminate the block */
	     {
	     /*5555555555555555555555555555555555555555555555555555555555555*/
	     temp_tim2 = timer2;  /* Freeze timer2 */

	     if (e_stop)
		{
		/* Now write to the holding ram for testing */
		if (hold_ctr < 2040)
		   {
		   ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		   ioport1 |= 0x38;           /* of calling the newdp() function.     */
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		   ioport1 |= 0x20;           /* of calling the newdp() function */
		   }
		goto ex;
		}

	     if ((ioport0 & 0x40) && (temp_tim2 >= p_check_z_speed))
		{
		if (temp_tim2 >= UNDER_FLOW_VAL);

		/* Check if a vacuum shot step, if so must abort entire
		   shot to prevent filling vacuum system if not a legit
		   go fast shot signal (the only acceptable way of terminating
		   a vacuum step). */
		else if (vac_on && while_off_flag)
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   fatal_error = 1;
		   SET_FATAL_ERR;
		   goto ex;  /* Let's get out of here */
		   }

		else
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   goto outputs;  /* True zspeed, end v_loop */
		   }
		}


	     if (tim1_ovfl_ctr == max_block_time)
		{
		/* Now write to the holding ram for testing */
		if (hold_ctr < 2040)
		   {
		   ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		   ioport1 |= 0x38;           /* of calling the newdp() function.     */
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		   ioport1 |= 0x20;           /* of calling the newdp() function */
		   }
		fatal_error = 2;
		SET_FATAL_ERR;
		goto ex;  /* Let's get out of here */
		}


	     if (!while_on_flag && !while_off_flag && !low_imp_input);
	     else
		{
		/* Termininating conditions of the v_loop that don't care about
		   the position setting.  Zspeed will terminate block. */

		/* If while conditional, check condition. */
		if (while_on_flag && !(*RD_DIGITAL_IO & input_number))
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   goto outputs;
		   }

		else if (while_off_flag && (*RD_DIGITAL_IO & input_number))
		   {
		   if (!vac_on);
		   else
		      {
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto outputs;
		      }
		   }

		else if (low_imp_input && low_imp_dac)  /* Input based low impact */
		   {
		   /* Check if a vacuum shot step, if so must abort entire
		      shot to prevent filling vacuum system if not a legit
		      go fast shot signal (the only acceptable way of terminating
		      a vacuum step). */
		   if (vac_on && while_off_flag)
		      {
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      fatal_error = 3;
		      SET_FATAL_ERR;
		      goto ex;  /* Let's get out of here */
		      }

		   else if (*RD_DIGITAL_IO & low_imp_input)
		      {
		      /* clear hso cam */
		      /*ioc2 = (temp_ioc2 | 0x80);*/
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto end_of_vloop;
		      }
		   }
		}

	     if (position)
		{
		if (vac_on && while_off_flag);
		else if (temp_tim2 >= position)
		   {
		   if (temp_tim2 >= UNDER_FLOW_VAL);
		   /* If vac wait and fill_test, halt plunger movement. */
		   else if (while_off_flag && fill_test && !vac_on)
		      {
		      e_stop = TRUE;
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto ex;
		      }

		   else
		      {
		      /* clear hso cam */
		      /*ioc2 = (temp_ioc2 | 0x80);*/
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto end_of_vloop;
		      }
		   }
		}

	     }while (1);  /* Loop until something terminates the loop */
   }

end_of_vloop: ;


/************** OUTPUT LOW IMPACT COMMAND HERE IF NEEDED *****************/

if (low_imp_dac)  /* If low impact required */
   out_dac_w_check(dac_command = low_imp_dac);



/**************** HANDLE DURING OR AT END OUTPUTS NOW *****************/
outputs: ;

if (e_stop) goto ex;

else
   {
   if (output_indicator == 999);

   /* If during output is on, turn it off now */
   else if (output_indicator == 3)  *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

   /* If At End  OFF output is required, do it now */
   else if (output_indicator == 4)  *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

   /* If At End  ON output is required, do it now */
   else if (output_indicator == 5)  *WR_DIGITAL_IO = (temp_d_io |= output_number);
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

   *block_ptr + 8 to + 16 reserved for future use.

   *block_ptr + 18  == Conditional analog check value.
   *block_ptr + 20  == Variable indicator.
   *block_ptr + 22  == Input check indication.  999 - unconditional,
	      xxxxxx yyy ccc bb aa
	      |      |   |   |  |
	      |      |   |   |  00 - If input off, 01 - If input on
	      |      |   |   |
	      |      |   |   00 - Analog channel 1,  01 - Analog channel 2
	      |      |   |   10 - Analog channel 3,  11 - Analog channel 4
	      |      |   |
	      |      |   000 - No analog checking, 001 - If <=, 010 - If >=
	      |      |   011 - If =, 100 - If !=
	      |      |   101 - 111 Undefined
	      |      |
	      |      000 - 110 Undefined but usable for future
	      |      111 - Unconditional block.  Set when 999d
	      |
	      Unused, set to all zeros.

   *block_ptr + 24  == Input to check. Ignored if previous is 999.
   *block_ptr + 26  == Output set or clear indication. 999 - no outputs to set.
		      0 - Beginning OFF, 1 - Beginning ON,
		      3 - During, 4 - At end OFF, 5 - At end ON.
   *block_ptr + 28  == Output to set / clear. Ignored if previous is 999.
*/

unsigned int rel_indicator, block_num, input_indicator, input_number;
unsigned int output_indicator, output_number, temp_ptr, type, pop();
unsigned int variable_indicator;
unsigned int temp, analog_val, ana_check_val;
unsigned char old_dp;
int push();

old_dp = ioport1;
ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function */
			   /* to save time.                   */

variable_indicator = *((unsigned int * )(block_ptr + 20));

rel_indicator = *((unsigned int * )(block_ptr + 2));

if (variable_indicator & 0x0004)
   block_num = *((unsigned int * )(*((unsigned int * )(block_ptr + 4))));
else  block_num = *((unsigned int * )(block_ptr + 4));

type = *((unsigned int * )(block_ptr + 6));

if (variable_indicator & 0x0200)
   ana_check_val = *((unsigned int * )(*((unsigned int * )(block_ptr + 18))));
else  ana_check_val = *((unsigned int * )(block_ptr + 18));

input_indicator = *((unsigned int * )(block_ptr + 22));

if (variable_indicator & 0x1000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 24))));
else  input_number = *((unsigned int * )(block_ptr + 24));

output_indicator = *((unsigned int * )(block_ptr + 26));

if (variable_indicator & 0x4000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */

if (input_indicator == 999);  /* Unconditional execution */

else if (input_indicator < 16)  /* Conditional If / While */
   {
   if (input_indicator == 1)  /* IF input on conditional execution. */
      {
      if (!(*RD_DIGITAL_IO & input_number)) {block_ptr += 30; goto ex;}
      }

   else if (input_indicator == 0) /* IF input off conditional execution. */
      {
      if (*RD_DIGITAL_IO & input_number) {block_ptr += 30; goto ex;}
      }
   }

else    /* Check analog conditional */
   {
   a_to_d_loop();

   temp = input_indicator; temp >>= 2; temp &= 0x0003; /* Extract channel */
   switch (temp)
       {
       case 0:  /* analog channel 1 */
	 analog_val = temp_ana[1];
	 break;

       case 1:  /* analog channel 2 */
	 analog_val = temp_ana[2];
	 break;

       case 2:  /* analog channel 3 */
	 analog_val = temp_ana[3];
	 break;

       case 3:  /* analog channel 4 */
	  analog_val = temp_ana[4];
	  break;
       }

   temp = input_indicator; temp >>= 4; temp &= 0x0007;  /* Extract operator */
   switch (temp)
      {
      case 0:   /* Indicates no analog checking. */
	break;

      case 1:  /* <= */
	if (analog_val <= ana_check_val)  break;
	else  {block_ptr += 30; goto ex;}
	break;

      case 2:  /* >= */
	if (analog_val >= ana_check_val)  break;
	else  {block_ptr += 30; goto ex;}
	break;

      case 3:  /* = */
	if (analog_val == ana_check_val)  break;
	else  {block_ptr += 30; goto ex;}
	break;

      case 4:  /* != */
	if (analog_val != ana_check_val)  break;
	else  {block_ptr += 30; goto ex;}
	break;

      case 5:
      case 6:
      case 7:
	break;   /* Undefined operators */
      }
   }

if (output_indicator == 999);

/* If Beginning ON or During (always turn on), turn on output. */
else if (output_indicator == 1 || output_indicator == 3)
     *WR_DIGITAL_IO = (temp_d_io |= output_number);

/* If Beginning OFF, turn off output. */
else if (output_indicator == 0)
     *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

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
else if (output_indicator == 3)  *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

/* If At End  OFF output is required, do it now */
else if (output_indicator == 4)  *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

/* If At End  ON output is required, do it now */
else if (output_indicator == 5)  *WR_DIGITAL_IO = (temp_d_io |= output_number);

ex: ;
ioport1 = old_dp;
}




setpos_block()
{
/* Framework for the set position block type */

/* *block_ptr + 2   == Position to set axis to in X4 counts.
   *block_ptr + 4 to + 16 reserved for future use.

   *block_ptr + 18  == Reserved for conditional analog check value
   *block_ptr + 20  == Variable indicator.
   *block_ptr + 22  == Input check indication.  999 - unconditional,
		      0 - If input off, 1 - If input on.
   *block_ptr + 24  == Input to check. Ignored if previous is 999.
   *block_ptr + 26  == Output set or clear indication. 999 - no outputs to set.
		      0 - Beginning OFF, 1 - Beginning ON,
		      3 - During, 4 - At end OFF, 5 - At end ON.
   *block_ptr + 28  == Output to set / clear. Ignored if previous is 999.
*/

unsigned int position, input_indicator, input_number;
unsigned int output_indicator, output_number, variable_indicator;
unsigned char old_dp;
int x;

old_dp = ioport1;
ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function */
			   /* to save time.                   */

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
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */


if (input_indicator == 999);  /* Unconditional execution */

else  /* Find condition */
   {
   if (input_indicator == 1)  /* IF input on conditional execution. */
      {
      if (!(*RD_DIGITAL_IO & input_number)) goto ex;
      }

   else if (input_indicator == 0) /* IF input off conditional execution. */
      {
      if (*RD_DIGITAL_IO & input_number) goto ex;
      }
   }


if (output_indicator == 999);

/* If Beginning ON or During (always turn on), turn on output. */
else if (output_indicator == 1 || output_indicator == 3)
     *WR_DIGITAL_IO = (temp_d_io |= output_number);

/* If Beginning OFF, turn off output. */
else if (output_indicator == 0)
     *WR_DIGITAL_IO = (temp_d_io &= ~output_number);


timer2 = position;  /* Set position. */

/* Now clean the slate */
dis_int();
ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
e_stop = FALSE;
initialize_ls(); /*   */
/* Now set up software timer 0 for E stop checking. */
hso_command = 0x18;
hso_time = timer1 + 3750;  /* 5ms */
en_int();

hold_ctr = 0;  /* Pointer for test purposes, saving cl info during shot. */

/* Code here to clear the TEMP_HOLDING hi ram used for saving cl info. */
ioport1 &= 0xC7;           /* select ram hi, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function */

for (x = 0xC000; x <= 0xCFFE; x++)
    *((unsigned char * ) x) = 0;

ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */


if (output_indicator == 999);

/* If during output is on, turn it off now */
else if (output_indicator == 3)  *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

/* If At End  OFF output is required, do it now */
else if (output_indicator == 4)  *WR_DIGITAL_IO = (temp_d_io &= ~output_number);

/* If At End  ON output is required, do it now */
else if (output_indicator == 5)  *WR_DIGITAL_IO = (temp_d_io |= output_number);


/* Now update the block_ptr to point to the correct next block to
   execute before exiting. */

ex: block_ptr += 30;
ioport1 = old_dp;
}



/* Possibly support in the future, as of now the prcntl block type
   below is not supported. */

prcntl_block()
{
/* Framework for the program control block type */

/* *block_ptr + 2   == 0 - Start, 1 - Stop, 2 - Kill.
   *block_ptr + 4   == Program #.
   *block_ptr + 6   == Block #.

   *block_ptr + 8 to + 16 reserved for future use.

   *block_ptr + 18  == Conditional analog check value.
   *block_ptr + 20  == Variable indicator.
   *block_ptr + 22  == Input check indication.  999 - unconditional,
	      xxxxxx yyy ccc bb aa
	      |      |   |   |  |
	      |      |   |   |  00 - If input off, 01 - If input on
	      |      |   |   |  10 - While input off, 11 - While input on
	      |      |   |   |
	      |      |   |   00 - Analog channel 1,  01 - Analog channel 2
	      |      |   |   10 - Analog channel 3,  11 - Analog channel 4
	      |      |   |
	      |      |   000 - No analog checking, 001 - If <=, 010 - If >=
	      |      |   011 - If =, 100 - If !=
	      |      |   101 - 111 Undefined
	      |      |
	      |      000 - 110 Undefined but usable for future
	      |      111 - Unconditional block.  Set when 999d
	      |
	      Unused, set to all zeros.

   *block_ptr + 24  == Input to check. Ignored if previous is 999.
   *block_ptr + 26  == Output set or clear indication. 999 - no outputs to set.
		      0 - Beginning OFF, 1 - Beginning ON,
		      3 - During, 4 - At end OFF, 5 - At end ON.
   *block_ptr + 28  == Output to set / clear. Ignored if previous is 999.
*/


/* Now update the block_ptr to point to the correct next block to
   execute before exiting. */
}


push(unsigned int item)
{
stack[stack_ptr++] = item;
}


unsigned int pop()
{
return (stack[--stack_ptr]);
}



set_parameters()
{
unsigned char old_dp, newdp();
unsigned int tempu;

/* This function will update the global parameters from the high ram.
   The program blocks that need the parameters will use the global values.
   This is done to speed up the execution of the blocks by elminating the
   need to reestablish the values every time the block is executed */

old_dp = newdp((unsigned char)RAM_HI_SEL);

vel_loop_gain = *(unsigned int * )VEL_LOOP_GAIN;

tempu = *(unsigned int * )VEL_LOOP_BK_VEL1;
bk_vel1 = (unsigned long)tempu * 100;

tempu = *(unsigned int * )VEL_LOOP_BK_VEL2;
bk_vel2 = (unsigned long)tempu * 100;

tempu = *(unsigned int * )VEL_LOOP_BK_VEL3;
bk_vel3 = (unsigned long)tempu * 100;
/* All break velocities are units * 10000 */


bkgn1 = *((unsigned int * )VEL_LOOP_BKGN1);
bkgn2 = *((unsigned int * )VEL_LOOP_BKGN2);
bkgn3 = *((unsigned int * )VEL_LOOP_BKGN3);
dither_time = *(unsigned int * )DITHER_TIME; /* # of sample periods before reversing dither */
dither_ampl = *(unsigned int * )DITHER_AMPL; /* # of dac counts to add / subtract */
vel_dividend = *(unsigned long * )VEL_DIVIDEND;
p_check_z_speed = *(unsigned int * )P_CHECK_Z_SPEED;
counts_per_unit = *(unsigned int * )COUNTS_PER_UNIT;
null_offset = *(unsigned int * )NULL_OFFSET;
lvdt_offset = *(unsigned int * )LVDT_OFFSET;
js_setting = *(unsigned int * )JS_SETTING;
retr_setting = *(unsigned int * )RETR_SETTING;
ft_setting = *(unsigned int * )FT_SETTING;
vel_multiplier = *(unsigned int * )VEL_MULTIPLIER;

/* Update the break velocities since the vel_multiplier will affect these. */
bk_vel1 = (bk_vel1 * (unsigned long)vel_multiplier) / 100;
bk_vel2 = (bk_vel2 * (unsigned long)vel_multiplier) / 100;
bk_vel3 = (bk_vel3 * (unsigned long)vel_multiplier) / 100;

ioport1 = old_dp;
}




/* Function valve test is used to perform an oscilloscope like function
   for testing valve response.  After the trigger level is reached, 2000
   samples will be taken at .5mS intervals and saved in high ram, beginning
   at 0xE000

   Perhaps this should be a block type.  by doing this, certain
   parameters could be set.  Also it may be possible to take a
   sample once a shot. */

valve_test()
{
unsigned int temp_dac_command, time, ctr, time_interval;
unsigned char old_dp, newdp(), timeout;
int trigger_level, max_samples;
void a_to_d_loop();
VAL_TST_RESULTS[2048];

old_dp = newdp((unsigned char)IO_SEL);
dwell_time_flag = 0; timeout = 0;
max_samples = 500; /* 1 second at 2mS sample rate */
trigger_level = 200;  /* Approx .5 volt */
temp_dac_command = dac_command;
ctr = 0;

ioport1 &= 0xC7;           /* select IO, do directly insted      */
ioport1 |= 0x20;           /* of calling the newdp() function    */
			   /* to save time.                      */

out_dac(dac_command = 0x2000);   /* 0 volts */
time = tim1_ovfl_ctr + 6;        /* .5 sec */
while (tim1_ovfl_ctr <= time);   /* Wait for valve to settle */

out_dac(dac_command = 11000);     /* Open valve.  Command is 3.5 volts */

time = tim1_ovfl_ctr + 6;        /* .5 sec */

do      /* Wait for trigger */
   {
   a_to_d_loop();
   if (tim1_ovfl_ctr >= time) {timeout = 1; break;}
   }while ((int)temp_ana[1] < trigger_level);

if (!timeout)
   {
   while (ctr <= max_samples)
      {
      time_interval = timer1 + 1500;  /* 2 mS later */
      dwell_time_flag = 0;

      ioport1 &= 0xC7;           /* select IO, do directly insted      */
      ioport1 |= 0x20;           /* of calling the newdp() function    */
				 /* to save time.                      */
      a_to_d_loop();


      ioport1 &= 0xC7;           /* select ram hi, do directly insted  */
      ioport1 |= 0x38;           /* of calling the newdp() function    */
				 /* to save time.                      */
      VAL_TST_RESULTS[ctr] = temp_ana[1];

      if (ctr <= (max_samples-1))
	 {
	 dis_int();
	 hso_command = 0x15;
	 hso_time = time_interval;
	 en_int();
	 }
      ctr++;
      while (!dwell_time_flag && ctr <= (max_samples-1));  /* Wait for .5 mS timeout */
      }
   }

else
   {
   /* Trigger never reached, no control of valve */
   ioport1 &= 0xC7;           /* select ram hi, do directly insted  */
   ioport1 |= 0x38;           /* of calling the newdp() function    */
			      /* to save time */

   /* Clear out results */
   while (ctr <= max_samples)
      {
      VAL_TST_RESULTS[ctr] = 0;
      ctr++;
      }
   }

dac_command = temp_dac_command;
out_dac(dac_command);   /* Restore dac to original level */
ioport1 = old_dp;
}




/* Function get_pos_frame() will take a sample every 25 uS of the states
   of the position transducer lines and save into high ram. */

get_pos_frame()
{
unsigned char old_dp;
int ctr, y;
unsigned int temp_time, temp_ctr;
POS_FRAME_RESULTS[2048];

old_dp = ioport1;
ioport1 &= 0xC7;           /* select ram hi, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function    */
			   /* to save time.                      */

ctr = 0;
hso_command = 0x1a;      /* Software timer 2 */
hso_time = timer1 + 15;  /* 20uS */

/* First attempt to trigger off the rising edge of the lead signal.
   Wait for one tim1_ovfl period (87.3813mS), if no trigger, go ahead
   and get channel information */

temp_ctr = tim1_ovfl_ctr + 2;

if (ioport0 & 0x0001)
   {
   while((ioport0 & 0x0001) && (tim1_ovfl_ctr <= temp_ctr));
   }

while(!(ioport0 & 0x0001) && (tim1_ovfl_ctr <= temp_ctr));


do
   {
   for (y = 0; y <= 10; y++);

   POS_FRAME_RESULTS[ctr] = ioport0; /* & 0x21;  /* Mask */
   ctr++;
   }while (ctr < 2048);

ioport1 = old_dp;
}





#if 0
      /* Boost code, insert into closed loop section later if desired */

      /* Now boost command depending on deviation from commanded velocity. */
      /* Make these gain settings future parameters. */
      /* First find who's bigger */
      if (act_vel > commanded_vel)
	 pct = act_vel / (commanded_vel / 100);
      else if (commanded_vel > act_vel)
	 pct = commanded_vel / (act_vel / 100);
      else
	 pct = 100;  /* Must be equal */

      if (pct > 400) temp_dac_command *= 4;      /* 25% or less */
      else if (pct > 200) temp_dac_command *= 3;   /* 50% or less */
      else if (pct > 111) temp_dac_command *= 2; /* 90% or less */
      /* If within 10%, no additional gain added */
#endif


#if 0

	Open and closed loop exit code.  This code is inserted into
	the various exit locations.


	     temp_tim2 = timer2;  /* Freeze timer2 */

	     if (e_stop)
		{
		/* Now write to the holding ram for testing */
		if (hold_ctr < 2040)
		   {
		   ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		   ioport1 |= 0x38;           /* of calling the newdp() function.     */
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   TEMP_HOLDING[hold_ctr++] = loop_num;
		   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		   ioport1 |= 0x20;           /* of calling the newdp() function */
		   }
		goto ex;
		}

	     if ((ioport0 & 0x40) && (temp_tim2 >= p_check_z_speed))
		{
		if (temp_tim2 >= UNDER_FLOW_VAL);

		/* Check if a vacuum shot step, if so must abort entire
		   shot to prevent filling vacuum system if not a legit
		   go fast shot signal (the only acceptable way of terminating
		   a vacuum step). */
		else if (vac_on && while_off_flag)
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      TEMP_HOLDING[hold_ctr++] = loop_num+1;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   fatal_error = 1;
		   SET_FATAL_ERR;
		   goto ex;  /* Let's get out of here */
		   }

		else
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      TEMP_HOLDING[hold_ctr++] = loop_num+2;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   goto outputs;  /* True zspeed, end v_loop */
		   }
		}


	     if (tim1_ovfl_ctr == max_block_time)
		{
		/* Now write to the holding ram for testing */
		if (hold_ctr < 2040)
		   {
		   ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		   ioport1 |= 0x38;           /* of calling the newdp() function.     */
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   TEMP_HOLDING[hold_ctr++] = loop_num+3;
		   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		   ioport1 |= 0x20;           /* of calling the newdp() function */
		   }
		fatal_error = 2;
		SET_FATAL_ERR;
		goto ex;  /* Let's get out of here */
		}


	     if (!while_on_flag && !while_off_flag && !low_imp_input);
	     else
		{
		/* Termininating conditions of the v_loop that don't care about
		   the position setting.  Zspeed will terminate block. */

		/* If while conditional, check condition. */
		if (while_on_flag && !(*RD_DIGITAL_IO & input_number))
		   {
		   /* Now write to the holding ram for testing */
		   if (hold_ctr < 2040)
		      {
		      ioport1 &= 0xC7;           /* select ram high, do directly insted  */
		      ioport1 |= 0x38;           /* of calling the newdp() function.     */
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      TEMP_HOLDING[hold_ctr++] = loop_num+4;
		      ioport1 &= 0xC7;           /* select I/O, do directly insted  */
		      ioport1 |= 0x20;           /* of calling the newdp() function */
		      }
		   goto outputs;
		   }

		else if (while_off_flag && (*RD_DIGITAL_IO & input_number))
		   {
		   if (!vac_on);
		   else
		      {
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 TEMP_HOLDING[hold_ctr++] = loop_num+5;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto outputs;
		      }
		   }

		else if (low_imp_input && low_imp_dac)  /* Input based low impact */
		   {
		   /* Check if a vacuum shot step, if so must abort entire
		      shot to prevent filling vacuum system if not a legit
		      go fast shot signal (the only acceptable way of terminating
		      a vacuum step). */
		   if (vac_on && while_off_flag)
		      {
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 TEMP_HOLDING[hold_ctr++] = loop_num+6;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      fatal_error = 3;
		      SET_FATAL_ERR;
		      goto ex;  /* Let's get out of here */
		      }

		   else if (*RD_DIGITAL_IO & low_imp_input)
		      {
		      /* clear hso cam */
		      /*ioc2 = (temp_ioc2 | 0x80);*/
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 TEMP_HOLDING[hold_ctr++] = loop_num+7;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto end_of_vloop;
		      }
		   }
		}

	     if (position)
		{
		if (vac_on && while_off_flag);
		else if (temp_tim2 >= position)
		   {
		   if (temp_tim2 >= UNDER_FLOW_VAL);
		   /* If vac wait and fill_test, halt plunger movement. */
		   else if (while_off_flag && fill_test && !vac_on)
		      {
		      e_stop = TRUE;
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 TEMP_HOLDING[hold_ctr++] = loop_num+8;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto ex;
		      }

		   else
		      {
		      /* clear hso cam */
		      /*ioc2 = (temp_ioc2 | 0x80);*/
		      /* Now write to the holding ram for testing */
		      if (hold_ctr < 2040)
			 {
			 ioport1 &= 0xC7;           /* select ram high, do directly insted  */
			 ioport1 |= 0x38;           /* of calling the newdp() function.     */
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 TEMP_HOLDING[hold_ctr++] = loop_num+9;
			 ioport1 &= 0xC7;           /* select I/O, do directly insted  */
			 ioport1 |= 0x20;           /* of calling the newdp() function */
			 }
		      goto end_of_vloop;
		      }
		   }
		}


#endif
