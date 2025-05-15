------------------- QUAD_DECODER.VHD ------------------------
----------- Copyright 2002, Visi-Trak Worldwide -------------
--
-- 2-8-02 This version of quad decoder outputs 4 speed pulses, 1 for each
-- of the quadrature quadrants.  If interfacing to a standard single 
-- quadrant velocity circuit, use the speed_pulse_main output as the speed pulse.

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;


entity quad_decoder is
	port(
		clk			: in	std_logic;
		rst			: in	std_logic;  
		ch_a		: in	std_logic;
		ch_b		: in	std_logic;
		ch_a_out	: out	std_logic;	 	-- Registered ch_a
		ch_b_out	: out	std_logic;  	-- Registered ch_b
		speed_pulse_q1	: out	std_logic;		-- Must update speed pulse if up_dn changes a'la Prince
		speed_pulse_q2	: out	std_logic;
		speed_pulse_q3	: out	std_logic;
		speed_pulse_main : out	std_logic; 
		up_dn		: out 	std_logic;
		x4			: out	std_logic;
		up_dn_changed_out : out std_logic;
		err			: out	std_logic); 
end quad_decoder;


architecture behave OF quad_decoder is

signal CNT : std_logic_vector(1 downto 0);
signal UP_DN_CHANGED : std_logic;
signal RAW_SP_Q1, RAW_SP_Q2, RAW_SP_Q3, RAW_SP_MAIN : std_logic;
signal X4_INTERNAL, UP_DN_INTERNAL : std_logic;
signal AAA, BBB, TMPAA, TMPBB : std_logic;
signal UD_CHANGED_FOR_OUTPUT : std_logic;

begin
-- This is a redesigned quadrature decoder that has been made completely
-- syncronous in operation.  There is no funky clocking or asyncronous signals in
-- this module. 



