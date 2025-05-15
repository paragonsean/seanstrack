--------- POSITION_VELOCITY_INTERFACE_UNIT.VHD SYNC VERSION W/ CAMS-----------------
----------------- Copyright 2001, 2002 Visi-Trak Worldwide -------------------
-- 9-7-01, 9-28-01, 2-15-02, 10-7-02
--
-- Combination position / velocity unit w/ altcams


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-- The position_velocity_interface_unit consists of a 24 bit position unit
-- and a 32 bit velocity unit.  Each unit generates an interrupt signal.
-- The position unit allows the saving 2 position setpoints
-- which are then compared to the position counter.  If a match occurs, an
-- interrupt signal is generated.  A ISTATUS register can be read which 
-- contains the non-masked CAM bit(s) that generated the interrupt.  An
-- IPEND register can also be read that contains all bits, masked and un-masked.
-- Reading the ISTATUS register will clear all IPEND and ISTATUS bit and
-- will also disable position interrupts until reenabled.
--
-- The velocity / sp unit allows an interrupt to be generated each speed
-- pulse.  
--
-- The sp value is latched for each of 4 quadrature quadrants.  Therefore
-- a position based data collection that generates an interrupt every
-- X1 pulse can read all 4 latched quadrature sp values and do in
-- effect an X4 data collection with little additional overhead.
--
-- Bit 31 of speed pulse latched data is the up_down signal.  Bit 30 is
-- the quad_err signal.  That leaves 30 bits (29 downto 0) for velocity
-- timing data.
--
-- 

-- 10-18-02 -- Reenabled a hardware velocity calculation on the 
-- Q1 speed pulse only.  This did not increase LE usage tremendously
-- so it represents a good compromise.

-- 11-19-02 -- Added the ability to preload the position counter.  Also added
-- another input, preload.  If preload is true, only the position counter will
-- be updated/reset.  The velocity items will not be affected.  Reset will also
-- update the position counter.  The position preload is sent as part of the
-- velocity divisor word.  3 downto 0 is the velocity divisor.  27 downto 4 will
-- be the position preload data.  31 downto 28 are still available.  

-- 12-10-02 -- Changed the way the q1 - q4 signals generate an
-- interrupt.  These 4 signals are now embedded with the sp
-- cam output bits.
--
-- 12-17-02 -- Major revision.  Elminated the cams.  Gave up on saving
-- timer setpoints in cam.  Too many problems rewriting cam on the fly.
-- Also elminated the position cam.  Two direct position comaprisons are 
-- allowed.  One for position based monitoring and one for the limit 
-- switches.  The limit switch setup will be somewhat of a compromise.
-- A cross between software and a purely hardware based comparison of
-- multiple position set points.

entity position_velocity_interface_unit is
	port(
		clk			: in	std_logic; 
		rst			: in	std_logic;
		preload     : in	std_logic;
		up_dn		: in	std_logic;
		x4			: in	std_logic;
		speed_pulse_q1	: in	std_logic;
		speed_pulse_q2	: in	std_logic;
		speed_pulse_q3	: in	std_logic;
		speed_pulse_q4  : in	std_logic;
		cs			: in	std_logic;
		addr		: in	std_logic_vector(3 downto 0);
		readn		: in	std_logic;
		writen		: in	std_logic;
		quad_err	: in	std_logic;
		data_input  : in std_logic_vector(31 downto 0);
		data_output : out std_logic_vector(31 downto 0);
		-- For simulation
		--temp_pos_out : out std_logic_vector(23 downto 0);  
		--temp_raw_v_count_out : out std_logic_vector(31 downto 0);
		--temp_latched_v_count_q1_out : out std_logic_vector(31 downto 0);
		--temp_latched_v_count_q2_out : out std_logic_vector(31 downto 0);
		--temp_latched_v_count_q3_out : out std_logic_vector(31 downto 0);
		--temp_latched_v_count_q4_out : out std_logic_vector(31 downto 0);
		--temp_velocity_out : out std_logic_vector(31 downto 0);
		--enable_velocity_ctr_out : out std_logic;
		--velocity_clk_divisor_out : out std_logic_vector(3 downto 0);
		--control_word_out : out std_logic_vector(31 downto 0);
		--sp_istatus_out : out std_logic_vector(9 downto 0);
		--pos_istatus_out : out std_logic_vector(7 downto 0);
		--
		sp_interrupt 	: out std_logic;
		pos_interrupt	    : out std_logic);
