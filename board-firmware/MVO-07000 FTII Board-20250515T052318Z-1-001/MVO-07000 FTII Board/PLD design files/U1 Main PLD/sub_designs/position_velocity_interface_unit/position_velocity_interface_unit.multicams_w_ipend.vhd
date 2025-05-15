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
-- The position unit allows the saving 8 position setpoints in CAM memory
-- which are then compared to the position counter.  If a match occurs, an
-- interrupt signal is generated.  A ISTATUS register can be read which 
-- contains the non-masked CAM bit(s) that generated the interrupt.  An
-- IPEND register can also be read that contains all bits, masked and un-masked.
-- Reading the ISTATUS register will clear all IPEND and ISTATUS bit and
-- will also disable position interrupts until reenabled.
--
-- The velocity / timer unit allows 6 time based setpoints saved in CAM and
-- will generate interrupts similiar to the position unit above.  In addition
-- to the CAM bits, each of the 4 speed pulses to the velocity unit can
-- generate an interrupt if so enabled.  
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
		--temp_pos_ipend_out : out std_logic_vector(7 downto 0); 
		--temp_timer_ipend_out : out std_logic_vector(9 downto 0);
		--enable_velocity_ctr_out : out std_logic;
		--velocity_clk_divisor_out : out std_logic_vector(3 downto 0);
		--control_word_out : out std_logic_vector(31 downto 0);
		--timer_istatus_out : out std_logic_vector(9 downto 0);
		--pos_istatus_out : out std_logic_vector(7 downto 0);
		--
		timer_interrupt 	: out std_logic;
		pos_interrupt	    : out std_logic);
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


