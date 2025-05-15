/*************************** cont35.c ***************************/

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

cont31 and cont32 were test versions and never went into production.

7-5-97  -- cont33  Made alarm output a normally on output.  It will
go off when a fatal error occurs.

Added a new parameter, sensor_deviation.  Along with this, a new special
command was added to the valve_step block type, -10.  When this is executed,
after at home, the current position will be checked against the +/- of this
value.  If outside of the range, warning #1 will be generated.  This
indicates that the position sensor is potentially bad.

Added warning output, terminal #11.  Made the block timeout fatal error #2
warning #2 insted.  A warning will act like an e_stop.  The board will
continue to function without the warning having to be cleared.  The shot
will abort but will be ready for the next one.

10-30-97 -- cont34   Inverted all outputs to the digital output port.
The inverting 74ACT563 chips were obsoleted.  They will be replaced with
74ACT573s which are noninverting.  Included control4.h instead of control2.h.
Control4.h has the limit switch outputs inverted as well.

Included old_el.h for all servo loop terminations.
old_el.h will look for the absence of the pump on signal as a
servo loop termination condition.

Simplified goose command.  Goose percentage will simply be outputted.
The controller will no longer wait for the velocity to come within
10% of set velocity before beginning to control.

When decelerating closed loop with a decel position.  The gain boost would
kick in when the triggering velocity was reached.  This causes a very rough
looking decel.  The boost was removed for the decel only.  Accelerating
will still have the boost when within the necessary speed range.

Found that when decelerating closed loop to a low speed without any decel.
position specified, the loop time would go to 25mS as the loop time is
determined by commanded velocity.  The resulting decel was quite slow.
Code was added to elminate this.

Found problems at Prince in Feb. 1996.  Made changes to Visi-Trak's version.
When transisoning from one closed loop step to another, without any
acceleration, you would get two distinct acceleration curves.  It was found
that an hso servo time operation was pending from the last profile step.
When the new one was step executed, there would be two servo time hso
operations in effect.  This would have the net affect of doubling the gain,
and thus the two accel. curves.  This would also affect a decel. without any
decel position specified.  Code was added to test for an unfinished
hso operation from the last profile step.

Fixed problem with output_number initialization from a variable that was in
all the control block functions.  Since output numbers have not been
specified with variables in the past, this never showed up.  Just happened to
run across it while changing other things.



10-31-97 -- cont35  Major revision.

Created a new function, check_conditional() for conditional block execution
checking.  Went through all functions and changed them to make use of this.
Added conditional checking for velocity and position and analog channels.

Changed val_step() block to terminate if the overflow counter (10 secs) is
exceeded, if a while conditional is in effect.  Previously, a while
conditional would override the 10 sec block counter.

Elminated the pr_cntl block format.  This was never supported anyway.

Include extloop1.h for all servo loop terminations.

Added a fifth limit switch.

Made e_stop checking part of the tim1_overflow isr.  e_stop is also
checked continously during the velocity control termination loop.  This
frees up an hso_command location since e_stop no longer uses the hso
soft timer.

now using enable() and disable() to control global interupts.

Elminated the hso timer in the velocity control servo loop.  Now timing
directly off of timer1.

Added warning 3.  This sets if during closed loop constant speed
operation, the commanded velocity exceeds 15 volts.  This is a velocity
error overflow.

Removed the hso timing from the val_step block.  Timing is done directly
with timer1.  The 10 second block time out is not possible when timer1
is being used in this manner.  This should not be a problem.

Added a new block type, the UPDATE block.

Added a zspeed output.  zspeed output is output #9.  After passing the
zspeed check position, the servo exit loop will look for pressure (voltage)
below the setpoint AND velocity below the setpoint.  Press is on analog input
channel #2.  If triggering based only on velocity is desired, the pressure
setpoint can be set high.  Likewise if triggering only on pressure is desired
the velocity setpoint can be set high.

Added an new "special dac command" for the val_step() block, -11, which
set the parameters.  Since parameters can now be altered with the update()
block, need a way to set the parameters from control.prg

