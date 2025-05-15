//Legal Notice: (C)2005 Altera Corporation. All rights reserved.  Your
//use of Altera Corporation's design tools, logic functions and other
//software and tools, and its AMPP partner logic functions, and any
//output files any of the foregoing (including device programming or
//simulation files), and any associated documentation or information are
//expressly subject to the terms and conditions of the Altera Program
//License Subscription Agreement or other applicable license agreement,
//including, without limitation, that your use is for the sole purpose
//of programming logic devices manufactured by Altera and sold by Altera
//or its authorized distributors.  Please refer to the applicable
//agreement for further details.


// turn off bogus verilog processor warnings 
// altera message_off 10034 10035 10036 10037 10230 

// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on
module ad_rom_lane0_module (
                             // inputs:
                              address,
                              clk,
                              reset_n,

                             // outputs:
                              q
                           )
;

  output  [ 15: 0] q;
  input   [  6: 0] address;
  input            clk;
  input            reset_n;

  reg     [ 15: 0] mem_array [127: 0];
  wire    [ 15: 0] q;
  reg     [  6: 0] read_address;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          read_address <= 0;
      else if (1)
          read_address <= address;
    end


  // Data read is synchronized through latent_rdaddress.
  assign q = mem_array[read_address];

initial
    $readmemh("ad_rom_lane0.dat", mem_array);

//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  always @(address)
//    begin
//      if (1)
//          read_address <= address;
//    end
//
//
//  lpm_rom lpm_rom_component
//    (
//      .address (read_address),
//      .inclock (clk),
//      .q (q)
//    );
//
//  defparam lpm_rom_component.lpm_address_control = "REGISTERED",
//           lpm_rom_component.lpm_file = "ad_rom_lane0.mif",
//           lpm_rom_component.lpm_outdata = "UNREGISTERED",
//           lpm_rom_component.lpm_width = 16,
//           lpm_rom_component.lpm_widthad = 7,
//           lpm_rom_component.suppress_memory_conversion_warnings = "ON";
//
//synthesis read_comments_as_HDL off

endmodule


module ad_rom (
                // inputs:
                 address,
                 clk,
                 reset_n,

                // outputs:
                 readdata
              )
;

  output  [ 15: 0] readdata;
  input   [  6: 0] address;
  input            clk;
  input            reset_n;

  wire    [  6: 0] ad_rom_lane0_address;
  wire    [ 15: 0] q_lane_0;
  wire    [ 15: 0] readdata;
  wire             select_all_chunks;
  assign select_all_chunks = 1;
  assign ad_rom_lane0_address = address;
  //Atomic memory instance: ad_rom_lane0
  //Size: 128(words) X 16(bits)

  ad_rom_lane0_module ad_rom_lane0
    (
      .address (ad_rom_lane0_address),
      .clk     (clk),
      .q       (q_lane_0),
      .reset_n (reset_n)
    );

  assign readdata = q_lane_0;
  //s1, which is an e_avalon_slave
  //s2, which is an e_avalon_slave

endmodule

