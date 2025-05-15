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
module ad_result_ram_lane1_module (
                                    // inputs:
                                     clk,
                                     data,
                                     rdaddress,
                                     rdclken,
                                     reset_n,
                                     wraddress,
                                     wrclock,
                                     wren,

                                    // outputs:
                                     q
                                  )
;

  output  [  7: 0] q;
  input            clk;
  input   [  7: 0] data;
  input   [  4: 0] rdaddress;
  input            rdclken;
  input            reset_n;
  input   [  4: 0] wraddress;
  input            wrclock;
  input            wren;

  reg     [  7: 0] mem_array [ 31: 0];
  wire    [  7: 0] q;
  reg     [  4: 0] read_address;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          read_address <= 0;
      else if (rdclken)
          read_address <= rdaddress;
    end


  // Data read is synchronized through latent_rdaddress.
  assign q = mem_array[read_address];

initial
    $readmemh("ad_result_ram_lane1.dat", mem_array);
  always @(posedge wrclock)
    begin
      // Write data
      if (wren)
          mem_array[wraddress] <= data;
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  always @(rdaddress)
//    begin
//      if (1)
//          read_address <= rdaddress;
//    end
//
//
//  lpm_ram_dp lpm_ram_dp_component
//    (
//      .data (data),
//      .q (q),
//      .rdaddress (read_address),
//      .rdclken (rdclken),
//      .rdclock (clk),
//      .wraddress (wraddress),
//      .wrclock (wrclock),
//      .wren (wren)
//    );
//
//  defparam lpm_ram_dp_component.lpm_file = "ad_result_ram_lane1.mif",
//           lpm_ram_dp_component.lpm_hint = "USE_EAB=ON",
//           lpm_ram_dp_component.lpm_indata = "REGISTERED",
//           lpm_ram_dp_component.lpm_outdata = "UNREGISTERED",
//           lpm_ram_dp_component.lpm_rdaddress_control = "REGISTERED",
//           lpm_ram_dp_component.lpm_width = 8,
//           lpm_ram_dp_component.lpm_widthad = 5,
//           lpm_ram_dp_component.lpm_wraddress_control = "REGISTERED",
//           lpm_ram_dp_component.suppress_memory_conversion_warnings = "ON";
//
//synthesis read_comments_as_HDL off

endmodule


module ad_result_ram_lane0_module (
                                    // inputs:
                                     clk,
                                     data,
                                     rdaddress,
                                     rdclken,
                                     reset_n,
                                     wraddress,
                                     wrclock,
                                     wren,

                                    // outputs:
                                     q
                                  )
;

  output  [  7: 0] q;
  input            clk;
  input   [  7: 0] data;
  input   [  4: 0] rdaddress;
  input            rdclken;
  input            reset_n;
  input   [  4: 0] wraddress;
  input            wrclock;
  input            wren;

  reg     [  7: 0] mem_array [ 31: 0];
  wire    [  7: 0] q;
  reg     [  4: 0] read_address;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          read_address <= 0;
      else if (rdclken)
          read_address <= rdaddress;
    end


  // Data read is synchronized through latent_rdaddress.
  assign q = mem_array[read_address];

initial
    $readmemh("ad_result_ram_lane0.dat", mem_array);
  always @(posedge wrclock)
    begin
      // Write data
      if (wren)
          mem_array[wraddress] <= data;
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  always @(rdaddress)
//    begin
//      if (1)
//          read_address <= rdaddress;
//    end
//
//
//  lpm_ram_dp lpm_ram_dp_component
//    (
//      .data (data),
//      .q (q),
//      .rdaddress (read_address),
//      .rdclken (rdclken),
//      .rdclock (clk),
//      .wraddress (wraddress),
//      .wrclock (wrclock),
//      .wren (wren)
//    );
//
//  defparam lpm_ram_dp_component.lpm_file = "ad_result_ram_lane0.mif",
//           lpm_ram_dp_component.lpm_hint = "USE_EAB=ON",
//           lpm_ram_dp_component.lpm_indata = "REGISTERED",
//           lpm_ram_dp_component.lpm_outdata = "UNREGISTERED",
//           lpm_ram_dp_component.lpm_rdaddress_control = "REGISTERED",
//           lpm_ram_dp_component.lpm_width = 8,
//           lpm_ram_dp_component.lpm_widthad = 5,
//           lpm_ram_dp_component.lpm_wraddress_control = "REGISTERED",
//           lpm_ram_dp_component.suppress_memory_conversion_warnings = "ON";
//
//synthesis read_comments_as_HDL off

endmodule


module ad_result_ram (
                       // inputs:
                        address,
                        clk,
                        reset_n,
                        writebyteenable,
                        writedata,

                       // outputs:
                        readdata
                     )
;

  output  [ 15: 0] readdata;
  input   [  4: 0] address;
  input            clk;
  input            reset_n;
  input   [  1: 0] writebyteenable;
  input   [ 15: 0] writedata;

  wire    [  4: 0] ad_result_ram_lane0_address;
  wire    [  4: 0] ad_result_ram_lane1_address;
  wire    [  7: 0] d_lane_0;
  wire    [  7: 0] d_lane_1;
  wire    [  7: 0] q_lane_0;
  wire    [  7: 0] q_lane_1;
  wire    [ 15: 0] readdata;
  wire             select_all_chunks;
  wire             write_lane_0;
  wire             write_lane_1;
  assign write_lane_1 = writebyteenable[1];
  assign select_all_chunks = 1;
  assign ad_result_ram_lane1_address = address;
  //Atomic memory instance: ad_result_ram_lane1
  //Size: 32(words) X 8(bits)

  ad_result_ram_lane1_module ad_result_ram_lane1
    (
      .clk       (clk),
      .data      (d_lane_1),
      .q         (q_lane_1),
      .rdaddress (ad_result_ram_lane1_address),
      .rdclken   (1'b1),
      .reset_n   (reset_n),
      .wraddress (ad_result_ram_lane1_address),
      .wrclock   (clk),
      .wren      (write_lane_1)
    );

  assign write_lane_0 = writebyteenable[0];
  assign select_all_chunks = 1;
  assign ad_result_ram_lane0_address = address;
  //Atomic memory instance: ad_result_ram_lane0
  //Size: 32(words) X 8(bits)

  ad_result_ram_lane0_module ad_result_ram_lane0
    (
      .clk       (clk),
      .data      (d_lane_0),
      .q         (q_lane_0),
      .rdaddress (ad_result_ram_lane0_address),
      .rdclken   (1'b1),
      .reset_n   (reset_n),
      .wraddress (ad_result_ram_lane0_address),
      .wrclock   (clk),
      .wren      (write_lane_0)
    );

  assign readdata = {q_lane_1,
    q_lane_0};

  assign {d_lane_1,
d_lane_0} = writedata;
  //s1, which is an e_avalon_slave
  //s2, which is an e_avalon_slave

endmodule