added 240 counts (3"/120mm) to the vacuum ending position and will abort
the shot if the position exceeds this length.  This is an additional vaccum
safty.  Previously, the shot would keep going if the OK for FS signal was
not sent.



Still need to do:

1, Also for Merc. Allow for a prop sequence.  Die opens about half way without
   follow through for some period of time.  Then die fully opens and follow
   through is initiated. Slow shot and accumulator outputs would have to be
   turned off when the die first opens and then probably only pump would be
   turned on for the completion of the follow through.

2, Must make sensor deviation work like cont34.

3, Make a way to select between several shot takeoff configurations.
   The control.prg file will have to be modified for this.

4, Look into BDW problem.  If transducer test is taking place during a
   shot, vacuum could fill with metal.

5, Create working copies of all relevent parameters in set_parameters()
   to avoid having to make conversions in vel_control().  By working
   directly with raw velocity counts and so on, the servo loop will be
   more efficient.

6, Add follow through stop code to the new control.prg program.
*/



#include <80C196.H>
#include <STDIO.H>
#include <CONTROL4.H>
#pragma interrupt(cmd_interp=29, soft_timer_isr=5, non_mask_int=31)
#pragma interrupt(tim1_overflow=0, hso_isr=3)
#define TRUE 1
#define FALSE 0
#define VER_NUM 35
#define VALVE_CENTER 0x2000 + null_offset + lvdt_offset
#define VALVE_NULL 0x2000 + null_offset

/* Definitions for conditional execution block execution. */
#define GOOD_TO_GO 1
#define CONDITION_NOT_MET 0
#define WHILE_ON 2
#define WHILE_OFF 3
#define WHILE_PHYSICAL 4



	/* Global register definitions */
	register int dac_command;
	register unsigned int rst, temp_d_io;
	register unsigned int temp_ana[5];
	register unsigned int status, block_ptr;
	unsigned int ls1, ls2, ls3, ls4, ls5;
	unsigned int tls1, tls2, tls3, tls4, tls5;
	unsigned char temp_int_mask, temp_imask1, temp_ioc2, temp_misc;
	unsigned char gn_ch1, gn_ch2, gn_ch3, gn_ch4;
	unsigned char int_flag, reg_tst_flg;
	unsigned char program_downloaded, zspeed_outputted;
	volatile register unsigned int tim1_ovfl_ctr;
	unsigned char control_active, dwell_time_flag, e_stop;
	unsigned int stack[20], fatal_error, warning, prev_warning;
	unsigned char stack_ptr, still_waiting, timer1_overflowed;
	int hold_ctr;


	/* Parameter definitions */
	unsigned int vel_loop_gain, bkgn1, bkgn2, bkgn3;
	unsigned long vel_dividend, bk_vel1, bk_vel2, bk_vel3;
	unsigned int dither_time, dither_ampl;
	unsigned int counts_per_unit, vel_multiplier;
	int null_offset, lvdt_offset;
	unsigned int js_setting, retr_setting, ft_setting;
	unsigned int sensor_deviation, pos_check_zspeed;
	unsigned int vel_check_zspeed;
	int press_check_zspeed;  /* voltage * 1000 */
	int ft_stop_pos;

	/* Working definitions of the various parameters that could
	   benefit from having working copies.  More to come later. */

	unsigned int w_vel_check_zspeed;  /* Raw velocity counts after conversion by set_parameters(). */
	int w_press_check_zspeed;  /* Raw ADC ch.2 word after conversion by set_parameters(). */



	main()
	{
	unsigned char newdp();
	void reset(), change_channel();
	void cmd_interp(), vel_loop(), non_mask_int(), tim1_overflow();
	void chk_lim_sws();
	int out_dac(), out_dac_w_check(), initialize_ls();
	void a_to_d_loop();

	/* now initialize variables / set default settings */

	ioport1 = 0xA0; /* select I/O page directly */
	*WR_STATUS = 0x8000;

	disable();
	temp_imask1 = 0x20;
	temp_ioc2 = 0x06;
	temp_int_mask = 0x29;
	wsr = 0;
	ipend1 = 0, int_pending = 0;
	int_mask = temp_int_mask, imask1 = temp_imask1;
	timer2 = 0;
	ioc0 = 0x08, ioc2 = temp_ioc2, sp_con = 0;
	ioc1 = 0x94;
	int_flag = FALSE, reg_tst_flg = FALSE;
	stack_ptr = 0; fatal_error = 0; warning = 0;

	if (rst != 0xAAAA) /* power up */
	   {
	   ALL_OUTPUTS_OFF;  /* Clear all of the Opto outputs. */

	   /* Initialize global parameters. */
	   gn_ch1 = 0, gn_ch2 = 0x01, gn_ch3 = 0x02, gn_ch4 = 0x03;
	   ls1 = 160, ls2 = 320, ls3 = 480, ls4 = 640, ls5 = 800;

	   /* Offsets are parameters but initialize now */
	   null_offset = 0, lvdt_offset = 0;

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
	ALL_OUTPUTS_OFF;  /* Clear all of the Opto outputs. */

	WRITE_STATUS;
	enable();

	if (program_downloaded) control_active = 1;
	else control_active = 0;


begin:  ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
	imask1 = (temp_imask1 |= 0x20); /* allow command interrupts */

	if (!program_downloaded) control_active = 0;
	newdp((unsigned char)IO_SEL);

	if (dac_command < 0) dac_command = 0;
	out_dac(dac_command = (VALVE_CENTER));  /* Center valve. */

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

	   /* Now make sure fault / warning outputs are enabled (turned off). */
	   ENABLE_WARNING;
	   ENABLE_FATAL;
	   }

	while (!control_active);  /* Wait until control active */

	SET_CONTROL_ACTIVE;
	CLEAR_ERRORS;
	WRITE_STATUS;

	/* Now disable the fault / warning outputs (trun on!!). */
	DISABLE_WARNING;
	DISABLE_FATAL;
	control_supervisor();
	goto begin;
	}




void soft_timer_isr()  /* Higher priority than hso_isr below. */
{
register unsigned int temp_ios1;
unsigned char old_dp, newdp();
int out_dac();


temp_ios1 = ios1;
temp_ios1 &= 0x0f;

old_dp = newdp((unsigned char)IO_SEL);

if (temp_ios1)
   {
   if (temp_ios1 & 0x01)          /* Software timer 0 */
      {
      }

   if (temp_ios1 & 0x02)          /* Software timer 1 */
      {
      }

   if (temp_ios1 & 0x04)          /* Software timer 2 */
      {
      }

   if (temp_ios1 & 0x08)          /* Software timer 3 */
      {
      }
   }

ioport1 = old_dp;
}




/***************************************************************************
Routine hso_isr() is an isr which uses the HSO pin interrupt vector.
The physical outputs on the HSO pins are not used for anything.
*/

void hso_isr()  /* Lower priority than software timer isr above */
{
register unsigned int temp_ios2;
unsigned char old_dp, newdp();

temp_ios2 = ios2;
temp_ios2 &= 0x3f;

old_dp = newdp((unsigned char)IO_SEL);

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
      }



   if (temp_ios2 & 0x10)          /* HSO 4, Limit switch #5 */
      {
      if (!(temp_d_io & 0x0200))
	 {
	 LS5_ON;
	 hso_command = 0x54;
	 hso_time = (tls5 = ls5 - 1);
	 }
      else
	 {
	 LS5_OFF;
	 hso_command = 0x54;
	 hso_time = (tls5 = ls5);
	 }
      }


   if (temp_ios2 & 0x20)          /* HSO 5, General purpose timer */
      {
      dwell_time_flag = 1;
      still_waiting = 0;  /* No longer waiting */
      }
   }

