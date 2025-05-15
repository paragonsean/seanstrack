------------------- UBE_CONVERTER.VHD ------------------------
----------- Copyright 2004, Visi-Trak Worldwide -------------
--
-- This circuit takes in a Ube up pulse and down pulse and converts it into a
-- quadrature output


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;


entity ube_converter is
	port(
		clk			: in	std_logic;
		rst			: in	std_logic;  
		count_up	: in	std_logic;
		count_down	: in	std_logic;
		ch_a_out	: out	std_logic;	
		ch_b_out	: out	std_logic);  
end ube_converter;


architecture behave OF ube_converter is

signal COUNT_UP_INTERNAL, COUNT_DOWN_INTERNAL : std_logic;
signal CU, PREV_CU, CD, PREV_CD, A, B : std_logic;

begin

process (clk)
		begin
		if (clk 'event and clk = '1') then
			PREV_CU <= CU;
			CU <= count_up;
			PREV_CD <= CD;
			CD <= count_down;
		else null;
		end if;
end process;

COUNT_UP_INTERNAL <= '1' when ((CU /= PREV_CU) and (CU = '1')) else '0';
COUNT_DOWN_INTERNAL <= '1' when ((CD /= PREV_CD) and (CD = '1')) else '0';				

-- Quadrature generator Process.  This will assemble a
-- quadrature signal based on the seperate Ube count up or count down signals.
process (clk, rst)
		begin
		if rst = '1' then
			A <= '0';
			B <= '0';
			
		elsif (clk 'event and clk = '1') then
			if (COUNT_UP_INTERNAL = '1') then
				A <= '0';
				B <= '0';
				-- Count up
				if ((A = '0' and B = '0') or (A = '1' and B = '0')) then
					A <= '1';
				else null;
				end if; 
						
				if ((A = '1' and B = '0') or (A = '1' and B = '1')) then
					B <= '1';
				else null;	
				end if; 	
			elsif (COUNT_DOWN_INTERNAL = '1') then 
				A <= '0';
				B <= '0';
				-- Count down
				if ((A = '0' and B = '1') or (A = '1' and B = '1')) then
					A <= '1';
				else null;
				end if; 
						
				if ((A = '0' and B = '0') or (A = '0' and B = '1')) then
					B <= '1';
				else null;	
				end if;	
			else null;
			end if;
		else null;
		end if;	
end process;

ch_a_out <= A; 
ch_b_out <= B;

end behave;