component gp_timer_cam
	PORT
	(
		pattern		: IN STD_LOGIC_VECTOR (31 DOWNTO 0);
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
signal CONTROL_WORD : std_logic_vector(31 downto 0);
signal POSITION_PRELOAD : std_logic_vector(23 downto 0);
signal ADDR_1110_INPUT_DATA : std_logic_vector(31 downto 0); -- Velocity divisor/position preload

-- VELOCITY / TIMER SPECIFIC STUFF
signal TIMER_INT_ENBL : std_logic;
signal VELOCITY_COUNT, INPUT_TO_TIMER_CAM : std_logic_vector(31 downto 0);
signal TIMER_IPEND, TIMER_IMASK, TIMER_ISTATUS : std_logic_vector(9 downto 0);
signal TIMER_CAM_OUTPUT_BITS : std_logic_vector(5 downto 0);
signal COMPLETE_TIMER_CAM_OUTPUT_BITS : std_logic_vector(9 downto 0);
signal TIMER_IPEND_RD_ENBL, TIMER_CAM_INT_PROCESS_ENBL, VELOCITY_CNT_ENBL : std_logic;  
signal TIMER_ISTATUS_RD_ENBL : std_logic;
signal REQ_TO_DISABLE_VEL_INT : std_logic;
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
signal GP_TIMER_CAM1_MBITS, GP_TIMER_CAM2_MBITS : std_logic_vector(1 downto 0);
signal GP_TIMER_CAM3_MBITS, GP_TIMER_CAM4_MBITS : std_logic_vector(1 downto 0);
signal GP_TIMER_CAM5_MBITS, GP_TIMER_CAM6_MBITS : std_logic_vector(1 downto 0);
signal REQ_FOR_GP_TIMER_CAM1_WR_ENBL, GP_TIMER_CAM1_WR_ENBL : std_logic; 
signal REQ_FOR_GP_TIMER_CAM2_WR_ENBL, GP_TIMER_CAM2_WR_ENBL : std_logic; 
signal REQ_FOR_GP_TIMER_CAM3_WR_ENBL, GP_TIMER_CAM3_WR_ENBL : std_logic; 
signal REQ_FOR_GP_TIMER_CAM4_WR_ENBL, GP_TIMER_CAM4_WR_ENBL : std_logic; 
signal REQ_FOR_GP_TIMER_CAM5_WR_ENBL, GP_TIMER_CAM5_WR_ENBL : std_logic; 
signal REQ_FOR_GP_TIMER_CAM6_WR_ENBL, GP_TIMER_CAM6_WR_ENBL : std_logic;  
signal NOT_REQ_FOR_GP_TIMER_CAM1_WR_ENBL : std_logic; 
signal NOT_REQ_FOR_GP_TIMER_CAM2_WR_ENBL : std_logic; 
signal NOT_REQ_FOR_GP_TIMER_CAM3_WR_ENBL : std_logic; 
signal NOT_REQ_FOR_GP_TIMER_CAM4_WR_ENBL : std_logic; 
signal NOT_REQ_FOR_GP_TIMER_CAM5_WR_ENBL : std_logic; 
signal NOT_REQ_FOR_GP_TIMER_CAM6_WR_ENBL : std_logic;  
signal INPUT_TO_GP_TIMER_CAM1 : std_logic_vector(31 downto 0);
signal INPUT_TO_GP_TIMER_CAM2 : std_logic_vector(31 downto 0);
signal INPUT_TO_GP_TIMER_CAM3 : std_logic_vector(31 downto 0);
signal INPUT_TO_GP_TIMER_CAM4 : std_logic_vector(31 downto 0);
signal INPUT_TO_GP_TIMER_CAM5 : std_logic_vector(31 downto 0);
signal INPUT_TO_GP_TIMER_CAM6 : std_logic_vector(31 downto 0); 

-- POSITION SPECIFIC STUFF
signal POS_INT_ENBL : std_logic;
signal POSITION_COUNT, INPUT_TO_POS_CAM : std_logic_vector(23 downto 0);
signal POSITION_COUNT_32BIT : std_logic_vector(31 downto 0);
signal POS_CAM_OUTPUT_BITS, POS_IPEND, POS_IMASK, POS_ISTATUS : std_logic_vector(7 downto 0); 
signal POS_IPEND_RD_ENBL, POS_ISTATUS_RD_ENBL : std_logic; 
signal POS_CTR_ENBL : std_logic;
signal POS_CAM_INT_PROCESS_ENBL : std_logic;
signal REQ_TO_DISABLE_POS_INT : std_logic; 
signal GP_POS_CAM1_MBITS, GP_POS_CAM2_MBITS : std_logic_vector(1 downto 0);
signal REQ_FOR_LS_CAM_WR_ENBL, LS_CAM_WR_ENBL : std_logic; 
signal REQ_FOR_GP_POS_CAM1_WR_ENBL, GP_POS_CAM1_WR_ENBL : std_logic; 
signal REQ_FOR_GP_POS_CAM2_WR_ENBL, GP_POS_CAM2_WR_ENBL : std_logic; 
signal NOT_REQ_FOR_GP_POS_CAM1_WR_ENBL : std_logic; 
signal NOT_REQ_FOR_GP_POS_CAM2_WR_ENBL : std_logic; 
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
REQ_FOR_LS_CAM_WR_ENBL <= '1' when WR_ENBL = '1' and addr <= "0101" else '0';
REQ_FOR_GP_POS_CAM1_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "0110" else '0';
REQ_FOR_GP_POS_CAM2_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "0111" else '0';
REQ_FOR_GP_TIMER_CAM1_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "1000" else '0';
REQ_FOR_GP_TIMER_CAM2_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "1001" else '0';
REQ_FOR_GP_TIMER_CAM3_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "1010" else '0';
REQ_FOR_GP_TIMER_CAM4_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "1011" else '0';
REQ_FOR_GP_TIMER_CAM5_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "1100" else '0';
REQ_FOR_GP_TIMER_CAM6_WR_ENBL <= '1' when WR_ENBL = '1' and addr = "1101" else '0';

NOT_REQ_FOR_GP_POS_CAM1_WR_ENBL <= not REQ_FOR_GP_POS_CAM1_WR_ENBL;
NOT_REQ_FOR_GP_POS_CAM2_WR_ENBL <= not REQ_FOR_GP_POS_CAM2_WR_ENBL;  
NOT_REQ_FOR_GP_TIMER_CAM1_WR_ENBL <= not REQ_FOR_GP_TIMER_CAM1_WR_ENBL;
NOT_REQ_FOR_GP_TIMER_CAM2_WR_ENBL <= not REQ_FOR_GP_TIMER_CAM2_WR_ENBL;  
NOT_REQ_FOR_GP_TIMER_CAM3_WR_ENBL <= not REQ_FOR_GP_TIMER_CAM3_WR_ENBL; 
NOT_REQ_FOR_GP_TIMER_CAM4_WR_ENBL <= not REQ_FOR_GP_TIMER_CAM4_WR_ENBL;  
NOT_REQ_FOR_GP_TIMER_CAM5_WR_ENBL <= not REQ_FOR_GP_TIMER_CAM5_WR_ENBL;  
NOT_REQ_FOR_GP_TIMER_CAM6_WR_ENBL <= not REQ_FOR_GP_TIMER_CAM6_WR_ENBL;  



POS_IPEND_RD_ENBL <= '1' when RD_ENBL = '1' and addr = "0001" else '0';
TIMER_IPEND_RD_ENBL <= '1' when RD_ENBL = '1' and addr = "0101" else '0';
POS_ISTATUS_RD_ENBL <= '1' when RD_ENBL = '1' and addr = "0010" else '0';
TIMER_ISTATUS_RD_ENBL <= '1' when RD_ENBL = '1' and addr = "0110" else '0';

-- CONTROL_WORD(29 downto 18) available
POS_IMASK <= CONTROL_WORD(7 downto 0);
TIMER_IMASK <= CONTROL_WORD(17 downto 8);
POS_INT_ENBL <= CONTROL_WORD(30);
TIMER_INT_ENBL <= CONTROL_WORD(31);

-- Muxes for CAM inputs
INPUT_TO_LS_CAM <= data_input(23 downto 0) when LS_CAM_WR_ENBL = '1' else POSITION_COUNT;
INPUT_TO_GP_POS_CAM1 <= data_input(23 downto 0) when GP_POS_CAM1_WR_ENBL = '1' else POSITION_COUNT;
INPUT_TO_GP_POS_CAM2 <= data_input(23 downto 0) when GP_POS_CAM2_WR_ENBL = '1' else POSITION_COUNT;

INPUT_TO_GP_TIMER_CAM1 <= data_input when GP_TIMER_CAM1_WR_ENBL = '1' else VELOCITY_COUNT;
INPUT_TO_GP_TIMER_CAM2 <= data_input when GP_TIMER_CAM2_WR_ENBL = '1' else VELOCITY_COUNT;
INPUT_TO_GP_TIMER_CAM3 <= data_input when GP_TIMER_CAM3_WR_ENBL = '1' else VELOCITY_COUNT;
INPUT_TO_GP_TIMER_CAM4 <= data_input when GP_TIMER_CAM4_WR_ENBL = '1' else VELOCITY_COUNT;
INPUT_TO_GP_TIMER_CAM5 <= data_input when GP_TIMER_CAM5_WR_ENBL = '1' else VELOCITY_COUNT;
INPUT_TO_GP_TIMER_CAM6 <= data_input when GP_TIMER_CAM6_WR_ENBL = '1' else VELOCITY_COUNT;


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
--temp_pos_ipend_out <= POS_IPEND;
--temp_timer_ipend_out <= TIMER_IPEND;
--enable_velocity_ctr_out <= ENABLE_VELOCITY_CTR;
--velocity_clk_divisor_out <= VEL_CLK_DIVISOR;
--control_word_out <= CONTROL_WORD;
--timer_istatus_out <= TIMER_ISTATUS;
--pos_istatus_out <= POS_ISTATUS;


-------- PROCESSES FOR GENERATING CAM WR ENBLS ------

-- Cam writes are synced to the falling edge of clk.  Avalon operation
-- occur on the rising edge.  These process take the Nios' writen
-- and synchronizes it to the falling edge for the cams.
--
-- Must specify 2 wait states when setting up the Nios to 
-- write the cams.  The total write cycle will be 3 clk cycles.

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_LS_CAM_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					LS_CAM_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					LS_CAM_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
			end if;
		else null;
		end if;
end process;

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_GP_POS_CAM1_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					GP_POS_CAM1_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					GP_POS_CAM1_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
			end if;
		else null;
		end if;
end process;

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_GP_POS_CAM2_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					GP_POS_CAM2_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					GP_POS_CAM2_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
			end if;
		else null;
		end if;
end process;

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_GP_TIMER_CAM1_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					GP_TIMER_CAM1_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					GP_TIMER_CAM1_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
			end if;
		else null;
		end if;
end process;

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_GP_TIMER_CAM2_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					GP_TIMER_CAM2_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					GP_TIMER_CAM2_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
			end if;
		else null;
		end if;
end process;

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_GP_TIMER_CAM3_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					GP_TIMER_CAM3_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					GP_TIMER_CAM3_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
			end if;
		else null;
		end if;
end process;

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_GP_TIMER_CAM4_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					GP_TIMER_CAM4_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					GP_TIMER_CAM4_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
			end if;
		else null;
		end if;
end process;

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_GP_TIMER_CAM5_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					GP_TIMER_CAM5_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					GP_TIMER_CAM5_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
			end if;
		else null;
		end if;
end process;

process(clk, rst)
	variable STEP1, STEP2 : std_logic;
	begin
		if (rst = '1') then
			STEP1 := '0'; STEP2 := '0';
		elsif (clk 'event and clk = '0') then -- falling edge
			if (REQ_FOR_GP_TIMER_CAM6_WR_ENBL = '1') then
				if (STEP1 = '0' and STEP2 = '0') then -- First time through
					GP_TIMER_CAM6_WR_ENBL <= '1';
					STEP1 := '1';
				elsif (STEP1 = '1') then
					STEP1 := '0';
					STEP2 := '1';
				elsif (STEP2 = '1') then
					GP_TIMER_CAM6_WR_ENBL <= '0';
					STEP2 := '0';
				else null;
				end if;
			else null;
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
		enable	 => NOT_REQ_FOR_GP_POS_CAM1_WR_ENBL,
		sclr	 => rst,
		data	 => GP_POS_CAM1_MBITS(0),
		q	 => POS_CAM_OUTPUT_BITS(6)
	);


gp_pos_latch_inst2 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => NOT_REQ_FOR_GP_POS_CAM2_WR_ENBL,
		sclr	 => rst,
		data	 => GP_POS_CAM2_MBITS(0),
		q	 => POS_CAM_OUTPUT_BITS(7)
	);