ioport1 = old_dp;
}





void a_to_d_loop()
/* IO page must be selected before calling this function */
{
unsigned int y;
extern unsigned char gn_ch1, gn_ch2, gn_ch3, gn_ch4;

ioport1 &= 0xBF;           /* trig conversion */
ioport1 |= 0x40;

*WR_A_TO_D = gn_ch2;
y = 5; y = 3; y = 2; y = 1; /* Delay */
temp_ana[1] = *RD_A_TO_D & 0x0fff;
if (temp_ana[1] & 0x0800) temp_ana[1] |= 0xf000; /* Extend signed value to 16 bits. */

ioport1 &= 0xBF;           /* trig conversion */
ioport1 |= 0x40;

*WR_A_TO_D = gn_ch3;
y = 5; y = 3; y = 2; y = 1;  /* Delay */
temp_ana[2] = *RD_A_TO_D & 0x0fff;
if (temp_ana[2] & 0x0800) temp_ana[2] |= 0xf000; /* Extend signed value to 16 bits. */

ioport1 &= 0xBF;           /* trig conversion */
ioport1 |= 0x40;

*WR_A_TO_D = gn_ch4;
y = 5; y = 3; y = 2; y = 1;  /* Delay */
temp_ana[3] = *RD_A_TO_D & 0x0fff;
if (temp_ana[3] & 0x0800) temp_ana[3] |= 0xf000; /* Extend signed value to 16 bits. */

ioport1 &= 0xBF;           /* trig conversion */
ioport1 |= 0x40;

*WR_A_TO_D = gn_ch1;
y = 5; y = 3; y = 2; y = 1;  /* Delay */
temp_ana[4] = *RD_A_TO_D & 0x0fff;
if (temp_ana[4] & 0x0800) temp_ana[4] |= 0xf000; /* Extend signed value to 16 bits. */
}




       void cmd_interp()
       {
       unsigned int command, temp_command, b, temp_data;
       unsigned char newdp(), old_page, no_int;
       unsigned int get_count();
       void ram_test(), test_dio();
       void change_channel();
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
		      DISABLE_FATAL;   /* Disable fault (turn on) output */
		      DISABLE_WARNING; /* Disable warning (turn on) output */
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
		 case 0x801a:  /* Set ls5 */
		      ls5 = *AT_DATA;
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
		 case 0x801f:  /* Read back ls5 */
		      *DATA_OUT = ls5;
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

			 enable();
			 valve_test();
			 disable();
			 /*imask1 = temp_imask1;*/
			 }
		      no_int = TRUE;
		      break;
		 case 0x8026:  /* Write directly to opto */
		      temp_data = *AT_DATA & 0x00f0; /* bits 4 - 7 only */
		      temp_d_io &= 0xff0f;
		      temp_d_io |= temp_data;
		      *WR_DIGITAL_IO = ~(temp_d_io);
		      no_int = TRUE;
		      break;
		 case 0x8027:  /* Set individual Opto bits */
		      temp_data = *AT_DATA & 0x00ff; /* Only allowable bits */
		      temp_d_io |= temp_data;
		      *WR_DIGITAL_IO = ~(temp_d_io);
		      no_int = TRUE;
		      break;
		 case 0x8028:  /* Clear individual Opto bits */
		      temp_data = *AT_DATA & 0x00ff; /* Only allowable bits */
		      temp_d_io &= ~temp_data;
		      *WR_DIGITAL_IO = ~(temp_d_io);
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

		      enable();
		      get_pos_frame();
		      disable();
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
		      *DATA_OUT = prev_warning;
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
       /* e_stop checking is now part of this isr.  The
	  e_stop will be checked approx. every 87mS and continuously
	  during the velocity control termination loop. */
       unsigned char old_dp;

       tim1_ovfl_ctr++;

       old_dp = ioport1;
       ioport1 &= 0xC7;           /* select IO, do directly insted      */
       ioport1 |= 0x20;           /* of calling the newdp() function    */
				  /* to save time.                      */

       if ((*RD_DIGITAL_IO & 0x8000) && control_active && !e_stop)  /* E stop input present */
	 e_stop = TRUE;

       timer1_overflowed = 1;
       ioport1 = old_dp;
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

       *WR_DIGITAL_IO = ~(0);
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0)
	  err = TRUE;

       *WR_DIGITAL_IO = ~(0x5555);
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0x5555)
	  err = TRUE;

       *WR_DIGITAL_IO = ~(0xAAAA);
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0xAAAA)
	  err = TRUE;

       *WR_DIGITAL_IO = ~(0x6666);
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0x6666)
	  err = TRUE;

       *WR_DIGITAL_IO = ~(0x9999);
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0x9999)
	  err = TRUE;

       *WR_DIGITAL_IO = ~(0xFFFF);
       for (c = 0; c <= 0x2700; c++);
       if (*RD_DIGITAL_IO != 0xFFFF)
	  err = TRUE;

       *WR_DIGITAL_IO = ~(0);
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

       disable();
       ALL_LS_OFF;

       hso_command = 0x50, hso_time = (tls1 = ls1);
       hso_command = 0x51, hso_time = (tls2 = ls2);
       hso_command = 0x52, hso_time = (tls3 = ls3);
       hso_command = 0x53, hso_time = (tls4 = ls4);
       hso_command = 0x54, hso_time = (tls5 = ls5);
       enable();

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
   4  ==  Update

