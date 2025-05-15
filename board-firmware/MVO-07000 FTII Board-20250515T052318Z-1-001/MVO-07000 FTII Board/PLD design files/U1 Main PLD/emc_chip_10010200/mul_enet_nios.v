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
module MUL_enet_nios_black_box_module (
                                        // inputs:
                                         aclr,
                                         clken,
                                         clock,
                                         dataa,
                                         datab,

                                        // outputs:
                                         result
                                      )
;

  output  [ 31: 0] result;
  input            aclr;
  input            clken;
  input            clock;
  input   [ 15: 0] dataa;
  input   [ 15: 0] datab;

  reg     [ 31: 0] p1_result;
  reg     [ 31: 0] result;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  always @(posedge clock or negedge ~(aclr))
    begin
      if (~(aclr) == 0)
        begin
          p1_result <= 0;
          result <= 0;
        end
      else if (clken)
        begin
          p1_result <= dataa * datab;
          result <= p1_result;
        end
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  lpm_mult the_lpm_mult
//    (
//      .aclr (aclr),
//      .clken (clken),
//      .clock (clock),
//      .dataa (dataa),
//      .datab (datab),
//      .result (result)
//    );
//
//  defparam the_lpm_mult.lpm_pipeline = 2,
//           the_lpm_mult.lpm_representation = "UNSIGNED",
//           the_lpm_mult.lpm_widtha = 16,
//           the_lpm_mult.lpm_widthb = 16,
//           the_lpm_mult.lpm_widthp = 32,
//           the_lpm_mult.lpm_widths = 32;
//
//synthesis read_comments_as_HDL off

endmodule


module MUL_enet_nios (
                       // inputs:
                        aclr,
                        clken,
                        clock,
                        dataa,
                        datab,

                       // outputs:
                        result
                     )
;

  output  [ 31: 0] result;
  input            aclr;
  input            clken;
  input            clock;
  input   [ 15: 0] dataa;
  input   [ 15: 0] datab;

  wire    [ 31: 0] result;
  //s1, which is an e_custom_instruction_slave
  MUL_enet_nios_black_box_module the_MUL_enet_nios_black_box_module
    (
      .aclr   (aclr),
      .clken  (clken),
      .clock  (clock),
      .dataa  (dataa),
      .datab  (datab),
      .result (result)
    );


endmodule