end position_velocity_interface_unit;


architecture behave OF position_velocity_interface_unit is

-- COMMON STUFF
signal WR_ENBL, RD_ENBL, FORCE_HIGH, FORCE_LOW : std_logic;
signal REQ_TO_DISABLE_INT : std_logic;
signal CONTROL_WORD : std_logic_vector(31 downto 0);
signal POSITION_PRELOAD : std_logic_vector(23 downto 0);
signal ADDR_1110_INPUT_DATA : std_logic_vector(31 downto 0); -- Velocity divisor/position preload

-- VELOCITY / SP SPECIFIC STUFF
signal SP_INT_ENBL : std_logic;
signal VELOCITY_COUNT : std_logic_vector(31 downto 0);
signal SP_INT_BITS, SP_IMASK, SP_ISTATUS : std_logic_vector(3 downto 0);
signal VELOCITY_CNT_ENBL : std_logic;  
signal SP_ISTATUS_RD_ENBL : std_logic;
signal REQ_TO_DISABLE_SP_INT : std_logic;
signal LATCHED_V_COUNT_Q1 : std_logic_vector(31 downto 0);
signal LATCHED_V_COUNT_Q2 : std_logic_vector(31 downto 0);
signal LATCHED_V_COUNT_Q3 : std_logic_vector(31 downto 0);
signal LATCHED_V_COUNT_Q4 : std_logic_vector(31 downto 0);
signal VEL_CLK_DIVISOR : std_logic_vector(3 downto 0);
signal VEL_CTR_CLK_ENBL, VEL_ENBL_CTR_CLK_ENBL, ENABLE_VELOCITY_CTR : std_logic;
signal REQ_FOR_X4_SP_INTERRUPT_Q4, REQ_TO_CLEAR_SP_INTERRUPTS : std_logic;
signal REQ_FOR_X4_SP_INTERRUPT_Q1, REQ_FOR_X4_SP_INTERRUPT_Q2 : std_logic; 
signal REQ_FOR_X4_SP_INTERRUPT_Q3 : std_logic;
signal LAST_SP_WAS_Q1 : std_logic;
signal VELOCITY : std_logic_vector(31 downto 0);
signal PREVIOUS_LATCHED_V_COUNT_Q1 : std_logic_vector(31 downto 0);

-- POSITION SPECIFIC STUFF
signal POS_INT_ENBL, POS_EQ_POS1, POS_EQ_POS2 : std_logic;
signal POSITION_COUNT, POS1, POS2 : std_logic_vector(23 downto 0);
signal POSITION_COUNT_32BIT : std_logic_vector(31 downto 0);
signal POS_INT_BITS, POS_IMASK, POS_ISTATUS : std_logic_vector(1 downto 0); 
signal POS_ISTATUS_RD_ENBL : std_logic; 
signal POS_CTR_ENBL : std_logic;
signal REQ_TO_DISABLE_POS_INT : std_logic; 


begin
FORCE_HIGH <= '1';
FORCE_LOW <= '0';

WR_ENBL <= '1' when (cs = '1' and writen = '0') else '0'; -- This unit selected and write enable active low	
RD_ENBL <= '1' when (cs = '1' and readn = '0') else '0';

POS_ISTATUS_RD_ENBL <= '1' when RD_ENBL = '1' and addr = "0010" else '0';
SP_ISTATUS_RD_ENBL <= '1' when RD_ENBL = '1' and addr = "0110" else '0';