gp_timer_cam_inst1 : gp_timer_cam PORT MAP (
		pattern	 => INPUT_TO_GP_TIMER_CAM1,
		wraddress	 => FORCE_LOW_V,
		wren	 => GP_TIMER_CAM1_WR_ENBL,
		inclock	 => clk,
		mbits	 => GP_TIMER_CAM1_MBITS
	);

gp_timer_cam_inst2 : gp_timer_cam PORT MAP (
		pattern	 => INPUT_TO_GP_TIMER_CAM2,
		wraddress	 => FORCE_LOW_V,
		wren	 => GP_TIMER_CAM2_WR_ENBL,
		inclock	 => clk,
		mbits	 => GP_TIMER_CAM2_MBITS
	);
	
gp_timer_cam_inst3 : gp_timer_cam PORT MAP (
		pattern	 => INPUT_TO_GP_TIMER_CAM3,
		wraddress	 => FORCE_LOW_V,
		wren	 => GP_TIMER_CAM3_WR_ENBL,
		inclock	 => clk,
		mbits	 => GP_TIMER_CAM3_MBITS
	);
	
gp_timer_cam_inst4 : gp_timer_cam PORT MAP (
		pattern	 => INPUT_TO_GP_TIMER_CAM4,
		wraddress	 => FORCE_LOW_V,
		wren	 => GP_TIMER_CAM4_WR_ENBL,
		inclock	 => clk,
		mbits	 => GP_TIMER_CAM4_MBITS
	);
	
