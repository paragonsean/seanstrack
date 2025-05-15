module ad_nios_hidden_lcell_30F0 (
                                   // inputs:
                                    cascin,
                                    dataa,
                                    datab,
                                    datac,
                                    datad,

                                   // outputs:
                                    cascout
                                 )
;

  parameter packed_mode = "false";
  parameter output_mode = "comb_and_reg";
  parameter power_up = "low";
  parameter lut_mask = "ffff";
  parameter cin_used = "false";
  parameter operation_mode = "counter";


  output           cascout;
  input            cascin;
  input            dataa;
  input            datab;
  input            datac;
  input            datad;

  wire             cascout;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  // This module is a placeholder.  The assignment is never executed
  assign cascout = datac;


//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  apex20k_lcell the_apex20k_lcell
//    (
//      .cascin (cascin),
//      .cascout (cascout),
//      .dataa (dataa),
//      .datab (datab),
//      .datac (datac),
//      .datad (datad)
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