-- This process handles the UP_DN_CHANGED signal.  After
-- sensing that up down has changed, it waits for the main speed pulse
-- to go high.  After it goes high, it then waits for
-- it to go low.  Upon going low, the outputted up down changed
-- signal will be reset.  The main speed pulse is the last one to transition
-- after an up / down change, thus that one is used.
process (clk, rst)
		variable SP_MAIN_HAS_GONE_HI : std_logic;
		begin
		if (rst = '1') then
			SP_MAIN_HAS_GONE_HI := '0';
			UD_CHANGED_FOR_OUTPUT <= '0';
		elsif (clk 'event and clk = '1') then
			if (UP_DN_CHANGED = '1') then
				UD_CHANGED_FOR_OUTPUT <= '1';
			else null;
			end if;
								
			if (SP_MAIN_HAS_GONE_HI = '0' and RAW_SP_MAIN = '1') then
				SP_MAIN_HAS_GONE_HI := '1';
			else null;
			end if;
						
			if (SP_MAIN_HAS_GONE_HI = '1' and RAW_SP_MAIN = '0') then -- Falling edge of speed pulse
				UD_CHANGED_FOR_OUTPUT <= '0';
				SP_MAIN_HAS_GONE_HI := '0';	 
			else null;
			end if;
		else null;
		end if;
end process;

up_dn_changed_out <= UD_CHANGED_FOR_OUTPUT;


		
-- Speed pulse generator
--
-- This is basically a 2 bit counter that counts X4 pulses.  Every 4th.
-- pulse generates a speed pulse.  If a change in up_dn occurs, the counter
-- is reset.  Therefore speed pulses cannot occur if up_dn changes.  There must
-- be 4 X4 pulses after a reset or an up_dn change before a speed pulse can be
-- generated.
process (clk, rst)
		begin
		if rst = '1' or UP_DN_CHANGED = '1' then
			CNT <= "00";
			RAW_SP_Q1 <= '0';
			RAW_SP_Q2 <= '0';
			RAW_SP_Q3 <= '0';
			RAW_SP_MAIN <= '0';
		elsif (clk 'event and clk = '1') then	
			if (X4_INTERNAL = '1') then
				RAW_SP_Q1 <= '0';
				RAW_SP_Q2 <= '0';
				RAW_SP_Q3 <= '0';
				RAW_SP_MAIN <= '0';
				
				if (CNT = "00") then
					RAW_SP_Q1 <= '1';
				else null;
				end if;
				
				if (CNT = "01") then
					RAW_SP_Q2 <= '1';
				else null;
				end if;
				
				if (CNT = "10") then
					RAW_SP_Q3 <= '1';
				else null;
				end if;
				
				if (CNT = "11") then
					RAW_SP_MAIN <= '1';
				else null;
				end if;
				
				CNT <= CNT + '1';
					
			else null;
			end if;
		else null;
		end if;	
end process;

speed_pulse_q1 <= RAW_SP_Q1;
speed_pulse_q2 <= RAW_SP_Q2;
speed_pulse_q3 <= RAW_SP_Q3;
speed_pulse_main <= RAW_SP_MAIN;

-- Quadrature decoder
--
-- The quadrature decoder will decode the channel A and B transducer pulses
-- and generate the necessary signals for connecting the decoder to the
-- position and velocity counter circuits.
process (clk, rst)
	variable STATE	: std_logic_vector (3 downto 0);
	variable REQUEST_FOR_X4_PULSE, OK_FOR_X4_PULSE, PREV_UP_DN : std_logic;
	begin
		if (rst = '1') then  -- Async reset
			err <= '0';
			PREV_UP_DN := UP_DN_INTERNAL;
			REQUEST_FOR_X4_PULSE := '0';
			OK_FOR_X4_PULSE := '0';
			X4_INTERNAL <= '0';
			UP_DN_CHANGED <= '1';
			UP_DN_INTERNAL <= '1';
			AAA <= '0';
			BBB <= '0';
			TMPAA <= '0';
			TMPBB <= '0';
			STATE := "0000";
		elsif (clk 'event and clk = '1') then
		 	   AAA <= TMPAA; BBB <= TMPBB;		-- Preserve the current states
			   TMPAA <= ch_a; TMPBB <= ch_b;	-- And get the new states
			   STATE := BBB & AAA & TMPBB & TMPAA;	-- aaa and bbb previous ch_a and ch_b states, aa and bb are the current states
				
				X4_INTERNAL <= '0';	
				
				-- Now process 4x pulse requests.  If up_dn has changed, hold
				-- another clock cycle to allow up_dn to settle.
				if (PREV_UP_DN /= UP_DN_INTERNAL) then
					UP_DN_CHANGED <= '1'; 
					PREV_UP_DN := UP_DN_INTERNAL;
				elsif (REQUEST_FOR_X4_PULSE = '1') then
					OK_FOR_X4_PULSE := '1';
					REQUEST_FOR_X4_PULSE := '0';
				elsif (OK_FOR_X4_PULSE = '1') then 	
					X4_INTERNAL <= '1';
					OK_FOR_X4_PULSE := '0';
					UP_DN_CHANGED <= '0';
				else null;
				end if;
				
				case STATE is
					when "0001"|"0111"|"1110"|"1000"=> -- Count up states
						UP_DN_INTERNAL <= '1'; -- Active high for count up
						REQUEST_FOR_X4_PULSE := '1';
					when "0010"|"1011"|"1101"|"0100"=> -- Count down states
						UP_DN_INTERNAL <= '0';
						REQUEST_FOR_X4_PULSE := '1';
					when "0011"|"0110"|"1001"|"1100"=> -- Error states
						err <= '1';
						X4_INTERNAL <= '0';
						REQUEST_FOR_X4_PULSE := '0';
					when others=>
						null;
				end case;
		else null;
		end if;
end process;

x4 <= X4_INTERNAL;
up_dn <= UP_DN_INTERNAL;
ch_a_out <= TMPAA;
ch_b_out <= TMPBB;

end behave;