gp_timer_cam_inst5 : gp_timer_cam PORT MAP (
		pattern	 => INPUT_TO_GP_TIMER_CAM5,
		wraddress	 => FORCE_LOW_V,
		wren	 => GP_TIMER_CAM5_WR_ENBL,
		inclock	 => clk,
		mbits	 => GP_TIMER_CAM5_MBITS
	);
	
gp_timer_cam_inst6 : gp_timer_cam PORT MAP (
		pattern	 => INPUT_TO_GP_TIMER_CAM6,
		wraddress	 => FORCE_LOW_V,
		wren	 => GP_TIMER_CAM6_WR_ENBL,
		inclock	 => clk,
		mbits	 => GP_TIMER_CAM6_MBITS
	);				

gp_timer_latch_inst1 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => NOT_REQ_FOR_GP_TIMER_CAM1_WR_ENBL,
		sclr	 => rst,
		data	 => GP_TIMER_CAM1_MBITS(0),
		q	 => TIMER_CAM_OUTPUT_BITS(0)
	);

gp_timer_latch_inst2 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => NOT_REQ_FOR_GP_TIMER_CAM2_WR_ENBL,
		sclr	 => rst,
		data	 => GP_TIMER_CAM2_MBITS(0),
		q	 => TIMER_CAM_OUTPUT_BITS(1)
	);