*/

old_dp = ioport1;
block_ptr = 0xf000;

ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function      */
			   /* to save time.                        */
e_stop = FALSE;
fatal_error = FALSE;
warning = FALSE;


set_parameters();  /* Set closed loop velocity parameters. */

while ((*(unsigned int * )block_ptr != 999) && control_active)
    {
    if (e_stop || stack_ptr > 20)  /* Reset control program if e_stop is detected. */
       {
       disable();
       ioport1 &= 0xC7;           /* select IO, do directly insted      */
       ioport1 |= 0x20;           /* of calling the newdp() function    */
				  /* to save time.                      */

       ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
       e_stop = FALSE;
       block_ptr = 0xf000;
       stack_ptr = 0;
       out_dac(dac_command = (VALVE_CENTER));
       *WR_DIGITAL_IO = ~(temp_d_io &= 0xfe00);  /* Don't turn off limit ls's, fault or warning. */
       initialize_ls();

       ioport1 &= 0xC7;           /* select ram high, do directly insted  */
       ioport1 |= 0x38;           /* of calling the newdp() function      */
				  /* to save time.                        */

       enable();
       continue;
       }

    if (fatal_error)
       {
       ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
       ioport1 &= 0xC7;           /* select IO, do directly insted      */
       ioport1 |= 0x20;           /* of calling the newdp() function    */
				  /* to save time.                      */

       /* Now turn off all outputs except the limit switches. */
       ALL_OFF_EXCEPT_LIMIT_SWITCHES;
       ENABLE_FATAL; /* Enable (turn off) the fatal error output. */
       out_dac(dac_command = (VALVE_CENTER));
       block_ptr = 0xf000;
       stack_ptr = 0;
       break;
       /* Status bits will be updated in main loop after begin: */
       }


    if (warning)
       {
       /* Code here to handle warning detection. */
       disable();
       ioport1 &= 0xC7;           /* select IO, do directly insted      */
       ioport1 |= 0x20;           /* of calling the newdp() function    */
				  /* to save time.                      */

       ioc2 = (temp_ioc2 | 0x80);  /* clear hso cam */
       block_ptr = 0xf000;
       stack_ptr = 0;
       out_dac(dac_command = (VALVE_CENTER));

       /* Now turn off all outputs. Leave the limit switches alone. */
       ALL_OFF_EXCEPT_LIMIT_SWITCHES;
       ENABLE_WARNING;  /* Enable (turn off) the warning output. */

       prev_warning = warning;  /* Archive warning */

       initialize_ls();


       ioport1 &= 0xC7;           /* select ram high, do directly insted  */
       ioport1 |= 0x38;           /* of calling the newdp() function      */
				  /* to save time. */

       warning = 0;  /* Clear warning, action has now been taken. */
       enable();
       continue;
       }


    if (!program_downloaded) break;

    if (*(unsigned int * )block_ptr == 3) {vel_control(); continue;}
    if (*(unsigned int * )block_ptr == 0) {val_step(); continue;}
    if (*(unsigned int * )block_ptr == 1) {jump_block(); continue;}
    if (*(unsigned int * )block_ptr == 2) {setpos_block(); continue;}
    if (*(unsigned int * )block_ptr == 4) {update_block(); continue;}
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

   *block_ptr + 12 to + 18 reserved for future use.

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
      |      | |   1010 - 1111 Undefined
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


   SPECIAL DAC COMMAND SETTINGS

   1,   -1  == Leave dac output unchanged.
   2,   -2  == Skip entire block.
   3,   -3  == Null valve -- zero output + null_offset.
   4,   -4  == Center valve -- zero output + null_offset + lvdt_offset.
   5,   -5  == Jog shot setting (parameter based).
   6,   -6  == Retract valve setting (parameter based).
   7,   -7  == Follow through valve setting (parameter based).
   8,   -8  == Set shot in progress status bit.
   9,   -9  == Clear shot in progress status bit.
   10,  -10 == Check position sensor for deviation after at home.
   11,  -11 == Update parameters

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

int final_dac_command, while_flag, set_parameters();
unsigned int position, l_dwell_time, s_dwell_time;
unsigned int input_indicator, input_number;
unsigned int output_indicator, output_number, temp_time, variable_indicator;
unsigned char old_dp;
unsigned char while_on_flag, while_off_flag, while_physical_flag;
unsigned int analog_val, ana_check_val;
unsigned int max_block_time;
int out_dac_w_check(), temp_tim2, check_conditional(), accel;


old_dp = ioport1;
ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function */
			   /* to save time.                   */

/* Initialize variables */
while_flag = 0, while_on_flag = 0, while_off_flag = 0;
while_physical_flag = 0;

max_block_time = tim1_ovfl_ctr + 115;   /* 10 seconds max block time */

variable_indicator = *((unsigned int * )(block_ptr + 20));

if (variable_indicator & 0x0002)
   final_dac_command = *((unsigned int * )(*((unsigned int * )(block_ptr + 2))));
else  final_dac_command = *((unsigned int * )(block_ptr + 2));

/* Now check if final_dac_command == -2.  This is a quick way to skip the
   entire block if necessary. */
if (final_dac_command == -2) goto ex;

if (variable_indicator & 0x0004)
   accel = *((int * )(*((unsigned int * )(block_ptr + 4))));
else  accel = *((int * )(block_ptr + 4));

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
   output_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));



ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */


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


