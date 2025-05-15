--------- SPI_16.VHD SYNC VERSION -----------------
----------------- Copyright 2009 Visi-Trak Worldwide -------------------
-- 2-9-09 
-- This is a 16 bit SPI master that seeks to imitate the function of Altera's SPI
-- peripheral.  The polarity input defines the inactive or off state of the sclk output.
-- A polarity setting of zero means that sclk is low during its inactive state.  A setting
-- of one indicates that sclk is on during its inactive state.
--
-- The phase input defines what half of the sclk cycle is the active half, that is the
-- half of the cycle that clocks data into the external device connected to the SPI.
-- Setting phase to zero, means that the first half of the sclk cycle is the active half.
-- Setting phase to one indicates that the 2nd. half of sclk is the active half.
--
-- clk_divisor divides the clk from 1 to 15 to control the frequency of sclk.  sclk
-- frequency will always be half of the divided clk.  If clk_divisor is set to 1 and
-- the clk frequency is 33.33MHz then sclk will be 16.67MHz
--   
-- A complete xmit or xmit/rec cycle consists of 18 steps.  Step 1 is a prolog. Steps 2
-- to 17 are the 16 sclk steps to xmit data to the external device.  Step 18 is an epilog.
-- Each of the 18 steps is broken in half and tracked with a counter that counts from
-- 0 to 35.  There are two counter counts per step.


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity spi_16 is
	port(
		clk			: in	std_logic;
		rst			: in	std_logic;
		phase		: in	std_logic; -- 0 = first half of sclk is active, 1 = 2nd. half is active
		polarity    : in	std_logic; -- 0 = sclk is zero when idle. 1 = sclk is one when idle.
		msb_first	: in 	std_logic;
		miso        : in	std_logic;
		wr			: in	std_logic; -- Load value to be written and initiate a complete cycle, this will clear data_ready
		clk_divisor : in    std_logic_vector(3 downto 0); -- To allow dividing the clock from 1 to 15
		peripheral_sel : in    std_logic_vector(2 downto 0); 
		data_to_peripheral : in std_logic_vector(15 downto 0);
		--
		--s_rec_out		: out std_logic_vector(1 downto 0); -- For sim
		--wr_xmit_out : out std_logic; -- For sim
		mosi		: out std_logic;
		sclk	    : out std_logic;
		ssn			: out std_logic_vector(7 downto 0);
		data_from_peripheral : out std_logic_vector(15 downto 0));
end spi_16;


architecture behave OF spi_16 is

-- Universal 16 bit shift register
component u_shiftreg_16 is
	port(
		clk, rst, clk_enbl, rin, lin : in	std_logic; 
		s : in    std_logic_vector(1 downto 0); -- function select
		d : in	std_logic_vector(15 downto 0);
		--
		q : out std_logic_vector(15 downto 0));
end component;


-- Define u_shiftreg_16 function select inputs
constant HOLD : std_logic_vector := "00";
constant LOAD : std_logic_vector := "01";
constant SHIFT_RT : std_logic_vector := "10"; -- For MSB first peripherals
constant SHIFT_LT : std_logic_vector := "11"; -- For LSB first peripherals

signal SCLKK, SSN_ENBL, BEGIN_CYC, CLOCK_ENBL, WR_DETECTED, WR_XMIT : std_logic;
signal Q_XMIT : std_logic_vector(15 downto 0);
signal RIN_REC, LIN_REC : std_logic;
signal S_REC, S_XMIT, SHIFT : std_logic_vector(1 downto 0);

begin
--s_rec_out <= S_REC; -- For sim
--wr_xmit_out <= WR_XMIT; -- For sim

-- Always parallel loaded therefore we'll ground the serial input pins.
-- This transmits data to the peripheral.  If the peripheral sends data back, it will be received by the
-- the second shift register below.
xmit_shift_reg_inst1: u_shiftreg_16 port map(
		clk			=> clk,
		rst			=> rst,  
		clk_enbl	=> CLOCK_ENBL,
		rin			=> '0',  
		lin			=> '0',  
		s			=> S_XMIT,
		d			=> data_to_peripheral,
		q			=> Q_XMIT
		);				

-- Always serially loaded, ground the parallel load input pins.
-- This recieves data from the peripheral as data is being clocked to the peripheral.		
rec_shift_reg_inst1: u_shiftreg_16 port map(
		clk			=> clk,
		rst			=> rst,  
		clk_enbl	=> CLOCK_ENBL, 
		rin			=> RIN_REC,  
		lin			=> LIN_REC,  
		s			=> S_REC,
		d			=> (others=>'0'),  
		q			=> data_from_peripheral
		);		

		
mosi <= Q_XMIT(15) when msb_first = '1' else Q_XMIT(0); -- Transmitted serial data to peripheral	
LIN_REC <= miso when msb_first = '1' else '0'; -- Shift in MSB first, received serial data from peripheral
RIN_REC <= miso when msb_first = '0' else '0'; -- - or - Shift in LSB first
SHIFT <= SHIFT_LT when msb_first = '1' else SHIFT_RT; -- Value we'll send to the S inputs on the shift registers
ssn(0) <= '0' when peripheral_sel = "000" and SSN_ENBL = '1' else '1';
ssn(1) <= '0' when peripheral_sel = "001" and SSN_ENBL = '1' else '1';
ssn(2) <= '0' when peripheral_sel = "010" and SSN_ENBL = '1' else '1';
ssn(3) <= '0' when peripheral_sel = "011" and SSN_ENBL = '1' else '1';
ssn(4) <= '0' when peripheral_sel = "100" and SSN_ENBL = '1' else '1';
ssn(5) <= '0' when peripheral_sel = "101" and SSN_ENBL = '1' else '1';
ssn(6) <= '0' when peripheral_sel = "110" and SSN_ENBL = '1' else '1';
ssn(7) <= '0' when peripheral_sel = "111" and SSN_ENBL = '1' else '1';