gp_timer_latch_inst3 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => NOT_REQ_FOR_GP_TIMER_CAM3_WR_ENBL,
		sclr	 => rst,
		data	 => GP_TIMER_CAM3_MBITS(0),
		q	 => TIMER_CAM_OUTPUT_BITS(2)
	);

gp_timer_latch_inst4 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => NOT_REQ_FOR_GP_TIMER_CAM4_WR_ENBL,
		sclr	 => rst,
		data	 => GP_TIMER_CAM4_MBITS(0),
		q	 => TIMER_CAM_OUTPUT_BITS(3)
	);

gp_timer_latch_inst5 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => NOT_REQ_FOR_GP_TIMER_CAM5_WR_ENBL,
		sclr	 => rst,
		data	 => GP_TIMER_CAM5_MBITS(0),
		q	 => TIMER_CAM_OUTPUT_BITS(4)
	);

gp_timer_latch_inst6 : latch_w_enbl PORT MAP (
		clock	 => clk,
		enable	 => NOT_REQ_FOR_GP_TIMER_CAM6_WR_ENBL,
		sclr	 => rst,
		data	 => GP_TIMER_CAM6_MBITS(0),
		q	 => TIMER_CAM_OUTPUT_BITS(5)
	);


-- Add in the quad int signals
COMPLETE_TIMER_CAM_OUTPUT_BITS <= REQ_FOR_X4_SP_INTERRUPT_Q4 &
								    REQ_FOR_X4_SP_INTERRUPT_Q3 & 
									  REQ_FOR_X4_SP_INTERRUPT_Q2 &
										REQ_FOR_X4_SP_INTERRUPT_Q1 &
										  TIMER_CAM_OUTPUT_BITS;
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
						data_output <= ("000000000000000000000000" & POS_IPEND);
					when "0010"=>
						data_output <= ("000000000000000000000000" & POS_ISTATUS);  
					when "0011"=>
						data_output <= ("00000000000000" & TIMER_IMASK & POS_IMASK);  
					when "0100"=>
						null; -- Reserved  
					when "0101"=>
						data_output <= ("0000000000000000000000" & TIMER_IPEND);			
					when "0110"=>
						data_output <= ("0000000000000000000000" & TIMER_ISTATUS);	
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
			
			if (REQ_TO_DISABLE_VEL_INT = '1') then
				CONTROL_WORD(31) <= '0'; -- Disable velocity / timer interrupts
			else null;
			end if;
			
			if (WR_ENBL = '1') then
				case addr is
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

