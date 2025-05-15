------------------- QUAD_ENCODER.VHD ------------------------
----------- Copyright 2007, Visi-Trak Worldwide -------------
--
-- Quadrature encoder function 
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;


entity quad_encoder is
	port(
		clk			: in	std_logic;
		rst			: in	std_logic;  
		x4			: in	std_logic;
		ud			: in	std_logic;
		ch_a_out	: out	std_logic;	
		ch_b_out	: out	std_logic);  
end quad_encoder;


architecture behave OF quad_encoder is

signal A, B, PREV_X4 : std_logic;

begin

-- Quadrature generator Process.  This will assemble a
-- quadrature signal based on the x4 and ud signals.
process (clk, rst)
		begin
		if rst = '1' then
			A <= '0';
			B <= '0';
			PREV_X4 <= x4;
			
		elsif (clk 'event and clk = '1') then
			PREV_X4 <= x4;
			if ((x4 /= PREV_X4) and x4 = '1') then -- Capture x4 rising edge
				A <= '0';
				B <= '0';
				if (ud = '1') then
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

ch_a_out <= A; 
ch_b_out <= B;

end behave;



