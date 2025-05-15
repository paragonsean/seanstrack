--------- POSITION_VELOCITY_INTERFACE_UNIT.VHD SYNC VERSION W/ CAMS-----------------
----------------- Copyright 2001, 2002 Visi-Trak Worldwide -------------------
-- 9-7-01, 9-28-01, 2-15-02, 10-7-02, 12-24-02
--
-- Combination position / velocity unit w/ altcams


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-- The position_velocity_interface_unit consists of a 24 bit position unit
-- and a 32 bit velocity unit.  Each unit generates an interrupt signal.
-- The position unit allows the saving 8 position setpoints in CAM memory
-- which are then compared to the position counter.  If a match occurs, an
-- interrupt signal is generated.  A ISTATUS register can be read which 
-- contains the non-masked CAM bit(s) that generated the interrupt.  An
-- IPEND register can also be read that contains all bits, masked and un-masked.
-- Reading the ISTATUS register will clear all IPEND and ISTATUS bit and
-- will also disable position interrupts until reenabled.
--
-- The velocity / timer unit allows each of the 4 speed pulses
-- to the velocity unit can generate an interrupt if so enabled.  
--
-- The timer value is latched for each of 4 quadrature quadrants.  Therefore
-- a position based data collection that generates an interrupt every
-- X1 pulse can read all 4 latched quadrature timer values and do in
-- effect an X4 data collection with little additional overhead.
--
-- Bit 31 of speed pulse latched data is the up_down signal.  Bit 30 is
-- the quad_err signal.  That leaves 30 bits (29 downto 0) for velocity
-- timing data.
--
-- By un-commenting out the stuff in the vlatch process and the labeled
-- signals, a hardware velocity calculation can be reenabled.  This greatly
-- increases the LE usage however.
--
-- This module is intended to interface to a quadrature decoder.

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
-- interrupt.  These 4 signals are now embedded with the timer
-- cam output bits.

-- 12-24-02 -- Removed the timer cam.  There are currently 3 cams.  One
-- for the limit switches with 6 entries.  This cannot be rewritten on the
-- fly.  There are 2 additional seperate cams that can be rewritten on the
-- fly.  These are for general purpose use.  If cams are to be rewritten
-- on the fly, there must be a seperate cam for each setpoint, otherwise
-- if the setpoint being entered matches an existing entry, a false match
-- will be generated.  The 4 quadrature quadrants interrupts have been 
-- combined with the position cam bits.  Therefore only one interrupt 
-- signal is generated.
 
-- 12-26-02 -- Removed the processes to generate the cam wr enbls.
-- Found that the cams can be written directly without inverted clocks
-- by specifying 1 wait state, 2 clock pulses total for write / reads.
-- Changed the interrupt process accordingly.


-- NIOS BUST BE SETUP FOR 1 WAIT STATE WHEN READING / WRITING THIS PERIPHERAL.

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
		--ipend_out : out std_logic_vector(11 downto 0); 
		--enable_velocity_ctr_out : out std_logic;
		--velocity_clk_divisor_out : out std_logic_vector(3 downto 0);
		--istatus_out : out std_logic_vector(11 downto 0);
		--
		interrupt	    : out std_logic);
end position_velocity_interface_unit;


architecture behave OF position_velocity_interface_unit is

component ls_cam
	PORT
	(
		pattern		: IN STD_LOGIC_VECTOR (23 DOWNTO 0);
		wraddress	: IN STD_LOGIC_VECTOR (2 DOWNTO 0);
		wren		: IN STD_LOGIC  := '0';
		inclock		: IN STD_LOGIC ;
		mbits		: OUT STD_LOGIC_VECTOR (5 DOWNTO 0)
	);
end component;


component gp_pos_cam 
	PORT
	(
		pattern		: IN STD_LOGIC_VECTOR (23 DOWNTO 0);
		wraddress	: IN STD_LOGIC_VECTOR (0 DOWNTO 0);
		wren		: IN STD_LOGIC  := '0';
		inclock		: IN STD_LOGIC ;
		mbits		: OUT STD_LOGIC_VECTOR (1 DOWNTO 0)
	);
end component;


component latch_w_enbl
	PORT
	(
		clock		: IN STD_LOGIC ;
		enable		: IN STD_LOGIC ;
		sclr		: IN STD_LOGIC ;
		data		: IN STD_LOGIC ;
		q			: OUT STD_LOGIC 
	);
