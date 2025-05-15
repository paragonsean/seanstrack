------------------- DIV_BY_5.VHD ------------------------
----------- Copyright 2003, Visi-Trak Worldwide -------------
--
-- Programmable quadrature Divide by N function
--
--
-- 7-14-03 This entity takes a quadrature signal as an input,
-- runs it through a simple quad. decoder, divides the X4 pulse
-- by 5 if the input divby5 is true and regenerates a quadrature
-- signal.  This can then be sent to the standard quad decoder /
-- pv unit combo and processed as any other quadrature signal.
-- If divby5 is low, the input quadrature is passed
-- through undivided. 
--
-- 9-24-03 Made divider function programmable from 1 to 15,
-- with a 4 bit selector nibble.


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;


entity div_by_5 is
	port(
		clk			: in	std_logic;
		rst			: in	std_logic;  
		ch_a		: in	std_logic;
		ch_b		: in	std_logic;
		divisor		: in	std_logic_vector(3 downto 0);
		--divby5		: in	std_logic;
		ch_a_out	: out	std_logic;	
		ch_b_out	: out	std_logic);  
end div_by_5;


architecture behave OF div_by_5 is

signal X4_INTERNAL, DIVIDED_X4, UP_DN_INTERNAL : std_logic;
signal AAA, BBB, TMPAA, TMPBB, A, B : std_logic;

begin

-- Simple quadrature decoder.  No error checking is done.
-- To divide a quadrature, UD and X4 signals must be generated.
-- We'll generate these here.  Other processes will divide
-- the X4 by 5 and reconstitute the quadrature.
process (clk, rst)
	variable STATE	: std_logic_vector (3 downto 0);
	variable REQUEST_FOR_X4_PULSE, OK_FOR_X4_PULSE : std_logic;
	begin
		if (rst = '1') then  -- Async reset
			REQUEST_FOR_X4_PULSE := '0';
			OK_FOR_X4_PULSE := '0';
			X4_INTERNAL <= '0';
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
				if (REQUEST_FOR_X4_PULSE = '1') then
					OK_FOR_X4_PULSE := '1';
					REQUEST_FOR_X4_PULSE := '0';
				elsif (OK_FOR_X4_PULSE = '1') then 	
					X4_INTERNAL <= '1';
					OK_FOR_X4_PULSE := '0';
				else null;
				end if;
				
				case STATE is
					when "0001"=> -- Count up quadrant #1
						UP_DN_INTERNAL <= '1'; -- Active high for count up
						REQUEST_FOR_X4_PULSE := '1';
					when "0111"=> -- Count up quadrant #2
						UP_DN_INTERNAL <= '1'; -- Active high for count up
						REQUEST_FOR_X4_PULSE := '1';
					when "1110"=> -- Count up quadrant #3
						UP_DN_INTERNAL <= '1'; -- Active high for count up
						REQUEST_FOR_X4_PULSE := '1';
					when "1000"=> -- Count up quadrant #4
						UP_DN_INTERNAL <= '1'; -- Active high for count up
						REQUEST_FOR_X4_PULSE := '1';
					when "0010"=> -- Count down quadrant #4
						UP_DN_INTERNAL <= '0';
						REQUEST_FOR_X4_PULSE := '1';
					when "1011"=> -- Count down quadrant #3
						UP_DN_INTERNAL <= '0';
						REQUEST_FOR_X4_PULSE := '1';
					when "1101"=> -- Count down quadrant #2
						UP_DN_INTERNAL <= '0';
						REQUEST_FOR_X4_PULSE := '1';
					when "0100"=> -- Count down quadrant #1
						UP_DN_INTERNAL <= '0';
						REQUEST_FOR_X4_PULSE := '1';
					when "0011"|"0110"|"1001"|"1100"=> -- Error states
						X4_INTERNAL <= '0';
						REQUEST_FOR_X4_PULSE := '0';
					when others=>
						null;
				end case;
		else null;
		end if;
end process;


-- Divider process
process (clk, rst)
		variable DIVCNT	: std_logic_vector (3 downto 0);
		variable PREV_UD : std_logic;
		begin
		if rst = '1' then
			DIVCNT := "0000";
			PREV_UD := '1'; -- UP_DN_INTERNAL resets to 1
			
		elsif (clk 'event and clk = '1') then
			DIVIDED_X4 <= '0';
			-- Sense UD changes based on the raw undivided
			-- UD signal.  Reset the divided UD signal if
			-- UD changes.
			if (PREV_UD /= UP_DN_INTERNAL) then
				DIVCNT := "0000";
				DIVIDED_X4 <= '1'; -- Send an X4 pulse when direction changes
			elsif (X4_INTERNAL = '1') then
				--if (DIVCNT < "100") then				
				if (DIVCNT < (divisor-1)) then
					DIVCNT := DIVCNT + '1';
				--elsif (DIVCNT = "100") then
				elsif (DIVCNT = (divisor-"0001")) then
					DIVIDED_X4 <= '1';
					DIVCNT := "0000";	 
				else null;
				end if;
			else null;
			end if;
			PREV_UD := UP_DN_INTERNAL;
		else null;
		end if;	
end process;


-- Quadrature generator Process.  This will assemble a
-- quadrature signal based on the divided x4 signal.
process (clk, rst)
		variable DIVCNT	: std_logic_vector (2 downto 0);
		variable PREV_UD : std_logic;
		begin
		if rst = '1' then
			A <= '0';
			B <= '0';
			
		elsif (clk 'event and clk = '1') then
			if (DIVIDED_X4 = '1') then
				A <= '0';
				B <= '0';
				if (UP_DN_INTERNAL = '1') then
					-- Count up
					if ((A = '0' and B = '0') or (A = '1' and B = '0')) then
						A <= '1';
					else null;
					end if; 
						
					if ((A = '1' and B = '0') or (A = '1' and B = '1')) then
						B <= '1';
					else null;	
					end if; 	
				else 
					-- Count down
					if ((A = '0' and B = '1') or (A = '1' and B = '1')) then
						A <= '1';
					else null;
					end if; 
						
					if ((A = '0' and B = '0') or (A = '0' and B = '1')) then
						B <= '1';
					else null;	
					end if;	
				end if;
			else null;
			end if;
		else null;
		end if;	
end process;

--ch_a_out <= A when divby5 = '1' else ch_a; 
--ch_b_out <= B when divby5 = '1' else ch_b;
ch_a_out <= A when divisor > "0001" else ch_a; 
ch_b_out <= B when divisor > "0001" else ch_b;

end behave;