/***** IF A POSITION IS SET, WAIT UNTIL REACHED BEFORE CONTINUING ******/

if ((int)position > 0)
   {
   do  /* wait for position */
      {
      temp_tim2 = timer2;  /* Freeze timer2 */

      if (e_stop) goto ex;

      if (tim1_ovfl_ctr == max_block_time)  goto outputs;

      if(while_flag)  /* While conditional present */
	 {
	 if (while_off_flag && (*RD_DIGITAL_IO & input_number))
	    goto end_of_dac_control;
	 else if (while_on_flag && (!(*RD_DIGITAL_IO & input_number)))
	    goto end_of_dac_control;
	 else if (while_physical_flag)
	    {
	    if (!(check_conditional(input_indicator, input_number)))
	       goto end_of_dac_control;
	    }
	 }

      }while (temp_tim2 < (int)position);  /* wait for position */
   }


/********** IF LONG DWELL, SHORT DWELL OR BOTH ARE NEEDED, DO NOW **********/

/* For delays > 87.3813 mS to 5726 seconds (1.59 hours). */
if (l_dwell_time == 0);
else
   {
   temp_time = tim1_ovfl_ctr + l_dwell_time;

   do
      {
      if (e_stop)  goto ex;

      if(while_flag)  /* While conditional present */
	 {
	 if (while_off_flag && (*RD_DIGITAL_IO & input_number))
	    goto end_of_dac_control;
	 else if (while_on_flag && (!(*RD_DIGITAL_IO & input_number)))
	    goto end_of_dac_control;
	 else if (while_physical_flag)
	    {
	    if (!(check_conditional(input_indicator, input_number)))
	       goto end_of_dac_control;
	    }
	 }
      }while (tim1_ovfl_ctr != temp_time);
   }