end component;


-- COMMON STUFF
signal WR_ENBL, RD_ENBL, FORCE_HIGH, FORCE_LOW, NOT_CLK : std_logic;
signal FORCE_LOW_V : std_logic_vector(0 downto 0);
signal REQ_TO_DISABLE_INT : std_logic;
signal POSITION_PRELOAD : std_logic_vector(23 downto 0);
signal INT_ENBL : std_logic;
signal IPEND_RD_ENBL, ISTATUS_RD_ENBL : std_logic; 
signal ISTATUS, IPEND, IMASK, INT_BITS : std_logic_vector(11 downto 0);
signal Q1_HAS_CHANGED, Q2_HAS_CHANGED, Q3_HAS_CHANGED, Q4_HAS_CHANGED : std_logic;
signal INT_SIG : std_logic;

-- VELOCITY / SP SPECIFIC STUFF
signal VELOCITY_COUNT : std_logic_vector(31 downto 0);
signal SP_INT_PROCESS_ENBL  : std_logic;  
signal REQ_TO_CLEAR_SP_INTERRUPTS : std_logic;
signal LATCHED_V_COUNT_Q1 : std_logic_vector(31 downto 0);
signal LATCHED_V_COUNT_Q2 : std_logic_vector(31 downto 0);
signal LATCHED_V_COUNT_Q3 : std_logic_vector(31 downto 0);
signal LATCHED_V_COUNT_Q4 : std_logic_vector(31 downto 0);
signal VEL_CLK_DIVISOR : std_logic_vector(3 downto 0);
signal VEL_CTR_CLK_ENBL, VEL_ENBL_CTR_CLK_ENBL, ENABLE_VELOCITY_CTR : std_logic;
signal REQ_FOR_X4_SP_INTERRUPT_Q1 : std_logic;
signal REQ_FOR_X4_SP_INTERRUPT_Q2 : std_logic; 
signal REQ_FOR_X4_SP_INTERRUPT_Q3 : std_logic;
signal REQ_FOR_X4_SP_INTERRUPT_Q4  : std_logic;
signal LAST_SP_WAS_Q1 : std_logic;
signal VELOCITY : std_logic_vector(31 downto 0);
signal PREVIOUS_LATCHED_V_COUNT_Q1 : std_logic_vector(31 downto 0);


-- POSITION SPECIFIC STUFF
signal POSITION_COUNT, INPUT_TO_POS_CAM : std_logic_vector(23 downto 0);
signal POSITION_COUNT_32BIT : std_logic_vector(31 downto 0);
signal POS_CAM_OUTPUT_BITS  : std_logic_vector(7 downto 0); 
signal POS_CTR_ENBL : std_logic;
signal GP_POS_CAM1_MBITS, GP_POS_CAM2_MBITS : std_logic_vector(1 downto 0);
signal LS_CAM_WR_ENBL : std_logic; 
signal GP_POS_CAM1_WR_ENBL : std_logic; 
signal GP_POS_CAM2_WR_ENBL : std_logic; 
signal GP_CAM1_LATCH_ENBL : std_logic; 
signal GP_CAM2_LATCH_ENBL : std_logic; 
signal INPUT_TO_LS_CAM : std_logic_vector(23 downto 0);
signal INPUT_TO_GP_POS_CAM1 : std_logic_vector(23 downto 0);
signal INPUT_TO_GP_POS_CAM2 : std_logic_vector(23 downto 0);

begin
FORCE_HIGH <= '1';
FORCE_LOW <= '0';
FORCE_LOW_V <= "0";

WR_ENBL <= '1' when (cs = '1' and writen = '0') else '0'; -- This unit selected and write enable active low	
RD_ENBL <= '1' when (cs = '1' and readn = '0') else '0';
--NOT_CLK <= not clk;

-- Generate cam write signals
LS_CAM_WR_ENBL <= '1' when WR_ENBL = '1' and addr <= "0101" else '0';
GP_POS_CAM1_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "0110" else '0';
GP_POS_CAM2_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "0111" else '0';


IPEND_RD_ENBL <= '1' when RD_ENBL = '1' and addr = "0001" else '0';
ISTATUS_RD_ENBL <= '1' when RD_ENBL = '1' and addr = "0010" else '0';