-- If writing the CAM, don't process CAM output bits
--POS_CAM_INT_PROCESS_ENBL <= not POS_CAM_WR_ENBL; 
POS_CAM_INT_PROCESS_ENBL <= FORCE_HIGH;
process (clk, rst)
variable PREV_CAM_OUTPUT_BITS : std_logic_vector(7 downto 0);
variable HAVE_READ_ISTATUS : std_logic;
variable CAM_BITS_THAT_HAVE_SET, RESULT : std_logic_vector(7 downto 0);
begin
	if (rst = '1') then
		CAM_BITS_THAT_HAVE_SET := "00000000";
		PREV_CAM_OUTPUT_BITS := "00000000";
		POS_IPEND <= "00000000";
		HAVE_READ_ISTATUS := '0';
		REQ_TO_DISABLE_POS_INT <= '0';
	elsif (clk 'event and clk = '1') then
		if (POS_CAM_INT_PROCESS_ENBL = '1') then
			
			REQ_TO_DISABLE_POS_INT <= '0';
			
			-- Reading the ISTATUS register will clear the IPEND register
			-- which will in turn clear the ISTATUS register.  Position
			-- interrupts will be disabled.  Reading the IPEND does not
			-- affect anything.
			if (POS_ISTATUS_RD_ENBL = '1') then
				HAVE_READ_ISTATUS := '1';
			else null;
			end if;
			
			if (HAVE_READ_ISTATUS = '1' and POS_ISTATUS_RD_ENBL = '0') then -- Just finished reading IPEND, ack interrupt at this time
				CAM_BITS_THAT_HAVE_SET := "00000000"; -- Ack interrupt by clearing changed bits that will clear the interrupt pin
				REQ_TO_DISABLE_POS_INT <= '1';
				HAVE_READ_ISTATUS := '0';	
			elsif (POS_CAM_OUTPUT_BITS /= PREV_CAM_OUTPUT_BITS) then
				-- By xoring the prev cam output bits with the updated cam output
				-- bits and then anding the result to the updated cam output bits,
				-- it can be determined which cam output bits changed from low
				-- to high, INT_CAM_BITS_THAT_HAVE_SET.  Then these are ored to 
				-- the current CAM_BITS_THAT_HAVE_SET to create an updated 
				-- CAM_BITS_THAT_HAVE_SET.  Cam output bits that go from 
				-- high to low are thus ignored.   
				CAM_BITS_THAT_HAVE_SET := CAM_BITS_THAT_HAVE_SET or ((PREV_CAM_OUTPUT_BITS xor POS_CAM_OUTPUT_BITS) and POS_CAM_OUTPUT_BITS);			
				PREV_CAM_OUTPUT_BITS := POS_CAM_OUTPUT_BITS;
			else null;
			end if;
			
			POS_IPEND <= CAM_BITS_THAT_HAVE_SET;
			
		else null;
		end if;
	else null;
	end if;
end process;

pos_interrupt <= '1' when ((POS_IPEND and not POS_IMASK) > "00000000")
						and POS_INT_ENBL = '1'
							else '0';
							
-- ISTATUS contains the non-masked IPEND bits that have generated an interrupt.
POS_ISTATUS <= (POS_IMASK xor POS_IPEND) and POS_IPEND;


---------------------- VELOCITY CODE ----------------------------------

VEL_ENBL_CTR_CLK_ENBL <= FORCE_HIGH;
--VEL_ENBL_CTR_CLK_ENBL <= not TIMER_CAM_WR_ENBL; -- Stop counting when writing cam
										
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


-- This process forces a count after a cam write when the vel clk is set
-- to divide by 2.  When using this process, the vel enbl ctr must be
-- disabled during cam writes.  This process uses an extra 100 LEs or so
-- but allows operation of vel clock at divide by 2 with no possibility
-- of missed cam matches after cam writes.
--process (clk, rst)
--	variable ENABLE_WAS_LOW : std_logic;
--	variable TIMER_CAM_WR_ENBL_WAS_HI : std_logic;
--	begin
--		if (rst = '1') then
--			VELOCITY_COUNT <= "00000000000000000000000000000000";
--			ENABLE_WAS_LOW := '0'; -- Assume enable was high, not critical
--			TIMER_CAM_WR_ENBL_WAS_HI := '0';
--		elsif (clk 'event and clk = '1') then
--			if ((TIMER_CAM_WR_ENBL_WAS_HI = '1' and TIMER_CAM_WR_ENBL = '0')
--					and VEL_CLK_DIVISOR = "0001") then
--				-- Force a count to compensate for the missed count
--				-- caused by writing the cam.  Only necessary if the
--				-- vel counter is set to divide by 2.  Any slower and
--				-- there will not be missed counts when writing cam.
--				VELOCITY_COUNT <= VELOCITY_COUNT + '1'; 
--				TIMER_CAM_WR_ENBL_WAS_HI := '0'; -- Not high anymore
--			elsif (VEL_CTR_CLK_ENBL = '1') then
--				if (ENABLE_WAS_LOW = '1') then -- First rise of enable, increment counter
--					VELOCITY_COUNT <= VELOCITY_COUNT + '1';
--					ENABLE_WAS_LOW := '0'; -- Not low any more
--				else null;
--				end if;
--			else null;	
--			end if;
--				
--			if (VEL_CTR_CLK_ENBL = '0') then
--				ENABLE_WAS_LOW := '1';
--			else null;
--			end if;
--			
--			if (TIMER_CAM_WR_ENBL = '1') then
--				TIMER_CAM_WR_ENBL_WAS_HI := '1';
--			else null;
--			end if;
--		
--		else null;
--     	end if;
--end process;


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


---------------- TIMER CAM INTERRUPT CODE ----------------------

-- If writing the CAM, don't process CAM output bits
--TIMER_CAM_INT_PROCESS_ENBL <= not TIMER_CAM_WR_ENBL; 
TIMER_CAM_INT_PROCESS_ENBL <= FORCE_HIGH;

process (clk, rst)
variable PREV_CAM_OUTPUT_BITS : std_logic_vector(9 downto 0);
variable HAVE_READ_ISTATUS : std_logic;
variable CAM_BITS_THAT_HAVE_SET, RESULT : std_logic_vector(9 downto 0);
begin
	if (rst = '1') then
		CAM_BITS_THAT_HAVE_SET := "0000000000";
		PREV_CAM_OUTPUT_BITS := "0000000000";
		TIMER_IPEND <= "0000000000";
		HAVE_READ_ISTATUS := '0';
		VELOCITY_CNT_ENBL <= '1';
		REQ_TO_DISABLE_VEL_INT <= '0';
		REQ_TO_CLEAR_SP_INTERRUPTS <= '0';
	elsif (clk 'event and clk = '1') then
		if (TIMER_CAM_INT_PROCESS_ENBL = '1') then
		
			REQ_TO_DISABLE_VEL_INT <= '0';
			REQ_TO_CLEAR_SP_INTERRUPTS <= '0';
			
			-- Reading the ISTATUS register will clear the IPEND register
			-- which will in turn clear the ISTATUS register.  Timer
			-- interrupts will be disabled.  Reading the IPEND does not
			-- affect anything.
			if (TIMER_ISTATUS_RD_ENBL = '1') then
				HAVE_READ_ISTATUS := '1';
			else null;
			end if;
			
			if (HAVE_READ_ISTATUS = '1' and TIMER_ISTATUS_RD_ENBL = '0') then -- Just finished reading IPEND, ack interrupt at this time
				CAM_BITS_THAT_HAVE_SET := "0000000000"; -- Ack interrupt by clearing changed bits that will clear the interrupt pin
				REQ_TO_DISABLE_VEL_INT <= '1';
				REQ_TO_CLEAR_SP_INTERRUPTS <= '1';
				HAVE_READ_ISTATUS := '0';	
			elsif (COMPLETE_TIMER_CAM_OUTPUT_BITS /= PREV_CAM_OUTPUT_BITS) then
				-- By xoring the prev cam output bits with the updated cam output
				-- bits and then anding the result to the updated cam output bits,
				-- it can be determined which cam output bits changed from low
				-- to high, INT_CAM_BITS_THAT_HAVE_SET.  Then these are ored to 
				-- the current CAM_BITS_THAT_HAVE_SET to create an updated 
				-- CAM_BITS_THAT_HAVE_SET.  Cam output bits that go from 
				-- high to low are thus ignored.
				CAM_BITS_THAT_HAVE_SET := CAM_BITS_THAT_HAVE_SET or 
					((PREV_CAM_OUTPUT_BITS xor COMPLETE_TIMER_CAM_OUTPUT_BITS) and 
							COMPLETE_TIMER_CAM_OUTPUT_BITS);
				
				PREV_CAM_OUTPUT_BITS := COMPLETE_TIMER_CAM_OUTPUT_BITS; 
				
				--CAM_BITS_THAT_HAVE_SET := (CAM_BITS_THAT_HAVE_SET or 
				--	((PREV_CAM_OUTPUT_BITS xor COMPLETE_TIMER_CAM_OUTPUT_BITS) and 
				--			COMPLETE_TIMER_CAM_OUTPUT_BITS)) and
				--				not TIMER_IMASK;			
				
				--PREV_CAM_OUTPUT_BITS := COMPLETE_TIMER_CAM_OUTPUT_BITS or TIMER_IMASK;
			else null;
			end if;
			
			TIMER_IPEND <= CAM_BITS_THAT_HAVE_SET;
			
		else null;
		end if;
	else null;
	end if;
end process;


timer_interrupt <= '1'	when ((TIMER_IPEND and not TIMER_IMASK) > "0000000000")
							and TIMER_INT_ENBL = '1'
								else '0';
--timer_interrupt <= '1' when TIMER_IPEND > "0000000000";

-- ISTATUS contains the non-masked IPEND bits that have generated an interrupt.
TIMER_ISTATUS <= (TIMER_IMASK xor TIMER_IPEND) and TIMER_IPEND;
--TIMER_ISTATUS <= TIMER_IPEND;
end behave;