/* For delays < 87.3813 mS to 1.3333 uS */
if (s_dwell_time == 0);
else
   {
   disable();
   dwell_time_flag = 0;

   hso_command = 0x15;
   hso_time = timer1 + s_dwell_time; /*  timer1 + s_dwell_time;*/

   enable();

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

/***************************************************************************/

/* Now check for special dac commands (less than zero). */
if (final_dac_command < 0)
   {
   switch (final_dac_command)
      {
      case -1:  /* Leave dac output unchanged */
	 break;

      case -2:  /* Skip entire block */
	 goto ex;
	 break;

      case -3:  /* Null valve */
	 /* Since the out_dac_w_check will add both the null and lvdt offsets,
	    the out_dac() function must be used. */
	 out_dac(dac_command = (VALVE_NULL));
	 break;

      case -4:  /* Center valve */
	 out_dac_w_check(dac_command = 0x2000);
	 break;

      case -5:  /* Jog shot valve setting */
	 out_dac_w_check(dac_command = js_setting);
	 break;

      case -6:  /* Retract valve setting */
	 out_dac_w_check(dac_command = retr_setting);
	 break;

      case -7:  /* Follow through valve setting */
	 out_dac_w_check(dac_command = ft_setting);
	 break;

      case -8:  /* Set SHOT_IN_PROGRESS status bit */
	 SET_SHOT_IN_PROGRESS;
	 WRITE_STATUS;
	 break;

      case -9:  /* Clear SHOT_IN_PROGRESS status bit */
	 CLR_SHOT_IN_PROGRESS;
	 WRITE_STATUS;
	 break;

      case -10:  /* Check sensor deviation */
	 temp_tim2 = timer2; /* Freeze timer2 */

	 if (temp_tim2 == 0);  /* Cant do better than this */
	 else if (temp_tim2 > 0)
	    {
	    if (temp_tim2 >= sensor_deviation)
	       {
	       warning = 1; /* Transducer warning */
	       SET_WARNING;
	       goto ex;  /* Let's get out of here */
	       }
	    }
	 else if (temp_tim2 < 0)
	    {
	    if (temp_tim2 <= (0-sensor_deviation))
	       {
	       warning = 1; /* Transducer warning */
	       SET_WARNING;
	       goto ex;  /* Let's get out of here */
	       }
	    }
	 break;

      case -11:  /* Update parameters. Since UPDATE block can modify parameters. */
	 set_parameters();
	 break;
      }
   }

/***** NOW OUTPUT TO DAC, EITHER ALL OUT OR CONTROLLED ACCELERATION *****/

else if (accel == 0)  /* All out acceleration */
   out_dac_w_check(dac_command = final_dac_command);

else   /* Stepped output is required */
   {
   if (final_dac_command < dac_command) accel = 0 - accel; /* Decrement */

   /* If accel increment > current step, write command and move on */
   if ((accel > 0) && (accel > (final_dac_command - dac_command)))
      out_dac_w_check(dac_command = final_dac_command);

   else if ((accel < 0) && (accel < (final_dac_command - dac_command)))
      out_dac_w_check(dac_command = final_dac_command);

   else
      {
      out_dac_w_check(dac_command += accel);
      while (((accel>0) && (dac_command<final_dac_command)) ||
		  ((accel<0) && (dac_command>final_dac_command)))
	  {
	  disable();
	  dwell_time_flag = 0;

	  hso_command = 0x15;
	  hso_time = timer1 + 3750;
	  enable();

	  do     /* Wait for time out */
	      {
	      if (e_stop)  /* E stop input present */
		 goto ex;

	      if (tim1_ovfl_ctr == max_block_time)  break;

	      if (while_flag)  /* While conditional present */
		 {
		 if (while_off_flag && (*RD_DIGITAL_IO & input_number))
		    goto end_of_dac_control;
		 else if (while_on_flag && (!(*RD_DIGITAL_IO & input_number)))
		    goto end_of_dac_control;
		 else if (while_physical_flag)
		    {
		    if (!(check_conditional(input_indicator, input_number)))
		    goto end_of_dac_control;
		    }
		 }
	      }while (!dwell_time_flag);  /* Wait for time out */

	  out_dac_w_check(dac_command += accel);
	  }
      out_dac_w_check(dac_command = final_dac_command);  /* Write final value to DAC */
      }
   }

end_of_dac_control: ;


/*********** IF WHILE CONDITION STILL TRUE, WAIT UNTIL FALSE *************/

if (e_stop) goto ex;

else
   {
   if (while_flag)
      {
      do
	 {
	 if (e_stop) goto ex;

	 if (tim1_ovfl_ctr == max_block_time)  break;

	 if (while_off_flag && (*RD_DIGITAL_IO & input_number))
	    break;

	 else if (while_on_flag && (!(*RD_DIGITAL_IO & input_number)))
	    break;

	 else if (while_physical_flag)
	    {
	    if (!(check_conditional(input_indicator, input_number)))
	       break;
	    }

	 }while (1);  /* Loop until forced out */
      }


/**************** CHECK FOR DURING OR AT END OUTPUTS NOW ****************/
outputs: ;

   if (output_indicator == 999);

   /* If during output is on, turn it off now */
   else if (output_indicator == 3)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

   /* If At End  OFF output is required, do it now */
   else if (output_indicator == 4)  *WR_DIGITAL_IO = ~(temp_d_io &= ~output_number);

   /* If At End  ON output is required, do it now */
   else if (output_indicator == 5)  *WR_DIGITAL_IO = ~(temp_d_io |= output_number);
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
      |      | |   1010 - 1111 Undefined
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
int out_dac_w_check(), low_imp_dac, pct, dac_increment;
int while_flag, check_conditional();
unsigned int input_indicator, input_number, position;
unsigned int output_indicator, output_number, variable_indicator;
unsigned int dither_time, dither_ampl, low_imp_input, servo_time, prev_servo_time;
unsigned char old_dp, while_on_flag, while_off_flag, while_physical_flag;
unsigned char vac_on, slow, fill_test, overflow, timeout, first_time_through;
unsigned char last_step;
unsigned int temp, analog_val, ana_check_val;
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
ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function      */
			   /* to save time.                        */


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

if (variable_indicator & 0x0200)
   ana_check_val = *((unsigned int * )(*((unsigned int * )(block_ptr + 18))));
else  ana_check_val = *((unsigned int * )(block_ptr + 18));

input_indicator = *((unsigned int * )(block_ptr + 22));

if (variable_indicator & 0x1000)
   input_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 24))));
else  input_number = *((unsigned int * )(block_ptr + 24));

output_indicator = *((unsigned int * )(block_ptr + 26));

if (variable_indicator & 0x4000)
   output_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
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

		out_dac_w_check(dac_command);

		do
			{
			/*1111111111111111111111111111111111111111111111111111111111111*/

			#include <EXTLOOP1.H>

			}while (timer2 < newt2);  /* Wait for next sample */
		} /* for loop */
	  }  /* Closed loop accel */

   else if (goose)  /* Legitiment goose command */
      {
      out_dac_w_check(dac_command = goose);
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

      out_dac_w_check(dac_command);

      /* Make waiting loop a do loop to ensure at least one pass through */
      do /* Wait for next sample */
	     {
	     /*222222222222222222222222222222222222222222222222222222*/

	     #include <EXTLOOP1.H>

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
			out_dac_w_check(dac_command);

	       do
				{
				/*333333333333333333333333333333333333333333333333333333333333*/

				#include <EXTLOOP1.H>

				}while (timer2 < newt2);  /* Wait for next sample */
			}
		}  /* End of controlled accel loop. */
      }

   /* ol_setting is the raw dac command */
   out_dac_w_check(dac_command = ol_setting);
   loop_num = 500;  /* For diagnostics */

   do  /* Wait for something to terminate the block */
	     {
	     /*4444444444444444444444444444444444444444444444444444444444444*/

	     #include <EXTLOOP1.H>

	     }while (1);  /* Loop until something terminates the loop */
   }