-- Muxes for CAM inputs
INPUT_TO_LS_CAM <= data_input(23 downto 0) when LS_CAM_WR_ENBL = '1' else POSITION_COUNT;
INPUT_TO_GP_POS_CAM1 <= data_input(23 downto 0) when GP_POS_CAM1_WR_ENBL = '1' else POSITION_COUNT;
INPUT_TO_GP_POS_CAM2 <= data_input(23 downto 0) when GP_POS_CAM2_WR_ENBL = '1' else POSITION_COUNT;


INT_BITS <= REQ_FOR_X4_SP_INTERRUPT_Q4 &
					REQ_FOR_X4_SP_INTERRUPT_Q3 & 
						REQ_FOR_X4_SP_INTERRUPT_Q2 &
							REQ_FOR_X4_SP_INTERRUPT_Q1 &
								POS_CAM_OUTPUT_BITS;

------------------- FOR SIMULATION ----------------------

--temp_pos_out <= POSITION_COUNT;		-- For simulation
--temp_raw_v_count_out <= VELOCITY_COUNT;
--temp_latched_v_count_q1_out <= LATCHED_V_COUNT_Q1;
--temp_latched_v_count_q2_out <= LATCHED_V_COUNT_Q2;
--temp_latched_v_count_q3_out <= LATCHED_V_COUNT_Q3;
--temp_latched_v_count_q4_out <= LATCHED_V_COUNT_Q4;
--temp_velocity_out <= VELOCITY;
--ipend_out <= IPEND;
--enable_velocity_ctr_out <= ENABLE_VELOCITY_CTR;
--velocity_clk_divisor_out <= VEL_CLK_DIVISOR;
--istatus_out <= ISTATUS;


-------- PROCESSES FOR GENERATING CAM LATCH ENBLS ------