-- Process to extend wr pulse to clock enable so it gets detected if dividing down
-- the clock.  
process (clk, rst)
begin
if (rst = '1') then
	WR_DETECTED <= '0';
	WR_XMIT <= '0';
elsif (clk'event and clk = '1') then
	if (wr = '1') then
		WR_DETECTED <= '1';
	else null;
	end if;
	if (CLOCK_ENBL = '1') then
		WR_XMIT <= '0'; -- Defaults
				
		if (WR_DETECTED = '1') then
			WR_XMIT <= '1';
			WR_DETECTED <= '0';
		else null;
		end if;
	else null;
	end if;
else null;
end if;	
end process;

-- Clock divider process
process (clk, rst)
variable CTR : std_logic_vector(3 downto 0);
begin
	if (rst = '1') then
		CTR := "0000";
	elsif (clk'event and clk = '1') then
		CLOCK_ENBL <= '0'; -- Default
		
		CTR := CTR + "0001";
		if (CTR >= clk_divisor) then -- A clk_divisor of "0000" will be considered div. by 1
			CLOCK_ENBL <= '1';
			CTR := "0000";
		else null;
		end if;	
	else null;
	end if;
end process;


-- Process to transmit data to the peripheral.  The SCLKK frequency is one half of the enabled clock input.
-- Frequency of the SPI is controlled by the external entity connecting to the SPI by controlling the clk_enbl input.
process (clk, rst, polarity)
variable CTR : std_logic_vector(5 downto 0);
begin
	if (rst = '1') then
		CTR := "000000"; -- 2 counts per SCLKK cycle, 16 cycles and a total of 32 counts + 2 counts initialization and 2 counts for the end of cycle.
		S_XMIT <= HOLD; 
		S_REC <= HOLD;
		SSN_ENBL <= '0';
		BEGIN_CYC <= '0';
		if (polarity = '1') then
			SCLKK <= '1';  -- Normally high when idle
		else SCLKK <= '0'; -- Low when idle
		end if;
	elsif (clk'event and clk='1') then
		if (CLOCK_ENBL = '1') then
			SSN_ENBL <= '0'; 
			S_XMIT <= HOLD; -- Next bit will be shifted now as we're preparing to HOLD for the next half
			S_REC <= HOLD;
			BEGIN_CYC <= '0';
						
			-- Load xmit shift register and prepare for a cycle
			if (WR_XMIT = '1') then -- Begin a new cycle
				S_XMIT <= LOAD;
				BEGIN_CYC <= '1';
			else null;
			end if; 
			
			if (BEGIN_CYC = '1' or CTR /= "000000") then
				-- If the phase = 0, then the first half of the SCLKK cycle is the active half.
				-- If this is the case, we need to prep the receive shift register to shift incoming
				-- data immediately when the cycle begins starting with count "000010" below. 
				-- We'll ignore count "000000", nothing to do there.
				
				-- Counts 0 and 1 are "prolog" to the 16 sclk cycles.  sclk is not toggled
				-- during the prolog.  Nothing is done on count 0.
				if (CTR = "000001") then
					if (phase = '0') then 
						S_REC <= SHIFT;
					else null;
					end if;
				else null;
				end if;	
				
				-- Counts 2 to 34 are the 16 sclk cycles, 2 counts per cycle
				if (CTR > "000001" and CTR < "100010") then  -- One of 16 SCLKK cycles, two CTR counts per SCLKK cycle
					SCLKK <= not SCLKK; -- Toggle SCLKK, polarity determines the starting level
					
					-- Data is shifted out the shift register in the "off" SCLKK portion of the cycle.
					-- The shift register is set up during the "on" or active portion.  Phase determines what half
					-- of an SCLK_cycle is acrive and what half is inactive.  That's why there is 2 counter counts
					-- per SCLKK cycle.
					-- If phase = 0, data is clocked during the first half of SCLKK.  The shift register is
					-- set up such that data will be shifted during the next half of SCLKK, the inactive period.
					--
					-- If phase = 1, the opposite is true.  The 2nd. half of SCLKK is active and the first half is 
					-- inactive.  Data to the peipheral is always clocked during the active portion of SCLKK and
					-- the shift register is set up at this time to be shifted during the inactive half of the cycle.
					if ((CTR(0) = '0' and phase = '0') or (CTR(0) = '1' and phase = '1')) then -- Active half of SCLKK
						S_XMIT <= SHIFT; -- Prep xmit for shift during the active half of SCLKK as previous bit shifted is clocked into the peripheral
					else 
						if (CTR /= "100001") then -- If final SCLKK count, we don't want to shift the rec SR anymore, we already have all the bits. 
							S_REC <= SHIFT; -- Prep rec for shift during the inactive half of the cycle
						else null;
						end if;	
					end if;
				else 
					null;
				end if;
				
				-- Counts 34 and 35 are the "epilog" after the 16 sclk cycles. We'll do 
				-- nothing on count 34 and terminate the cycle on count 35.  The sclk
				-- is not toggled during the epilog.
				if (CTR = "100011") then
					CTR := "000000";
					SSN_ENBL <= '0'; -- Disable SSN when count (cycle) complete
				else 
					CTR := CTR + "000001";
					SSN_ENBL <= '1'; -- Keep SSN active while we're counting
				end if;	
			else null;
			end if;			
		else null;
		end if;
	else null;
	end if;
end process;

sclk <= SCLKK;

end behave;
