------------------- WATCHDOG.VHD ------------------------
----------- Copyright 2007, Visi-Trak Worldwide -------------
--
-- Watchdog timer
--
-- Watchdog will count down and reach zero in one half second if not reset.
-- Constant resetting will keep the watchdog from timing out.


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;


entity stand_alone_watchdog is
	port(
		clk			: in	std_logic;
		rst			: in	std_logic;  
		timeout		: out	std_logic);  
end stand_alone_watchdog;


architecture behave OF stand_alone_watchdog is

signal COUNT	: std_logic_vector (24 downto 0);

begin

-- Counter process
--process (clk)
--		begin
--		if (clk 'event and clk = '1') then
--			if rst = '1' then -- Syncronous reset
--				COUNT <= "1011111010111100001000000"; -- 0x17D7840, .75 sec @ 33.33MHz
--			elsif (COUNT >= "0000000000000000000000001") then	
--				COUNT <= COUNT - "0000000000000000000000001";
--			else null;	
--			end if;	
--		else null;
--		end if;	
--end process;

-- Counter process
process (clk, rst)
		begin
		if (rst = '1') then
			COUNT <= "1011111010111100001000000"; -- 0x17D7840, .75 sec @ 33.33MHz
		elsif (clk 'event and clk = '1') then
			if (COUNT /= "0000000000000000000000000") then	
				COUNT <= COUNT - "0000000000000000000000001";
			else null;	
			end if;	
		else null;
		end if;	
end process;

timeout <= '1' when (COUNT = "0000000000000000000000000") else '0';

end behave;