process(clk)
	begin
		if (clk 'event and clk = '1') then 
			if (GP_POS_CAM1_WR_ENBL = '1') then
				GP_CAM1_LATCH_ENBL <= '0';
			else
				GP_CAM1_LATCH_ENBL <= '1';
			end if;
		else null;
		end if;
end process;

process(clk)
	begin
		if (clk 'event and clk = '1') then 
			if (GP_POS_CAM2_WR_ENBL = '1') then
				GP_CAM2_LATCH_ENBL <= '0';
			else
				GP_CAM2_LATCH_ENBL <= '1';
			end if;
		else null;
		end if;
end process;



---------------- COMPONENT INSTANTIATION ----------------
ls_cam_inst : ls_cam PORT MAP (
		pattern	 => INPUT_TO_LS_CAM,
		wraddress	 => addr(2 downto 0),
		wren	 => LS_CAM_WR_ENBL,
		inclock	 => clk,
		mbits	 => POS_CAM_OUTPUT_BITS(5 downto 0)
	);


gp_pos_cam_inst1 : gp_pos_cam PORT MAP (
		pattern	 => INPUT_TO_GP_POS_CAM1,
		wraddress	 => FORCE_LOW_V,
		wren	 => GP_POS_CAM1_WR_ENBL,
		inclock	 => clk,
		mbits	 => GP_POS_CAM1_MBITS
	);


gp_pos_cam_inst2 : gp_pos_cam PORT MAP (
		pattern	 => INPUT_TO_GP_POS_CAM2,
		wraddress	 => FORCE_LOW_V,
		wren	 => GP_POS_CAM2_WR_ENBL,
		inclock	 => clk,
		mbits	 => GP_POS_CAM2_MBITS
	);


gp_pos_latch_inst1 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => GP_CAM1_LATCH_ENBL,
		sclr	 => rst,
		data	 => GP_POS_CAM1_MBITS(0),
		q	 => POS_CAM_OUTPUT_BITS(6)
	);


gp_pos_latch_inst2 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => GP_CAM2_LATCH_ENBL,
		sclr	 => rst,
		data	 => GP_POS_CAM2_MBITS(0),
		q	 => POS_CAM_OUTPUT_BITS(7)
	);




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
						data_output <= ("00000000000000000000" & IPEND);
					when "0010"=>
						data_output <= ("00000000000000000000" & ISTATUS);  
					when "0011"=>
						data_output <= ("00000000000000000000" & IMASK); 
					when "0100"=>
						data_output <= ("0000000000000000000000000000000" & INT_ENBL);  
					when "0101"=>
						data_output <= ("00000000" & POSITION_PRELOAD); 			
					when "0110"=>
						data_output <= (INT_SIG & "000000000000000000000000000" & VEL_CLK_DIVISOR); 	
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
			-- Disable interrupt on reset.
			INT_ENBL <= '0';
		elsif (clk 'event and clk = '1') then
			if (REQ_TO_DISABLE_INT = '1') then
				INT_ENBL <= '0'; 
			else null;
			end if;
			
			if (WR_ENBL = '1') then
				case addr is
					when "1100"=>
						INT_ENBL <= data_input(0);  
					when "1101"=>
						IMASK <= data_input(11 downto 0);
					when "1110"=>  
						POSITION_PRELOAD <= data_input(23 downto 0);
					when "1111"=>  
						VEL_CLK_DIVISOR <= data_input(3 downto 0);
					when others=> -- "1000" and "1011" available
						null;
				end case;
			else null;
			end if;
		else null;
		end if;
end process;


------------------- INTERRUPT CODE ----------------------

process (clk, rst)
variable INT_BITS_THAT_HAVE_SET, PREV_INT_BITS : std_logic_vector(11 downto 0);
variable CNT : std_logic_vector(1 downto 0);
begin
	if (rst = '1') then
		INT_BITS_THAT_HAVE_SET := "000000000000";
		PREV_INT_BITS := "000000000000";
		IPEND <= "000000000000";
		REQ_TO_DISABLE_INT <= '0';
		REQ_TO_CLEAR_SP_INTERRUPTS <= '0';
		CNT := "00";
	elsif (clk 'event and clk = '1') then
			REQ_TO_DISABLE_INT <= '0';
			REQ_TO_CLEAR_SP_INTERRUPTS <= '0';
			
			-- Reading the ISTATUS register will clear the IPEND register
			-- which will in turn clear the ISTATUS register.  Timer
			-- interrupts will be disabled.  Reading the IPEND does not
			-- affect anything.
			if (ISTATUS_RD_ENBL = '1' and CNT = "00") then
				-- On the 1st clk pulse after ISTATUS_RD_ENBL goes high,
				-- clear ipend/istatus.  Since it takes 1 cp for istatus
				-- to ripple through to the data_output, valid data
				-- will be latched.  It was found during simulation
				-- that if a cam bit changed the same time istatus was
				-- being read, that cam match could be missed.  This
				-- eliminates that possibility.  No matter when a cam
				-- match takes place, there will be no missed matches.
				-- The nios must be set up so that a total of 2 clk pluses
				-- are required for read and writes.  This means using
				-- 1 wait state.
				INT_BITS_THAT_HAVE_SET := "000000000000"; -- Ack interrupt by clearing changed bits that will clear the interrupt pin
				REQ_TO_DISABLE_INT <= '1';
				REQ_TO_CLEAR_SP_INTERRUPTS <= '1';
				CNT := "01";
			elsif (ISTATUS_RD_ENBL = '0') then 
				CNT := "00";	
			else null;
			end if;

			if (INT_BITS /= PREV_INT_BITS) then
				-- By xoring the prev cam output bits with the updated cam output
				-- bits and then anding the result to the updated cam output bits,
				-- it can be determined which cam output bits changed from low
				-- to high, INT_CAM_BITS_THAT_HAVE_SET.  Then these are ored to 
				-- the current CAM_BITS_THAT_HAVE_SET to create an updated 
				-- CAM_BITS_THAT_HAVE_SET.  Cam output bits that go from 
				-- high to low are thus ignored.   
				INT_BITS_THAT_HAVE_SET := INT_BITS_THAT_HAVE_SET or ((PREV_INT_BITS xor INT_BITS) and INT_BITS);			
				PREV_INT_BITS := INT_BITS;
			else null;
			end if;
			
			IPEND <= INT_BITS_THAT_HAVE_SET;
			
	else null;
	end if;
end process;


--interrupt <= '1' when ((IPEND and not IMASK) > "000000000000")
--						and INT_ENBL = '1'
--							else '0';
							
-- ISTATUS contains the non-masked IPEND bits that have generated an interrupt.
ISTATUS <= (IMASK xor IPEND) and IPEND;

INT_SIG <= '1' when (ISTATUS > "000000000000")
						and INT_ENBL = '1'
							else '0';

interrupt <= INT_SIG;
---------------------- VELOCITY CODE ----------------------------------

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
	variable PREV_Q1, PREV_Q2, PREV_Q3, PREV_Q4 : std_logic;
	
	begin
		if (rst = '1') then
			LATCHED_V_COUNT_Q1 <= "00000000000000000000000000000000";
			LATCHED_V_COUNT_Q2 <= "00000000000000000000000000000000";
			LATCHED_V_COUNT_Q3 <= "00000000000000000000000000000000";
			LATCHED_V_COUNT_Q4 <= "00000000000000000000000000000000";
			-- 			
			PREVIOUS_LATCHED_V_COUNT_Q1 <= "00000000000000000000000000000000";
			--
			PREV_Q1 := '1'; -- Assume high at reset
			PREV_Q1 := '1';
			PREV_Q1 := '1';
			PREV_Q1 := '1';
			Q1_HAS_CHANGED <= '0';
			Q2_HAS_CHANGED <= '0';
			Q3_HAS_CHANGED <= '0';
			Q4_HAS_CHANGED <= '0';
			--
			REQ_FOR_X4_SP_INTERRUPT_Q1 <= '0';
			REQ_FOR_X4_SP_INTERRUPT_Q2 <= '0';
			REQ_FOR_X4_SP_INTERRUPT_Q3 <= '0';
			REQ_FOR_X4_SP_INTERRUPT_Q4 <= '0';
			--
			LAST_SP_WAS_Q1 <= '0';
			
		elsif (clk 'event and clk = '1') then
			LAST_SP_WAS_Q1 <= '0';
			Q1_HAS_CHANGED <= '0';		
			Q2_HAS_CHANGED <= '0';
			Q3_HAS_CHANGED <= '0';
			Q4_HAS_CHANGED <= '0';
			
			if (speed_pulse_q1 /= PREV_Q1) then
				PREV_Q1 := speed_pulse_q1;
				if (speed_pulse_q1 = '1') then
					Q1_HAS_CHANGED <= '1';
				else null;
				end if;	
			else null;
			end if;
			
			if (speed_pulse_q2 /= PREV_Q2) then
				PREV_Q2 := speed_pulse_q2;
				if (speed_pulse_q2 = '1') then
					Q2_HAS_CHANGED <= '1';
				else null;
				end if;	
			else null;
			end if;
			
			if (speed_pulse_q3 /= PREV_Q3) then
				PREV_Q3 := speed_pulse_q3;
				if (speed_pulse_q3 = '1') then
					Q3_HAS_CHANGED <= '1';
				else null;
				end if;	
			else null;
			end if;
			
			if (speed_pulse_q4 /= PREV_Q4) then
				PREV_Q4 := speed_pulse_q4;
				if (speed_pulse_q4 = '1') then
					Q4_HAS_CHANGED <= '1';
				else null;
				end if;	
			else null;
			end if;
			
			if (REQ_TO_CLEAR_SP_INTERRUPTS = '1') then
				REQ_FOR_X4_SP_INTERRUPT_Q1 <= '0';
				REQ_FOR_X4_SP_INTERRUPT_Q2 <= '0';
				REQ_FOR_X4_SP_INTERRUPT_Q3 <= '0';
				REQ_FOR_X4_SP_INTERRUPT_Q4 <= '0';
			else null;
			end if;
			
			if (Q1_HAS_CHANGED = '1') then -- SPQ1 has went lo to hi
				 -- Speed pulse has triggered
					PREVIOUS_LATCHED_V_COUNT_Q1 <= LATCHED_V_COUNT_Q1; -- Save present value
					LAST_SP_WAS_Q1 <= '1';
					LATCHED_V_COUNT_Q1 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0);
					--PREV_Q1 := speed_pulse_q1;
					REQ_FOR_X4_SP_INTERRUPT_Q1 <= '1';
			elsif (Q2_HAS_CHANGED = '1') then
				-- Speed pulse has triggered
					LATCHED_V_COUNT_Q2 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0);
					--PREV_Q2 := speed_pulse_q2; 
					REQ_FOR_X4_SP_INTERRUPT_Q2 <= '1';
			elsif (Q3_HAS_CHANGED = '1') then
				-- Speed pulse has triggered
					LATCHED_V_COUNT_Q3 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0); 
					--PREV_Q3 := speed_pulse_q3; 
					REQ_FOR_X4_SP_INTERRUPT_Q3 <= '1';
			elsif (Q4_HAS_CHANGED = '1') then
				-- Speed pulse has triggered
					LATCHED_V_COUNT_Q4 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0); 
					--PREV_Q4 := speed_pulse_q4; -- Not low any more
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

end behave;
