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
module USR0_enet_nios (
                        // inputs:
                         clk,
                         clk_en,
                         dataa,
                         datab,
                         reset,
                         start,

                        // outputs:
                         result
                      )
;

  output  [ 31: 0] result;
  input            clk;
  input            clk_en;
  input   [ 31: 0] dataa;
  input   [ 31: 0] datab;
  input            reset;
  input            start;

  wire    [ 31: 0] absolute_a;
  reg     [ 31: 0] b;
  wire    [ 31: 0] b_into_adder;
  wire             carry;
  wire             dataa_is_negative;
  wire             datab_is_negative;
  reg              datab_was_negative;
  reg              negate;
  wire    [ 65: 0] p1_q;
  wire             p1_subtract;
  reg     [ 65: 0] q;
  wire             reset_n;
  wire    [ 31: 0] result;
  reg              subtract;
  wire    [ 32: 0] sum;
  //s1, which is an e_custom_instruction_slave
  assign reset_n = ~reset;
  assign dataa_is_negative = dataa[31];
  assign datab_is_negative = datab[31];
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          datab_was_negative <= 0;
      else if (clk_en & start)
          datab_was_negative <= datab_is_negative;
    end


  assign absolute_a = (dataa_is_negative)? 0-dataa :
    dataa;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          b <= 0;
      else if (clk_en & start)
          b <= datab;
    end


  assign p1_subtract = (start)? (~datab_is_negative) :
    (carry ^ datab_was_negative);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          subtract <= 0;
      else if (clk_en)
          subtract <= p1_subtract;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          negate <= 0;
      else if (clk_en & start)
          negate <= dataa_is_negative ^ datab_is_negative;
    end


  assign p1_q = (start)? {{34 {1'b0}}, absolute_a[31 : 0]} :
    {sum[32 : 0], q[31 : 0], carry};

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          q <= 0;
      else if (clk_en)
          q <= p1_q;
    end


  assign b_into_adder = (subtract)? 0-b :
    b;

  assign {carry, sum} = {1'b0, q[64 : 32]} + 
    {1'b0, b_into_adder[31], b_into_adder[31 : 0]};

  assign result = (negate)? 0-q[31 : 0] :
    q[31 : 0];


endmodule

