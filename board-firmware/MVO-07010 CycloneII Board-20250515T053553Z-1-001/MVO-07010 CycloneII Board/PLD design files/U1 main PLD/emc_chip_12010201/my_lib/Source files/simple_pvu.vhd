--------- SIMPLE PVU-----------------
----------------- Copyright 2003 Visi-Trak Worldwide -------------------
--
-- Combination position / velocity unit without cams


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-- The position_velocity_interface_unit consists of a 24 bit position unit
-- and a 32 bit velocity unit.  

-- Bit 31 of speed pulse latched data is the up_down signal.  Bit 30 is
-- the quad_err signal.  That leaves 30 bits (29 downto 0) for velocity
-- timing data.
--
-- This module is intended to interface to a quadrature decoder.


-- NIOS BUST BE SETUP FOR 1 WAIT STATE WHEN READING / WRITING THIS PERIPHERAL.

entity simple_pvu is
	port(
		clk			: in	std_logic; 
		rst			: in	std_logic;
		preload     : in	std_logic;
		up_dn		: in	std_logic;
		x4			: in	std_logic;
		speed_pulse_q1	: in	std_logic;
		cs			: in	std_logic;
		addr		: in	std_logic_vector(3 downto 0);
		readn		: in	std_logic;
		writen		: in	std_logic;
		quad_err	: in	std_logic;
		-- For simulation
		--temp_pos_out : out std_logic_vector(23 downto 0);  
		--temp_raw_v_count_out : out std_logic_vector(31 downto 0);
		--temp_velocity_out : out std_logic_vector(31 downto 0);
		--enable_velocity_ctr_out : out std_logic;
		--velocity_clk_divisor_out : out std_logic_vector(3 downto 0);
		--
		data_input  : in std_logic_vector(31 downto 0);
		data_output : out std_logic_vector(31 downto 0));
end simple_pvu;


architecture behave OF simple_pvu is

-- COMMON STUFF
signal WR_ENBL, RD_ENBL, FORCE_HIGH, FORCE_LOW : std_logic;
signal POSITION_PRELOAD : std_logic_vector(23 downto 0);
signal Q1_HAS_CHANGED : std_logic;

-- VELOCITY / SP SPECIFIC STUFF
signal VELOCITY_COUNT : std_logic_vector(31 downto 0);
signal VEL_CLK_DIVISOR : std_logic_vector(3 downto 0);
signal VEL_CTR_CLK_ENBL, VEL_ENBL_CTR_CLK_ENBL, ENABLE_VELOCITY_CTR : std_logic;
signal LAST_SP_WAS_Q1 : std_logic;
signal VELOCITY : std_logic_vector(31 downto 0);
signal PREVIOUS_LATCHED_V_COUNT_Q1, LATCHED_V_COUNT_Q1 : std_logic_vector(31 downto 0);

-- POSITION SPECIFIC STUFF
signal POSITION_COUNT : std_logic_vector(23 downto 0);
signal POSITION_COUNT_32BIT : std_logic_vector(31 downto 0);
signal POS_CTR_ENBL : std_logic;

begin
FORCE_HIGH <= '1';
FORCE_LOW <= '0';

WR_ENBL <= '1' when (cs = '1' and writen = '0') else '0'; -- This unit selected and write enable active low	
RD_ENBL <= '1' when (cs = '1' and readn = '0') else '0';


------------------- FOR SIMULATION ----------------------

--temp_pos_out <= POSITION_COUNT;		-- For simulation
--temp_raw_v_count_out <= VELOCITY_COUNT;
--temp_velocity_out <= VELOCITY;
--enable_velocity_ctr_out <= ENABLE_VELOCITY_CTR;
--velocity_clk_divisor_out <= VEL_CLK_DIVISOR;




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
						null;
					when "0010"=>
						null;  
					when "0011"=>
						null; 
					when "0100"=>
						null;  
					when "0101"=>
						data_output <= ("00000000" & POSITION_PRELOAD); 			
					when "0110"=>
						data_output <= ("0000000000000000000000000000" & VEL_CLK_DIVISOR); 	
					when "0111"=>  
						null;
					when "1000"=>  
						null;
					when "1001"=>  
						null;
					when "1010"=>  
						null;			
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
			null;
		elsif (clk 'event and clk = '1') then
			if (WR_ENBL = '1') then
				case addr is
					when "1100"=>
						null;  
					when "1101"=>
						null;
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
	variable PREV_Q1 : std_logic;
	begin
		if (rst = '1') then
			PREVIOUS_LATCHED_V_COUNT_Q1 <= "00000000000000000000000000000000";
			Q1_HAS_CHANGED <= '0';
			LAST_SP_WAS_Q1 <= '0';
			PREV_Q1 := '1'; -- Assume high at reset
			
		elsif (clk 'event and clk = '1') then
			LAST_SP_WAS_Q1 <= '0';
			Q1_HAS_CHANGED <= '0';		
						
			if (speed_pulse_q1 /= PREV_Q1) then
				PREV_Q1 := speed_pulse_q1;
				if (speed_pulse_q1 = '1') then
					Q1_HAS_CHANGED <= '1';
				else null;
				end if;	
			else null;
			end if;
			
			if (Q1_HAS_CHANGED = '1') then -- SPQ1 has went lo to hi
				 -- Speed pulse has triggered
					PREVIOUS_LATCHED_V_COUNT_Q1 <= LATCHED_V_COUNT_Q1; -- Save present value
					LAST_SP_WAS_Q1 <= '1';
					LATCHED_V_COUNT_Q1 <= up_dn & quad_err & VELOCITY_COUNT(29 downto 0);
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