end_of_vloop: ;


/************** OUTPUT LOW IMPACT COMMAND HERE IF NEEDED *****************/

if (low_imp_dac)  /* If low impact required */
   out_dac_w_check(dac_command = low_imp_dac);



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
		   goto outputs;
		   }

		/* Check if pressure and vel. are below the threshold for setting the zspeed output. */
		else if (!zspeed_outputted)
		   {
		   /* First get the pressure value from ana. ch.#2 */
		   *WR_A_TO_D = gn_ch2;
		   ioport1 &= 0xBF;           /* trig conversion */
		   ioport1 |= 0x40;

		   /* Now delay */
		   *WR_A_TO_D = gn_ch1;
		   *WR_A_TO_D = gn_ch1;
		   *WR_A_TO_D = gn_ch1;
		   *WR_A_TO_D = gn_ch1;

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
		      }
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
      |      | |   1010 - 1111 Undefined
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
unsigned int temp, analog_val, ana_check_val;
unsigned char old_dp;
int push(), check_conditional();

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
   output_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */


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
      |      | |   1010 - 1111 Undefined
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
unsigned int output_indicator, output_number, variable_indicator;
unsigned char old_dp;
int x, check_conditional();

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
   output_number = *((unsigned int * )(*((unsigned int * )(block_ptr + 28))));
else  output_number = *((unsigned int * )(block_ptr + 28));


ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */


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
TURN_OFF_ZSPEED;
enable();

hold_ctr = 0;  /* Pointer for test purposes, saving cl info during shot. */

/* Now clear the TEMP_HOLDING hi ram used for saving cl info. */
ioport1 &= 0xC7;           /* select ram hi, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function */

for (x = 0xC000; x <= 0xCFFE; x++)
    *((unsigned char * ) x) = 0;



/********** NOW SEE IF ANY OUTPUTS NEED TO BE SET / CLEARED **********/

ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */

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
/* Program execution from high ram, block F000 to FEFE. */
/* Framework for the update block

   *block_ptr + 2  == Item to update
		      0 to 127 Variable numbers
		      128 == DAC output voltage * 1000, ch.1
		      129 == Reserved for DAC output voltage * 1000, ch.2
		      130 == Reserved for DAC output voltage * 1000, ch.3
		      131 == Reserved for DAC output voltage * 1000, ch.4
		      132 == Position in direct timer2 counts
		      133 == Warning
		      134 == Fatal error

   *block_ptr + 4  == 1st. operand
		      0 to 127 Variable numbers
		      128 == DAC output voltage * 1000, ch.1
		      129 == Reserved for DAC output voltage * 1000, ch.2
		      130 == Reserved for DAC output voltage * 1000, ch.3
		      131 == Reserved for DAC output voltage * 1000, ch.4
		      132 == Position in direct timer2 counts
		      133 == Velocity in direct eng. units * 100
		      134 == ADC input voltage * 1000, ch1
		      135 == ADC input voltage * 1000, ch2
		      136 == ADC input voltage * 1000, ch3
		      137 == ADC input voltage * 1000, ch4
		      200 == Constant

   *block_ptr + 6  == Operator
		      0 == +
		      1 == -
		      2 == *
		      3 == Reserved for /
		      4 == Reserved for &
		      5 == Reserved for |
		      6 == Reserved for ^ (XOR)
		      999 == No operator  Therefore cannot have a second operator.

   *block_ptr + 8  == 2nd. operand
		      0 to 127 Variable numbers
		      128 == DAC output voltage * 1000, ch.1
		      129 == Reserved for DAC output voltage * 1000, ch.2
		      130 == Reserved for DAC output voltage * 1000, ch.3
		      131 == Reserved for DAC output voltage * 1000, ch.4
		      132 == Position
		      133 == Velocity in direct eng. units * 100
		      134 == ADC input voltage * 1000, ch1
		      135 == ADC input voltage * 1000, ch2
		      136 == ADC input voltage * 1000, ch3
		      137 == ADC input voltage * 1000, ch4
		      200 == Constant
		      999 == no second operand

   *block_ptr + 10  == 1st operand constant if specified
   *block_ptr + 12  == 2nd operand constant if specified

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
      |      | |   1010 - 1111 Undefined
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

int out_dac(), check_conditional();
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

ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */
temp_vel_count = *VELOCITY;



ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function      */
			   /* to save time.                        */

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
else if (first_op == 132) first_op = timer2;  /* Position */
else if (first_op == 133)  /* Velocity */
   {
   /* Convert first_op to velocity in direct units X 100 */
   first_op = (int)((vel_dividend / temp_vel_count) / vel_multiplier);
   }
