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
module enet_boot_rom_lane0_chunk_256_module (
                                              // inputs:
                                               address,
                                               clk,
                                               reset_n,

                                              // outputs:
                                               q
                                            )
;

  output  [ 31: 0] q;
  input   [  7: 0] address;
  input            clk;
  input            reset_n;

  reg     [ 31: 0] mem_array [255: 0];
  wire    [ 31: 0] q;
  reg     [  7: 0] read_address;

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
    $readmemh("enet_boot_rom_lane0_chunk_256.dat", mem_array);

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
//           lpm_rom_component.lpm_file = "enet_boot_rom_lane0_chunk_256.mif",
//           lpm_rom_component.lpm_outdata = "UNREGISTERED",
//           lpm_rom_component.lpm_width = 32,
//           lpm_rom_component.lpm_widthad = 8,
//           lpm_rom_component.suppress_memory_conversion_warnings = "ON";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_boot_rom_lane0_leftover_module (
                                             // inputs:
                                              address,
                                              clk,
                                              reset_n,

                                             // outputs:
                                              q
                                           )
;

  output  [ 31: 0] q;
  input   [  6: 0] address;
  input            clk;
  input            reset_n;

  reg     [ 31: 0] mem_array [127: 0];
  wire    [ 31: 0] q;
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
    $readmemh("enet_boot_rom_lane0_leftover.dat", mem_array);

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
//           lpm_rom_component.lpm_file = "enet_boot_rom_lane0_leftover.mif",
//           lpm_rom_component.lpm_outdata = "UNREGISTERED",
//           lpm_rom_component.lpm_width = 32,
//           lpm_rom_component.lpm_widthad = 7,
//           lpm_rom_component.suppress_memory_conversion_warnings = "ON";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_boot_rom (
                       // inputs:
                        address,
                        clk,
                        reset_n,

                       // outputs:
                        readdata
                     )
;

  output  [ 31: 0] readdata;
  input   [  8: 0] address;
  input            clk;
  input            reset_n;

  wire    [  7: 0] enet_boot_rom_lane0_chunk_256_address;
  wire             enet_boot_rom_lane0_chunk_256_hi_addr;
  wire    [  7: 0] enet_boot_rom_lane0_chunk_256_lo_addr;
  wire    [  6: 0] enet_boot_rom_lane0_leftover_address;
  reg              mux_select_enet_boot_rom_lane0_chunk_256;
  wire    [ 31: 0] q_enet_boot_rom_lane0_chunk_256;
  wire    [ 31: 0] q_enet_boot_rom_lane0_leftover;
  wire    [ 31: 0] q_lane_0;
  wire    [ 31: 0] readdata;
  wire             select_all_chunks;
  wire             select_enet_boot_rom_lane0_chunk_256;
  wire             select_enet_boot_rom_lane0_leftover;
  assign select_all_chunks = 1;
  assign {enet_boot_rom_lane0_chunk_256_hi_addr, enet_boot_rom_lane0_chunk_256_lo_addr} = address;
  assign select_enet_boot_rom_lane0_chunk_256 = (~|(enet_boot_rom_lane0_chunk_256_hi_addr)) && select_all_chunks;
  assign select_enet_boot_rom_lane0_leftover = ( |(enet_boot_rom_lane0_chunk_256_hi_addr)) &&select_all_chunks;
  assign q_lane_0 = (mux_select_enet_boot_rom_lane0_chunk_256)? q_enet_boot_rom_lane0_chunk_256 :
    q_enet_boot_rom_lane0_leftover;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          mux_select_enet_boot_rom_lane0_chunk_256 <= 0;
      else if (1'b1)
          mux_select_enet_boot_rom_lane0_chunk_256 <= select_enet_boot_rom_lane0_chunk_256;
    end


  assign enet_boot_rom_lane0_chunk_256_address = address;
  //Atomic memory instance: enet_boot_rom_lane0_chunk_256
  //Size: 256(words) X 32(bits)

  enet_boot_rom_lane0_chunk_256_module enet_boot_rom_lane0_chunk_256
    (
      .address (enet_boot_rom_lane0_chunk_256_address),
      .clk     (clk),
      .q       (q_enet_boot_rom_lane0_chunk_256),
      .reset_n (reset_n)
    );

  assign enet_boot_rom_lane0_leftover_address = address;
  //Atomic memory instance: enet_boot_rom_lane0_leftover
  //Size: 128(words) X 32(bits)

  enet_boot_rom_lane0_leftover_module enet_boot_rom_lane0_leftover
    (
      .address (enet_boot_rom_lane0_leftover_address),
      .clk     (clk),
      .q       (q_enet_boot_rom_lane0_leftover),
      .reset_n (reset_n)
    );

  assign readdata = q_lane_0;
  //s1, which is an e_avalon_slave
  //s2, which is an e_avalon_slave

endmodule