-- CONTROL_WORD(29 downto 18) available
POS_IMASK <= CONTROL_WORD(1 downto 0);
SP_IMASK <= CONTROL_WORD(11 downto 8);
POS_INT_ENBL <= CONTROL_WORD(30);
SP_INT_ENBL <= CONTROL_WORD(31);



VEL_CLK_DIVISOR <= ADDR_1110_INPUT_DATA(3 downto 0);
POSITION_PRELOAD <= ADDR_1110_INPUT_DATA(27 downto 4);
-- ADDR_1110_INPUT_DATA(31 downto 28) available for future expansion.
 
------------------- FOR SIMULATION ----------------------

--temp_pos_out <= POSITION_COUNT;		-- For simulation
--temp_raw_v_count_out <= VELOCITY_COUNT;
--temp_latched_v_count_q1_out <= LATCHED_V_COUNT_Q1;
--temp_latched_v_count_q2_out <= LATCHED_V_COUNT_Q2;
--temp_latched_v_count_q3_out <= LATCHED_V_COUNT_Q3;
--temp_latched_v_count_q4_out <= LATCHED_V_COUNT_Q4;
--temp_velocity_out <= VELOCITY;
--enable_velocity_ctr_out <= ENABLE_VELOCITY_CTR;
--velocity_clk_divisor_out <= VEL_CLK_DIVISOR;
--control_word_out <= CONTROL_WORD;
--sp_istatus_out <= SP_ISTATUS;
--pos_istatus_out <= POS_ISTATUS;


------------------------ NEW STUFF ----------------------------
-- Add in the quad int signals

POS_EQ_POS1 <= '1' when POS1 = POSITION_COUNT else '0';
POS_EQ_POS2 <= '1' when POS2 = POSITION_COUNT else '0';

POS_INT_BITS <= POS_EQ_POS2 & POS_EQ_POS1;

SP_INT_BITS <= REQ_FOR_X4_SP_INTERRUPT_Q4 &
								    REQ_FOR_X4_SP_INTERRUPT_Q3 & 
									  REQ_FOR_X4_SP_INTERRUPT_Q2 &
										REQ_FOR_X4_SP_INTERRUPT_Q1;
										
										
										
---------------------- POSITION COUNTER ----------------------

POS_CTR_ENBL <= x4;

