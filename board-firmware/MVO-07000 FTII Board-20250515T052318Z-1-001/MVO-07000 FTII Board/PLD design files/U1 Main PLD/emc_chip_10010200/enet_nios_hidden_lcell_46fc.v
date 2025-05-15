module enet_nios_hidden_lcell_46FC (
                                     // inputs:
                                      aclr,
                                      clk,
                                      dataa,
                                      datab,
                                      datac,
                                      datad,
                                      ena,

                                     // outputs:
                                      combout,
                                      regout
                                   )
;

  parameter output_mode = "comb_and_reg";
  parameter power_up = "low";
  parameter lut_mask = "ffff";
  parameter packed_mode = "false";
  parameter cin_used = "false";
  parameter operation_mode = "counter";


  output           combout;
  output           regout;
  input            aclr;
  input            clk;
  input            dataa;
  input            datab;
  input            datac;
  input            datad;
  input            ena;

  wire             combout;
  wire             regout;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  // This module is a placeholder.  The assignment is never executed
  assign regout = datac;


//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  apex20k_lcell the_apex20k_lcell
//    (
//      .aclr (aclr),
//      .clk (clk),
//      .combout (combout),
//      .dataa (dataa),
//      .datab (datab),
//      .datac (datac),
//      .datad (datad),
//      .ena (ena),
//      .regout (regout)
//    );
//
//  defparam the_apex20k_lcell.cin_used = cin_used,
//           the_apex20k_lcell.lut_mask = lut_mask,
//           the_apex20k_lcell.operation_mode = operation_mode,
//           the_apex20k_lcell.output_mode = output_mode,
//           the_apex20k_lcell.packed_mode = packed_mode,
//           the_apex20k_lcell.power_up = power_up;
//
//synthesis read_comments_as_HDL off

endmodule