else if (first_op == 134 || first_op == 135 ||
		first_op == 136 || first_op == 137) /* Analog input voltage */
   {
   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
   ioport1 |= 0x20;           /* of calling the newdp() function */
			      /* to save time.                   */

   a_to_d_loop();

   if (first_op == 134)
      tempi = (int)temp_ana[1];
   else if (first_op == 135)
      tempi = (int)temp_ana[2];
   else if (first_op == 136)
      tempi = (int)temp_ana[3];
   else if (first_op == 137)
      tempi = (int)temp_ana[4];

   /* Convert first_op to voltage * 1000. */
   first_op = (int)((((long)tempi*1000)*5)/2047);


   ioport1 &= 0xC7;           /* select ram high, do directly insted  */
   ioport1 |= 0x38;           /* of calling the newdp() function      */
			      /* to save time.                        */
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
else if (second_op == 132) second_op = timer2;  /* Position */
else if (second_op == 133)  /* Velocity */
   {
   /* Convert second_op to velocity in direct units X 100 */
   second_op = (int)((vel_dividend / temp_vel_count) / vel_multiplier);
   }
else if (second_op == 134 || second_op == 135 ||
		second_op == 136 || second_op == 137) /* Analog input voltage */
   {
   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
   ioport1 |= 0x20;           /* of calling the newdp() function */
			      /* to save time.                   */

   a_to_d_loop();

   if (second_op == 134)
      tempi = (int)temp_ana[1];
   else if (second_op == 135)
      tempi = (int)temp_ana[2];
   else if (second_op == 136)
      tempi = (int)temp_ana[3];
   else if (second_op == 137)
      tempi = (int)temp_ana[4];

   /* Convert second_op to voltage * 1000. */
   second_op = (int)((((long)tempi*1000)*5)/2047);

   ioport1 &= 0xC7;           /* select ram high, do directly insted  */
   ioport1 |= 0x38;           /* of calling the newdp() function      */
			      /* to save time.                        */
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


ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */


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

ioport1 &= 0xC7;           /* select ram high, do directly insted  */
ioport1 |= 0x38;           /* of calling the newdp() function      */
			   /* to save time.                        */

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
   out_dac(dac_command = (unsigned int)result);
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

ioport1 &= 0xC7;           /* select I/O, do directly insted  */
ioport1 |= 0x20;           /* of calling the newdp() function */
			   /* to save time.                   */

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

   ioport1 &= 0xC7;           /* select I/O, do directly insted  */
   ioport1 |= 0x20;           /* of calling the newdp() function */

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
	    a_to_d_loop();

	    if (temp_input_indicator == 0x0002)
	       tempi = (int)temp_ana[1];
	    if (temp_input_indicator == 0x0003)
	       tempi = (int)temp_ana[2];
	    if (temp_input_indicator == 0x0004)
	       tempi = (int)temp_ana[3];
	    if (temp_input_indicator == 0x0005)
	       tempi = (int)temp_ana[4];

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
	    ioport1 &= 0xC7;       /* select ram hi, do directly insted */
	    ioport1 |= 0x38;       /* of calling the newdp() function   */

	    temp_input_indicator = (temp_input_indicator * 2) + 0xff00; /* Convert to variable's address. */
	    physical_operand = *((int *)temp_input_indicator);
	    break;

	 case 0x0009:  /* 2nd. page variable, V64 - V127 */
	    temp_input_indicator = (input_indicator >> 10) + 64;

	    /* temp_input_indicator is now the variable number. */
	    /* temp_input_indicator is now the variable number. */
	    ioport1 &= 0xC7;       /* select ram hi, do directly insted */
	    ioport1 |= 0x38;       /* of calling the newdp() function   */

	    temp_input_indicator = (temp_input_indicator * 2) + 0xff00; /* Convert to variable's address. */
	    physical_operand = *((int *)temp_input_indicator);
	    break;

	 default:       /* 0x000a - 0x000f undefined */
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
pos_check_zspeed = *(unsigned int * )POS_CHECK_ZSPEED;
counts_per_unit = *(unsigned int * )COUNTS_PER_UNIT;
null_offset = *(unsigned int * )NULL_OFFSET;
lvdt_offset = *(unsigned int * )LVDT_OFFSET;
js_setting = *(unsigned int * )JS_SETTING;
retr_setting = *(unsigned int * )RETR_SETTING;
ft_setting = *(unsigned int * )FT_SETTING;
vel_multiplier = *(unsigned int * )VEL_MULTIPLIER;
sensor_deviation = *(unsigned int * )SENSOR_DEVIATION;
press_check_zspeed = *(unsigned int * )PRESS_CHECK_ZSPEED;
vel_check_zspeed = *(unsigned int * )VEL_CHECK_ZSPEED;
ft_stop_pos = *(unsigned int * )FT_STOP_POS;

/* Update the break velocities since the vel_multiplier will affect these. */
bk_vel1 = (bk_vel1 * (unsigned long)vel_multiplier) / 100;
bk_vel2 = (bk_vel2 * (unsigned long)vel_multiplier) / 100;
bk_vel3 = (bk_vel3 * (unsigned long)vel_multiplier) / 100;


/* Now establish working parameters. */

w_vel_check_zspeed = (unsigned int)(vel_dividend / ((unsigned long)vel_check_zspeed * (unsigned long)vel_multiplier)); /* Raw velocity counts. */
w_press_check_zspeed = (int)(((long)press_check_zspeed * 2047) / 5000); /* Raw ADC counts. */

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
	 disable();
	 hso_command = 0x15;
	 hso_time = time_interval;
	 enable();
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
/*hso_command = 0x1a;      /* Software timer 2 */
/*hso_time = timer1 + 15;  /* 20uS */

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