-- Position counter, 24 bit up/down with enable and async clear
   process (clk, rst, preload)
		variable DIRECTION : std_logic;
		begin
			if (rst = '1' or preload = '1') then
				POSITION_COUNT <= POSITION_PRELOAD;
					
      		elsif (clk'EVENT and clk = '1') then
         		if POS_CTR_ENBL = '1' then
					if (up_dn = '1') then
         				DIRECTION := '1';
      				elsif (up_dn = '0') then
         				DIRECTION := '0';
					else null;
      				end if;

					-- Since the x4 signal is only high for 1 clk period, no
					-- need to detect when it falls and then sense only the 
					-- first rise.  If quad decoder is changed for some reason
					-- and x4's high time is increased, this process will have to
					-- be changed to sense first rise.
					if (DIRECTION = '1') then -- Count up
               			POSITION_COUNT <= POSITION_COUNT + '1';
					elsif (DIRECTION = '0') then
						POSITION_COUNT <= POSITION_COUNT - '1';
					else null;
					end if;
				else null;	
            	end if;
         	else null;	
      		end if;
end process;


---------------------------- OUTPUT DATA ------------------------------------

-- Expand the 24 bit signed value to occupy a full 32 bits.	
POSITION_COUNT_32BIT <= "00000000" & POSITION_COUNT when POSITION_COUNT(23) = '0'
	else "11111111" & POSITION_COUNT;


process (clk)
	begin
		if (clk 'event and clk = '1') then
		
			if (RD_ENBL = '1') then
				case addr is
					when "0000"=>  
						data_output <= 	POSITION_COUNT_32BIT;
					when "0001"=>  
						null; -- Reserved
					when "0010"=>
						data_output <= ("000000000000000000000000000000" & POS_ISTATUS);  
					when "0011"=>
						null; -- Reserved  
					when "0100"=>
						null; -- Reserved  
					when "0101"=>
						null; -- Reserved			
					when "0110"=>
						data_output <= ("0000000000000000000000000000" & SP_ISTATUS);	
					when "0111"=>  
						data_output <= LATCHED_V_COUNT_Q1;
					when "1000"=>  
						data_output <= LATCHED_V_COUNT_Q2;
					when "1001"=>  
						data_output <= LATCHED_V_COUNT_Q3;
					when "1010"=>  
						data_output <= LATCHED_V_COUNT_Q4;			
					when "1011"=>  
						data_output <= VELOCITY;
					when "1100"=> 
						data_output <= VELOCITY_COUNT;
					when others=> -- Addresses 1101 to 1111 are available for outputting data
						null;
				end case;
			else null;
			end if;
		else null;
		end if;
end process;


----------------------------- INPUT DATA -----------------------------------

process (clk, rst)
	begin
		if (rst = '1') then
			-- Disable position and velocity interrupts on reset.
			CONTROL_WORD(30) <= '0';
			CONTROL_WORD(31) <= '0';
		elsif (clk 'event and clk = '1') then
			if (REQ_TO_DISABLE_POS_INT = '1') then
				CONTROL_WORD(30) <= '0'; -- Disable position interrupts
			else null;
			end if;
			
			if (REQ_TO_DISABLE_SP_INT = '1') then
				CONTROL_WORD(31) <= '0'; -- Disable velocity / sp interrupts
			else null;
			end if;
			
			if (WR_ENBL = '1') then
				case addr is
					when "0000"=>
						POS1 <= data_input(23 downto 0);
					when "0001"=>
						POS2 <= data_input(23 downto 0);
					when "1110"=>  
						ADDR_1110_INPUT_DATA <= data_input;
					when "1111"=>  
						CONTROL_WORD <= data_input;
					when others=>
						null;
				end case;
			else null;
			end if;
		else null;
		end if;
end process;


-------------- POSITION CAM INTERRUPT CODE ------------------------------------

process (clk, rst)
variable PREV_INT_BITS, POS_INT_BITS_THAT_HAVE_SET : std_logic_vector(1 downto 0);
variable HAVE_READ_ISTATUS : std_logic;
begin
	if (rst = '1') then
		POS_INT_BITS_THAT_HAVE_SET := "00";
		PREV_INT_BITS := "00";
		HAVE_READ_ISTATUS := '0';
		REQ_TO_DISABLE_POS_INT <= '0';
	elsif (clk 'event and clk = '1') then
			REQ_TO_DISABLE_POS_INT <= '0';
			
			if (POS_ISTATUS_RD_ENBL = '1') then
				HAVE_READ_ISTATUS := '1';
			else null;
			end if;
			
			if (HAVE_READ_ISTATUS = '1' and POS_ISTATUS_RD_ENBL = '0') then -- Ack int
				POS_INT_BITS_THAT_HAVE_SET := "00"; -- Ack interrupt by clearing changed bits that will clear the interrupt pin
				REQ_TO_DISABLE_POS_INT <= '1';
				HAVE_READ_ISTATUS := '0';	
			elsif (POS_INT_BITS /= PREV_INT_BITS) then
				-- By xoring the prev cam output bits with the updated cam output
				-- bits and then anding the result to the updated cam output bits,
				-- it can be determined which cam output bits changed from low
				-- to high, INT_CAM_BITS_THAT_HAVE_SET.  Then these are ored to 
				-- the current CAM_BITS_THAT_HAVE_SET to create an updated 
				-- CAM_BITS_THAT_HAVE_SET.  Cam output bits that go from 
				-- high to low are thus ignored.   
				POS_INT_BITS_THAT_HAVE_SET := POS_INT_BITS_THAT_HAVE_SET or ((PREV_INT_BITS xor POS_INT_BITS) and POS_INT_BITS);			
				PREV_INT_BITS := POS_INT_BITS;
			else null;
			end if;
	else null;
	end if;
end process;

pos_interrupt <= '1' when ((POS_INT_BITS_THAT_HAVE_SET and not POS_IMASK) > "00")
						and POS_INT_ENBL = '1'
							else '0';
							
-- ISTATUS contains the non-masked bits that have generated an interrupt.
POS_ISTATUS <= (POS_IMASK xor POS_INT_BITS_THAT_HAVE_SET) and POS_INT_BITS_THAT_HAVE_SET;


---------------------- VELOCITY CODE ----------------------------------

--VEL_ENBL_CTR_CLK_ENBL <= VELOCITY_CNT_ENBL;
VEL_ENBL_CTR_CLK_ENBL <= FORCE_HIGH;
										
-- Velocity enable counter, 4 bit count up with clk enable
process (clk, rst)
	variable CNT : std_logic_vector(3 downto 0);
	begin
		if (rst = '1') then
			CNT := "0000";
		elsif (clk 'event and clk = '1') then
			if VEL_ENBL_CTR_CLK_ENBL = '1' then
           		CNT := CNT + '1';
			else null;
        	end if;
		else null;
      	end if;
		
		-- Now set enable output
		case VEL_CLK_DIVISOR is
			when "0001"=>  -- Divide by 2
				ENABLE_VELOCITY_CTR <= CNT(0);
			when "0010"=>  -- Divide by 4
				ENABLE_VELOCITY_CTR <= CNT(1);
			when "0100"=>  -- Divide by 8
				ENABLE_VELOCITY_CTR <= CNT(2);
			when "1000"=>  -- Divide by 16
				ENABLE_VELOCITY_CTR <= CNT(3);
			when others=>  -- Default to divide by 2
				ENABLE_VELOCITY_CTR <= CNT(0);
		end case;
end process;


------------------------- VELOCITY COUNTER -------------------------

VEL_CTR_CLK_ENBL <= ENABLE_VELOCITY_CTR;

-- Velocity counter, 32 bit count up with enable and async reset.
-- Enable will only allow one counter increment.  Then enable must
-- toggle before another clock pulse can increment the counter.
-- This is done so that the output bits of a 4 bit enable counter can be
-- used to divide down the clk frequency.
process (clk, rst)
	variable ENABLE_WAS_LOW : std_logic;
	begin
		if (rst = '1') then
			VELOCITY_COUNT <= "00000000000000000000000000000000";
			ENABLE_WAS_LOW := '0'; -- Assume enable was high, not critical
		elsif (clk 'event and clk = '1') then
			if (VEL_CTR_CLK_ENBL = '1') then
				if (ENABLE_WAS_LOW = '1') then -- First rise of enable, increment counter
					VELOCITY_COUNT <= VELOCITY_COUNT + '1';
					ENABLE_WAS_LOW := '0'; -- Not low any more
				else null;
				end if;
			elsif (VEL_CTR_CLK_ENBL = '0') then
				ENABLE_WAS_LOW := '1';
			else null;	
        	end if;
		else null;
      	end if;
end process;


-------------------------- VELOCITY LATCH ----------------------------

process(clk, rst)
	variable SPQ1_WAS_LOW, SPQ2_WAS_LOW, SPQ3_WAS_LOW, SPQ4_WAS_LOW : std_logic;
	
	begin
		if (rst = '1') then
			LATCHED_V_COUNT_Q1 <= "00000000000000000000000000000000";
			LATCHED_V_COUNT_Q2 <= "00000000000000000000000000000000";
			LATCHED_V_COUNT_Q3 <= "00000000000000000000000000000000";
			LATCHED_V_COUNT_Q4 <= "00000000000000000000000000000000";
			-- 			
			PREVIOUS_LATCHED_V_COUNT_Q1 <= "00000000000000000000000000000000";
			--
			SPQ1_WAS_LOW := '0'; -- Assume high at reset
			SPQ2_WAS_LOW := '0';
			SPQ3_WAS_LOW := '0';
			SPQ4_WAS_LOW := '0';
			--
			REQ_FOR_X4_SP_INTERRUPT_Q1 <= '0';
			REQ_FOR_X4_SP_INTERRUPT_Q2 <= '0';
			REQ_FOR_X4_SP_INTERRUPT_Q3 <= '0';
			REQ_FOR_X4_SP_INTERRUPT_Q4 <= '0';
			--
			LAST_SP_WAS_Q1 <= '0';
			
		elsif (clk 'event and clk = '1') then
			LAST_SP_WAS_Q1 <= '0';
			
			if (REQ_TO_CLEAR_SP_INTERRUPTS = '1') then
				REQ_FOR_X4_SP_INTERRUPT_Q1 <= '0';
				REQ_FOR_X4_SP_INTERRUPT_Q2 <= '0';
				REQ_FOR_X4_SP_INTERRUPT_Q3 <= '0';
				REQ_FOR_X4_SP_INTERRUPT_Q4 <= '0';
			else null;
			end if;
			
			if (speed_pulse_q1 = '0') then
				SPQ1_WAS_LOW := '1';
			else null;
			end if;
			
			if (speed_pulse_q2 = '0') then
				SPQ2_WAS_LOW := '1';
			else null;
			end if;
			
			if (speed_pulse_q3 = '0') then
				SPQ3_WAS_LOW := '1';
			else null;
			end if;
			
			if (speed_pulse_q4 = '0') then
				SPQ4_WAS_LOW := '1';
			else null;
			end if;
				
				
			if (speed_pulse_q1 = '1' and SPQ1_WAS_LOW = '1') then
				 -- Speed pulse has triggered
					PREVIOUS_LATCHED_V_COUNT_Q1 <= LATCHED_V_COUNT_Q1; -- Save present value
					LAST_SP_WAS_Q1 <= '1';
					LATCHED_V_COUNT_Q1 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0);
					SPQ1_WAS_LOW := '0'; -- Not low any more
					REQ_FOR_X4_SP_INTERRUPT_Q1 <= '1';
			elsif (speed_pulse_q2 = '1' and SPQ2_WAS_LOW = '1') then
				-- Speed pulse has triggered
					LATCHED_V_COUNT_Q2 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0);
					SPQ2_WAS_LOW := '0'; -- Not low any more
					REQ_FOR_X4_SP_INTERRUPT_Q2 <= '1';
			elsif (speed_pulse_q3 = '1' and SPQ3_WAS_LOW = '1') then
				-- Speed pulse has triggered
					LATCHED_V_COUNT_Q3 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0); 
					SPQ3_WAS_LOW := '0'; -- Not low any more
					REQ_FOR_X4_SP_INTERRUPT_Q3 <= '1';
			elsif (speed_pulse_q4 = '1' and SPQ4_WAS_LOW = '1') then
				-- Speed pulse has triggered
					LATCHED_V_COUNT_Q4 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0); 
					SPQ4_WAS_LOW := '0'; -- Not low any more
					REQ_FOR_X4_SP_INTERRUPT_Q4 <= '1';
			else null;
			end if;		
			
			-- Do a hardware velocity calculation.		
			if (LAST_SP_WAS_Q1 = '1') then
				if (LATCHED_V_COUNT_Q1(30) = '1' or PREVIOUS_LATCHED_V_COUNT_Q1(30) = '1') then -- Quad error
					VELOCITY <= "01010101010101010101010101010101"; --0x55555555 signifies quad error, flag bad velocity point.
				elsif (LATCHED_V_COUNT_Q1(31) /= PREVIOUS_LATCHED_V_COUNT_Q1(31)) then --Direction change
					VELOCITY <= "00000000000000000000000000000000"; --Zero speed.
				elsif (LATCHED_V_COUNT_Q1(31) = '0' and PREVIOUS_LATCHED_V_COUNT_Q1(31) = '0') then -- Moving in reverse
					VELOCITY <= PREVIOUS_LATCHED_V_COUNT_Q1 - LATCHED_V_COUNT_Q1; -- Negative velocity
				else -- Moving forward. 
					VELOCITY <= LATCHED_V_COUNT_Q1 - PREVIOUS_LATCHED_V_COUNT_Q1;
				end if;
			else null;
			end if;
					
		else null;
		end if;
end process;


---------------- SP INTERRUPT CODE ----------------------

process (clk, rst)
variable PREV_SP_INT_BITS, SP_INT_BITS_THAT_HAVE_SET : std_logic_vector(3 downto 0);
variable HAVE_READ_ISTATUS : std_logic;
begin
	if (rst = '1') then
		SP_INT_BITS_THAT_HAVE_SET := "0000";
		PREV_SP_INT_BITS := "0000";
		HAVE_READ_ISTATUS := '0';
		VELOCITY_CNT_ENBL <= '1';
		REQ_TO_DISABLE_SP_INT <= '0';
		REQ_TO_CLEAR_SP_INTERRUPTS <= '0';
	elsif (clk 'event and clk = '1') then
			REQ_TO_DISABLE_SP_INT <= '0';
			REQ_TO_CLEAR_SP_INTERRUPTS <= '0';
			
			-- Reading the ISTATUS register will clear the IPEND register
			-- which will in turn clear the ISTATUS register.  Timer
			-- interrupts will be disabled.  Reading the IPEND does not
			-- affect anything.
			if (SP_ISTATUS_RD_ENBL = '1') then
				HAVE_READ_ISTATUS := '1';
			else null;
			end if;
			
			if (HAVE_READ_ISTATUS = '1' and SP_ISTATUS_RD_ENBL = '0') then -- Just finished reading IPEND, ack interrupt at this time
				SP_INT_BITS_THAT_HAVE_SET := "0000"; -- Ack interrupt by clearing changed bits that will clear the interrupt pin
				REQ_TO_DISABLE_SP_INT <= '1';
				REQ_TO_CLEAR_SP_INTERRUPTS <= '1';
				HAVE_READ_ISTATUS := '0';	
			elsif (SP_INT_BITS /= PREV_SP_INT_BITS) then
				-- By xoring the prev cam output bits with the updated cam output
				-- bits and then anding the result to the updated cam output bits,
				-- it can be determined which cam output bits changed from low
				-- to high, INT_CAM_BITS_THAT_HAVE_SET.  Then these are ored to 
				-- the current CAM_BITS_THAT_HAVE_SET to create an updated 
				-- CAM_BITS_THAT_HAVE_SET.  Cam output bits that go from 
				-- high to low are thus ignored. 
				SP_INT_BITS_THAT_HAVE_SET := SP_INT_BITS_THAT_HAVE_SET or ((PREV_SP_INT_BITS xor SP_INT_BITS) and SP_INT_BITS);			
				PREV_SP_INT_BITS := SP_INT_BITS;
			else null;
			end if;
	else null;
	end if;
end process;


sp_interrupt <= '1'	when ((SP_INT_BITS_THAT_HAVE_SET and not SP_IMASK) > "0000")
							and SP_INT_ENBL = '1'
								else '0';

-- ISTATUS contains the non-masked int bits that have generated an interrupt.
SP_ISTATUS <= (SP_IMASK xor SP_INT_BITS_THAT_HAVE_SET) and SP_INT_BITS_THAT_HAVE_SET;

end behave;
