--------- U_SHIFTREG_16.VHD SYNC VERSION -----------------
----------------- Copyright 2009 Visi-Trak Worldwide -------------------
-- 2-9-09 
--
--   


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


-- 16 bit universal shift register that functions like a 16 bit version of the 74194.
entity u_shiftreg_16 is
	port(
		clk : in	std_logic; 
		rst : in	std_logic; 
		clk_enbl : in	std_logic; 
		rin : in	std_logic; 
		lin	: in	std_logic; 
		s : in    std_logic_vector(1 downto 0); -- function select
		d : in	std_logic_vector(15 downto 0); 
		--
		q	: out std_logic_vector(15 downto 0));
end u_shiftreg_16;


architecture behave OF u_shiftreg_16 is

signal IQ: std_logic_vector(15 downto 0);
begin
	process (clk, rst, IQ)
	begin
		if (rst = '1') then
			IQ <= (others=>'0'); -- Async clear
		elsif (clk'event and clk='1') then
			if (clk_enbl = '1') then
				case s is
					when "00" => null;    -- Hold
					when "01" => IQ <= d; -- Load
					when "10" => IQ <= rin & IQ(15 downto 1);  -- Shift right
					when "11" => IQ <= IQ(14 downto 0) & lin; -- Shift left
					when others => null;
				end case;
			else null;
			end if;	
		else null;
		end if;
	end process;
	
q <= IQ;

end behave;
