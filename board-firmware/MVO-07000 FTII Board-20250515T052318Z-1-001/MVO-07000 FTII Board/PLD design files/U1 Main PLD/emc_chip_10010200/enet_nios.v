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
module enet_nios_opcode_display_unit (
                                       // inputs:
                                        instruction
                                     )
;

  input   [ 15: 0] instruction;

  wire    [  7: 0] opcode;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  assign opcode = ((((~instruction[15] && ~instruction[14] && ~instruction[13] && ~instruction[12] && ~instruction[11] && ~instruction[10]))))? 8'd0 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] && ~instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] && ~instruction[7]))))? 8'd1 :
    ((((~instruction[15] && ~instruction[14] && ~instruction[13] && ~instruction[12] && ~instruction[11] &&  instruction[10]))))? 8'd2 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] && ~instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] &&  instruction[7]))))? 8'd3 :
    ((((~instruction[15] && ~instruction[14] && ~instruction[13] && ~instruction[12] &&  instruction[11] && ~instruction[10]))))? 8'd4 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] && ~instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] && ~instruction[7]))))? 8'd5 :
    ((((~instruction[15] && ~instruction[14] && ~instruction[13] && ~instruction[12] &&  instruction[11] &&  instruction[10]))))? 8'd6 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] && ~instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] &&  instruction[7]))))? 8'd7 :
    ((((~instruction[15] && ~instruction[14] && ~instruction[13] &&  instruction[12] && ~instruction[11] && ~instruction[10]))))? 8'd8 :
    ((((~instruction[15] && ~instruction[14] && ~instruction[13] &&  instruction[12] && ~instruction[11] &&  instruction[10]))))? 8'd9 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] && ~instruction[10]) && ~instruction[9] && ~instruction[8]))))? 8'd10 :
    ((((~instruction[15] && ~instruction[14] && ~instruction[13] &&  instruction[12] &&  instruction[11] && ~instruction[10]))))? 8'd11 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] && ~instruction[10]) && ~instruction[9] &&  instruction[8]))))? 8'd12 :
    ((((~instruction[15] && ~instruction[14] && ~instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]))))? 8'd13 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] && ~instruction[10]) &&  instruction[9] && ~instruction[8]))))? 8'd14 :
    ((((~instruction[15] && ~instruction[14] &&  instruction[13] && ~instruction[12] && ~instruction[11] && ~instruction[10]))))? 8'd15 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] && ~instruction[10]) &&  instruction[9] &&  instruction[8]))))? 8'd16 :
    ((((~instruction[15] && ~instruction[14] &&  instruction[13] && ~instruction[12] && ~instruction[11] &&  instruction[10]))))? 8'd17 :
    ((((~instruction[15] && ~instruction[14] &&  instruction[13] && ~instruction[12] &&  instruction[11] && ~instruction[10]))))? 8'd18 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] && ~instruction[7] && ~instruction[6] && ~instruction[5]))))? 8'd19 :
    ((((~instruction[15] && ~instruction[14] &&  instruction[13] && ~instruction[12] &&  instruction[11] &&  instruction[10]))))? 8'd20 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] && ~instruction[7] && ~instruction[6] &&  instruction[5]))))? 8'd21 :
    ((((~instruction[15] && ~instruction[14] &&  instruction[13] &&  instruction[12] && ~instruction[11] && ~instruction[10]))))? 8'd22 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] && ~instruction[7] &&  instruction[6] && ~instruction[5]))))? 8'd23 :
    ((((~instruction[15] && ~instruction[14] &&  instruction[13] &&  instruction[12] && ~instruction[11] &&  instruction[10]))))? 8'd24 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] && ~instruction[7] &&  instruction[6] &&  instruction[5]))))? 8'd25 :
    ((((~instruction[15] && ~instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] && ~instruction[10]))))? 8'd26 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] &&  instruction[7] && ~instruction[6] && ~instruction[5]))))? 8'd27 :
    ((((~instruction[15] && ~instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]))))? 8'd28 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] &&  instruction[7] && ~instruction[6] &&  instruction[5]))))? 8'd29 :
    ((((~instruction[15] &&  instruction[14] && ~instruction[13] && ~instruction[12] && ~instruction[11] && ~instruction[10]))))? 8'd30 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] && ~instruction[8] &&  instruction[7] &&  instruction[6] && ~instruction[5]))))? 8'd31 :
    ((((~instruction[15] &&  instruction[14] && ~instruction[13] && ~instruction[12] && ~instruction[11] &&  instruction[10]))))? 8'd32 :
    ((((~instruction[15] &&  instruction[14] && ~instruction[13] && ~instruction[12] &&  instruction[11] && ~instruction[10]))))? 8'd33 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] &&  instruction[8] && ~instruction[7] && ~instruction[6] && ~instruction[5]))))? 8'd34 :
    ((((~instruction[15] &&  instruction[14] && ~instruction[13] && ~instruction[12] &&  instruction[11] &&  instruction[10]))))? 8'd35 :
    ((((~instruction[15] &&  instruction[14] && ~instruction[13] &&  instruction[12] && ~instruction[11] && ~instruction[10]))))? 8'd36 :
    ((((~instruction[15] &&  instruction[14] && ~instruction[13] &&  instruction[12] && ~instruction[11] &&  instruction[10]))))? 8'd37 :
    ((((~instruction[15] &&  instruction[14] && ~instruction[13] &&  instruction[12] &&  instruction[11] && ~instruction[10]))))? 8'd38 :
    ((((~instruction[15] &&  instruction[14] && ~instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]))))? 8'd39 :
    ((((~instruction[15] &&  instruction[14] &&  instruction[13] && ~instruction[12] && ~instruction[11] && ~instruction[10]))))? 8'd40 :
    ((((~instruction[15] &&  instruction[14] &&  instruction[13] && ~instruction[12] && ~instruction[11] &&  instruction[10]))))? 8'd41 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] &&  instruction[8] &&  instruction[7] && ~instruction[6] &&  instruction[5]))))? 8'd42 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) && ~instruction[9] &&  instruction[8] &&  instruction[7] &&  instruction[6] && ~instruction[5]))))? 8'd43 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] && ~instruction[7] && ~instruction[6] && ~instruction[5]))))? 8'd44 :
    ((((~instruction[15] &&  instruction[14] &&  instruction[13] && ~instruction[12] &&  instruction[11] && ~instruction[10]))))? 8'd45 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] && ~instruction[7] && ~instruction[6] &&  instruction[5]))))? 8'd46 :
    ((((~instruction[15] &&  instruction[14] &&  instruction[13] && ~instruction[12] &&  instruction[11] &&  instruction[10]))))? 8'd47 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] && ~instruction[7] &&  instruction[6] && ~instruction[5]))))? 8'd48 :
    ((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] && ~instruction[11] && ~instruction[10]))))? 8'd49 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] && ~instruction[7] &&  instruction[6] &&  instruction[5]))))? 8'd50 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] &&  instruction[7] && ~instruction[6] && ~instruction[5]))))? 8'd51 :
    (((( instruction[15] && ~instruction[14] && ~instruction[13] && ~instruction[12] && ~instruction[11]))))? 8'd52 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] &&  instruction[7] && ~instruction[6] &&  instruction[5]))))? 8'd53 :
    (((( instruction[15] && ~instruction[14] && ~instruction[13] && ~instruction[12] &&  instruction[11]))))? 8'd54 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] &&  instruction[7] &&  instruction[6] && ~instruction[5]))))? 8'd55 :
    (((( instruction[15] && ~instruction[14] && ~instruction[13] &&  instruction[12] && ~instruction[11]))))? 8'd56 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] && ~instruction[8] &&  instruction[7] &&  instruction[6] &&  instruction[5]))))? 8'd57 :
    (((( instruction[15] && ~instruction[14] && ~instruction[13] &&  instruction[12] &&  instruction[11]))))? 8'd58 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] &&  instruction[8] && ~instruction[7] && ~instruction[6] && ~instruction[5]))))? 8'd59 :
    (((( instruction[15] && ~instruction[14] &&  instruction[13] && ~instruction[12]))))? 8'd60 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] &&  instruction[8] && ~instruction[7] && ~instruction[6] &&  instruction[5]))))? 8'd61 :
    (((( instruction[15] && ~instruction[14] &&  instruction[13] &&  instruction[12]))))? 8'd62 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] &&  instruction[8] && ~instruction[7] &&  instruction[6] && ~instruction[5]))))? 8'd63 :
    (((( instruction[15] &&  instruction[14] && ~instruction[13]))))? 8'd64 :
    (((((~instruction[15] &&  instruction[14] &&  instruction[13] &&  instruction[12] &&  instruction[11] &&  instruction[10]) &&  instruction[9] &&  instruction[8] &&  instruction[7] &&  instruction[6] && ~instruction[5]))))? 8'd65 :
    (((( instruction[15] &&  instruction[14] &&  instruction[13]))))? 8'd66 :
    8'd67;


//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on

endmodule


module enet_nios_interrupt_control (
                                     // inputs:
                                      IE,
                                      IPRI,
                                      cancel_branch_delay_slot,
                                      clk,
                                      do_not_interrupt,
                                      instruction_1,
                                      irq,
                                      irqnumber,
                                      is_cancelled,
                                      is_cancelled_from_commit_stage,
                                      is_neutrino,
                                      op_is_trap,
                                      pipe_run,
                                      reset_n,
                                      subinstruction,
                                      trap_request_overflow,
                                      trap_request_underflow,

                                     // outputs:
                                      do_cancel_next_instruction,
                                      do_force_trap,
                                      forced_trap_instruction,
                                      trap_properly_received
                                   )
;

  output           do_cancel_next_instruction;
  output           do_force_trap;
  output  [ 15: 0] forced_trap_instruction;
  output           trap_properly_received;
  input            IE;
  input   [  5: 0] IPRI;
  input            cancel_branch_delay_slot;
  input            clk;
  input            do_not_interrupt;
  input   [ 15: 0] instruction_1;
  input            irq;
  input   [  5: 0] irqnumber;
  input            is_cancelled;
  input            is_cancelled_from_commit_stage;
  input            is_neutrino;
  input            op_is_trap;
  input            pipe_run;
  input            reset_n;
  input   [  5: 0] subinstruction;
  input            trap_request_overflow;
  input            trap_request_underflow;

  wire             already_processing_trap;
  reg              d1_irq;
  reg     [  5: 0] d1_irqnumber;
  reg              d2_irq;
  reg     [  5: 0] d2_irqnumber;
  wire             do_cancel_next_instruction;
  wire             do_force_trap;
  reg              dont_forget_im_processing_a_trap;
  wire    [ 15: 0] forced_trap_instruction;
  wire    [  5: 0] forced_trap_number;
  wire    [ 15: 0] instruction;
  reg              interrupt_pending;
  wire             interruptable_instruction;
  wire             local_pipe_clk_en;
  wire             oci_irq;
  wire             pipe_state_we;
  wire             trap_ok;
  wire             trap_properly_received;
  wire             trap_request;
  assign instruction = instruction_1;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          d1_irqnumber <= 0;
      else if (1'b1)
          d1_irqnumber <= irqnumber & ~({6{oci_irq}});
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          d2_irqnumber <= 0;
      else if (1'b1)
          d2_irqnumber <= d1_irqnumber;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          d1_irq <= 0;
      else if (1'b1)
          d1_irq <= irq | oci_irq;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          d2_irq <= 0;
      else if (1'b1)
          d2_irq <= d1_irq && (d1_irqnumber < IPRI);
    end


  assign oci_irq = 0;
  assign trap_request = d2_irq || trap_request_underflow || trap_request_overflow;
  assign do_force_trap = trap_request && trap_ok;
  assign forced_trap_number = (trap_request_underflow)? 1 :
    (trap_request_overflow)? 2 :
    d2_irqnumber;

  assign forced_trap_instruction = {6'b011110,
    10'b0000000000} | {6'b000000,
    2'b01,
    8'b00000000}|{10'b0000000000,
    forced_trap_number};

  assign trap_ok = interruptable_instruction     && 
    ~already_processing_trap       &&
    ~interrupt_pending             &&
    (IE | (0));

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          interrupt_pending <= 0;
      else if (local_pipe_clk_en)
          if (trap_properly_received)
              interrupt_pending <= 0;
          else if (do_force_trap)
              interrupt_pending <= -1;
    end


  assign trap_properly_received = ( op_is_trap                     ) && 
    (~is_neutrino                    ) &&               
    (~is_cancelled_from_commit_stage );

  assign interruptable_instruction = (trap_request_underflow                  ) ||
    (trap_request_overflow                   ) ||
    ( (~do_not_interrupt                ) && 
    (~|subinstruction                 ) &&
    (~is_neutrino                     ) &&            
    (~is_cancelled_from_commit_stage  )    );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dont_forget_im_processing_a_trap <= 0;
      else if (pipe_state_we)
          if (op_is_trap)
              dont_forget_im_processing_a_trap <= -1;
          else if (1'b1)
              dont_forget_im_processing_a_trap <= 0;
    end


  assign already_processing_trap = ( op_is_trap                     &&  
    ~is_neutrino                    && 
    ~is_cancelled_from_commit_stage   ) ||
    (dont_forget_im_processing_a_trap  );

  assign do_cancel_next_instruction = cancel_branch_delay_slot && 
    ~is_cancelled            &&
    ~is_neutrino;


endmodule


module enet_nios_address_request (
                                   // inputs:
                                    clk,
                                    d1_instruction_fifo_read_data_bad,
                                    do_branch,
                                    do_jump,
                                    ic_read,
                                    ic_wait,
                                    instruction_fifo_read_data_bad,
                                    pipe_run,
                                    reset_n,
                                    target_address,

                                   // outputs:
                                    ic_flush,
                                    p1_flush,
                                    pc
                                 )
;

  output           ic_flush;
  output           p1_flush;
  output  [ 21: 0] pc;
  input            clk;
  input            d1_instruction_fifo_read_data_bad;
  input            do_branch;
  input            do_jump;
  input            ic_read;
  input            ic_wait;
  input            instruction_fifo_read_data_bad;
  input            pipe_run;
  input            reset_n;
  input   [ 21: 0] target_address;

  reg              ic_flush;
  wire    [ 21: 0] next_pc;
  wire    [ 21: 0] next_pc_plus_one;
  wire             nonsequential_pc;
  wire             p1_flush;
  reg     [ 21: 0] pc;
  wire             pc_clken;
  reg              remember_to_flush;
  reg              waiting_for_delay_slot;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          pc <= 0;
      else if (pc_clken)
          pc <= next_pc;
    end


  assign next_pc_plus_one = pc + 1;
  assign pc_clken = (ic_read | p1_flush) & ~ic_wait;
  assign next_pc = (do_jump | do_branch | 
    (remember_to_flush & ~waiting_for_delay_slot)) ? 
    target_address                              :
    next_pc_plus_one;

  assign nonsequential_pc = (do_branch | do_jump) & pipe_run;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          ic_flush <= 0;
      else if (~ic_wait)
          ic_flush <= p1_flush;
    end


  assign p1_flush = (nonsequential_pc  & ~d1_instruction_fifo_read_data_bad )  | 
    (remember_to_flush & ~waiting_for_delay_slot            );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          remember_to_flush <= 0;
      else if (1'b1)
          if (p1_flush & ~ic_wait)
              remember_to_flush <= 0;
          else if (nonsequential_pc & 
                       (d1_instruction_fifo_read_data_bad | ic_wait))
              remember_to_flush <= -1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          waiting_for_delay_slot <= 0;
      else if (pipe_run)
          if (~instruction_fifo_read_data_bad)
              waiting_for_delay_slot <= 0;
          else if (nonsequential_pc && d1_instruction_fifo_read_data_bad)
              waiting_for_delay_slot <= -1;
    end



endmodule


module enet_nios_target_address (
                                  // inputs:
                                   branch_base,
                                   clk,
                                   do_branch,
                                   do_jump,
                                   jump_target_address,
                                   pipe_run,
                                   reset_n,
                                   signed_branch_offset,

                                  // outputs:
                                   target_address
                                )
;

  output  [ 21: 0] target_address;
  input   [ 21: 0] branch_base;
  input            clk;
  input            do_branch;
  input            do_jump;
  input   [ 21: 0] jump_target_address;
  input            pipe_run;
  input            reset_n;
  input   [ 21: 0] signed_branch_offset;

  wire    [ 21: 0] branch_target_address;
  wire    [ 21: 0] current_target_address;
  reg     [ 21: 0] last_target_address;
  wire    [ 21: 0] target_address;
  assign branch_target_address = branch_base + signed_branch_offset;
  assign target_address = (do_jump || do_branch)   ? 
    current_target_address  : 
    last_target_address;


//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  assign current_target_address = do_jump ? jump_target_address : branch_target_address;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          last_target_address <= 0;
      else if (pipe_run && (do_jump || do_branch))
          last_target_address <= current_target_address;
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  
//  enet_nios_hidden_lcell_46FC target_address_0
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[0]),
//      .dataa   (jump_target_address[0]),
//      .datab   (branch_target_address[0]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[0])
//    );
//  defparam target_address_0.cin_used = "false",
//           target_address_0.lut_mask = "ACAC",
//           target_address_0.operation_mode = "normal",
//           target_address_0.output_mode = "comb_and_reg",
//           target_address_0.packed_mode = "false",
//           target_address_0.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_1
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[1]),
//      .dataa   (jump_target_address[1]),
//      .datab   (branch_target_address[1]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[1])
//    );
//  defparam target_address_1.cin_used = "false",
//           target_address_1.lut_mask = "ACAC",
//           target_address_1.operation_mode = "normal",
//           target_address_1.output_mode = "comb_and_reg",
//           target_address_1.packed_mode = "false",
//           target_address_1.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_2
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[2]),
//      .dataa   (jump_target_address[2]),
//      .datab   (branch_target_address[2]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[2])
//    );
//  defparam target_address_2.cin_used = "false",
//           target_address_2.lut_mask = "ACAC",
//           target_address_2.operation_mode = "normal",
//           target_address_2.output_mode = "comb_and_reg",
//           target_address_2.packed_mode = "false",
//           target_address_2.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_3
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[3]),
//      .dataa   (jump_target_address[3]),
//      .datab   (branch_target_address[3]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[3])
//    );
//  defparam target_address_3.cin_used = "false",
//           target_address_3.lut_mask = "ACAC",
//           target_address_3.operation_mode = "normal",
//           target_address_3.output_mode = "comb_and_reg",
//           target_address_3.packed_mode = "false",
//           target_address_3.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_4
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[4]),
//      .dataa   (jump_target_address[4]),
//      .datab   (branch_target_address[4]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[4])
//    );
//  defparam target_address_4.cin_used = "false",
//           target_address_4.lut_mask = "ACAC",
//           target_address_4.operation_mode = "normal",
//           target_address_4.output_mode = "comb_and_reg",
//           target_address_4.packed_mode = "false",
//           target_address_4.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_5
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[5]),
//      .dataa   (jump_target_address[5]),
//      .datab   (branch_target_address[5]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[5])
//    );
//  defparam target_address_5.cin_used = "false",
//           target_address_5.lut_mask = "ACAC",
//           target_address_5.operation_mode = "normal",
//           target_address_5.output_mode = "comb_and_reg",
//           target_address_5.packed_mode = "false",
//           target_address_5.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_6
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[6]),
//      .dataa   (jump_target_address[6]),
//      .datab   (branch_target_address[6]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[6])
//    );
//  defparam target_address_6.cin_used = "false",
//           target_address_6.lut_mask = "ACAC",
//           target_address_6.operation_mode = "normal",
//           target_address_6.output_mode = "comb_and_reg",
//           target_address_6.packed_mode = "false",
//           target_address_6.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_7
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[7]),
//      .dataa   (jump_target_address[7]),
//      .datab   (branch_target_address[7]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[7])
//    );
//  defparam target_address_7.cin_used = "false",
//           target_address_7.lut_mask = "ACAC",
//           target_address_7.operation_mode = "normal",
//           target_address_7.output_mode = "comb_and_reg",
//           target_address_7.packed_mode = "false",
//           target_address_7.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_8
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[8]),
//      .dataa   (jump_target_address[8]),
//      .datab   (branch_target_address[8]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[8])
//    );
//  defparam target_address_8.cin_used = "false",
//           target_address_8.lut_mask = "ACAC",
//           target_address_8.operation_mode = "normal",
//           target_address_8.output_mode = "comb_and_reg",
//           target_address_8.packed_mode = "false",
//           target_address_8.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_9
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[9]),
//      .dataa   (jump_target_address[9]),
//      .datab   (branch_target_address[9]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[9])
//    );
//  defparam target_address_9.cin_used = "false",
//           target_address_9.lut_mask = "ACAC",
//           target_address_9.operation_mode = "normal",
//           target_address_9.output_mode = "comb_and_reg",
//           target_address_9.packed_mode = "false",
//           target_address_9.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_10
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[10]),
//      .dataa   (jump_target_address[10]),
//      .datab   (branch_target_address[10]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[10])
//    );
//  defparam target_address_10.cin_used = "false",
//           target_address_10.lut_mask = "ACAC",
//           target_address_10.operation_mode = "normal",
//           target_address_10.output_mode = "comb_and_reg",
//           target_address_10.packed_mode = "false",
//           target_address_10.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_11
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[11]),
//      .dataa   (jump_target_address[11]),
//      .datab   (branch_target_address[11]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[11])
//    );
//  defparam target_address_11.cin_used = "false",
//           target_address_11.lut_mask = "ACAC",
//           target_address_11.operation_mode = "normal",
//           target_address_11.output_mode = "comb_and_reg",
//           target_address_11.packed_mode = "false",
//           target_address_11.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_12
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[12]),
//      .dataa   (jump_target_address[12]),
//      .datab   (branch_target_address[12]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[12])
//    );
//  defparam target_address_12.cin_used = "false",
//           target_address_12.lut_mask = "ACAC",
//           target_address_12.operation_mode = "normal",
//           target_address_12.output_mode = "comb_and_reg",
//           target_address_12.packed_mode = "false",
//           target_address_12.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_13
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[13]),
//      .dataa   (jump_target_address[13]),
//      .datab   (branch_target_address[13]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[13])
//    );
//  defparam target_address_13.cin_used = "false",
//           target_address_13.lut_mask = "ACAC",
//           target_address_13.operation_mode = "normal",
//           target_address_13.output_mode = "comb_and_reg",
//           target_address_13.packed_mode = "false",
//           target_address_13.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_14
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[14]),
//      .dataa   (jump_target_address[14]),
//      .datab   (branch_target_address[14]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[14])
//    );
//  defparam target_address_14.cin_used = "false",
//           target_address_14.lut_mask = "ACAC",
//           target_address_14.operation_mode = "normal",
//           target_address_14.output_mode = "comb_and_reg",
//           target_address_14.packed_mode = "false",
//           target_address_14.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_15
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[15]),
//      .dataa   (jump_target_address[15]),
//      .datab   (branch_target_address[15]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[15])
//    );
//  defparam target_address_15.cin_used = "false",
//           target_address_15.lut_mask = "ACAC",
//           target_address_15.operation_mode = "normal",
//           target_address_15.output_mode = "comb_and_reg",
//           target_address_15.packed_mode = "false",
//           target_address_15.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_16
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[16]),
//      .dataa   (jump_target_address[16]),
//      .datab   (branch_target_address[16]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[16])
//    );
//  defparam target_address_16.cin_used = "false",
//           target_address_16.lut_mask = "ACAC",
//           target_address_16.operation_mode = "normal",
//           target_address_16.output_mode = "comb_and_reg",
//           target_address_16.packed_mode = "false",
//           target_address_16.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_17
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[17]),
//      .dataa   (jump_target_address[17]),
//      .datab   (branch_target_address[17]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[17])
//    );
//  defparam target_address_17.cin_used = "false",
//           target_address_17.lut_mask = "ACAC",
//           target_address_17.operation_mode = "normal",
//           target_address_17.output_mode = "comb_and_reg",
//           target_address_17.packed_mode = "false",
//           target_address_17.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_18
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[18]),
//      .dataa   (jump_target_address[18]),
//      .datab   (branch_target_address[18]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[18])
//    );
//  defparam target_address_18.cin_used = "false",
//           target_address_18.lut_mask = "ACAC",
//           target_address_18.operation_mode = "normal",
//           target_address_18.output_mode = "comb_and_reg",
//           target_address_18.packed_mode = "false",
//           target_address_18.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_19
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[19]),
//      .dataa   (jump_target_address[19]),
//      .datab   (branch_target_address[19]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[19])
//    );
//  defparam target_address_19.cin_used = "false",
//           target_address_19.lut_mask = "ACAC",
//           target_address_19.operation_mode = "normal",
//           target_address_19.output_mode = "comb_and_reg",
//           target_address_19.packed_mode = "false",
//           target_address_19.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_20
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[20]),
//      .dataa   (jump_target_address[20]),
//      .datab   (branch_target_address[20]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[20])
//    );
//  defparam target_address_20.cin_used = "false",
//           target_address_20.lut_mask = "ACAC",
//           target_address_20.operation_mode = "normal",
//           target_address_20.output_mode = "comb_and_reg",
//           target_address_20.packed_mode = "false",
//           target_address_20.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_46FC target_address_21
//    (
//      .aclr    (~reset_n),
//      .clk     (clk),
//      .combout (current_target_address[21]),
//      .dataa   (jump_target_address[21]),
//      .datab   (branch_target_address[21]),
//      .datac   (do_jump),
//      .datad   (1'b0),
//      .ena     (pipe_run && (do_jump | do_branch)),
//      .regout  (last_target_address[21])
//    );
//  defparam target_address_21.cin_used = "false",
//           target_address_21.lut_mask = "ACAC",
//           target_address_21.operation_mode = "normal",
//           target_address_21.output_mode = "comb_and_reg",
//           target_address_21.packed_mode = "false",
//           target_address_21.power_up = "low";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_nios_instruction_fetch (
                                     // inputs:
                                      branch_base,
                                      clk,
                                      d1_instruction_fifo_read_data_bad,
                                      do_branch,
                                      do_jump,
                                      ic_read,
                                      ic_wait,
                                      instruction_fifo_read_data_bad,
                                      jump_target_address,
                                      pipe_run,
                                      reset_n,
                                      signed_branch_offset,

                                     // outputs:
                                      ic_address,
                                      ic_flush,
                                      p1_flush,
                                      target_address
                                   )
  /* synthesis auto_dissolve = "FALSE" */ ;

  output  [ 22: 0] ic_address;
  output           ic_flush;
  output           p1_flush;
  output  [ 21: 0] target_address;
  input   [ 21: 0] branch_base;
  input            clk;
  input            d1_instruction_fifo_read_data_bad;
  input            do_branch;
  input            do_jump;
  input            ic_read;
  input            ic_wait;
  input            instruction_fifo_read_data_bad;
  input   [ 21: 0] jump_target_address;
  input            pipe_run;
  input            reset_n;
  input   [ 21: 0] signed_branch_offset;

  wire    [ 22: 0] ic_address;
  wire             ic_flush;
  wire             p1_flush;
  wire    [ 21: 0] pc;
  wire    [ 21: 0] target_address;
  enet_nios_address_request the_enet_nios_address_request
    (
      .clk                               (clk),
      .d1_instruction_fifo_read_data_bad (d1_instruction_fifo_read_data_bad),
      .do_branch                         (do_branch),
      .do_jump                           (do_jump),
      .ic_flush                          (ic_flush),
      .ic_read                           (ic_read),
      .ic_wait                           (ic_wait),
      .instruction_fifo_read_data_bad    (instruction_fifo_read_data_bad),
      .p1_flush                          (p1_flush),
      .pc                                (pc),
      .pipe_run                          (pipe_run),
      .reset_n                           (reset_n),
      .target_address                    (target_address)
    );

  enet_nios_target_address the_enet_nios_target_address
    (
      .branch_base          (branch_base),
      .clk                  (clk),
      .do_branch            (do_branch),
      .do_jump              (do_jump),
      .jump_target_address  (jump_target_address),
      .pipe_run             (pipe_run),
      .reset_n              (reset_n),
      .signed_branch_offset (signed_branch_offset),
      .target_address       (target_address)
    );

  assign ic_address = {pc, {1'b0}};

endmodule


module enet_nios_cpu_instruction_fifo_fifo_module (
                                                    // inputs:
                                                     clk,
                                                     clk_en,
                                                     fifo_read,
                                                     fifo_wr_data,
                                                     fifo_write,
                                                     flush,
                                                     ic_wait,
                                                     reset_n,

                                                    // outputs:
                                                     fifo_rd_data,
                                                     fifo_read_data_bad,
                                                     ic_read
                                                  )
;

  output  [ 15: 0] fifo_rd_data;
  output           fifo_read_data_bad;
  output           ic_read;
  input            clk;
  input            clk_en;
  input            fifo_read;
  input   [ 15: 0] fifo_wr_data;
  input            fifo_write;
  input            flush;
  input            ic_wait;
  input            reset_n;

  wire             bad_news;
  reg              continue_read_cycle;
  reg              dont_forget_to_reset_ic_read;
  wire             fifo_becoming_empty;
  wire             fifo_dec;
  wire             fifo_inc;
  wire    [ 15: 0] fifo_rd_data;
  wire             fifo_read_data_bad;
  reg     [ 15: 0] fifo_reg_0;
  wire             fifo_reg_0_read_select;
  wire             fifo_reg_0_write_select;
  reg     [ 15: 0] fifo_reg_1;
  wire             fifo_reg_1_read_select;
  wire             fifo_reg_1_write_select;
  reg     [ 15: 0] fifo_reg_2;
  wire             fifo_reg_2_read_select;
  wire             fifo_reg_2_write_select;
  reg              ic_read;
  wire             ic_read_confusion;
  wire             ic_read_prime;
  reg              internal_fifo_empty;
  wire    [ 15: 0] internal_fifo_rd_data;
  wire    [  2: 0] next_read_pointer;
  wire    [  2: 0] read_pointer;
  wire             reset_ic_read;
  wire             set_ic_read;
  wire    [  2: 0] unxshiftxread_pointerxxrdaddress_calculatorx0_in;
  reg     [  2: 0] unxshiftxread_pointerxxrdaddress_calculatorx0_out;
  wire    [  2: 0] unxshiftxwrite_pointerxxwraddress_calculatorx1_in;
  reg     [  2: 0] unxshiftxwrite_pointerxxwraddress_calculatorx1_out;
  wire    [  2: 0] write_pointer;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          internal_fifo_empty <= 1;
      else if (clk_en)
          if (fifo_becoming_empty)
              internal_fifo_empty <= -1;
          else if (fifo_write & ~fifo_read)
              internal_fifo_empty <= 0;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          ic_read <= 0;
      else if (~ic_wait)
          if (reset_ic_read)
              ic_read <= 0;
          else if (set_ic_read)
              ic_read <= -1;
    end


  assign set_ic_read = fifo_becoming_empty | internal_fifo_empty;
  assign reset_ic_read = (fifo_write & ~fifo_read        ) ||   (dont_forget_to_reset_ic_read &&       ~internal_fifo_empty           );
  assign ic_read_confusion = set_ic_read & reset_ic_read;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dont_forget_to_reset_ic_read <= 1'b1;
      else if (1'b1)
          if ((dont_forget_to_reset_ic_read && ~ic_wait             ) ||
                (internal_fifo_empty && dont_forget_to_reset_ic_read ))
              dont_forget_to_reset_ic_read <= 0;
          else if ((fifo_write & ~fifo_read) && ic_wait)
              dont_forget_to_reset_ic_read <= -1;
    end


  assign fifo_inc = fifo_write;
  assign fifo_dec = fifo_read & ~(fifo_read_data_bad);
  //rdaddress_calculator, which is an e_shift_register
  //wraddress_calculator, which is an e_shift_register
  assign next_read_pointer = {read_pointer[1 : 0], read_pointer[2]};
  assign fifo_becoming_empty = ((next_read_pointer==write_pointer) 
    & (fifo_read & ~fifo_write ) )
    | flush;

  assign ic_read_prime = internal_fifo_empty || continue_read_cycle;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          continue_read_cycle <= 0;
      else if (1'b1)
          if (~ic_wait)
              continue_read_cycle <= 0;
          else if (ic_read_prime)
              continue_read_cycle <= -1;
    end


  assign fifo_read_data_bad = (internal_fifo_empty & ~(fifo_write)) | flush;
  assign bad_news = ic_read_prime ^ ic_read;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          fifo_reg_0 <= 0;
      else if (fifo_reg_0_write_select & fifo_write)
          fifo_reg_0 <= fifo_wr_data;
    end


  assign fifo_reg_0_write_select = write_pointer[0];
  assign fifo_reg_0_read_select = read_pointer[0];
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          fifo_reg_1 <= 0;
      else if (fifo_reg_1_write_select & fifo_write)
          fifo_reg_1 <= fifo_wr_data;
    end


  assign fifo_reg_1_write_select = write_pointer[1];
  assign fifo_reg_1_read_select = read_pointer[1];
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          fifo_reg_2 <= 0;
      else if (fifo_reg_2_write_select & fifo_write)
          fifo_reg_2 <= fifo_wr_data;
    end


  assign fifo_reg_2_write_select = write_pointer[2];
  assign fifo_reg_2_read_select = read_pointer[2];
  assign internal_fifo_rd_data = (fifo_reg_0_read_select)? fifo_reg_0 :
    (fifo_reg_1_read_select)? fifo_reg_1 :
    fifo_reg_2;

  assign fifo_rd_data = internal_fifo_empty ? fifo_wr_data : internal_fifo_rd_data;
  //rdaddress_calculator_reg, which is an e_register
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          unxshiftxread_pointerxxrdaddress_calculatorx0_out <= 1;
      else if (clk_en)
          unxshiftxread_pointerxxrdaddress_calculatorx0_out <= unxshiftxread_pointerxxrdaddress_calculatorx0_in;
    end


  assign unxshiftxread_pointerxxrdaddress_calculatorx0_in = (flush)? 1 :
    (fifo_dec)? {unxshiftxread_pointerxxrdaddress_calculatorx0_out[1 : 0],
    read_pointer[2]} :
    unxshiftxread_pointerxxrdaddress_calculatorx0_out;

  assign read_pointer = unxshiftxread_pointerxxrdaddress_calculatorx0_out;
  //wraddress_calculator_reg, which is an e_register
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          unxshiftxwrite_pointerxxwraddress_calculatorx1_out <= 1;
      else if (clk_en)
          unxshiftxwrite_pointerxxwraddress_calculatorx1_out <= unxshiftxwrite_pointerxxwraddress_calculatorx1_in;
    end


  assign unxshiftxwrite_pointerxxwraddress_calculatorx1_in = (flush)? 1 :
    (fifo_inc)? {unxshiftxwrite_pointerxxwraddress_calculatorx1_out[1 : 0],
    write_pointer[2]} :
    unxshiftxwrite_pointerxxwraddress_calculatorx1_out;

  assign write_pointer = unxshiftxwrite_pointerxxwraddress_calculatorx1_out;

endmodule


module enet_nios_instruction_receive (
                                       // inputs:
                                        clk,
                                        commit,
                                        do_branch,
                                        do_force_trap,
                                        do_jump,
                                        feed_new_instruction,
                                        forced_trap_instruction,
                                        ic_datavalid,
                                        ic_readdata,
                                        ic_wait,
                                        is_subinstruction,
                                        p1_flush,
                                        pipe_freeze,
                                        pipe_run,
                                        reset_n,
                                        target_address,
                                        trap_properly_received,

                                       // outputs:
                                        d1_instruction_fifo_out,
                                        d1_instruction_fifo_read_data_bad,
                                        force_trap_acknowledge,
                                        ic_read,
                                        instruction,
                                        instruction_fifo_read_data_bad,
                                        next_instruction_address
                                     )
  /* synthesis auto_dissolve = "FALSE" */ ;

  output  [ 15: 0] d1_instruction_fifo_out;
  output           d1_instruction_fifo_read_data_bad;
  output           force_trap_acknowledge;
  output           ic_read;
  output  [ 15: 0] instruction;
  output           instruction_fifo_read_data_bad;
  output  [ 21: 0] next_instruction_address;
  input            clk;
  input            commit;
  input            do_branch;
  input            do_force_trap;
  input            do_jump;
  input            feed_new_instruction;
  input   [ 15: 0] forced_trap_instruction;
  input            ic_datavalid;
  input   [ 15: 0] ic_readdata;
  input            ic_wait;
  input            is_subinstruction;
  input            p1_flush;
  input            pipe_freeze;
  input            pipe_run;
  input            reset_n;
  input   [ 21: 0] target_address;
  input            trap_properly_received;

  reg     [ 15: 0] d1_instruction_fifo_out;
  reg              d1_instruction_fifo_read_data_bad;
  reg              dont_forget_to_force_trap;
  wire             force_trap_acknowledge;
  wire             ic_read;
  wire    [ 15: 0] instruction;
  wire    [ 15: 0] instruction_fifo_out;
  wire             instruction_fifo_read;
  wire             instruction_fifo_read_data_bad;
  reg     [ 21: 0] next_instruction_address;
  wire             next_instruction_address_enable;
  wire    [ 21: 0] p1_next_instruction_address;
  reg              use_saved_next_address;
  enet_nios_cpu_instruction_fifo_fifo_module the_enet_nios_cpu_instruction_fifo_fifo_module
    (
      .clk                (clk),
      .clk_en             (1'b1),
      .fifo_rd_data       (instruction_fifo_out),
      .fifo_read          (instruction_fifo_read),
      .fifo_read_data_bad (instruction_fifo_read_data_bad),
      .fifo_wr_data       (ic_readdata),
      .fifo_write         (ic_datavalid),
      .flush              (p1_flush),
      .ic_read            (ic_read),
      .ic_wait            (ic_wait),
      .reset_n            (reset_n)
    );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          d1_instruction_fifo_read_data_bad <= 1;
      else if (instruction_fifo_read)
          d1_instruction_fifo_read_data_bad <= (instruction_fifo_read_data_bad) | p1_flush;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          d1_instruction_fifo_out <= 0;
      else if (instruction_fifo_read)
          d1_instruction_fifo_out <= instruction_fifo_out;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dont_forget_to_force_trap <= 0;
      else if (pipe_run)
          if (trap_properly_received)
              dont_forget_to_force_trap <= 0;
          else if (do_force_trap)
              dont_forget_to_force_trap <= -1;
    end


  assign force_trap_acknowledge = dont_forget_to_force_trap;
  assign instruction = force_trap_acknowledge    ? 
    forced_trap_instruction :
    d1_instruction_fifo_out;

  assign instruction_fifo_read = commit | (d1_instruction_fifo_read_data_bad & ~pipe_freeze);
  assign next_instruction_address_enable = ( feed_new_instruction) && 
    (~is_subinstruction   ) &&
    (~pipe_freeze         ) && 
    (~d1_instruction_fifo_read_data_bad) &&
    (~force_trap_acknowledge);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          next_instruction_address <= 0;
      else if (next_instruction_address_enable)
          next_instruction_address <= p1_next_instruction_address;
    end


  assign p1_next_instruction_address = (do_branch | do_jump | use_saved_next_address) ? 
    target_address                                 : 
    next_instruction_address + 1;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          use_saved_next_address <= 0;
      else if (pipe_run)
          if ((do_jump | do_branch) & ~next_instruction_address_enable)
              use_saved_next_address <= -1;
          else if (next_instruction_address_enable)
              use_saved_next_address <= 0;
    end



endmodule


module enet_nios_wait_counter (
                                // inputs:
                                 clk,
                                 commit,
                                 d1_instruction_fifo_read_data_bad,
                                 do_force_trap,
                                 do_iSKPx,
                                 hold_for_hazard,
                                 is_cancelled,
                                 is_neutrino,
                                 must_run_to_completion,
                                 op_jmpcall,
                                 op_save_restore,
                                 pipe_run,
                                 reset_n,
                                 subinstruction,
                                 trap_if_restore,
                                 trap_if_save,

                                // outputs:
                                 feed_new_instruction
                              )
;

  output           feed_new_instruction;
  input            clk;
  input            commit;
  input            d1_instruction_fifo_read_data_bad;
  input            do_force_trap;
  input            do_iSKPx;
  input            hold_for_hazard;
  input            is_cancelled;
  input            is_neutrino;
  input            must_run_to_completion;
  input            op_jmpcall;
  input            op_save_restore;
  input            pipe_run;
  input            reset_n;
  input   [  5: 0] subinstruction;
  input            trap_if_restore;
  input            trap_if_save;

  wire             feed_new_instruction;
  wire    [  2: 0] instruction_delay;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire    [  2: 0] unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_in;
  reg     [  2: 0] unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_out;
  wire             wait_counter_output;
  wire             wait_once_after;
  assign local_pipe_clk_en = commit;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign instruction_delay = (((d1_instruction_fifo_read_data_bad) &  
    (commit                            )))? 3'b111 :
    (wait_once_after)? 3'b110 :
    (must_run_to_completion)? 3'b000 :
    3'b111;

  assign wait_once_after = (op_jmpcall) || (op_save_restore && (trap_if_save    || 
    trap_if_restore ||
    ~is_neutrino      ) );

  //wait_counter_shift_register, which is an e_shift_register
  assign feed_new_instruction = ((wait_counter_output) | (|subinstruction)) & ( ~hold_for_hazard                        ) & ( ~do_force_trap                        );
  //wait_counter_shift_register_reg, which is an e_register
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_out <= 0;
      else if (pipe_run           && 
                            (~|subinstruction) && 
                            ~hold_for_hazard)
          unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_out <= unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_in;
    end


  assign unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_in = (commit)? instruction_delay :
    (1)? {1'b1,
    unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_out[2 : 1]} :
    unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_out;

  assign wait_counter_output = unxshiftxxwait_counter_outputxwait_counter_shift_registerx2_out[0];

endmodule


module enet_nios_subinstruction_unit (
                                       // inputs:
                                        clk,
                                        commit,
                                        d1_instruction_fifo_out,
                                        d1_instruction_fifo_read_data_bad,
                                        feed_new_instruction,
                                        force_trap_acknowledge,
                                        is_cancelled,
                                        is_neutrino,
                                        pipe_run,
                                        reset_n,

                                       // outputs:
                                        is_subinstruction,
                                        subinstruction
                                     )
;

  output           is_subinstruction;
  output  [  5: 0] subinstruction;
  input            clk;
  input            commit;
  input   [ 15: 0] d1_instruction_fifo_out;
  input            d1_instruction_fifo_read_data_bad;
  input            feed_new_instruction;
  input            force_trap_acknowledge;
  input            is_cancelled;
  input            is_neutrino;
  input            pipe_run;
  input            reset_n;

  wire             is_subinstruction;
  wire             local_pipe_clk_en;
  wire    [  5: 0] p1_subinstruction;
  wire    [  5: 0] p1_subinstruction_load_value;
  wire             pipe_state_we;
  wire             subcount_en;
  reg     [  5: 0] subinstruction;
  assign local_pipe_clk_en = commit;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign subcount_en = feed_new_instruction & pipe_run;
  //subinstruction_counter, which is an e_register
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          subinstruction <= 0;
      else if (subcount_en)
          subinstruction <= p1_subinstruction;
    end


  assign p1_subinstruction = ((is_subinstruction))? subinstruction - 1 :
    p1_subinstruction_load_value;

  assign p1_subinstruction_load_value = (d1_instruction_fifo_read_data_bad)? 0 :
    (force_trap_acknowledge)? 3 :
    ((~d1_instruction_fifo_out[15] && ~d1_instruction_fifo_out[14] && ~d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] && ~d1_instruction_fifo_out[10]))? 1 :
    ((((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] && ~d1_instruction_fifo_out[10]) && ~d1_instruction_fifo_out[9] &&  d1_instruction_fifo_out[8])))? 3 :
    ((~d1_instruction_fifo_out[15] && ~d1_instruction_fifo_out[14] && ~d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]))? 1 :
    ((~d1_instruction_fifo_out[15] && ~d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] && ~d1_instruction_fifo_out[12] && ~d1_instruction_fifo_out[11] && ~d1_instruction_fifo_out[10]))? 1 :
    ((~d1_instruction_fifo_out[15] && ~d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] && ~d1_instruction_fifo_out[12] && ~d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]))? 1 :
    ((~d1_instruction_fifo_out[15] && ~d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] && ~d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] && ~d1_instruction_fifo_out[10]))? 1 :
    ((~d1_instruction_fifo_out[15] && ~d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] && ~d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]))? 1 :
    ((((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]) && ~d1_instruction_fifo_out[9] && ~d1_instruction_fifo_out[8] &&  d1_instruction_fifo_out[7] && ~d1_instruction_fifo_out[6] &&  d1_instruction_fifo_out[5])))? 1 :
    ((((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]) && ~d1_instruction_fifo_out[9] && ~d1_instruction_fifo_out[8] &&  d1_instruction_fifo_out[7] &&  d1_instruction_fifo_out[6] && ~d1_instruction_fifo_out[5])))? 1 :
    ((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] && ~d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] && ~d1_instruction_fifo_out[11] && ~d1_instruction_fifo_out[10]))? 1 :
    ((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] && ~d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] && ~d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]))? 1 :
    ((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] && ~d1_instruction_fifo_out[11] && ~d1_instruction_fifo_out[10]))? 34 :
    ((((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]) &&  d1_instruction_fifo_out[9] && ~d1_instruction_fifo_out[8] &&  d1_instruction_fifo_out[7] && ~d1_instruction_fifo_out[6] && ~d1_instruction_fifo_out[5])))? 1 :
    ((((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]) &&  d1_instruction_fifo_out[9] && ~d1_instruction_fifo_out[8] &&  d1_instruction_fifo_out[7] && ~d1_instruction_fifo_out[6] &&  d1_instruction_fifo_out[5])))? 2 :
    ((((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]) &&  d1_instruction_fifo_out[9] && ~d1_instruction_fifo_out[8] &&  d1_instruction_fifo_out[7] &&  d1_instruction_fifo_out[6] && ~d1_instruction_fifo_out[5])))? 1 :
    ((((~d1_instruction_fifo_out[15] &&  d1_instruction_fifo_out[14] &&  d1_instruction_fifo_out[13] &&  d1_instruction_fifo_out[12] &&  d1_instruction_fifo_out[11] &&  d1_instruction_fifo_out[10]) &&  d1_instruction_fifo_out[9] &&  d1_instruction_fifo_out[8] && ~d1_instruction_fifo_out[7] &&  d1_instruction_fifo_out[6] && ~d1_instruction_fifo_out[5])))? 1 :
    6'b0;

  assign is_subinstruction = (|(subinstruction)) & (~is_neutrino);

endmodule


module enet_nios_instruction_scheduler (
                                         // inputs:
                                          clk,
                                          d1_instruction_fifo_out,
                                          d1_instruction_fifo_read_data_bad,
                                          do_cancel_next_instruction,
                                          do_force_trap,
                                          do_iPFXx,
                                          do_iSKPx,
                                          do_skip,
                                          force_trap_acknowledge,
                                          hold_for_hazard,
                                          is_cancelled,
                                          must_run_to_completion,
                                          op_jmpcall,
                                          op_save_restore,
                                          pipe_run,
                                          reset_n,
                                          trap_if_restore,
                                          trap_if_save,

                                         // outputs:
                                          commit,
                                          feed_new_instruction,
                                          is_cancelled_from_commit_stage,
                                          is_neutrino,
                                          is_subinstruction,
                                          subinstruction
                                       )
;

  output           commit;
  output           feed_new_instruction;
  output           is_cancelled_from_commit_stage;
  output           is_neutrino;
  output           is_subinstruction;
  output  [  5: 0] subinstruction;
  input            clk;
  input   [ 15: 0] d1_instruction_fifo_out;
  input            d1_instruction_fifo_read_data_bad;
  input            do_cancel_next_instruction;
  input            do_force_trap;
  input            do_iPFXx;
  input            do_iSKPx;
  input            do_skip;
  input            force_trap_acknowledge;
  input            hold_for_hazard;
  input            is_cancelled;
  input            must_run_to_completion;
  input            op_jmpcall;
  input            op_save_restore;
  input            pipe_run;
  input            reset_n;
  input            trap_if_restore;
  input            trap_if_save;

  wire             cancel_delay_slot_acknowledge;
  wire             commit;
  reg              dont_forget_to_cancel_delay_slot;
  reg              dont_forget_to_skip;
  wire             feed_new_instruction;
  wire             instruction_not_ready;
  reg              is_cancelled_from_commit_stage;
  reg              is_neutrino;
  wire             is_subinstruction;
  wire             local_pipe_clk_en;
  wire             p1_is_neutrino;
  wire             pipe_state_we;
  wire             qualified_skip_request;
  wire             skip_acknowledge;
  wire    [  5: 0] subinstruction;
  wire             waiting_for_skip_outcome;
  assign local_pipe_clk_en = commit;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_wait_counter the_enet_nios_wait_counter
    (
      .clk                               (clk),
      .commit                            (commit),
      .d1_instruction_fifo_read_data_bad (d1_instruction_fifo_read_data_bad),
      .do_force_trap                     (do_force_trap),
      .do_iSKPx                          (do_iSKPx),
      .feed_new_instruction              (feed_new_instruction),
      .hold_for_hazard                   (hold_for_hazard),
      .is_cancelled                      (is_cancelled),
      .is_neutrino                       (is_neutrino),
      .must_run_to_completion            (must_run_to_completion),
      .op_jmpcall                        (op_jmpcall),
      .op_save_restore                   (op_save_restore),
      .pipe_run                          (pipe_run),
      .reset_n                           (reset_n),
      .subinstruction                    (subinstruction),
      .trap_if_restore                   (trap_if_restore),
      .trap_if_save                      (trap_if_save)
    );

  enet_nios_subinstruction_unit the_enet_nios_subinstruction_unit
    (
      .clk                               (clk),
      .commit                            (commit),
      .d1_instruction_fifo_out           (d1_instruction_fifo_out),
      .d1_instruction_fifo_read_data_bad (d1_instruction_fifo_read_data_bad),
      .feed_new_instruction              (feed_new_instruction),
      .force_trap_acknowledge            (force_trap_acknowledge),
      .is_cancelled                      (is_cancelled),
      .is_neutrino                       (is_neutrino),
      .is_subinstruction                 (is_subinstruction),
      .pipe_run                          (pipe_run),
      .reset_n                           (reset_n),
      .subinstruction                    (subinstruction)
    );

  assign commit = ( feed_new_instruction             ) && 
    (~is_subinstruction                ) &&         
    ( pipe_run                         );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          is_neutrino <= 1;
      else if (pipe_run)
          is_neutrino <= p1_is_neutrino;
    end


  assign p1_is_neutrino = instruction_not_ready && (~hold_for_hazard);
  assign instruction_not_ready = (~feed_new_instruction                 ) | 
    (d1_instruction_fifo_read_data_bad   &
    ~is_subinstruction                   );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          is_cancelled_from_commit_stage <= 0;
      else if (commit)
          if (skip_acknowledge ||
                       cancel_delay_slot_acknowledge)
              is_cancelled_from_commit_stage <= -1;
          else if (~p1_is_neutrino)
              is_cancelled_from_commit_stage <= 0;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dont_forget_to_cancel_delay_slot <= 0;
      else if (pipe_run)
          if ((commit && (~d1_instruction_fifo_read_data_bad)) && cancel_delay_slot_acknowledge)
              dont_forget_to_cancel_delay_slot <= 0;
          else if (do_cancel_next_instruction)
              dont_forget_to_cancel_delay_slot <= -1;
    end


  assign cancel_delay_slot_acknowledge = do_cancel_next_instruction || 
    dont_forget_to_cancel_delay_slot;

  assign waiting_for_skip_outcome = 1'b1;
  assign qualified_skip_request = waiting_for_skip_outcome && do_skip;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dont_forget_to_skip <= 0;
      else if (pipe_run)
          if (commit            && 
                        ~p1_is_neutrino   &&
                        skip_acknowledge  &&
                        ~do_iPFXx)
              dont_forget_to_skip <= 0;
          else if (qualified_skip_request)
              dont_forget_to_skip <= -1;
    end


  assign skip_acknowledge = qualified_skip_request || 
    dont_forget_to_skip;


endmodule


module enet_nios_commitment_maker (
                                    // inputs:
                                     branch_base,
                                     clk,
                                     do_branch,
                                     do_cancel_next_instruction,
                                     do_force_trap,
                                     do_iPFXx,
                                     do_iSKPx,
                                     do_jump,
                                     do_skip,
                                     forced_trap_instruction,
                                     hold_for_hazard,
                                     ic_datavalid,
                                     ic_readdata,
                                     ic_wait,
                                     is_cancelled,
                                     jump_target_address,
                                     must_run_to_completion,
                                     op_jmpcall,
                                     op_save_restore,
                                     pipe_freeze,
                                     pipe_run,
                                     reset_n,
                                     signed_branch_offset,
                                     trap_if_restore,
                                     trap_if_save,
                                     trap_properly_received,

                                    // outputs:
                                     commit,
                                     ic_address,
                                     ic_flush,
                                     ic_read,
                                     instruction,
                                     is_cancelled_from_commit_stage,
                                     is_neutrino,
                                     next_instruction_address,
                                     subinstruction
                                  )
;

  output           commit;
  output  [ 22: 0] ic_address;
  output           ic_flush;
  output           ic_read;
  output  [ 15: 0] instruction;
  output           is_cancelled_from_commit_stage;
  output           is_neutrino;
  output  [ 21: 0] next_instruction_address;
  output  [  5: 0] subinstruction;
  input   [ 21: 0] branch_base;
  input            clk;
  input            do_branch;
  input            do_cancel_next_instruction;
  input            do_force_trap;
  input            do_iPFXx;
  input            do_iSKPx;
  input            do_jump;
  input            do_skip;
  input   [ 15: 0] forced_trap_instruction;
  input            hold_for_hazard;
  input            ic_datavalid;
  input   [ 15: 0] ic_readdata;
  input            ic_wait;
  input            is_cancelled;
  input   [ 21: 0] jump_target_address;
  input            must_run_to_completion;
  input            op_jmpcall;
  input            op_save_restore;
  input            pipe_freeze;
  input            pipe_run;
  input            reset_n;
  input   [ 21: 0] signed_branch_offset;
  input            trap_if_restore;
  input            trap_if_save;
  input            trap_properly_received;

  wire             commit;
  wire    [ 15: 0] d1_instruction_fifo_out;
  wire             d1_instruction_fifo_read_data_bad;
  wire             feed_new_instruction;
  wire             force_trap_acknowledge;
  wire    [ 22: 0] ic_address;
  wire             ic_flush;
  wire             ic_read;
  wire    [ 15: 0] instruction;
  wire             instruction_fifo_read_data_bad;
  wire             is_cancelled_from_commit_stage;
  wire             is_neutrino;
  wire             is_subinstruction;
  wire             local_pipe_clk_en;
  wire    [ 21: 0] next_instruction_address;
  wire             p1_flush;
  wire             pipe_state_we;
  wire    [  5: 0] subinstruction;
  wire    [ 21: 0] target_address;
  assign local_pipe_clk_en = commit;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_instruction_fetch the_enet_nios_instruction_fetch
    (
      .branch_base                       (branch_base),
      .clk                               (clk),
      .d1_instruction_fifo_read_data_bad (d1_instruction_fifo_read_data_bad),
      .do_branch                         (do_branch),
      .do_jump                           (do_jump),
      .ic_address                        (ic_address),
      .ic_flush                          (ic_flush),
      .ic_read                           (ic_read),
      .ic_wait                           (ic_wait),
      .instruction_fifo_read_data_bad    (instruction_fifo_read_data_bad),
      .jump_target_address               (jump_target_address),
      .p1_flush                          (p1_flush),
      .pipe_run                          (pipe_run),
      .reset_n                           (reset_n),
      .signed_branch_offset              (signed_branch_offset),
      .target_address                    (target_address)
    );

  enet_nios_instruction_receive the_enet_nios_instruction_receive
    (
      .clk                               (clk),
      .commit                            (commit),
      .d1_instruction_fifo_out           (d1_instruction_fifo_out),
      .d1_instruction_fifo_read_data_bad (d1_instruction_fifo_read_data_bad),
      .do_branch                         (do_branch),
      .do_force_trap                     (do_force_trap),
      .do_jump                           (do_jump),
      .feed_new_instruction              (feed_new_instruction),
      .force_trap_acknowledge            (force_trap_acknowledge),
      .forced_trap_instruction           (forced_trap_instruction),
      .ic_datavalid                      (ic_datavalid),
      .ic_read                           (ic_read),
      .ic_readdata                       (ic_readdata),
      .ic_wait                           (ic_wait),
      .instruction                       (instruction),
      .instruction_fifo_read_data_bad    (instruction_fifo_read_data_bad),
      .is_subinstruction                 (is_subinstruction),
      .next_instruction_address          (next_instruction_address),
      .p1_flush                          (p1_flush),
      .pipe_freeze                       (pipe_freeze),
      .pipe_run                          (pipe_run),
      .reset_n                           (reset_n),
      .target_address                    (target_address),
      .trap_properly_received            (trap_properly_received)
    );

  enet_nios_instruction_scheduler the_enet_nios_instruction_scheduler
    (
      .clk                               (clk),
      .commit                            (commit),
      .d1_instruction_fifo_out           (d1_instruction_fifo_out),
      .d1_instruction_fifo_read_data_bad (d1_instruction_fifo_read_data_bad),
      .do_cancel_next_instruction        (do_cancel_next_instruction),
      .do_force_trap                     (do_force_trap),
      .do_iPFXx                          (do_iPFXx),
      .do_iSKPx                          (do_iSKPx),
      .do_skip                           (do_skip),
      .feed_new_instruction              (feed_new_instruction),
      .force_trap_acknowledge            (force_trap_acknowledge),
      .hold_for_hazard                   (hold_for_hazard),
      .is_cancelled                      (is_cancelled),
      .is_cancelled_from_commit_stage    (is_cancelled_from_commit_stage),
      .is_neutrino                       (is_neutrino),
      .is_subinstruction                 (is_subinstruction),
      .must_run_to_completion            (must_run_to_completion),
      .op_jmpcall                        (op_jmpcall),
      .op_save_restore                   (op_save_restore),
      .pipe_run                          (pipe_run),
      .reset_n                           (reset_n),
      .subinstruction                    (subinstruction),
      .trap_if_restore                   (trap_if_restore),
      .trap_if_save                      (trap_if_save)
    );


endmodule


module enet_nios_2ei4_unit (
                             // inputs:
                              imm5,
                              instruction_1,
                              is_cancelled,
                              is_neutrino,
                              pipe_run,

                             // outputs:
                              twoEi4
                           )
;

  output  [ 15: 0] twoEi4;
  input   [  4: 0] imm5;
  input   [ 15: 0] instruction_1;
  input            is_cancelled;
  input            is_neutrino;
  input            pipe_run;

  wire    [ 15: 0] instruction;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire             result_bit_0;
  wire             result_bit_1;
  wire             result_bit_10;
  wire             result_bit_11;
  wire             result_bit_12;
  wire             result_bit_13;
  wire             result_bit_14;
  wire             result_bit_15;
  wire             result_bit_2;
  wire             result_bit_3;
  wire             result_bit_4;
  wire             result_bit_5;
  wire             result_bit_6;
  wire             result_bit_7;
  wire             result_bit_8;
  wire             result_bit_9;
  wire    [ 15: 0] twoEi4;
  assign instruction = instruction_1;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign result_bit_15 = imm5[3 : 0] == 15;
  assign result_bit_14 = imm5[3 : 0] == 14;
  assign result_bit_13 = imm5[3 : 0] == 13;
  assign result_bit_12 = imm5[3 : 0] == 12;
  assign result_bit_11 = imm5[3 : 0] == 11;
  assign result_bit_10 = imm5[3 : 0] == 10;
  assign result_bit_9 = imm5[3 : 0] == 9;
  assign result_bit_8 = imm5[3 : 0] == 8;
  assign result_bit_7 = imm5[3 : 0] == 7;
  assign result_bit_6 = imm5[3 : 0] == 6;
  assign result_bit_5 = imm5[3 : 0] == 5;
  assign result_bit_4 = imm5[3 : 0] == 4;
  assign result_bit_3 = imm5[3 : 0] == 3;
  assign result_bit_2 = imm5[3 : 0] == 2;
  assign result_bit_1 = imm5[3 : 0] == 1;
  assign result_bit_0 = imm5[3 : 0] == 0;
  assign twoEi4 = {result_bit_15,
    result_bit_14,
    result_bit_13,
    result_bit_12,
    result_bit_11,
    result_bit_10,
    result_bit_9,
    result_bit_8,
    result_bit_7,
    result_bit_6,
    result_bit_5,
    result_bit_4,
    result_bit_3,
    result_bit_2,
    result_bit_1,
    result_bit_0};


endmodule


module enet_nios_constant_generator (
                                      // inputs:
                                       byte_offset,
                                       clk,
                                       do_iLDx,
                                       do_iMOVHI,
                                       do_iSTx,
                                       do_narrow_stack_offset,
                                       do_normal_offset,
                                       do_normal_static_write,
                                       do_save_offset,
                                       do_stack_offset,
                                       do_trap_offset,
                                       imm5,
                                       instruction_1,
                                       is_cancelled,
                                       is_neutrino,
                                       narrow_stack_offset,
                                       op_b_from_2Ei5,
                                       op_b_from_Ki5,
                                       op_b_from_reg_b,
                                       op_b_from_reg_or_const,
                                       op_is_PFX,
                                       override_b_control,
                                       pipe_run,
                                       prefix_payload,
                                       reset_n,
                                       save_offset,
                                       stack_offset,
                                       subinstruction,
                                       trap_offset,

                                      // outputs:
                                       const,
                                       do_override_op_b,
                                       offset,
                                       op_b_from_reg_really,
                                       op_b_hi_from_zero,
                                       op_b_lo_from_zero
                                    )
;

  output  [ 15: 0] const;
  output           do_override_op_b;
  output  [ 22: 0] offset;
  output           op_b_from_reg_really;
  output           op_b_hi_from_zero;
  output           op_b_lo_from_zero;
  input   [  1: 0] byte_offset;
  input            clk;
  input            do_iLDx;
  input            do_iMOVHI;
  input            do_iSTx;
  input            do_narrow_stack_offset;
  input            do_normal_offset;
  input            do_normal_static_write;
  input            do_save_offset;
  input            do_stack_offset;
  input            do_trap_offset;
  input   [  4: 0] imm5;
  input   [ 15: 0] instruction_1;
  input            is_cancelled;
  input            is_neutrino;
  input   [  9: 0] narrow_stack_offset;
  input            op_b_from_2Ei5;
  input            op_b_from_Ki5;
  input            op_b_from_reg_b;
  input            op_b_from_reg_or_const;
  input            op_is_PFX;
  input            override_b_control;
  input            pipe_run;
  input   [ 10: 0] prefix_payload;
  input            reset_n;
  input   [  7: 0] save_offset;
  input   [  7: 0] stack_offset;
  input   [  5: 0] subinstruction;
  input   [  5: 0] trap_offset;

  reg     [ 10: 0] K;
  reg     [ 15: 0] const;
  reg              do_override_op_b;
  wire    [  4: 0] imm5_adjusted;
  wire    [ 15: 0] instruction;
  reg              last_instruction_was_prefix;
  wire             leave_op_b_hi_alone;
  wire             local_pipe_clk_en;
  wire    [ 22: 0] offset;
  reg     [  1: 0] offset_lsbs;
  wire             op_b_from_Ki5_really;
  reg              op_b_from_reg_really;
  reg              op_b_hi_from_zero;
  reg              op_b_lo_from_zero;
  wire             op_uses_Ki5;
  wire    [ 15: 0] p1_const;
  wire             p1_do_override_op_b;
  wire    [  1: 0] p1_offset_lsbs;
  wire             p1_op_b_from_reg_really;
  wire             pipe_state_we;
  wire    [ 15: 0] sigmaK;
  wire    [ 15: 0] twoEi4;
  wire    [  7: 0] word_part_of_narrow_stack_offset;
  assign instruction = instruction_1;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_2ei4_unit the_enet_nios_2ei4_unit
    (
      .imm5          (imm5),
      .instruction_1 (instruction_1),
      .is_cancelled  (is_cancelled),
      .is_neutrino   (is_neutrino),
      .pipe_run      (pipe_run),
      .twoEi4        (twoEi4)
    );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          K <= 0;
      else if (pipe_state_we)
          if (~op_is_PFX)
              K <= 0;
          else 
            K <= prefix_payload;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          last_instruction_was_prefix <= 0;
      else if (pipe_state_we)
          last_instruction_was_prefix <= op_is_PFX;
    end


  assign sigmaK = {{5 {K[10]}},
    K};

  assign imm5_adjusted = imm5;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          const <= 0;
      else if (local_pipe_clk_en)
          const <= p1_const;
    end


  assign p1_const = ({16 {op_b_from_2Ei5}} & twoEi4) |
    ({16 {op_uses_Ki5}} & {K,
    imm5_adjusted}) |
    ({16 {do_normal_offset}} & sigmaK) |
    ({16 {do_save_offset}} & {save_offset,
    {2{1'b0}}}) |
    ({16 {do_stack_offset}} & stack_offset) |
    ({16 {do_trap_offset}} & trap_offset) |
    ({16 {do_narrow_stack_offset}} & word_part_of_narrow_stack_offset);

  assign word_part_of_narrow_stack_offset = narrow_stack_offset[9 : 2];
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          offset_lsbs <= 0;
      else if (local_pipe_clk_en)
          offset_lsbs <= p1_offset_lsbs;
    end


  assign p1_offset_lsbs = (do_narrow_stack_offset)? narrow_stack_offset[1 : 0] :
    (do_normal_static_write)? byte_offset :
    2'b0;

  assign offset = {{5 {const[15]}},
    const,
    offset_lsbs};

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_b_lo_from_zero <= 0;
      else if (local_pipe_clk_en)
          op_b_lo_from_zero <= do_iMOVHI ||(op_b_from_2Ei5 && imm5[4]);
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_b_hi_from_zero <= 0;
      else if (local_pipe_clk_en)
          op_b_hi_from_zero <= do_save_offset                                ||
                    (op_b_from_Ki5_really && ~leave_op_b_hi_alone) ||
                    (op_b_from_2Ei5 && ~imm5[4]           );

    end


  assign leave_op_b_hi_alone = do_iMOVHI || do_iLDx || do_iSTx || do_trap_offset;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          do_override_op_b <= 0;
      else if (local_pipe_clk_en)
          do_override_op_b <= p1_do_override_op_b;
    end


  assign p1_do_override_op_b = override_b_control && (subinstruction == 0);
  assign p1_op_b_from_reg_really = (op_b_from_reg_b || ((~last_instruction_was_prefix)&& 
    ( op_b_from_reg_or_const     )  ) ) && 
    (~p1_do_override_op_b                                  );

  assign op_uses_Ki5 = op_b_from_reg_or_const || op_b_from_Ki5;
  assign op_b_from_Ki5_really = op_b_from_Ki5 || 
    (( last_instruction_was_prefix) && 
    ( op_b_from_reg_or_const)        );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_b_from_reg_really <= 0;
      else if (local_pipe_clk_en)
          op_b_from_reg_really <= p1_op_b_from_reg_really;
    end



endmodule


module enet_nios_reg_index_calculator (
                                        // inputs:
                                         a_field,
                                         a_index_from_o7,
                                         a_index_from_sp,
                                         a_index_from_zero,
                                         b_field,
                                         b_index_from_sp,
                                         b_index_from_zero,
                                         clk,
                                         commit,
                                         dest_index_from_o7,
                                         dest_index_from_sp,
                                         dest_index_from_zero,
                                         instruction_0,
                                         is_cancelled,
                                         is_neutrino,
                                         op_subroutine,
                                         p_field,
                                         reset_n,

                                        // outputs:
                                         a_local,
                                         b_local,
                                         dest_local
                                      )
;

  output  [  4: 0] a_local;
  output  [  4: 0] b_local;
  output  [  4: 0] dest_local;
  input   [  4: 0] a_field;
  input            a_index_from_o7;
  input            a_index_from_sp;
  input            a_index_from_zero;
  input   [  4: 0] b_field;
  input            b_index_from_sp;
  input            b_index_from_zero;
  input            clk;
  input            commit;
  input            dest_index_from_o7;
  input            dest_index_from_sp;
  input            dest_index_from_zero;
  input   [ 15: 0] instruction_0;
  input            is_cancelled;
  input            is_neutrino;
  input            op_subroutine;
  input   [  1: 0] p_field;
  input            reset_n;

  reg     [  4: 0] a_local;
  wire             b_index_from_a;
  wire             b_index_from_p;
  reg     [  4: 0] b_local;
  reg     [  4: 0] dest_local;
  wire             local_pipe_clk_en;
  wire    [  4: 0] p1_a_local;
  wire    [  4: 0] p1_b_local;
  wire    [  4: 0] p1_dest_local;
  wire             pipe_state_we;
  wire    [  4: 0] pre_b_local;
  assign local_pipe_clk_en = commit;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign p1_a_local = ({5 {a_index_from_sp}} & 5'd14) |
    ({5 {a_index_from_o7}} & 5'd15) |
    ({5 {a_index_from_zero}} & 5'd0) |
    ({5 {(~(a_index_from_sp)) && (~(a_index_from_o7)) && (~(a_index_from_zero))}} & a_field);

  assign pre_b_local = ({5 {b_index_from_a}} & a_field) |
    ({5 {b_index_from_p}} & {3'b100,
    p_field}) |
    ({5 {b_index_from_sp}} & 5'd14) |
    ({5 {(~(b_index_from_a)) && (~(b_index_from_p)) && (~(b_index_from_sp))}} & b_field);

  assign p1_b_local = pre_b_local & ~({5 {b_index_from_zero}});
  assign p1_dest_local = ({5 {dest_index_from_sp}} & 5'd14) |
    ({5 {dest_index_from_o7}} & 5'd15) |
    ({5 {dest_index_from_zero}} & 5'd0) |
    ({5 {(~(dest_index_from_sp)) && (~(dest_index_from_o7)) && (~(dest_index_from_zero))}} & a_field);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dest_local <= 0;
      else if (local_pipe_clk_en)
          dest_local <= p1_dest_local;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          a_local <= 0;
      else if (local_pipe_clk_en)
          a_local <= p1_a_local;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          b_local <= 0;
      else if (local_pipe_clk_en)
          b_local <= p1_b_local;
    end


  assign b_index_from_a = (instruction_0[15 : 12] == 4'b0111)&& (instruction_0[10] | instruction_0[11]);
  assign b_index_from_p = instruction_0[15 : 14] == 2'b10;

endmodule


module enet_nios_branch_unit (
                               // inputs:
                                branch_offset,
                                instruction_1,
                                is_cancelled,
                                is_cancelled_from_commit_stage,
                                is_neutrino,
                                next_instruction_address,
                                op_is_branch,
                                pipe_run,

                               // outputs:
                                branch_base,
                                do_branch,
                                signed_branch_offset
                             )
;

  output  [ 21: 0] branch_base;
  output           do_branch;
  output  [ 21: 0] signed_branch_offset;
  input   [ 10: 0] branch_offset;
  input   [ 15: 0] instruction_1;
  input            is_cancelled;
  input            is_cancelled_from_commit_stage;
  input            is_neutrino;
  input   [ 21: 0] next_instruction_address;
  input            op_is_branch;
  input            pipe_run;

  wire    [ 21: 0] branch_base;
  wire             do_branch;
  wire    [ 15: 0] instruction;
  wire             local_pipe_clk_en;
  wire    [ 21: 0] next_instruction_address_1;
  wire             offset_sign_bit;
  wire             pipe_state_we;
  wire    [ 21: 0] signed_branch_offset;
  assign instruction = instruction_1;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign do_branch = ( op_is_branch                   ) && 
    (~is_neutrino                    ) &&             
    (~is_cancelled_from_commit_stage );

  assign offset_sign_bit = branch_offset[10];
  assign signed_branch_offset = {{11{offset_sign_bit}},
    branch_offset};

  assign next_instruction_address_1 = next_instruction_address;
  assign branch_base = next_instruction_address_1;

endmodule


module enet_nios_index_match_unit (
                                    // inputs:
                                     CWP,
                                     a_local,
                                     b_local,
                                     clk,
                                     dest_local_2,
                                     dest_local_3,
                                     do_iRESTORE,
                                     do_iSAVE,
                                     instruction_1,
                                     is_cancelled_from_commit_stage,
                                     is_neutrino,
                                     op_uses_a_pre_alu,
                                     op_uses_b_pre_alu,
                                     pipe_run,
                                     reg_not_modified,
                                     reset_n,
                                     subinstruction,

                                    // outputs:
                                     a_matches_dest1,
                                     a_matches_dest2,
                                     b_matches_dest1,
                                     b_matches_dest2,
                                     dest_cwp,
                                     hold_for_hazard,
                                     is_cancelled,
                                     qualified_do_iRESTORE,
                                     qualified_do_iSAVE
                                  )
;

  output           a_matches_dest1;
  output           a_matches_dest2;
  output           b_matches_dest1;
  output           b_matches_dest2;
  output  [  4: 0] dest_cwp;
  output           hold_for_hazard;
  output           is_cancelled;
  output           qualified_do_iRESTORE;
  output           qualified_do_iSAVE;
  input   [  4: 0] CWP;
  input   [  4: 0] a_local;
  input   [  4: 0] b_local;
  input            clk;
  input   [  4: 0] dest_local_2;
  input   [  4: 0] dest_local_3;
  input            do_iRESTORE;
  input            do_iSAVE;
  input   [ 15: 0] instruction_1;
  input            is_cancelled_from_commit_stage;
  input            is_neutrino;
  input            op_uses_a_pre_alu;
  input            op_uses_b_pre_alu;
  input            pipe_run;
  input            reg_not_modified;
  input            reset_n;
  input   [  5: 0] subinstruction;

  wire             a_hazard1;
  wire             a_hazard2;
  reg              a_matches_dest1;
  reg              a_matches_dest2;
  wire             b_hazard1;
  wire             b_hazard2;
  reg              b_matches_dest1;
  reg              b_matches_dest2;
  wire             data_hazard;
  wire    [  4: 0] dest_cwp;
  wire             hold_for_hazard;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             local_pipe_clk_en;
  reg              next_stage_modifies_register;
  wire             p1_a_matches_dest1;
  wire             p1_a_matches_dest2;
  wire             p1_b_matches_dest1;
  wire             p1_b_matches_dest2;
  wire             pipe_state_we;
  wire             qualified_do_iRESTORE;
  wire             qualified_do_iSAVE;
  reg              second_stage_modifies_register;
  assign instruction = instruction_1;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign p1_a_matches_dest1 = (a_local == dest_local_2) && 
    (next_stage_modifies_register);

  assign p1_b_matches_dest1 = (b_local == dest_local_2) && 
    (next_stage_modifies_register);

  assign p1_a_matches_dest2 = (a_local == dest_local_3) && 
    (second_stage_modifies_register);

  assign p1_b_matches_dest2 = (b_local == dest_local_3) && 
    (second_stage_modifies_register);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          a_matches_dest1 <= 0;
      else if (local_pipe_clk_en)
          a_matches_dest1 <= p1_a_matches_dest1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          a_matches_dest2 <= 0;
      else if (local_pipe_clk_en)
          a_matches_dest2 <= p1_a_matches_dest2;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          b_matches_dest1 <= 0;
      else if (local_pipe_clk_en)
          b_matches_dest1 <= p1_b_matches_dest1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          b_matches_dest2 <= 0;
      else if (local_pipe_clk_en)
          b_matches_dest2 <= p1_b_matches_dest2;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          next_stage_modifies_register <= 0;
      else if (local_pipe_clk_en)
          next_stage_modifies_register <= ~reg_not_modified        &&
                    ~is_cancelled            &&
                    ~(|subinstruction)       &&
                    ~is_neutrino;

    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          second_stage_modifies_register <= 0;
      else if (local_pipe_clk_en)
          second_stage_modifies_register <= next_stage_modifies_register;
    end


  assign b_hazard2 = p1_b_matches_dest1 && op_uses_b_pre_alu;
  assign a_hazard2 = p1_a_matches_dest1 && op_uses_a_pre_alu;
  assign a_hazard1 = 1'b0;
  assign b_hazard1 = 1'b0;
  assign data_hazard = a_hazard1 || b_hazard1 || 
    a_hazard2 || b_hazard2;

  assign hold_for_hazard = (data_hazard                     ) && 
    (~is_neutrino                    ) &&
    (~is_cancelled_from_commit_stage ) &&
    (next_stage_modifies_register    );

  assign is_cancelled = is_cancelled_from_commit_stage || 
    hold_for_hazard;

  assign dest_cwp = CWP;
  assign qualified_do_iSAVE = do_iSAVE    && ~is_neutrino && ~is_cancelled;
  assign qualified_do_iRESTORE = do_iRESTORE && ~is_neutrino && ~is_cancelled;

endmodule


module enet_nios_jump_unit (
                             // inputs:
                              dest_local_2,
                              instruction_2,
                              is_cancelled_2,
                              is_neutrino_2,
                              op_is_jump,
                              pipe_run,
                              subinstruction_2,

                             // outputs:
                              do_jump
                           )
;

  output           do_jump;
  input   [  4: 0] dest_local_2;
  input   [ 15: 0] instruction_2;
  input            is_cancelled_2;
  input            is_neutrino_2;
  input            op_is_jump;
  input            pipe_run;
  input   [  5: 0] subinstruction_2;

  wire    [  4: 0] dest_local;
  wire             do_jump;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_2;
  assign dest_local = dest_local_2;
  assign instruction = instruction_2;
  assign is_cancelled = is_cancelled_2;
  assign subinstruction = subinstruction_2;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign do_jump = ( op_is_jump    ) && 
    (~is_neutrino   ) &&
    (~is_cancelled  ) && (pipe_run);


endmodule


module enet_nios_offset_adder (
                                // inputs:
                                 base_address,
                                 clk,
                                 dest_local_2,
                                 instruction_2,
                                 is_cancelled_2,
                                 is_neutrino_2,
                                 offset,
                                 pipe_run,
                                 reset_n,
                                 subinstruction_2,

                                // outputs:
                                 dc_address
                              )
;

  output  [ 22: 0] dc_address;
  input   [ 22: 0] base_address;
  input            clk;
  input   [  4: 0] dest_local_2;
  input   [ 15: 0] instruction_2;
  input            is_cancelled_2;
  input            is_neutrino_2;
  input   [ 22: 0] offset;
  input            pipe_run;
  input            reset_n;
  input   [  5: 0] subinstruction_2;

  wire    [ 22: 0] aligned_base_address;
  reg     [ 22: 0] dc_address;
  wire    [  4: 0] dest_local;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire    [ 22: 0] p1_d_address;
  wire             pipe_state_we;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_2;
  assign dest_local = dest_local_2;
  assign instruction = instruction_2;
  assign is_cancelled = is_cancelled_2;
  assign subinstruction = subinstruction_2;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dc_address <= 0;
      else if (local_pipe_clk_en)
          dc_address <= p1_d_address;
    end


  assign p1_d_address = aligned_base_address + offset;
  assign aligned_base_address = {base_address[22 : 2],
    {2{1'b0}}};


endmodule


module enet_nios_data_master (
                               // inputs:
                                base_address,
                                clk,
                                dc_wait,
                                dest_local_2,
                                do_dynamic_narrow_write,
                                do_iTRAP_0,
                                do_write_16,
                                do_write_8,
                                instruction_2,
                                is_cancelled_2,
                                is_neutrino_2,
                                offset,
                                op_a,
                                op_is_LDx,
                                op_is_STx,
                                reset_n,
                                subinstruction_2,

                               // outputs:
                                dc_address,
                                dc_byteenable,
                                dc_read,
                                dc_write,
                                dc_writedata,
                                pipe_freeze,
                                pipe_run
                             )
  /* synthesis auto_dissolve = "FALSE" */ ;

  output  [ 22: 0] dc_address;
  output  [  3: 0] dc_byteenable;
  output           dc_read;
  output           dc_write;
  output  [ 31: 0] dc_writedata;
  output           pipe_freeze;
  output           pipe_run;
  input   [ 22: 0] base_address;
  input            clk;
  input            dc_wait;
  input   [  4: 0] dest_local_2;
  input            do_dynamic_narrow_write;
  input            do_iTRAP_0;
  input            do_write_16;
  input            do_write_8;
  input   [ 15: 0] instruction_2;
  input            is_cancelled_2;
  input            is_neutrino_2;
  input   [ 22: 0] offset;
  input   [ 31: 0] op_a;
  input            op_is_LDx;
  input            op_is_STx;
  input            reset_n;
  input   [  5: 0] subinstruction_2;

  wire    [ 22: 0] dc_address;
  reg     [  3: 0] dc_byteenable;
  reg              dc_read;
  wire             dc_read_pre;
  reg              dc_write;
  wire    [ 31: 0] dc_writedata;
  wire    [  4: 0] dest_local;
  wire             do_load;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire    [  3: 0] p1_byteenable;
  wire             pipe_freeze;
  wire             pipe_run;
  wire             pipe_state_we;
  wire    [  5: 0] subinstruction;
  wire    [  1: 0] which_byte;
  wire             which_halfword;
  assign is_neutrino = is_neutrino_2;
  assign dest_local = dest_local_2;
  assign instruction = instruction_2;
  assign is_cancelled = is_cancelled_2;
  assign subinstruction = subinstruction_2;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_offset_adder the_enet_nios_offset_adder
    (
      .base_address     (base_address),
      .clk              (clk),
      .dc_address       (dc_address),
      .dest_local_2     (dest_local_2),
      .instruction_2    (instruction_2),
      .is_cancelled_2   (is_cancelled_2),
      .is_neutrino_2    (is_neutrino_2),
      .offset           (offset),
      .pipe_run         (pipe_run),
      .reset_n          (reset_n),
      .subinstruction_2 (subinstruction_2)
    );

  assign pipe_freeze = dc_wait && (dc_read || dc_write);
  assign pipe_run = ~pipe_freeze;
  assign dc_writedata = op_a;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dc_read <= 0;
      else if (local_pipe_clk_en)
          dc_read <= dc_read_pre;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dc_write <= 0;
      else if (local_pipe_clk_en)
          dc_write <= op_is_STx  && ((~is_neutrino ) &&
                    (~is_cancelled) );

    end


  assign dc_read_pre = do_load    && ((~is_neutrino ) &&
    (~is_cancelled) );

  assign do_load = op_is_LDx || do_iTRAP_0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dc_byteenable <= 0;
      else if (local_pipe_clk_en)
          dc_byteenable <= p1_byteenable;
    end


  assign p1_byteenable = ({4 {(do_write_16 && (which_halfword == 0))}} & 4'b0011) |
    ({4 {(do_write_16 && (which_halfword == 1))}} & 4'b1100) |
    ({4 {(do_write_8  && (which_byte     == 0))}} & 4'b0001) |
    ({4 {(do_write_8  && (which_byte     == 1))}} & 4'b0010) |
    ({4 {(do_write_8  && (which_byte     == 2))}} & 4'b0100) |
    ({4 {(do_write_8  && (which_byte     == 3))}} & 4'b1000) |
    ({4 {(~((do_write_16 && (which_halfword == 0)))) && (~((do_write_16 && (which_halfword == 1)))) && (~((do_write_8  && (which_byte     == 0)))) && (~((do_write_8  && (which_byte     == 1)))) && (~((do_write_8  && (which_byte     == 2)))) && (~((do_write_8  && (which_byte     == 3))))}} & -1);

  assign which_byte = do_dynamic_narrow_write ? base_address[1 : 0] : offset[1 : 0];
  assign which_halfword = do_dynamic_narrow_write ? base_address[1] : offset[1];

endmodule


module enet_nios_op_b_mux (
                            // inputs:
                             alu_result,
                             clk,
                             const,
                             dest_local_2,
                             do_fetch_vector,
                             do_fwd_b_alu,
                             do_fwd_b_fetch,
                             do_override_op_b,
                             instruction_2,
                             is_cancelled_2,
                             is_neutrino_2,
                             op_b_from_reg_really,
                             op_b_hi_from_zero,
                             op_b_lo_from_zero,
                             overridden_opB,
                             pipe_run,
                             raw_reg_b,
                             request_stable_op_b,
                             reset_n,
                             subinstruction_2,

                            // outputs:
                             base_address,
                             op_b,
                             p1_op_b_lo
                          )
;

  output  [ 22: 0] base_address;
  output  [ 31: 0] op_b;
  output  [ 15: 0] p1_op_b_lo;
  input   [ 31: 0] alu_result;
  input            clk;
  input   [ 15: 0] const;
  input   [  4: 0] dest_local_2;
  input            do_fetch_vector;
  input            do_fwd_b_alu;
  input            do_fwd_b_fetch;
  input            do_override_op_b;
  input   [ 15: 0] instruction_2;
  input            is_cancelled_2;
  input            is_neutrino_2;
  input            op_b_from_reg_really;
  input            op_b_hi_from_zero;
  input            op_b_lo_from_zero;
  input   [ 15: 0] overridden_opB;
  input            pipe_run;
  input   [ 31: 0] raw_reg_b;
  input            request_stable_op_b;
  input            reset_n;
  input   [  5: 0] subinstruction_2;

  wire    [ 15: 0] base_addr_hi;
  wire    [ 15: 0] base_addr_lo;
  wire    [ 22: 0] base_address;
  wire    [  4: 0] dest_local;
  wire             dont_change_op_b;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire    [ 31: 0] op_b;
  wire             op_b_enable;
  reg     [ 15: 0] op_b_hi;
  reg     [ 15: 0] op_b_lo;
  wire    [ 15: 0] p1_op_b_hi;
  wire    [ 15: 0] p1_op_b_lo;
  wire             pipe_state_we;
  wire             sel_alu_result;
  wire             sel_const_hi;
  wire             sel_const_lo;
  wire             sel_override_hi;
  wire             sel_override_lo;
  wire             sel_raw_reg_b;
  wire    [  5: 0] subinstruction;
  wire             unxcascade_chainxx10;
  wire             unxcascade_chainxx11;
  wire             unxcascade_chainxx12;
  wire             unxcascade_chainxx13;
  wire             unxcascade_chainxx14;
  wire             unxcascade_chainxx15;
  wire             unxcascade_chainxx16;
  wire             unxcascade_chainxx17;
  wire             unxcascade_chainxx18;
  wire             unxcascade_chainxx19;
  wire             unxcascade_chainxx20;
  wire             unxcascade_chainxx21;
  wire             unxcascade_chainxx22;
  wire             unxcascade_chainxx23;
  wire             unxcascade_chainxx24;
  wire             unxcascade_chainxx25;
  wire             unxcascade_chainxx26;
  wire             unxcascade_chainxx27;
  wire             unxcascade_chainxx28;
  wire             unxcascade_chainxx29;
  wire             unxcascade_chainxx3;
  wire             unxcascade_chainxx30;
  wire             unxcascade_chainxx31;
  wire             unxcascade_chainxx32;
  wire             unxcascade_chainxx33;
  wire             unxcascade_chainxx34;
  wire             unxcascade_chainxx4;
  wire             unxcascade_chainxx5;
  wire             unxcascade_chainxx6;
  wire             unxcascade_chainxx7;
  wire             unxcascade_chainxx8;
  wire             unxcascade_chainxx9;
  wire    [ 31: 0] vecbase_mux;
  assign is_neutrino = is_neutrino_2;
  assign dest_local = dest_local_2;
  assign instruction = instruction_2;
  assign is_cancelled = is_cancelled_2;
  assign subinstruction = subinstruction_2;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign op_b = {op_b_hi,
    op_b_lo};

  assign base_address = {base_addr_hi,
    base_addr_lo};

  assign op_b_enable = pipe_run && ~dont_change_op_b;
  assign sel_override_lo = do_override_op_b | op_b_lo_from_zero;
  assign sel_raw_reg_b = (op_b_from_reg_really && ~do_fwd_b_fetch) |
    (do_fetch_vector                          );

  assign sel_alu_result = (op_b_from_reg_really &&  do_fwd_b_fetch) |
    (do_fetch_vector                          );

  assign sel_const_lo = ~do_override_op_b && ~do_fetch_vector && 
    (~op_b_from_reg_really | op_b_lo_from_zero);

  assign sel_override_hi = do_override_op_b | op_b_hi_from_zero;
  assign sel_const_hi = ~do_override_op_b && ~do_fetch_vector && 
    (~op_b_from_reg_really | op_b_hi_from_zero);

  assign vecbase_mux = 4718336;
  assign dont_change_op_b = request_stable_op_b && 
    ~do_fwd_b_alu;


//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_b_lo <= 0;
      else if (op_b_enable)
          op_b_lo <= p1_op_b_lo;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_b_hi <= 0;
      else if (op_b_enable)
          op_b_hi <= p1_op_b_hi;
    end


  assign base_addr_hi = ({16 {~(sel_alu_result && ~sel_raw_reg_b)}} | alu_result[31 : 16]) &
    ({16 {~(sel_raw_reg_b  && ~sel_alu_result)}} | raw_reg_b[31 : 16]) &
    ({16 {~(sel_raw_reg_b  &&  sel_alu_result)}} | vecbase_mux[31 : 16]);

  assign p1_op_b_hi = ({16 {~sel_override_hi}} | overridden_opB) &
    ({16 {~sel_const_hi}} | const) &
    ({16 {~(sel_const_hi && sel_override_hi)}} | 16'b0) &
    ({16 {~(sel_alu_result && ~sel_raw_reg_b)}} | alu_result[31 : 16]) &
    ({16 {~(sel_raw_reg_b  && ~sel_alu_result)}} | raw_reg_b[31 : 16]) &
    ({16 {~(sel_raw_reg_b  &&  sel_alu_result)}} | vecbase_mux[31 : 16]);

  assign base_addr_lo = ({16 {~(sel_alu_result && ~sel_raw_reg_b)}} | alu_result[15 : 0]) &
    ({16 {~(sel_raw_reg_b  && ~sel_alu_result)}} | raw_reg_b[15 : 0]) &
    ({16 {~(sel_raw_reg_b  &&  sel_alu_result)}} | vecbase_mux[15 : 0]);

  assign p1_op_b_lo = ({16 {~sel_override_lo}} | overridden_opB) &
    ({16 {~sel_const_lo}} | const) &
    ({16 {~(sel_const_lo && sel_override_lo)}} | 16'b0) &
    ({16 {~(sel_alu_result && ~sel_raw_reg_b)}} | alu_result[15 : 0]) &
    ({16 {~(sel_raw_reg_b  && ~sel_alu_result)}} | raw_reg_b[15 : 0]) &
    ({16 {~(sel_raw_reg_b  &&  sel_alu_result)}} | vecbase_mux[15 : 0]);


//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx3),
//      .clk     (clk),
//      .combout (p1_op_b_hi[0]),
//      .dataa   (overridden_opB[0]),
//      .datab   (const[0]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[0])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F0
//    (
//      .cascout (unxcascade_chainxx3),
//      .combout (base_addr_hi[0]),
//      .dataa   (raw_reg_b[16]),
//      .datab   (alu_result[16]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F0.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F0.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F0.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F0.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F0.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F0.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC1
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx4),
//      .clk     (clk),
//      .combout (p1_op_b_lo[0]),
//      .dataa   (overridden_opB[0]),
//      .datab   (const[0]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[0])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC1.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC1.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC1.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC1.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC1.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC1.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F1
//    (
//      .cascout (unxcascade_chainxx4),
//      .combout (base_addr_lo[0]),
//      .dataa   (raw_reg_b[0]),
//      .datab   (alu_result[0]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F1.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F1.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F1.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F1.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F1.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F1.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC2
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx5),
//      .clk     (clk),
//      .combout (p1_op_b_hi[1]),
//      .dataa   (overridden_opB[1]),
//      .datab   (const[1]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[1])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC2.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC2.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC2.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC2.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC2.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC2.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F2
//    (
//      .cascout (unxcascade_chainxx5),
//      .combout (base_addr_hi[1]),
//      .dataa   (raw_reg_b[17]),
//      .datab   (alu_result[17]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F2.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F2.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F2.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F2.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F2.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F2.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC3
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx6),
//      .clk     (clk),
//      .combout (p1_op_b_lo[1]),
//      .dataa   (overridden_opB[1]),
//      .datab   (const[1]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[1])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC3.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC3.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC3.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC3.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC3.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC3.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F3
//    (
//      .cascout (unxcascade_chainxx6),
//      .combout (base_addr_lo[1]),
//      .dataa   (raw_reg_b[1]),
//      .datab   (alu_result[1]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F3.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F3.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F3.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F3.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F3.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F3.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC4
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx7),
//      .clk     (clk),
//      .combout (p1_op_b_hi[2]),
//      .dataa   (overridden_opB[2]),
//      .datab   (const[2]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[2])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC4.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC4.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC4.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC4.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC4.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC4.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F4
//    (
//      .cascout (unxcascade_chainxx7),
//      .combout (base_addr_hi[2]),
//      .dataa   (raw_reg_b[18]),
//      .datab   (alu_result[18]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F4.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F4.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F4.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F4.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F4.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F4.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC5
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx8),
//      .clk     (clk),
//      .combout (p1_op_b_lo[2]),
//      .dataa   (overridden_opB[2]),
//      .datab   (const[2]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[2])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC5.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC5.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC5.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC5.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC5.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC5.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F5
//    (
//      .cascout (unxcascade_chainxx8),
//      .combout (base_addr_lo[2]),
//      .dataa   (raw_reg_b[2]),
//      .datab   (alu_result[2]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F5.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F5.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F5.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F5.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F5.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F5.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC6
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx9),
//      .clk     (clk),
//      .combout (p1_op_b_hi[3]),
//      .dataa   (overridden_opB[3]),
//      .datab   (const[3]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[3])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC6.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC6.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC6.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC6.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC6.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC6.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F6
//    (
//      .cascout (unxcascade_chainxx9),
//      .combout (base_addr_hi[3]),
//      .dataa   (raw_reg_b[19]),
//      .datab   (alu_result[19]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F6.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F6.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F6.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F6.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F6.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F6.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC7
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx10),
//      .clk     (clk),
//      .combout (p1_op_b_lo[3]),
//      .dataa   (overridden_opB[3]),
//      .datab   (const[3]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[3])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC7.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC7.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC7.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC7.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC7.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC7.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F7
//    (
//      .cascout (unxcascade_chainxx10),
//      .combout (base_addr_lo[3]),
//      .dataa   (raw_reg_b[3]),
//      .datab   (alu_result[3]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F7.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F7.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F7.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F7.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F7.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F7.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC8
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx11),
//      .clk     (clk),
//      .combout (p1_op_b_hi[4]),
//      .dataa   (overridden_opB[4]),
//      .datab   (const[4]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[4])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC8.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC8.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC8.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC8.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC8.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC8.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F8
//    (
//      .cascout (unxcascade_chainxx11),
//      .combout (base_addr_hi[4]),
//      .dataa   (raw_reg_b[20]),
//      .datab   (alu_result[20]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F8.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F8.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F8.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F8.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F8.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F8.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC9
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx12),
//      .clk     (clk),
//      .combout (p1_op_b_lo[4]),
//      .dataa   (overridden_opB[4]),
//      .datab   (const[4]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[4])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC9.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC9.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC9.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC9.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC9.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC9.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F9
//    (
//      .cascout (unxcascade_chainxx12),
//      .combout (base_addr_lo[4]),
//      .dataa   (raw_reg_b[4]),
//      .datab   (alu_result[4]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F9.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F9.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F9.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F9.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F9.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F9.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC10
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx13),
//      .clk     (clk),
//      .combout (p1_op_b_hi[5]),
//      .dataa   (overridden_opB[5]),
//      .datab   (const[5]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[5])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC10.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC10.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC10.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC10.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC10.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC10.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F10
//    (
//      .cascout (unxcascade_chainxx13),
//      .combout (base_addr_hi[5]),
//      .dataa   (raw_reg_b[21]),
//      .datab   (alu_result[21]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F10.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F10.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F10.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F10.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F10.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F10.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC11
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx14),
//      .clk     (clk),
//      .combout (p1_op_b_lo[5]),
//      .dataa   (overridden_opB[5]),
//      .datab   (const[5]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[5])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC11.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC11.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC11.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC11.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC11.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC11.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F11
//    (
//      .cascout (unxcascade_chainxx14),
//      .combout (base_addr_lo[5]),
//      .dataa   (raw_reg_b[5]),
//      .datab   (alu_result[5]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F11.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F11.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F11.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F11.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F11.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F11.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC12
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx15),
//      .clk     (clk),
//      .combout (p1_op_b_hi[6]),
//      .dataa   (overridden_opB[6]),
//      .datab   (const[6]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[6])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC12.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC12.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC12.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC12.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC12.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC12.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F12
//    (
//      .cascout (unxcascade_chainxx15),
//      .combout (base_addr_hi[6]),
//      .dataa   (raw_reg_b[22]),
//      .datab   (alu_result[22]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F12.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F12.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F12.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F12.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F12.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F12.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC13
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx16),
//      .clk     (clk),
//      .combout (p1_op_b_lo[6]),
//      .dataa   (overridden_opB[6]),
//      .datab   (const[6]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[6])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC13.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC13.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC13.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC13.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC13.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC13.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F13
//    (
//      .cascout (unxcascade_chainxx16),
//      .combout (base_addr_lo[6]),
//      .dataa   (raw_reg_b[6]),
//      .datab   (alu_result[6]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F13.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F13.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F13.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F13.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F13.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F13.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC14
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx17),
//      .clk     (clk),
//      .combout (p1_op_b_hi[7]),
//      .dataa   (overridden_opB[7]),
//      .datab   (const[7]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[7])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC14.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC14.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC14.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC14.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC14.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC14.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F14
//    (
//      .cascout (unxcascade_chainxx17),
//      .combout (base_addr_hi[7]),
//      .dataa   (raw_reg_b[23]),
//      .datab   (alu_result[23]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F14.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F14.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F14.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F14.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F14.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F14.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC15
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx18),
//      .clk     (clk),
//      .combout (p1_op_b_lo[7]),
//      .dataa   (overridden_opB[7]),
//      .datab   (const[7]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[7])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC15.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC15.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC15.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC15.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC15.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC15.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F15
//    (
//      .cascout (unxcascade_chainxx18),
//      .combout (base_addr_lo[7]),
//      .dataa   (raw_reg_b[7]),
//      .datab   (alu_result[7]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F15.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F15.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F15.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F15.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F15.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F15.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC16
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx19),
//      .clk     (clk),
//      .combout (p1_op_b_hi[8]),
//      .dataa   (overridden_opB[8]),
//      .datab   (const[8]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[8])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC16.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC16.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC16.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC16.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC16.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC16.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F16
//    (
//      .cascout (unxcascade_chainxx19),
//      .combout (base_addr_hi[8]),
//      .dataa   (raw_reg_b[24]),
//      .datab   (alu_result[24]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F16.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F16.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F16.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F16.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F16.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F16.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC17
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx20),
//      .clk     (clk),
//      .combout (p1_op_b_lo[8]),
//      .dataa   (overridden_opB[8]),
//      .datab   (const[8]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[8])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC17.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC17.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC17.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC17.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC17.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC17.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F17
//    (
//      .cascout (unxcascade_chainxx20),
//      .combout (base_addr_lo[8]),
//      .dataa   (raw_reg_b[8]),
//      .datab   (alu_result[8]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F17.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F17.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F17.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F17.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F17.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F17.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC18
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx21),
//      .clk     (clk),
//      .combout (p1_op_b_hi[9]),
//      .dataa   (overridden_opB[9]),
//      .datab   (const[9]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[9])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC18.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC18.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC18.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC18.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC18.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC18.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F18
//    (
//      .cascout (unxcascade_chainxx21),
//      .combout (base_addr_hi[9]),
//      .dataa   (raw_reg_b[25]),
//      .datab   (alu_result[25]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F18.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F18.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F18.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F18.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F18.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F18.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC19
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx22),
//      .clk     (clk),
//      .combout (p1_op_b_lo[9]),
//      .dataa   (overridden_opB[9]),
//      .datab   (const[9]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[9])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC19.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC19.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC19.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC19.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC19.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC19.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F19
//    (
//      .cascout (unxcascade_chainxx22),
//      .combout (base_addr_lo[9]),
//      .dataa   (raw_reg_b[9]),
//      .datab   (alu_result[9]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F19.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F19.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F19.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F19.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F19.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F19.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC20
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx23),
//      .clk     (clk),
//      .combout (p1_op_b_hi[10]),
//      .dataa   (overridden_opB[10]),
//      .datab   (const[10]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[10])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC20.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC20.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC20.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC20.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC20.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC20.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F20
//    (
//      .cascout (unxcascade_chainxx23),
//      .combout (base_addr_hi[10]),
//      .dataa   (raw_reg_b[26]),
//      .datab   (alu_result[26]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F20.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F20.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F20.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F20.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F20.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F20.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC21
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx24),
//      .clk     (clk),
//      .combout (p1_op_b_lo[10]),
//      .dataa   (overridden_opB[10]),
//      .datab   (const[10]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[10])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC21.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC21.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC21.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC21.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC21.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC21.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F21
//    (
//      .cascout (unxcascade_chainxx24),
//      .combout (base_addr_lo[10]),
//      .dataa   (raw_reg_b[10]),
//      .datab   (alu_result[10]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F21.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F21.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F21.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F21.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F21.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F21.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC22
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx25),
//      .clk     (clk),
//      .combout (p1_op_b_hi[11]),
//      .dataa   (overridden_opB[11]),
//      .datab   (const[11]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[11])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC22.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC22.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC22.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC22.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC22.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC22.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F22
//    (
//      .cascout (unxcascade_chainxx25),
//      .combout (base_addr_hi[11]),
//      .dataa   (raw_reg_b[27]),
//      .datab   (alu_result[27]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F22.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F22.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F22.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F22.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F22.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F22.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC23
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx26),
//      .clk     (clk),
//      .combout (p1_op_b_lo[11]),
//      .dataa   (overridden_opB[11]),
//      .datab   (const[11]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[11])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC23.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC23.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC23.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC23.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC23.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC23.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F23
//    (
//      .cascout (unxcascade_chainxx26),
//      .combout (base_addr_lo[11]),
//      .dataa   (raw_reg_b[11]),
//      .datab   (alu_result[11]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F23.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F23.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F23.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F23.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F23.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F23.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC24
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx27),
//      .clk     (clk),
//      .combout (p1_op_b_hi[12]),
//      .dataa   (overridden_opB[12]),
//      .datab   (const[12]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[12])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC24.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC24.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC24.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC24.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC24.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC24.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F24
//    (
//      .cascout (unxcascade_chainxx27),
//      .combout (base_addr_hi[12]),
//      .dataa   (raw_reg_b[28]),
//      .datab   (alu_result[28]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F24.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F24.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F24.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F24.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F24.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F24.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC25
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx28),
//      .clk     (clk),
//      .combout (p1_op_b_lo[12]),
//      .dataa   (overridden_opB[12]),
//      .datab   (const[12]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[12])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC25.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC25.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC25.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC25.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC25.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC25.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F25
//    (
//      .cascout (unxcascade_chainxx28),
//      .combout (base_addr_lo[12]),
//      .dataa   (raw_reg_b[12]),
//      .datab   (alu_result[12]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F25.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F25.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F25.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F25.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F25.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F25.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC26
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx29),
//      .clk     (clk),
//      .combout (p1_op_b_hi[13]),
//      .dataa   (overridden_opB[13]),
//      .datab   (const[13]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[13])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC26.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC26.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC26.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC26.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC26.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC26.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F26
//    (
//      .cascout (unxcascade_chainxx29),
//      .combout (base_addr_hi[13]),
//      .dataa   (raw_reg_b[29]),
//      .datab   (alu_result[29]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F26.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F26.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F26.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F26.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F26.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F26.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC27
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx30),
//      .clk     (clk),
//      .combout (p1_op_b_lo[13]),
//      .dataa   (overridden_opB[13]),
//      .datab   (const[13]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[13])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC27.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC27.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC27.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC27.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC27.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC27.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F27
//    (
//      .cascout (unxcascade_chainxx30),
//      .combout (base_addr_lo[13]),
//      .dataa   (raw_reg_b[13]),
//      .datab   (alu_result[13]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F27.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F27.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F27.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F27.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F27.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F27.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC28
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx31),
//      .clk     (clk),
//      .combout (p1_op_b_hi[14]),
//      .dataa   (overridden_opB[14]),
//      .datab   (const[14]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[14])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC28.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC28.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC28.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC28.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC28.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC28.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F28
//    (
//      .cascout (unxcascade_chainxx31),
//      .combout (base_addr_hi[14]),
//      .dataa   (raw_reg_b[30]),
//      .datab   (alu_result[30]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F28.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F28.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F28.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F28.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F28.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F28.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC29
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx32),
//      .clk     (clk),
//      .combout (p1_op_b_lo[14]),
//      .dataa   (overridden_opB[14]),
//      .datab   (const[14]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[14])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC29.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC29.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC29.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC29.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC29.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC29.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F29
//    (
//      .cascout (unxcascade_chainxx32),
//      .combout (base_addr_lo[14]),
//      .dataa   (raw_reg_b[14]),
//      .datab   (alu_result[14]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F29.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F29.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F29.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F29.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F29.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F29.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC30
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx33),
//      .clk     (clk),
//      .combout (p1_op_b_hi[15]),
//      .dataa   (overridden_opB[15]),
//      .datab   (const[15]),
//      .datac   (sel_override_hi),
//      .datad   (sel_const_hi),
//      .ena     (op_b_enable),
//      .regout  (op_b_hi[15])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC30.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC30.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC30.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC30.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC30.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC30.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F30
//    (
//      .cascout (unxcascade_chainxx33),
//      .combout (base_addr_hi[15]),
//      .dataa   (raw_reg_b[31]),
//      .datab   (alu_result[31]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F30.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F30.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_12F30.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F30.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F30.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F30.power_up = "low";
//
//  enet_nios_hidden_lcell_66FC the_enet_nios_hidden_lcell_66FC31
//    (
//      .aclr    (~reset_n),
//      .cascin  (unxcascade_chainxx34),
//      .clk     (clk),
//      .combout (p1_op_b_lo[15]),
//      .dataa   (overridden_opB[15]),
//      .datab   (const[15]),
//      .datac   (sel_override_lo),
//      .datad   (sel_const_lo),
//      .ena     (op_b_enable),
//      .regout  (op_b_lo[15])
//    );
//  defparam the_enet_nios_hidden_lcell_66FC31.cin_used = "false",
//           the_enet_nios_hidden_lcell_66FC31.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_66FC31.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_66FC31.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_66FC31.packed_mode = "false",
//           the_enet_nios_hidden_lcell_66FC31.power_up = "low";
//
//  enet_nios_hidden_lcell_12F0 the_enet_nios_hidden_lcell_12F31
//    (
//      .cascout (unxcascade_chainxx34),
//      .combout (base_addr_lo[15]),
//      .dataa   (raw_reg_b[15]),
//      .datab   (alu_result[15]),
//      .datac   (sel_raw_reg_b),
//      .datad   (sel_alu_result)
//    );
//  defparam the_enet_nios_hidden_lcell_12F31.cin_used = "false",
//           the_enet_nios_hidden_lcell_12F31.lut_mask = "FCAF",
//           the_enet_nios_hidden_lcell_12F31.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_12F31.output_mode = "comb",
//           the_enet_nios_hidden_lcell_12F31.packed_mode = "false",
//           the_enet_nios_hidden_lcell_12F31.power_up = "low";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_nios_op_a_mux (
                            // inputs:
                             alu_result,
                             clk,
                             dest_local_2,
                             do_fwd_a_fetch,
                             do_save_return_address,
                             instruction_2,
                             is_cancelled_2,
                             is_neutrino_2,
                             pipe_run,
                             raw_reg_a,
                             reset_n,
                             return_address_less_one,
                             subinstruction_2,

                            // outputs:
                             jump_target_address,
                             op_a,
                             op_a_from_reg_a
                          )
;

  output  [ 21: 0] jump_target_address;
  output  [ 31: 0] op_a;
  output           op_a_from_reg_a;
  input   [ 31: 0] alu_result;
  input            clk;
  input   [  4: 0] dest_local_2;
  input            do_fwd_a_fetch;
  input            do_save_return_address;
  input   [ 15: 0] instruction_2;
  input            is_cancelled_2;
  input            is_neutrino_2;
  input            pipe_run;
  input   [ 31: 0] raw_reg_a;
  input            reset_n;
  input   [ 21: 0] return_address_less_one;
  input   [  5: 0] subinstruction_2;

  wire    [  4: 0] dest_local;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire    [ 21: 0] jump_target_address;
  wire             local_pipe_clk_en;
  reg     [ 31: 0] op_a;
  wire             op_a_from_reg_a;
  wire    [ 31: 0] p1_op_a;
  wire             pipe_state_we;
  wire    [ 31: 0] reg_a;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_2;
  assign dest_local = dest_local_2;
  assign instruction = instruction_2;
  assign is_cancelled = is_cancelled_2;
  assign subinstruction = subinstruction_2;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_a <= 0;
      else if (local_pipe_clk_en)
          op_a <= p1_op_a;
    end


  assign jump_target_address = reg_a;
  assign p1_op_a = do_save_return_address ? return_address_less_one : reg_a;
  assign reg_a = do_fwd_a_fetch ? alu_result : raw_reg_a;
  assign op_a_from_reg_a = ~do_save_return_address;

endmodule


module enet_nios_data_forwarding_logic (
                                         // inputs:
                                          a_matches_dest1,
                                          a_matches_dest2,
                                          b_matches_dest1,
                                          b_matches_dest2,
                                          clk,
                                          dest_local_2,
                                          do_iMSTEP_0,
                                          do_iMSTEP_1,
                                          do_override_op_b,
                                          instruction_2,
                                          is_cancelled_2,
                                          is_neutrino_2,
                                          op_a_from_reg_a,
                                          op_b_from_reg_really,
                                          pipe_run,
                                          reset_n,
                                          subinstruction_2,

                                         // outputs:
                                          do_fwd_a_alu,
                                          do_fwd_a_fetch,
                                          do_fwd_b_alu,
                                          do_fwd_b_fetch
                                       )
;

  output           do_fwd_a_alu;
  output           do_fwd_a_fetch;
  output           do_fwd_b_alu;
  output           do_fwd_b_fetch;
  input            a_matches_dest1;
  input            a_matches_dest2;
  input            b_matches_dest1;
  input            b_matches_dest2;
  input            clk;
  input   [  4: 0] dest_local_2;
  input            do_iMSTEP_0;
  input            do_iMSTEP_1;
  input            do_override_op_b;
  input   [ 15: 0] instruction_2;
  input            is_cancelled_2;
  input            is_neutrino_2;
  input            op_a_from_reg_a;
  input            op_b_from_reg_really;
  input            pipe_run;
  input            reset_n;
  input   [  5: 0] subinstruction_2;

  wire    [  4: 0] dest_local;
  reg              do_fwd_a_alu;
  wire             do_fwd_a_fetch;
  reg              do_fwd_b_alu;
  wire             do_fwd_b_fetch;
  wire             force_fwd_A;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  reg              mstep_grab_fw_op_b;
  wire             p1_do_fwd_a_alu;
  wire             p1_do_fwd_b_alu;
  wire             pipe_state_we;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_2;
  assign dest_local = dest_local_2;
  assign instruction = instruction_2;
  assign is_cancelled = is_cancelled_2;
  assign subinstruction = subinstruction_2;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          do_fwd_a_alu <= 0;
      else if (local_pipe_clk_en)
          do_fwd_a_alu <= p1_do_fwd_a_alu || force_fwd_A;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          do_fwd_b_alu <= 0;
      else if (local_pipe_clk_en)
          do_fwd_b_alu <= p1_do_fwd_b_alu;
    end


  assign force_fwd_A = do_override_op_b || do_iMSTEP_1;
  assign p1_do_fwd_a_alu = (a_matches_dest1      ) &&
    (op_a_from_reg_a      );

  assign p1_do_fwd_b_alu = (b_matches_dest1      ) &&
    (op_b_from_reg_really );

  assign do_fwd_a_fetch = a_matches_dest2      ;
  assign do_fwd_b_fetch = (b_matches_dest2      ) ||
    (mstep_grab_fw_op_b   );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          mstep_grab_fw_op_b <= 0;
      else if (pipe_state_we)
          mstep_grab_fw_op_b <= p1_do_fwd_b_alu && do_iMSTEP_0;
    end



endmodule


module enet_nios_op_fetch (
                            // inputs:
                             a_matches_dest1,
                             a_matches_dest2,
                             alu_result,
                             b_matches_dest1,
                             b_matches_dest2,
                             byterot_F_control,
                             clk,
                             const,
                             dest_local_2,
                             do_byterot_1,
                             do_fetch_vector,
                             do_force_shift_0,
                             do_force_shift_1,
                             do_iMOVI,
                             do_iMSTEP_0,
                             do_iMSTEP_1,
                             do_iSWAP,
                             do_iTRAP_n,
                             do_inv_all_b,
                             do_override_op_b,
                             do_save_return_address,
                             instruction_2,
                             is_cancelled_2,
                             is_eightie,
                             is_neutrino_2,
                             is_sixteenie,
                             next_instruction_address,
                             op_b_from_reg_really,
                             op_b_hi_from_zero,
                             op_b_lo_from_zero,
                             op_is_MOVHI,
                             overridden_byte_compl,
                             overridden_byte_zero,
                             overridden_byterot_sel_hi16,
                             overridden_byterot_sel_lo16,
                             overridden_opB,
                             pipe_run,
                             raw_reg_a,
                             raw_reg_b,
                             request_stable_op_b,
                             reset_n,
                             subinstruction_2,
                             zero_all_b_control,

                            // outputs:
                             base_address,
                             byte_complement,
                             byte_zero,
                             byterot_sel_hi16,
                             byterot_sel_lo16,
                             custom_instruction_prefix,
                             do_fwd_a_alu,
                             do_fwd_b_alu,
                             jump_target_address,
                             op_a,
                             op_b,
                             op_b_is_overridden,
                             shiftValue
                          )
;

  output  [ 22: 0] base_address;
  output  [  3: 0] byte_complement;
  output  [  3: 0] byte_zero;
  output  [  3: 0] byterot_sel_hi16;
  output  [  3: 0] byterot_sel_lo16;
  output  [ 10: 0] custom_instruction_prefix;
  output           do_fwd_a_alu;
  output           do_fwd_b_alu;
  output  [ 21: 0] jump_target_address;
  output  [ 31: 0] op_a;
  output  [ 31: 0] op_b;
  output           op_b_is_overridden;
  output  [  4: 0] shiftValue;
  input            a_matches_dest1;
  input            a_matches_dest2;
  input   [ 31: 0] alu_result;
  input            b_matches_dest1;
  input            b_matches_dest2;
  input            byterot_F_control;
  input            clk;
  input   [ 15: 0] const;
  input   [  4: 0] dest_local_2;
  input            do_byterot_1;
  input            do_fetch_vector;
  input            do_force_shift_0;
  input            do_force_shift_1;
  input            do_iMOVI;
  input            do_iMSTEP_0;
  input            do_iMSTEP_1;
  input            do_iSWAP;
  input            do_iTRAP_n;
  input            do_inv_all_b;
  input            do_override_op_b;
  input            do_save_return_address;
  input   [ 15: 0] instruction_2;
  input            is_cancelled_2;
  input            is_eightie;
  input            is_neutrino_2;
  input            is_sixteenie;
  input   [ 21: 0] next_instruction_address;
  input            op_b_from_reg_really;
  input            op_b_hi_from_zero;
  input            op_b_lo_from_zero;
  input            op_is_MOVHI;
  input   [  3: 0] overridden_byte_compl;
  input   [  3: 0] overridden_byte_zero;
  input   [  3: 0] overridden_byterot_sel_hi16;
  input   [  3: 0] overridden_byterot_sel_lo16;
  input   [ 15: 0] overridden_opB;
  input            pipe_run;
  input   [ 31: 0] raw_reg_a;
  input   [ 31: 0] raw_reg_b;
  input            request_stable_op_b;
  input            reset_n;
  input   [  5: 0] subinstruction_2;
  input            zero_all_b_control;

  wire    [ 22: 0] base_address;
  reg     [  3: 0] byte_complement;
  reg     [  3: 0] byte_zero;
  reg     [  3: 0] byterot_sel_hi16;
  reg     [  3: 0] byterot_sel_lo16;
  reg     [ 10: 0] custom_instruction_prefix;
  wire    [  4: 0] dest_local;
  wire             do_byterot_F;
  wire             do_fwd_a_alu;
  wire             do_fwd_a_fetch;
  wire             do_fwd_b_alu;
  wire             do_fwd_b_fetch;
  wire             do_zero_all_b;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire    [ 21: 0] jump_target_address;
  wire             local_pipe_clk_en;
  wire    [ 21: 0] next_instruction_address_1;
  reg     [ 21: 0] next_instruction_address_2;
  wire    [ 31: 0] op_a;
  wire             op_a_from_reg_a;
  wire    [ 31: 0] op_b;
  reg              op_b_is_overridden;
  wire    [  3: 0] p1_byte_complement;
  wire    [  3: 0] p1_byte_zero;
  wire    [  3: 0] p1_byterot_sel_hi16;
  wire    [  3: 0] p1_byterot_sel_lo16;
  wire    [ 15: 0] p1_op_b_lo;
  wire    [  4: 0] p1_shiftValue;
  wire             pipe_state_we;
  wire    [ 21: 0] return_address_less_one;
  reg     [  4: 0] shiftValue;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_2;
  assign dest_local = dest_local_2;
  assign instruction = instruction_2;
  assign is_cancelled = is_cancelled_2;
  assign subinstruction = subinstruction_2;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_op_b_mux the_enet_nios_op_b_mux
    (
      .alu_result           (alu_result),
      .base_address         (base_address),
      .clk                  (clk),
      .const                (const),
      .dest_local_2         (dest_local_2),
      .do_fetch_vector      (do_fetch_vector),
      .do_fwd_b_alu         (do_fwd_b_alu),
      .do_fwd_b_fetch       (do_fwd_b_fetch),
      .do_override_op_b     (do_override_op_b),
      .instruction_2        (instruction_2),
      .is_cancelled_2       (is_cancelled_2),
      .is_neutrino_2        (is_neutrino_2),
      .op_b                 (op_b),
      .op_b_from_reg_really (op_b_from_reg_really),
      .op_b_hi_from_zero    (op_b_hi_from_zero),
      .op_b_lo_from_zero    (op_b_lo_from_zero),
      .overridden_opB       (overridden_opB),
      .p1_op_b_lo           (p1_op_b_lo),
      .pipe_run             (pipe_run),
      .raw_reg_b            (raw_reg_b),
      .request_stable_op_b  (request_stable_op_b),
      .reset_n              (reset_n),
      .subinstruction_2     (subinstruction_2)
    );

  enet_nios_op_a_mux the_enet_nios_op_a_mux
    (
      .alu_result              (alu_result),
      .clk                     (clk),
      .dest_local_2            (dest_local_2),
      .do_fwd_a_fetch          (do_fwd_a_fetch),
      .do_save_return_address  (do_save_return_address),
      .instruction_2           (instruction_2),
      .is_cancelled_2          (is_cancelled_2),
      .is_neutrino_2           (is_neutrino_2),
      .jump_target_address     (jump_target_address),
      .op_a                    (op_a),
      .op_a_from_reg_a         (op_a_from_reg_a),
      .pipe_run                (pipe_run),
      .raw_reg_a               (raw_reg_a),
      .reset_n                 (reset_n),
      .return_address_less_one (return_address_less_one),
      .subinstruction_2        (subinstruction_2)
    );

  enet_nios_data_forwarding_logic the_enet_nios_data_forwarding_logic
    (
      .a_matches_dest1      (a_matches_dest1),
      .a_matches_dest2      (a_matches_dest2),
      .b_matches_dest1      (b_matches_dest1),
      .b_matches_dest2      (b_matches_dest2),
      .clk                  (clk),
      .dest_local_2         (dest_local_2),
      .do_fwd_a_alu         (do_fwd_a_alu),
      .do_fwd_a_fetch       (do_fwd_a_fetch),
      .do_fwd_b_alu         (do_fwd_b_alu),
      .do_fwd_b_fetch       (do_fwd_b_fetch),
      .do_iMSTEP_0          (do_iMSTEP_0),
      .do_iMSTEP_1          (do_iMSTEP_1),
      .do_override_op_b     (do_override_op_b),
      .instruction_2        (instruction_2),
      .is_cancelled_2       (is_cancelled_2),
      .is_neutrino_2        (is_neutrino_2),
      .op_a_from_reg_a      (op_a_from_reg_a),
      .op_b_from_reg_really (op_b_from_reg_really),
      .pipe_run             (pipe_run),
      .reset_n              (reset_n),
      .subinstruction_2     (subinstruction_2)
    );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_b_is_overridden <= 0;
      else if (local_pipe_clk_en)
          op_b_is_overridden <= do_override_op_b;
    end


  assign next_instruction_address_1 = next_instruction_address;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          next_instruction_address_2 <= 0;
      else if (pipe_run)
          next_instruction_address_2 <= next_instruction_address_1;
    end


  assign return_address_less_one = next_instruction_address_2;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          shiftValue <= 0;
      else if (local_pipe_clk_en)
          shiftValue <= p1_shiftValue;
    end


  assign p1_shiftValue = do_force_shift_0 ?  0       : 
    do_force_shift_1 ?  1       : 
    p1_op_b_lo[4 : 0];

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          custom_instruction_prefix <= 0;
      else if (local_pipe_clk_en)
          custom_instruction_prefix <= const[10 : 0];
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          byte_zero <= 0;
      else if (local_pipe_clk_en)
          byte_zero <= p1_byte_zero;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          byte_complement <= 0;
      else if (local_pipe_clk_en)
          byte_complement <= p1_byte_complement;
    end


  assign p1_byte_zero = ({4 {do_override_op_b}} & overridden_byte_zero) |
    ({4 {do_zero_all_b}} & -1) |
    ({4 {do_iMOVI}} & 4'b1100) |
    ({4 {op_is_MOVHI}} & 4'b0011) |
    ({4 {(~(do_override_op_b)) && (~(do_zero_all_b)) && (~(do_iMOVI)) && (~(op_is_MOVHI))}} & 0);

  assign p1_byte_complement = ({4 {do_override_op_b}} & overridden_byte_compl) |
    ({4 {do_inv_all_b}} & -1) |
    ({4 {is_eightie}} & -2) |
    ({4 {is_sixteenie}} & 4'b1100) |
    ({4 {(~(do_override_op_b)) && (~(do_inv_all_b)) && (~(is_eightie)) && (~(is_sixteenie))}} & 0);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          byterot_sel_lo16 <= 0;
      else if (local_pipe_clk_en)
          byterot_sel_lo16 <= p1_byterot_sel_lo16;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          byterot_sel_hi16 <= 0;
      else if (local_pipe_clk_en)
          byterot_sel_hi16 <= p1_byterot_sel_hi16;
    end


  assign p1_byterot_sel_hi16 = ({4 {do_override_op_b}} & overridden_byterot_sel_hi16) |
    ({4 {do_byterot_1}} & 4'b0001) |
    ({4 {do_iSWAP}} & 4'b0100) |
    ({4 {op_is_MOVHI}} & 4'b1111) |
    ({4 {do_byterot_F}} & 4'b1111) |
    ({4 {(~(do_override_op_b)) && (~(do_byterot_1)) && (~(do_iSWAP)) && (~(op_is_MOVHI)) && (~(do_byterot_F))}} & 4'b0000);

  assign p1_byterot_sel_lo16 = ({4 {do_override_op_b}} & overridden_byterot_sel_lo16) |
    ({4 {do_byterot_1}} & 4'b0001) |
    ({4 {do_iSWAP}} & 4'b0100) |
    ({4 {op_is_MOVHI}} & 4'b0001) |
    ({4 {do_byterot_F}} & 4'b1111) |
    ({4 {(~(do_override_op_b)) && (~(do_byterot_1)) && (~(do_iSWAP)) && (~(op_is_MOVHI)) && (~(do_byterot_F))}} & 4'b0000);

  assign do_zero_all_b = zero_all_b_control || do_iTRAP_n;
  assign do_byterot_F = byterot_F_control  || do_iTRAP_n || do_iMSTEP_1;

endmodule


module enet_nios_aluadder (
                            // inputs:
                             clk,
                             computed_sreset_add,
                             dest_cwp_3,
                             dest_local_3,
                             do_iASRx_1,
                             do_iRxC_1,
                             force_carryin,
                             instruction_3,
                             is_cancelled_3,
                             is_neutrino_3,
                             pipe_run,
                             reset_n,
                             sload_add_with_b_control,
                             subinstruction_3,
                             true_regA,
                             true_regB,

                            // outputs:
                             aluaddresult,
                             carryout
                          )
;

  output  [ 31: 0] aluaddresult;
  output           carryout;
  input            clk;
  input            computed_sreset_add;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_iASRx_1;
  input            do_iRxC_1;
  input            force_carryin;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_neutrino_3;
  input            pipe_run;
  input            reset_n;
  input            sload_add_with_b_control;
  input   [  5: 0] subinstruction_3;
  input   [ 31: 0] true_regA;
  input   [ 31: 0] true_regB;

  reg     [ 31: 0] aluaddresult;
  wire    [ 32: 0] carry_chain;
  reg              carryout;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire    [ 31: 0] p1_aluaddresult;
  wire             pipe_state_we;
  wire             sload_add_with_b;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign carry_chain[0] = force_carryin;
  assign sload_add_with_b = sload_add_with_b_control || do_iASRx_1 || do_iRxC_1;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  assign {carry_chain[32], p1_aluaddresult} = true_regA + true_regB + carry_chain[0];
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          aluaddresult <= 0;
      else if (local_pipe_clk_en)
          if (computed_sreset_add)
              aluaddresult <= 0;
          else if (sload_add_with_b)
              aluaddresult <= true_regB;
          else 
            aluaddresult <= p1_aluaddresult;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          carryout <= 0;
      else if (local_pipe_clk_en)
          if (computed_sreset_add)
              carryout <= 0;
          else if (sload_add_with_b)
              carryout <= 1'b0;
          else 
            carryout <= carry_chain[32];
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_0
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[0]),
//      .clk    (clk),
//      .cout   (carry_chain[1]),
//      .dataa  (true_regA[0]),
//      .datab  (true_regB[0]),
//      .datac  (true_regB[0]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[0]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_0.cin_used = "true",
//           adder_bit_0.lut_mask = "96e8",
//           adder_bit_0.operation_mode = "counter",
//           adder_bit_0.output_mode = "comb_and_reg",
//           adder_bit_0.packed_mode = "false",
//           adder_bit_0.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_1
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[1]),
//      .clk    (clk),
//      .cout   (carry_chain[2]),
//      .dataa  (true_regA[1]),
//      .datab  (true_regB[1]),
//      .datac  (true_regB[1]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[1]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_1.cin_used = "true",
//           adder_bit_1.lut_mask = "96e8",
//           adder_bit_1.operation_mode = "counter",
//           adder_bit_1.output_mode = "comb_and_reg",
//           adder_bit_1.packed_mode = "false",
//           adder_bit_1.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_2
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[2]),
//      .clk    (clk),
//      .cout   (carry_chain[3]),
//      .dataa  (true_regA[2]),
//      .datab  (true_regB[2]),
//      .datac  (true_regB[2]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[2]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_2.cin_used = "true",
//           adder_bit_2.lut_mask = "96e8",
//           adder_bit_2.operation_mode = "counter",
//           adder_bit_2.output_mode = "comb_and_reg",
//           adder_bit_2.packed_mode = "false",
//           adder_bit_2.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_3
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[3]),
//      .clk    (clk),
//      .cout   (carry_chain[4]),
//      .dataa  (true_regA[3]),
//      .datab  (true_regB[3]),
//      .datac  (true_regB[3]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[3]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_3.cin_used = "true",
//           adder_bit_3.lut_mask = "96e8",
//           adder_bit_3.operation_mode = "counter",
//           adder_bit_3.output_mode = "comb_and_reg",
//           adder_bit_3.packed_mode = "false",
//           adder_bit_3.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_4
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[4]),
//      .clk    (clk),
//      .cout   (carry_chain[5]),
//      .dataa  (true_regA[4]),
//      .datab  (true_regB[4]),
//      .datac  (true_regB[4]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[4]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_4.cin_used = "true",
//           adder_bit_4.lut_mask = "96e8",
//           adder_bit_4.operation_mode = "counter",
//           adder_bit_4.output_mode = "comb_and_reg",
//           adder_bit_4.packed_mode = "false",
//           adder_bit_4.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_5
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[5]),
//      .clk    (clk),
//      .cout   (carry_chain[6]),
//      .dataa  (true_regA[5]),
//      .datab  (true_regB[5]),
//      .datac  (true_regB[5]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[5]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_5.cin_used = "true",
//           adder_bit_5.lut_mask = "96e8",
//           adder_bit_5.operation_mode = "counter",
//           adder_bit_5.output_mode = "comb_and_reg",
//           adder_bit_5.packed_mode = "false",
//           adder_bit_5.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_6
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[6]),
//      .clk    (clk),
//      .cout   (carry_chain[7]),
//      .dataa  (true_regA[6]),
//      .datab  (true_regB[6]),
//      .datac  (true_regB[6]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[6]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_6.cin_used = "true",
//           adder_bit_6.lut_mask = "96e8",
//           adder_bit_6.operation_mode = "counter",
//           adder_bit_6.output_mode = "comb_and_reg",
//           adder_bit_6.packed_mode = "false",
//           adder_bit_6.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_7
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[7]),
//      .clk    (clk),
//      .cout   (carry_chain[8]),
//      .dataa  (true_regA[7]),
//      .datab  (true_regB[7]),
//      .datac  (true_regB[7]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[7]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_7.cin_used = "true",
//           adder_bit_7.lut_mask = "96e8",
//           adder_bit_7.operation_mode = "counter",
//           adder_bit_7.output_mode = "comb_and_reg",
//           adder_bit_7.packed_mode = "false",
//           adder_bit_7.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_8
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[8]),
//      .clk    (clk),
//      .cout   (carry_chain[9]),
//      .dataa  (true_regA[8]),
//      .datab  (true_regB[8]),
//      .datac  (true_regB[8]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[8]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_8.cin_used = "true",
//           adder_bit_8.lut_mask = "96e8",
//           adder_bit_8.operation_mode = "counter",
//           adder_bit_8.output_mode = "comb_and_reg",
//           adder_bit_8.packed_mode = "false",
//           adder_bit_8.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_9
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[9]),
//      .clk    (clk),
//      .cout   (carry_chain[10]),
//      .dataa  (true_regA[9]),
//      .datab  (true_regB[9]),
//      .datac  (true_regB[9]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[9]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_9.cin_used = "true",
//           adder_bit_9.lut_mask = "96e8",
//           adder_bit_9.operation_mode = "counter",
//           adder_bit_9.output_mode = "comb_and_reg",
//           adder_bit_9.packed_mode = "false",
//           adder_bit_9.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_10
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[10]),
//      .clk    (clk),
//      .cout   (carry_chain[11]),
//      .dataa  (true_regA[10]),
//      .datab  (true_regB[10]),
//      .datac  (true_regB[10]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[10]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_10.cin_used = "true",
//           adder_bit_10.lut_mask = "96e8",
//           adder_bit_10.operation_mode = "counter",
//           adder_bit_10.output_mode = "comb_and_reg",
//           adder_bit_10.packed_mode = "false",
//           adder_bit_10.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_11
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[11]),
//      .clk    (clk),
//      .cout   (carry_chain[12]),
//      .dataa  (true_regA[11]),
//      .datab  (true_regB[11]),
//      .datac  (true_regB[11]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[11]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_11.cin_used = "true",
//           adder_bit_11.lut_mask = "96e8",
//           adder_bit_11.operation_mode = "counter",
//           adder_bit_11.output_mode = "comb_and_reg",
//           adder_bit_11.packed_mode = "false",
//           adder_bit_11.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_12
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[12]),
//      .clk    (clk),
//      .cout   (carry_chain[13]),
//      .dataa  (true_regA[12]),
//      .datab  (true_regB[12]),
//      .datac  (true_regB[12]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[12]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_12.cin_used = "true",
//           adder_bit_12.lut_mask = "96e8",
//           adder_bit_12.operation_mode = "counter",
//           adder_bit_12.output_mode = "comb_and_reg",
//           adder_bit_12.packed_mode = "false",
//           adder_bit_12.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_13
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[13]),
//      .clk    (clk),
//      .cout   (carry_chain[14]),
//      .dataa  (true_regA[13]),
//      .datab  (true_regB[13]),
//      .datac  (true_regB[13]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[13]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_13.cin_used = "true",
//           adder_bit_13.lut_mask = "96e8",
//           adder_bit_13.operation_mode = "counter",
//           adder_bit_13.output_mode = "comb_and_reg",
//           adder_bit_13.packed_mode = "false",
//           adder_bit_13.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_14
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[14]),
//      .clk    (clk),
//      .cout   (carry_chain[15]),
//      .dataa  (true_regA[14]),
//      .datab  (true_regB[14]),
//      .datac  (true_regB[14]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[14]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_14.cin_used = "true",
//           adder_bit_14.lut_mask = "96e8",
//           adder_bit_14.operation_mode = "counter",
//           adder_bit_14.output_mode = "comb_and_reg",
//           adder_bit_14.packed_mode = "false",
//           adder_bit_14.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_15
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[15]),
//      .clk    (clk),
//      .cout   (carry_chain[16]),
//      .dataa  (true_regA[15]),
//      .datab  (true_regB[15]),
//      .datac  (true_regB[15]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[15]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_15.cin_used = "true",
//           adder_bit_15.lut_mask = "96e8",
//           adder_bit_15.operation_mode = "counter",
//           adder_bit_15.output_mode = "comb_and_reg",
//           adder_bit_15.packed_mode = "false",
//           adder_bit_15.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_16
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[16]),
//      .clk    (clk),
//      .cout   (carry_chain[17]),
//      .dataa  (true_regA[16]),
//      .datab  (true_regB[16]),
//      .datac  (true_regB[16]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[16]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_16.cin_used = "true",
//           adder_bit_16.lut_mask = "96e8",
//           adder_bit_16.operation_mode = "counter",
//           adder_bit_16.output_mode = "comb_and_reg",
//           adder_bit_16.packed_mode = "false",
//           adder_bit_16.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_17
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[17]),
//      .clk    (clk),
//      .cout   (carry_chain[18]),
//      .dataa  (true_regA[17]),
//      .datab  (true_regB[17]),
//      .datac  (true_regB[17]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[17]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_17.cin_used = "true",
//           adder_bit_17.lut_mask = "96e8",
//           adder_bit_17.operation_mode = "counter",
//           adder_bit_17.output_mode = "comb_and_reg",
//           adder_bit_17.packed_mode = "false",
//           adder_bit_17.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_18
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[18]),
//      .clk    (clk),
//      .cout   (carry_chain[19]),
//      .dataa  (true_regA[18]),
//      .datab  (true_regB[18]),
//      .datac  (true_regB[18]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[18]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_18.cin_used = "true",
//           adder_bit_18.lut_mask = "96e8",
//           adder_bit_18.operation_mode = "counter",
//           adder_bit_18.output_mode = "comb_and_reg",
//           adder_bit_18.packed_mode = "false",
//           adder_bit_18.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_19
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[19]),
//      .clk    (clk),
//      .cout   (carry_chain[20]),
//      .dataa  (true_regA[19]),
//      .datab  (true_regB[19]),
//      .datac  (true_regB[19]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[19]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_19.cin_used = "true",
//           adder_bit_19.lut_mask = "96e8",
//           adder_bit_19.operation_mode = "counter",
//           adder_bit_19.output_mode = "comb_and_reg",
//           adder_bit_19.packed_mode = "false",
//           adder_bit_19.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_20
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[20]),
//      .clk    (clk),
//      .cout   (carry_chain[21]),
//      .dataa  (true_regA[20]),
//      .datab  (true_regB[20]),
//      .datac  (true_regB[20]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[20]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_20.cin_used = "true",
//           adder_bit_20.lut_mask = "96e8",
//           adder_bit_20.operation_mode = "counter",
//           adder_bit_20.output_mode = "comb_and_reg",
//           adder_bit_20.packed_mode = "false",
//           adder_bit_20.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_21
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[21]),
//      .clk    (clk),
//      .cout   (carry_chain[22]),
//      .dataa  (true_regA[21]),
//      .datab  (true_regB[21]),
//      .datac  (true_regB[21]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[21]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_21.cin_used = "true",
//           adder_bit_21.lut_mask = "96e8",
//           adder_bit_21.operation_mode = "counter",
//           adder_bit_21.output_mode = "comb_and_reg",
//           adder_bit_21.packed_mode = "false",
//           adder_bit_21.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_22
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[22]),
//      .clk    (clk),
//      .cout   (carry_chain[23]),
//      .dataa  (true_regA[22]),
//      .datab  (true_regB[22]),
//      .datac  (true_regB[22]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[22]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_22.cin_used = "true",
//           adder_bit_22.lut_mask = "96e8",
//           adder_bit_22.operation_mode = "counter",
//           adder_bit_22.output_mode = "comb_and_reg",
//           adder_bit_22.packed_mode = "false",
//           adder_bit_22.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_23
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[23]),
//      .clk    (clk),
//      .cout   (carry_chain[24]),
//      .dataa  (true_regA[23]),
//      .datab  (true_regB[23]),
//      .datac  (true_regB[23]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[23]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_23.cin_used = "true",
//           adder_bit_23.lut_mask = "96e8",
//           adder_bit_23.operation_mode = "counter",
//           adder_bit_23.output_mode = "comb_and_reg",
//           adder_bit_23.packed_mode = "false",
//           adder_bit_23.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_24
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[24]),
//      .clk    (clk),
//      .cout   (carry_chain[25]),
//      .dataa  (true_regA[24]),
//      .datab  (true_regB[24]),
//      .datac  (true_regB[24]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[24]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_24.cin_used = "true",
//           adder_bit_24.lut_mask = "96e8",
//           adder_bit_24.operation_mode = "counter",
//           adder_bit_24.output_mode = "comb_and_reg",
//           adder_bit_24.packed_mode = "false",
//           adder_bit_24.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_25
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[25]),
//      .clk    (clk),
//      .cout   (carry_chain[26]),
//      .dataa  (true_regA[25]),
//      .datab  (true_regB[25]),
//      .datac  (true_regB[25]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[25]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_25.cin_used = "true",
//           adder_bit_25.lut_mask = "96e8",
//           adder_bit_25.operation_mode = "counter",
//           adder_bit_25.output_mode = "comb_and_reg",
//           adder_bit_25.packed_mode = "false",
//           adder_bit_25.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_26
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[26]),
//      .clk    (clk),
//      .cout   (carry_chain[27]),
//      .dataa  (true_regA[26]),
//      .datab  (true_regB[26]),
//      .datac  (true_regB[26]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[26]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_26.cin_used = "true",
//           adder_bit_26.lut_mask = "96e8",
//           adder_bit_26.operation_mode = "counter",
//           adder_bit_26.output_mode = "comb_and_reg",
//           adder_bit_26.packed_mode = "false",
//           adder_bit_26.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_27
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[27]),
//      .clk    (clk),
//      .cout   (carry_chain[28]),
//      .dataa  (true_regA[27]),
//      .datab  (true_regB[27]),
//      .datac  (true_regB[27]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[27]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_27.cin_used = "true",
//           adder_bit_27.lut_mask = "96e8",
//           adder_bit_27.operation_mode = "counter",
//           adder_bit_27.output_mode = "comb_and_reg",
//           adder_bit_27.packed_mode = "false",
//           adder_bit_27.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_28
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[28]),
//      .clk    (clk),
//      .cout   (carry_chain[29]),
//      .dataa  (true_regA[28]),
//      .datab  (true_regB[28]),
//      .datac  (true_regB[28]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[28]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_28.cin_used = "true",
//           adder_bit_28.lut_mask = "96e8",
//           adder_bit_28.operation_mode = "counter",
//           adder_bit_28.output_mode = "comb_and_reg",
//           adder_bit_28.packed_mode = "false",
//           adder_bit_28.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_29
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[29]),
//      .clk    (clk),
//      .cout   (carry_chain[30]),
//      .dataa  (true_regA[29]),
//      .datab  (true_regB[29]),
//      .datac  (true_regB[29]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[29]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_29.cin_used = "true",
//           adder_bit_29.lut_mask = "96e8",
//           adder_bit_29.operation_mode = "counter",
//           adder_bit_29.output_mode = "comb_and_reg",
//           adder_bit_29.packed_mode = "false",
//           adder_bit_29.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_30
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[30]),
//      .clk    (clk),
//      .cout   (carry_chain[31]),
//      .dataa  (true_regA[30]),
//      .datab  (true_regB[30]),
//      .datac  (true_regB[30]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[30]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_30.cin_used = "true",
//           adder_bit_30.lut_mask = "96e8",
//           adder_bit_30.operation_mode = "counter",
//           adder_bit_30.output_mode = "comb_and_reg",
//           adder_bit_30.packed_mode = "false",
//           adder_bit_30.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4DEF adder_bit_31
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[31]),
//      .clk    (clk),
//      .cout   (carry_chain[32]),
//      .dataa  (true_regA[31]),
//      .datab  (true_regB[31]),
//      .datac  (true_regB[31]),
//      .ena    (pipe_run),
//      .regout (aluaddresult[31]),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam adder_bit_31.cin_used = "true",
//           adder_bit_31.lut_mask = "96e8",
//           adder_bit_31.operation_mode = "counter",
//           adder_bit_31.output_mode = "comb_and_reg",
//           adder_bit_31.packed_mode = "false",
//           adder_bit_31.power_up = "low";
//
//  
//  enet_nios_hidden_lcell_4CEF carryout_reg
//    (
//      .aclr   (~reset_n),
//      .cin    (carry_chain[32]),
//      .clk    (clk),
//      .dataa  (1'b0),
//      .datab  (1'b0),
//      .datac  (1'b0),
//      .ena    (pipe_run),
//      .regout (carryout),
//      .sclr   (computed_sreset_add),
//      .sload  (sload_add_with_b)
//    );
//  defparam carryout_reg.cin_used = "true",
//           carryout_reg.lut_mask = "d8d8",
//           carryout_reg.operation_mode = "counter",
//           carryout_reg.output_mode = "comb_and_reg",
//           carryout_reg.packed_mode = "false",
//           carryout_reg.power_up = "low";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_nios_op_a_condition (
                                  // inputs:
                                   aluaddresult,
                                   alulogicresult,
                                   dest_cwp_3,
                                   dest_local_3,
                                   do_fwd_a_alu,
                                   instruction_3,
                                   is_cancelled_3,
                                   is_neutrino_3,
                                   op_a,
                                   pipe_run,
                                   sel_rot1,
                                   shift_cycle_2,
                                   shiftresult,
                                   subinstruction_3,

                                  // outputs:
                                   true_regA
                                )
;

  output  [ 31: 0] true_regA;
  input   [ 31: 0] aluaddresult;
  input   [ 31: 0] alulogicresult;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_fwd_a_alu;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_neutrino_3;
  input   [ 31: 0] op_a;
  input            pipe_run;
  input            sel_rot1;
  input            shift_cycle_2;
  input   [ 31: 0] shiftresult;
  input   [  5: 0] subinstruction_3;

  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire             do_fwd_A;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire    [ 31: 0] shiftresult_rot1;
  wire    [  5: 0] subinstruction;
  wire    [ 31: 0] true_regA;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign do_fwd_A = do_fwd_a_alu;
  assign shiftresult_rot1 = {shiftresult[30 : 0],
    shiftresult[31]};

  assign true_regA = (do_fwd_A ? aluaddresult ^ alulogicresult : op_a    ) & 
    ( (sel_rot1 ? shiftresult_rot1 : shiftresult    ) | 
    ({32 {~shift_cycle_2}}        )   );


endmodule


module enet_nios_op_b_condition (
                                  // inputs:
                                   aluaddresult,
                                   alulogicresult,
                                   byte_complement,
                                   byte_zero,
                                   dest_cwp_3,
                                   dest_local_3,
                                   do_fwd_b_alu,
                                   instruction_3,
                                   is_cancelled_3,
                                   is_neutrino_3,
                                   op_b,
                                   pipe_run,
                                   subinstruction_3,

                                  // outputs:
                                   fw_op_b,
                                   true_regB
                                )
;

  output  [ 31: 0] fw_op_b;
  output  [ 31: 0] true_regB;
  input   [ 31: 0] aluaddresult;
  input   [ 31: 0] alulogicresult;
  input   [  3: 0] byte_complement;
  input   [  3: 0] byte_zero;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_fwd_b_alu;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_neutrino_3;
  input   [ 31: 0] op_b;
  input            pipe_run;
  input   [  5: 0] subinstruction_3;

  wire    [ 31: 0] byte_comp_mask;
  wire    [ 31: 0] byte_zero_mask;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire    [ 31: 0] fw_op_b;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire    [  5: 0] subinstruction;
  wire    [ 31: 0] true_regB;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign byte_zero_mask = {{8 {byte_zero[3]}},
    {8 {byte_zero[2]}},
    {8 {byte_zero[1]}},
    {8 {byte_zero[0]}}};

  assign byte_comp_mask = {{8 {byte_complement[3]}},
    {8 {byte_complement[2]}},
    {8 {byte_complement[1]}},
    {8 {byte_complement[0]}}};

  assign fw_op_b = (do_fwd_b_alu)                  ? 
    (aluaddresult ^ alulogicresult) : 
    (op_b                         );

  assign true_regB = byte_comp_mask ^ (~byte_zero_mask & fw_op_b);

endmodule


module enet_nios_adder_logic_lock_region (
                                           // inputs:
                                            alulogicresult,
                                            byte_complement,
                                            byte_zero,
                                            clk,
                                            computed_sreset_add,
                                            dest_cwp_3,
                                            dest_local_3,
                                            do_fwd_a_alu,
                                            do_fwd_b_alu,
                                            do_iASRx_1,
                                            do_iRxC_1,
                                            force_carryin,
                                            instruction_3,
                                            is_cancelled_3,
                                            is_neutrino_3,
                                            op_a,
                                            op_b,
                                            pipe_run,
                                            reset_n,
                                            sel_rot1,
                                            shift_cycle_2,
                                            shiftresult,
                                            sload_add_with_b_control,
                                            subinstruction_3,

                                           // outputs:
                                            aluaddresult,
                                            carryout,
                                            fw_op_b,
                                            true_regA,
                                            true_regB
                                         )
  /* synthesis auto_dissolve = "FALSE" */ ;

  output  [ 31: 0] aluaddresult;
  output           carryout;
  output  [ 31: 0] fw_op_b;
  output  [ 31: 0] true_regA;
  output  [ 31: 0] true_regB;
  input   [ 31: 0] alulogicresult;
  input   [  3: 0] byte_complement;
  input   [  3: 0] byte_zero;
  input            clk;
  input            computed_sreset_add;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_fwd_a_alu;
  input            do_fwd_b_alu;
  input            do_iASRx_1;
  input            do_iRxC_1;
  input            force_carryin;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_neutrino_3;
  input   [ 31: 0] op_a;
  input   [ 31: 0] op_b;
  input            pipe_run;
  input            reset_n;
  input            sel_rot1;
  input            shift_cycle_2;
  input   [ 31: 0] shiftresult;
  input            sload_add_with_b_control;
  input   [  5: 0] subinstruction_3;

  wire    [ 31: 0] aluaddresult;
  wire             carryout;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire    [ 31: 0] fw_op_b;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire    [  5: 0] subinstruction;
  wire    [ 31: 0] true_regA;
  wire    [ 31: 0] true_regB;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_aluadder the_enet_nios_aluadder
    (
      .aluaddresult             (aluaddresult),
      .carryout                 (carryout),
      .clk                      (clk),
      .computed_sreset_add      (computed_sreset_add),
      .dest_cwp_3               (dest_cwp_3),
      .dest_local_3             (dest_local_3),
      .do_iASRx_1               (do_iASRx_1),
      .do_iRxC_1                (do_iRxC_1),
      .force_carryin            (force_carryin),
      .instruction_3            (instruction_3),
      .is_cancelled_3           (is_cancelled_3),
      .is_neutrino_3            (is_neutrino_3),
      .pipe_run                 (pipe_run),
      .reset_n                  (reset_n),
      .sload_add_with_b_control (sload_add_with_b_control),
      .subinstruction_3         (subinstruction_3),
      .true_regA                (true_regA),
      .true_regB                (true_regB)
    );

  enet_nios_op_a_condition the_enet_nios_op_a_condition
    (
      .aluaddresult     (aluaddresult),
      .alulogicresult   (alulogicresult),
      .dest_cwp_3       (dest_cwp_3),
      .dest_local_3     (dest_local_3),
      .do_fwd_a_alu     (do_fwd_a_alu),
      .instruction_3    (instruction_3),
      .is_cancelled_3   (is_cancelled_3),
      .is_neutrino_3    (is_neutrino_3),
      .op_a             (op_a),
      .pipe_run         (pipe_run),
      .sel_rot1         (sel_rot1),
      .shift_cycle_2    (shift_cycle_2),
      .shiftresult      (shiftresult),
      .subinstruction_3 (subinstruction_3),
      .true_regA        (true_regA)
    );

  enet_nios_op_b_condition the_enet_nios_op_b_condition
    (
      .aluaddresult     (aluaddresult),
      .alulogicresult   (alulogicresult),
      .byte_complement  (byte_complement),
      .byte_zero        (byte_zero),
      .dest_cwp_3       (dest_cwp_3),
      .dest_local_3     (dest_local_3),
      .do_fwd_b_alu     (do_fwd_b_alu),
      .fw_op_b          (fw_op_b),
      .instruction_3    (instruction_3),
      .is_cancelled_3   (is_cancelled_3),
      .is_neutrino_3    (is_neutrino_3),
      .op_b             (op_b),
      .pipe_run         (pipe_run),
      .subinstruction_3 (subinstruction_3),
      .true_regB        (true_regB)
    );


endmodule


module enet_nios_alulogic (
                            // inputs:
                             clk,
                             control_register_result,
                             custom_instruction_result,
                             custom_instruction_start_mul,
                             dc_readdata,
                             dest_cwp_3,
                             dest_local_3,
                             do_custom_instruction,
                             do_iRDCTL,
                             instruction_3,
                             is_cancelled_3,
                             is_neutrino_3,
                             op_b_is_overridden,
                             pipe_run,
                             reset_n,
                             sel_memword,
                             sel_notb_x,
                             sel_rota_byte_0,
                             sel_rota_byte_1,
                             sel_rota_byte_2,
                             sel_rota_byte_3,
                             subinstruction_3,
                             true_regA,
                             true_regB,
                             unqualified_custom_instruction_start_0,
                             unqualified_custom_instruction_start_1,
                             unqualified_custom_instruction_start_2,
                             unqualified_custom_instruction_start_3,
                             unqualified_custom_instruction_start_4,

                            // outputs:
                             alulogicresult,
                             custom_instruction_clk_en,
                             custom_instruction_dataa,
                             custom_instruction_datab,
                             custom_instruction_reset,
                             custom_instruction_start
                          )
;

  output  [ 31: 0] alulogicresult;
  output           custom_instruction_clk_en;
  output  [ 31: 0] custom_instruction_dataa;
  output  [ 31: 0] custom_instruction_datab;
  output           custom_instruction_reset;
  output  [  5: 0] custom_instruction_start;
  input            clk;
  input   [ 31: 0] control_register_result;
  input   [ 31: 0] custom_instruction_result;
  input            custom_instruction_start_mul;
  input   [ 31: 0] dc_readdata;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_custom_instruction;
  input            do_iRDCTL;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_neutrino_3;
  input            op_b_is_overridden;
  input            pipe_run;
  input            reset_n;
  input            sel_memword;
  input            sel_notb_x;
  input   [  3: 0] sel_rota_byte_0;
  input   [  3: 0] sel_rota_byte_1;
  input   [  3: 0] sel_rota_byte_2;
  input   [  3: 0] sel_rota_byte_3;
  input   [  5: 0] subinstruction_3;
  input   [ 31: 0] true_regA;
  input   [ 31: 0] true_regB;
  input            unqualified_custom_instruction_start_0;
  input            unqualified_custom_instruction_start_1;
  input            unqualified_custom_instruction_start_2;
  input            unqualified_custom_instruction_start_3;
  input            unqualified_custom_instruction_start_4;

  reg     [ 31: 0] alulogicresult;
  wire    [  7: 0] casc_0_result_byte_0;
  wire    [  7: 0] casc_0_result_byte_1;
  wire    [  7: 0] casc_0_result_byte_2;
  wire    [  7: 0] casc_0_result_byte_3;
  wire    [  7: 0] casc_1_result_byte_0;
  wire    [  7: 0] casc_1_result_byte_1;
  wire    [  7: 0] casc_1_result_byte_2;
  wire    [  7: 0] casc_1_result_byte_3;
  wire    [  7: 0] casc_2_result_byte_0;
  wire    [  7: 0] casc_2_result_byte_1;
  wire    [  7: 0] casc_2_result_byte_2;
  wire    [  7: 0] casc_2_result_byte_3;
  wire    [  7: 0] casc_3_result_byte_0;
  wire    [  7: 0] casc_3_result_byte_1;
  wire    [  7: 0] casc_3_result_byte_2;
  wire    [  7: 0] casc_3_result_byte_3;
  wire             custom_instruction_clk_en;
  wire    [ 31: 0] custom_instruction_dataa;
  wire    [ 31: 0] custom_instruction_datab;
  wire             custom_instruction_reset;
  wire    [  5: 0] custom_instruction_start;
  wire             custom_instruction_start_0;
  wire             custom_instruction_start_1;
  wire             custom_instruction_start_2;
  wire             custom_instruction_start_3;
  wire             custom_instruction_start_4;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire    [ 31: 0] p1_alulogicresult;
  wire             pipe_state_we;
  wire    [  7: 0] result_byte_0;
  wire    [  7: 0] result_byte_1;
  wire    [  7: 0] result_byte_2;
  wire    [  7: 0] result_byte_3;
  wire             sel_notb;
  wire             sel_notb_1;
  wire    [  5: 0] subinstruction;
  wire    [  2: 0] unxcascade_chainxx35;
  wire    [  2: 0] unxcascade_chainxx36;
  wire    [  2: 0] unxcascade_chainxx37;
  wire    [  2: 0] unxcascade_chainxx38;
  wire    [  2: 0] unxcascade_chainxx39;
  wire    [  2: 0] unxcascade_chainxx40;
  wire    [  2: 0] unxcascade_chainxx41;
  wire    [  2: 0] unxcascade_chainxx42;
  wire    [  2: 0] unxcascade_chainxx43;
  wire    [  2: 0] unxcascade_chainxx44;
  wire    [  2: 0] unxcascade_chainxx45;
  wire    [  2: 0] unxcascade_chainxx46;
  wire    [  2: 0] unxcascade_chainxx47;
  wire    [  2: 0] unxcascade_chainxx48;
  wire    [  2: 0] unxcascade_chainxx49;
  wire    [  2: 0] unxcascade_chainxx50;
  wire    [  2: 0] unxcascade_chainxx51;
  wire    [  2: 0] unxcascade_chainxx52;
  wire    [  2: 0] unxcascade_chainxx53;
  wire    [  2: 0] unxcascade_chainxx54;
  wire    [  2: 0] unxcascade_chainxx55;
  wire    [  2: 0] unxcascade_chainxx56;
  wire    [  2: 0] unxcascade_chainxx57;
  wire    [  2: 0] unxcascade_chainxx58;
  wire    [  2: 0] unxcascade_chainxx59;
  wire    [  2: 0] unxcascade_chainxx60;
  wire    [  2: 0] unxcascade_chainxx61;
  wire    [  2: 0] unxcascade_chainxx62;
  wire    [  2: 0] unxcascade_chainxx63;
  wire    [  2: 0] unxcascade_chainxx64;
  wire    [  2: 0] unxcascade_chainxx65;
  wire    [  2: 0] unxcascade_chainxx66;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign custom_instruction_dataa = true_regA;
  assign custom_instruction_datab = true_regB;
  assign custom_instruction_clk_en = pipe_run;
  assign custom_instruction_reset = ~reset_n;
  assign custom_instruction_start_0 = unqualified_custom_instruction_start_0 & ~is_cancelled  & ~is_neutrino;
  assign custom_instruction_start[0] = custom_instruction_start_0;
  assign custom_instruction_start_1 = unqualified_custom_instruction_start_1 & ~is_cancelled  & ~is_neutrino;
  assign custom_instruction_start[1] = custom_instruction_start_1;
  assign custom_instruction_start_2 = unqualified_custom_instruction_start_2 & ~is_cancelled  & ~is_neutrino;
  assign custom_instruction_start[2] = custom_instruction_start_2;
  assign custom_instruction_start_3 = unqualified_custom_instruction_start_3 & ~is_cancelled  & ~is_neutrino;
  assign custom_instruction_start[3] = custom_instruction_start_3;
  assign custom_instruction_start_4 = unqualified_custom_instruction_start_4 & ~is_cancelled  & ~is_neutrino;
  assign custom_instruction_start[4] = custom_instruction_start_4;
  assign custom_instruction_start[5] = custom_instruction_start_mul;
  assign sel_notb = sel_notb_x || sel_notb_1;
  assign sel_notb_1 = op_b_is_overridden;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  assign casc_0_result_byte_3 = (sel_rota_byte_3[1 : 0] == 2'b00)? 8'b11111111 :
    (sel_rota_byte_3[1 : 0] == 2'b01)? true_regA[7 : 0] :
    (sel_rota_byte_3[1 : 0] == 2'b10)? true_regA[15 : 8] :
    8'b00000000;

  assign casc_1_result_byte_3 = (sel_rota_byte_3[3 : 2] == 2'b00)? 8'b11111111 :
    (sel_rota_byte_3[3 : 2] == 2'b01)? true_regA[23 : 16] :
    (sel_rota_byte_3[3 : 2] == 2'b10)? true_regA[31 : 24] :
    8'b00000000;

  assign casc_2_result_byte_3 = ({sel_notb,
    sel_memword} == 2'b00)? 8'b11111111 :
    ({sel_notb,
    sel_memword} == 2'b01)? dc_readdata[31 : 24] :
    ({sel_notb,
    sel_memword} == 2'b10)? ~true_regB[31 : 24] :
    8'b00000000;

  assign casc_3_result_byte_3 = ({do_iRDCTL,
    do_custom_instruction} == 2'b00)? 8'b11111111 :
    ({do_iRDCTL,
    do_custom_instruction} == 2'b01)? custom_instruction_result[31 : 24] :
    ({do_iRDCTL,
    do_custom_instruction} == 2'b10)? control_register_result[31 : 24] :
    8'b00000000;

  assign result_byte_3 = casc_3_result_byte_3 &
    casc_2_result_byte_3 &
    casc_1_result_byte_3 &       
    casc_0_result_byte_3;

  assign casc_0_result_byte_2 = (sel_rota_byte_2[1 : 0] == 2'b00)? 8'b11111111 :
    (sel_rota_byte_2[1 : 0] == 2'b01)? true_regA[7 : 0] :
    (sel_rota_byte_2[1 : 0] == 2'b10)? true_regA[15 : 8] :
    8'b00000000;

  assign casc_1_result_byte_2 = (sel_rota_byte_2[3 : 2] == 2'b00)? 8'b11111111 :
    (sel_rota_byte_2[3 : 2] == 2'b01)? true_regA[23 : 16] :
    (sel_rota_byte_2[3 : 2] == 2'b10)? true_regA[31 : 24] :
    8'b00000000;

  assign casc_2_result_byte_2 = ({sel_notb,
    sel_memword} == 2'b00)? 8'b11111111 :
    ({sel_notb,
    sel_memword} == 2'b01)? dc_readdata[23 : 16] :
    ({sel_notb,
    sel_memword} == 2'b10)? ~true_regB[23 : 16] :
    8'b00000000;

  assign casc_3_result_byte_2 = ({do_iRDCTL,
    do_custom_instruction} == 2'b00)? 8'b11111111 :
    ({do_iRDCTL,
    do_custom_instruction} == 2'b01)? custom_instruction_result[23 : 16] :
    ({do_iRDCTL,
    do_custom_instruction} == 2'b10)? control_register_result[23 : 16] :
    8'b00000000;

  assign result_byte_2 = casc_3_result_byte_2 &
    casc_2_result_byte_2 &
    casc_1_result_byte_2 &       
    casc_0_result_byte_2;

  assign casc_0_result_byte_1 = (sel_rota_byte_1[1 : 0] == 2'b00)? 8'b11111111 :
    (sel_rota_byte_1[1 : 0] == 2'b01)? true_regA[7 : 0] :
    (sel_rota_byte_1[1 : 0] == 2'b10)? true_regA[15 : 8] :
    8'b00000000;

  assign casc_1_result_byte_1 = (sel_rota_byte_1[3 : 2] == 2'b00)? 8'b11111111 :
    (sel_rota_byte_1[3 : 2] == 2'b01)? true_regA[23 : 16] :
    (sel_rota_byte_1[3 : 2] == 2'b10)? true_regA[31 : 24] :
    8'b00000000;

  assign casc_2_result_byte_1 = ({sel_notb,
    sel_memword} == 2'b00)? 8'b11111111 :
    ({sel_notb,
    sel_memword} == 2'b01)? dc_readdata[15 : 8] :
    ({sel_notb,
    sel_memword} == 2'b10)? ~true_regB[15 : 8] :
    8'b00000000;

  assign casc_3_result_byte_1 = ({do_iRDCTL,
    do_custom_instruction} == 2'b00)? 8'b11111111 :
    ({do_iRDCTL,
    do_custom_instruction} == 2'b01)? custom_instruction_result[15 : 8] :
    ({do_iRDCTL,
    do_custom_instruction} == 2'b10)? control_register_result[15 : 8] :
    8'b00000000;

  assign result_byte_1 = casc_3_result_byte_1 &
    casc_2_result_byte_1 &
    casc_1_result_byte_1 &       
    casc_0_result_byte_1;

  assign casc_0_result_byte_0 = (sel_rota_byte_0[1 : 0] == 2'b00)? 8'b11111111 :
    (sel_rota_byte_0[1 : 0] == 2'b01)? true_regA[7 : 0] :
    (sel_rota_byte_0[1 : 0] == 2'b10)? true_regA[15 : 8] :
    8'b00000000;

  assign casc_1_result_byte_0 = (sel_rota_byte_0[3 : 2] == 2'b00)? 8'b11111111 :
    (sel_rota_byte_0[3 : 2] == 2'b01)? true_regA[23 : 16] :
    (sel_rota_byte_0[3 : 2] == 2'b10)? true_regA[31 : 24] :
    8'b00000000;

  assign casc_2_result_byte_0 = ({sel_notb,
    sel_memword} == 2'b00)? 8'b11111111 :
    ({sel_notb,
    sel_memword} == 2'b01)? dc_readdata[7 : 0] :
    ({sel_notb,
    sel_memword} == 2'b10)? ~true_regB[7 : 0] :
    8'b00000000;

  assign casc_3_result_byte_0 = ({do_iRDCTL,
    do_custom_instruction} == 2'b00)? 8'b11111111 :
    ({do_iRDCTL,
    do_custom_instruction} == 2'b01)? custom_instruction_result[7 : 0] :
    ({do_iRDCTL,
    do_custom_instruction} == 2'b10)? control_register_result[7 : 0] :
    8'b00000000;

  assign result_byte_0 = casc_3_result_byte_0 &
    casc_2_result_byte_0 &
    casc_1_result_byte_0 &       
    casc_0_result_byte_0;

  assign p1_alulogicresult = {result_byte_3,
    result_byte_2,
    result_byte_1,
    result_byte_0};

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          alulogicresult <= 0;
      else if (local_pipe_clk_en)
          alulogicresult <= p1_alulogicresult;
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx35[0]),
//      .clk    (clk),
//      .dataa  (true_regA[0]),
//      .datab  (true_regA[8]),
//      .datac  (sel_rota_byte_0[0]),
//      .datad  (sel_rota_byte_0[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[0])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F0
//    (
//      .cascin  (unxcascade_chainxx35[1]),
//      .cascout (unxcascade_chainxx35[0]),
//      .dataa   (true_regA[16]),
//      .datab   (true_regA[24]),
//      .datac   (sel_rota_byte_0[2]),
//      .datad   (sel_rota_byte_0[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F0.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F0.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F0.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F0.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F0.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F0.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F1
//    (
//      .cascin  (unxcascade_chainxx35[2]),
//      .cascout (unxcascade_chainxx35[1]),
//      .dataa   (true_regB[0]),
//      .datab   (dc_readdata[0]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F1.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F1.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F1.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F1.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F1.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F1.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F0
//    (
//      .cascout (unxcascade_chainxx35[2]),
//      .dataa   (control_register_result[0]),
//      .datab   (custom_instruction_result[0]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F0.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F0.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F0.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F0.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F0.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F0.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC1
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx36[0]),
//      .clk    (clk),
//      .dataa  (true_regA[1]),
//      .datab  (true_regA[9]),
//      .datac  (sel_rota_byte_0[0]),
//      .datad  (sel_rota_byte_0[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[1])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC1.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC1.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC1.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC1.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC1.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC1.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F2
//    (
//      .cascin  (unxcascade_chainxx36[1]),
//      .cascout (unxcascade_chainxx36[0]),
//      .dataa   (true_regA[17]),
//      .datab   (true_regA[25]),
//      .datac   (sel_rota_byte_0[2]),
//      .datad   (sel_rota_byte_0[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F2.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F2.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F2.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F2.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F2.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F2.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F3
//    (
//      .cascin  (unxcascade_chainxx36[2]),
//      .cascout (unxcascade_chainxx36[1]),
//      .dataa   (true_regB[1]),
//      .datab   (dc_readdata[1]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F3.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F3.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F3.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F3.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F3.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F3.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F1
//    (
//      .cascout (unxcascade_chainxx36[2]),
//      .dataa   (control_register_result[1]),
//      .datab   (custom_instruction_result[1]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F1.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F1.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F1.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F1.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F1.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F1.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC2
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx37[0]),
//      .clk    (clk),
//      .dataa  (true_regA[2]),
//      .datab  (true_regA[10]),
//      .datac  (sel_rota_byte_0[0]),
//      .datad  (sel_rota_byte_0[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[2])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC2.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC2.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC2.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC2.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC2.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC2.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F4
//    (
//      .cascin  (unxcascade_chainxx37[1]),
//      .cascout (unxcascade_chainxx37[0]),
//      .dataa   (true_regA[18]),
//      .datab   (true_regA[26]),
//      .datac   (sel_rota_byte_0[2]),
//      .datad   (sel_rota_byte_0[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F4.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F4.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F4.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F4.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F4.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F4.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F5
//    (
//      .cascin  (unxcascade_chainxx37[2]),
//      .cascout (unxcascade_chainxx37[1]),
//      .dataa   (true_regB[2]),
//      .datab   (dc_readdata[2]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F5.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F5.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F5.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F5.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F5.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F5.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F2
//    (
//      .cascout (unxcascade_chainxx37[2]),
//      .dataa   (control_register_result[2]),
//      .datab   (custom_instruction_result[2]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F2.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F2.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F2.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F2.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F2.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F2.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC3
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx38[0]),
//      .clk    (clk),
//      .dataa  (true_regA[3]),
//      .datab  (true_regA[11]),
//      .datac  (sel_rota_byte_0[0]),
//      .datad  (sel_rota_byte_0[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[3])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC3.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC3.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC3.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC3.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC3.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC3.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F6
//    (
//      .cascin  (unxcascade_chainxx38[1]),
//      .cascout (unxcascade_chainxx38[0]),
//      .dataa   (true_regA[19]),
//      .datab   (true_regA[27]),
//      .datac   (sel_rota_byte_0[2]),
//      .datad   (sel_rota_byte_0[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F6.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F6.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F6.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F6.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F6.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F6.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F7
//    (
//      .cascin  (unxcascade_chainxx38[2]),
//      .cascout (unxcascade_chainxx38[1]),
//      .dataa   (true_regB[3]),
//      .datab   (dc_readdata[3]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F7.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F7.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F7.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F7.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F7.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F7.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F3
//    (
//      .cascout (unxcascade_chainxx38[2]),
//      .dataa   (control_register_result[3]),
//      .datab   (custom_instruction_result[3]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F3.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F3.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F3.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F3.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F3.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F3.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC4
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx39[0]),
//      .clk    (clk),
//      .dataa  (true_regA[4]),
//      .datab  (true_regA[12]),
//      .datac  (sel_rota_byte_0[0]),
//      .datad  (sel_rota_byte_0[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[4])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC4.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC4.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC4.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC4.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC4.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC4.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F8
//    (
//      .cascin  (unxcascade_chainxx39[1]),
//      .cascout (unxcascade_chainxx39[0]),
//      .dataa   (true_regA[20]),
//      .datab   (true_regA[28]),
//      .datac   (sel_rota_byte_0[2]),
//      .datad   (sel_rota_byte_0[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F8.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F8.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F8.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F8.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F8.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F8.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F9
//    (
//      .cascin  (unxcascade_chainxx39[2]),
//      .cascout (unxcascade_chainxx39[1]),
//      .dataa   (true_regB[4]),
//      .datab   (dc_readdata[4]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F9.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F9.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F9.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F9.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F9.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F9.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F4
//    (
//      .cascout (unxcascade_chainxx39[2]),
//      .dataa   (control_register_result[4]),
//      .datab   (custom_instruction_result[4]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F4.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F4.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F4.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F4.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F4.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F4.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC5
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx40[0]),
//      .clk    (clk),
//      .dataa  (true_regA[5]),
//      .datab  (true_regA[13]),
//      .datac  (sel_rota_byte_0[0]),
//      .datad  (sel_rota_byte_0[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[5])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC5.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC5.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC5.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC5.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC5.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC5.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F10
//    (
//      .cascin  (unxcascade_chainxx40[1]),
//      .cascout (unxcascade_chainxx40[0]),
//      .dataa   (true_regA[21]),
//      .datab   (true_regA[29]),
//      .datac   (sel_rota_byte_0[2]),
//      .datad   (sel_rota_byte_0[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F10.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F10.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F10.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F10.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F10.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F10.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F11
//    (
//      .cascin  (unxcascade_chainxx40[2]),
//      .cascout (unxcascade_chainxx40[1]),
//      .dataa   (true_regB[5]),
//      .datab   (dc_readdata[5]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F11.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F11.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F11.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F11.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F11.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F11.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F5
//    (
//      .cascout (unxcascade_chainxx40[2]),
//      .dataa   (control_register_result[5]),
//      .datab   (custom_instruction_result[5]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F5.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F5.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F5.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F5.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F5.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F5.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC6
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx41[0]),
//      .clk    (clk),
//      .dataa  (true_regA[6]),
//      .datab  (true_regA[14]),
//      .datac  (sel_rota_byte_0[0]),
//      .datad  (sel_rota_byte_0[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[6])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC6.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC6.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC6.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC6.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC6.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC6.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F12
//    (
//      .cascin  (unxcascade_chainxx41[1]),
//      .cascout (unxcascade_chainxx41[0]),
//      .dataa   (true_regA[22]),
//      .datab   (true_regA[30]),
//      .datac   (sel_rota_byte_0[2]),
//      .datad   (sel_rota_byte_0[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F12.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F12.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F12.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F12.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F12.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F12.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F13
//    (
//      .cascin  (unxcascade_chainxx41[2]),
//      .cascout (unxcascade_chainxx41[1]),
//      .dataa   (true_regB[6]),
//      .datab   (dc_readdata[6]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F13.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F13.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F13.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F13.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F13.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F13.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F6
//    (
//      .cascout (unxcascade_chainxx41[2]),
//      .dataa   (control_register_result[6]),
//      .datab   (custom_instruction_result[6]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F6.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F6.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F6.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F6.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F6.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F6.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC7
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx42[0]),
//      .clk    (clk),
//      .dataa  (true_regA[7]),
//      .datab  (true_regA[15]),
//      .datac  (sel_rota_byte_0[0]),
//      .datad  (sel_rota_byte_0[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[7])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC7.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC7.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC7.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC7.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC7.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC7.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F14
//    (
//      .cascin  (unxcascade_chainxx42[1]),
//      .cascout (unxcascade_chainxx42[0]),
//      .dataa   (true_regA[23]),
//      .datab   (true_regA[31]),
//      .datac   (sel_rota_byte_0[2]),
//      .datad   (sel_rota_byte_0[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F14.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F14.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F14.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F14.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F14.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F14.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F15
//    (
//      .cascin  (unxcascade_chainxx42[2]),
//      .cascout (unxcascade_chainxx42[1]),
//      .dataa   (true_regB[7]),
//      .datab   (dc_readdata[7]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F15.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F15.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F15.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F15.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F15.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F15.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F7
//    (
//      .cascout (unxcascade_chainxx42[2]),
//      .dataa   (control_register_result[7]),
//      .datab   (custom_instruction_result[7]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F7.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F7.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F7.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F7.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F7.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F7.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC8
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx43[0]),
//      .clk    (clk),
//      .dataa  (true_regA[0]),
//      .datab  (true_regA[8]),
//      .datac  (sel_rota_byte_1[0]),
//      .datad  (sel_rota_byte_1[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[8])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC8.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC8.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC8.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC8.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC8.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC8.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F16
//    (
//      .cascin  (unxcascade_chainxx43[1]),
//      .cascout (unxcascade_chainxx43[0]),
//      .dataa   (true_regA[16]),
//      .datab   (true_regA[24]),
//      .datac   (sel_rota_byte_1[2]),
//      .datad   (sel_rota_byte_1[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F16.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F16.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F16.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F16.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F16.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F16.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F17
//    (
//      .cascin  (unxcascade_chainxx43[2]),
//      .cascout (unxcascade_chainxx43[1]),
//      .dataa   (true_regB[8]),
//      .datab   (dc_readdata[8]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F17.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F17.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F17.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F17.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F17.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F17.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F8
//    (
//      .cascout (unxcascade_chainxx43[2]),
//      .dataa   (control_register_result[8]),
//      .datab   (custom_instruction_result[8]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F8.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F8.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F8.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F8.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F8.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F8.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC9
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx44[0]),
//      .clk    (clk),
//      .dataa  (true_regA[1]),
//      .datab  (true_regA[9]),
//      .datac  (sel_rota_byte_1[0]),
//      .datad  (sel_rota_byte_1[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[9])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC9.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC9.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC9.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC9.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC9.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC9.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F18
//    (
//      .cascin  (unxcascade_chainxx44[1]),
//      .cascout (unxcascade_chainxx44[0]),
//      .dataa   (true_regA[17]),
//      .datab   (true_regA[25]),
//      .datac   (sel_rota_byte_1[2]),
//      .datad   (sel_rota_byte_1[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F18.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F18.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F18.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F18.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F18.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F18.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F19
//    (
//      .cascin  (unxcascade_chainxx44[2]),
//      .cascout (unxcascade_chainxx44[1]),
//      .dataa   (true_regB[9]),
//      .datab   (dc_readdata[9]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F19.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F19.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F19.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F19.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F19.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F19.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F9
//    (
//      .cascout (unxcascade_chainxx44[2]),
//      .dataa   (control_register_result[9]),
//      .datab   (custom_instruction_result[9]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F9.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F9.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F9.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F9.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F9.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F9.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC10
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx45[0]),
//      .clk    (clk),
//      .dataa  (true_regA[2]),
//      .datab  (true_regA[10]),
//      .datac  (sel_rota_byte_1[0]),
//      .datad  (sel_rota_byte_1[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[10])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC10.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC10.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC10.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC10.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC10.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC10.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F20
//    (
//      .cascin  (unxcascade_chainxx45[1]),
//      .cascout (unxcascade_chainxx45[0]),
//      .dataa   (true_regA[18]),
//      .datab   (true_regA[26]),
//      .datac   (sel_rota_byte_1[2]),
//      .datad   (sel_rota_byte_1[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F20.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F20.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F20.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F20.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F20.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F20.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F21
//    (
//      .cascin  (unxcascade_chainxx45[2]),
//      .cascout (unxcascade_chainxx45[1]),
//      .dataa   (true_regB[10]),
//      .datab   (dc_readdata[10]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F21.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F21.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F21.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F21.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F21.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F21.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F10
//    (
//      .cascout (unxcascade_chainxx45[2]),
//      .dataa   (control_register_result[10]),
//      .datab   (custom_instruction_result[10]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F10.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F10.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F10.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F10.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F10.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F10.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC11
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx46[0]),
//      .clk    (clk),
//      .dataa  (true_regA[3]),
//      .datab  (true_regA[11]),
//      .datac  (sel_rota_byte_1[0]),
//      .datad  (sel_rota_byte_1[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[11])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC11.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC11.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC11.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC11.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC11.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC11.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F22
//    (
//      .cascin  (unxcascade_chainxx46[1]),
//      .cascout (unxcascade_chainxx46[0]),
//      .dataa   (true_regA[19]),
//      .datab   (true_regA[27]),
//      .datac   (sel_rota_byte_1[2]),
//      .datad   (sel_rota_byte_1[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F22.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F22.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F22.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F22.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F22.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F22.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F23
//    (
//      .cascin  (unxcascade_chainxx46[2]),
//      .cascout (unxcascade_chainxx46[1]),
//      .dataa   (true_regB[11]),
//      .datab   (dc_readdata[11]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F23.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F23.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F23.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F23.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F23.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F23.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F11
//    (
//      .cascout (unxcascade_chainxx46[2]),
//      .dataa   (control_register_result[11]),
//      .datab   (custom_instruction_result[11]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F11.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F11.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F11.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F11.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F11.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F11.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC12
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx47[0]),
//      .clk    (clk),
//      .dataa  (true_regA[4]),
//      .datab  (true_regA[12]),
//      .datac  (sel_rota_byte_1[0]),
//      .datad  (sel_rota_byte_1[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[12])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC12.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC12.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC12.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC12.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC12.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC12.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F24
//    (
//      .cascin  (unxcascade_chainxx47[1]),
//      .cascout (unxcascade_chainxx47[0]),
//      .dataa   (true_regA[20]),
//      .datab   (true_regA[28]),
//      .datac   (sel_rota_byte_1[2]),
//      .datad   (sel_rota_byte_1[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F24.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F24.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F24.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F24.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F24.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F24.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F25
//    (
//      .cascin  (unxcascade_chainxx47[2]),
//      .cascout (unxcascade_chainxx47[1]),
//      .dataa   (true_regB[12]),
//      .datab   (dc_readdata[12]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F25.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F25.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F25.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F25.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F25.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F25.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F12
//    (
//      .cascout (unxcascade_chainxx47[2]),
//      .dataa   (control_register_result[12]),
//      .datab   (custom_instruction_result[12]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F12.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F12.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F12.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F12.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F12.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F12.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC13
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx48[0]),
//      .clk    (clk),
//      .dataa  (true_regA[5]),
//      .datab  (true_regA[13]),
//      .datac  (sel_rota_byte_1[0]),
//      .datad  (sel_rota_byte_1[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[13])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC13.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC13.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC13.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC13.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC13.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC13.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F26
//    (
//      .cascin  (unxcascade_chainxx48[1]),
//      .cascout (unxcascade_chainxx48[0]),
//      .dataa   (true_regA[21]),
//      .datab   (true_regA[29]),
//      .datac   (sel_rota_byte_1[2]),
//      .datad   (sel_rota_byte_1[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F26.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F26.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F26.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F26.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F26.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F26.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F27
//    (
//      .cascin  (unxcascade_chainxx48[2]),
//      .cascout (unxcascade_chainxx48[1]),
//      .dataa   (true_regB[13]),
//      .datab   (dc_readdata[13]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F27.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F27.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F27.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F27.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F27.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F27.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F13
//    (
//      .cascout (unxcascade_chainxx48[2]),
//      .dataa   (control_register_result[13]),
//      .datab   (custom_instruction_result[13]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F13.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F13.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F13.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F13.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F13.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F13.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC14
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx49[0]),
//      .clk    (clk),
//      .dataa  (true_regA[6]),
//      .datab  (true_regA[14]),
//      .datac  (sel_rota_byte_1[0]),
//      .datad  (sel_rota_byte_1[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[14])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC14.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC14.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC14.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC14.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC14.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC14.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F28
//    (
//      .cascin  (unxcascade_chainxx49[1]),
//      .cascout (unxcascade_chainxx49[0]),
//      .dataa   (true_regA[22]),
//      .datab   (true_regA[30]),
//      .datac   (sel_rota_byte_1[2]),
//      .datad   (sel_rota_byte_1[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F28.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F28.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F28.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F28.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F28.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F28.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F29
//    (
//      .cascin  (unxcascade_chainxx49[2]),
//      .cascout (unxcascade_chainxx49[1]),
//      .dataa   (true_regB[14]),
//      .datab   (dc_readdata[14]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F29.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F29.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F29.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F29.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F29.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F29.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F14
//    (
//      .cascout (unxcascade_chainxx49[2]),
//      .dataa   (control_register_result[14]),
//      .datab   (custom_instruction_result[14]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F14.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F14.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F14.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F14.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F14.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F14.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC15
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx50[0]),
//      .clk    (clk),
//      .dataa  (true_regA[7]),
//      .datab  (true_regA[15]),
//      .datac  (sel_rota_byte_1[0]),
//      .datad  (sel_rota_byte_1[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[15])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC15.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC15.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC15.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC15.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC15.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC15.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F30
//    (
//      .cascin  (unxcascade_chainxx50[1]),
//      .cascout (unxcascade_chainxx50[0]),
//      .dataa   (true_regA[23]),
//      .datab   (true_regA[31]),
//      .datac   (sel_rota_byte_1[2]),
//      .datad   (sel_rota_byte_1[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F30.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F30.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F30.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F30.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F30.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F30.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F31
//    (
//      .cascin  (unxcascade_chainxx50[2]),
//      .cascout (unxcascade_chainxx50[1]),
//      .dataa   (true_regB[15]),
//      .datab   (dc_readdata[15]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F31.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F31.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F31.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F31.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F31.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F31.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F15
//    (
//      .cascout (unxcascade_chainxx50[2]),
//      .dataa   (control_register_result[15]),
//      .datab   (custom_instruction_result[15]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F15.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F15.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F15.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F15.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F15.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F15.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC16
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx51[0]),
//      .clk    (clk),
//      .dataa  (true_regA[0]),
//      .datab  (true_regA[8]),
//      .datac  (sel_rota_byte_2[0]),
//      .datad  (sel_rota_byte_2[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[16])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC16.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC16.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC16.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC16.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC16.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC16.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F32
//    (
//      .cascin  (unxcascade_chainxx51[1]),
//      .cascout (unxcascade_chainxx51[0]),
//      .dataa   (true_regA[16]),
//      .datab   (true_regA[24]),
//      .datac   (sel_rota_byte_2[2]),
//      .datad   (sel_rota_byte_2[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F32.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F32.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F32.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F32.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F32.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F32.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F33
//    (
//      .cascin  (unxcascade_chainxx51[2]),
//      .cascout (unxcascade_chainxx51[1]),
//      .dataa   (true_regB[16]),
//      .datab   (dc_readdata[16]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F33.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F33.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F33.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F33.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F33.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F33.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F16
//    (
//      .cascout (unxcascade_chainxx51[2]),
//      .dataa   (control_register_result[16]),
//      .datab   (custom_instruction_result[16]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F16.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F16.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F16.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F16.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F16.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F16.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC17
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx52[0]),
//      .clk    (clk),
//      .dataa  (true_regA[1]),
//      .datab  (true_regA[9]),
//      .datac  (sel_rota_byte_2[0]),
//      .datad  (sel_rota_byte_2[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[17])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC17.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC17.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC17.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC17.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC17.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC17.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F34
//    (
//      .cascin  (unxcascade_chainxx52[1]),
//      .cascout (unxcascade_chainxx52[0]),
//      .dataa   (true_regA[17]),
//      .datab   (true_regA[25]),
//      .datac   (sel_rota_byte_2[2]),
//      .datad   (sel_rota_byte_2[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F34.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F34.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F34.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F34.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F34.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F34.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F35
//    (
//      .cascin  (unxcascade_chainxx52[2]),
//      .cascout (unxcascade_chainxx52[1]),
//      .dataa   (true_regB[17]),
//      .datab   (dc_readdata[17]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F35.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F35.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F35.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F35.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F35.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F35.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F17
//    (
//      .cascout (unxcascade_chainxx52[2]),
//      .dataa   (control_register_result[17]),
//      .datab   (custom_instruction_result[17]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F17.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F17.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F17.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F17.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F17.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F17.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC18
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx53[0]),
//      .clk    (clk),
//      .dataa  (true_regA[2]),
//      .datab  (true_regA[10]),
//      .datac  (sel_rota_byte_2[0]),
//      .datad  (sel_rota_byte_2[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[18])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC18.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC18.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC18.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC18.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC18.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC18.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F36
//    (
//      .cascin  (unxcascade_chainxx53[1]),
//      .cascout (unxcascade_chainxx53[0]),
//      .dataa   (true_regA[18]),
//      .datab   (true_regA[26]),
//      .datac   (sel_rota_byte_2[2]),
//      .datad   (sel_rota_byte_2[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F36.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F36.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F36.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F36.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F36.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F36.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F37
//    (
//      .cascin  (unxcascade_chainxx53[2]),
//      .cascout (unxcascade_chainxx53[1]),
//      .dataa   (true_regB[18]),
//      .datab   (dc_readdata[18]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F37.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F37.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F37.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F37.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F37.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F37.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F18
//    (
//      .cascout (unxcascade_chainxx53[2]),
//      .dataa   (control_register_result[18]),
//      .datab   (custom_instruction_result[18]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F18.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F18.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F18.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F18.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F18.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F18.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC19
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx54[0]),
//      .clk    (clk),
//      .dataa  (true_regA[3]),
//      .datab  (true_regA[11]),
//      .datac  (sel_rota_byte_2[0]),
//      .datad  (sel_rota_byte_2[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[19])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC19.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC19.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC19.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC19.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC19.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC19.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F38
//    (
//      .cascin  (unxcascade_chainxx54[1]),
//      .cascout (unxcascade_chainxx54[0]),
//      .dataa   (true_regA[19]),
//      .datab   (true_regA[27]),
//      .datac   (sel_rota_byte_2[2]),
//      .datad   (sel_rota_byte_2[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F38.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F38.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F38.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F38.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F38.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F38.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F39
//    (
//      .cascin  (unxcascade_chainxx54[2]),
//      .cascout (unxcascade_chainxx54[1]),
//      .dataa   (true_regB[19]),
//      .datab   (dc_readdata[19]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F39.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F39.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F39.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F39.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F39.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F39.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F19
//    (
//      .cascout (unxcascade_chainxx54[2]),
//      .dataa   (control_register_result[19]),
//      .datab   (custom_instruction_result[19]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F19.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F19.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F19.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F19.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F19.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F19.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC20
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx55[0]),
//      .clk    (clk),
//      .dataa  (true_regA[4]),
//      .datab  (true_regA[12]),
//      .datac  (sel_rota_byte_2[0]),
//      .datad  (sel_rota_byte_2[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[20])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC20.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC20.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC20.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC20.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC20.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC20.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F40
//    (
//      .cascin  (unxcascade_chainxx55[1]),
//      .cascout (unxcascade_chainxx55[0]),
//      .dataa   (true_regA[20]),
//      .datab   (true_regA[28]),
//      .datac   (sel_rota_byte_2[2]),
//      .datad   (sel_rota_byte_2[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F40.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F40.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F40.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F40.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F40.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F40.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F41
//    (
//      .cascin  (unxcascade_chainxx55[2]),
//      .cascout (unxcascade_chainxx55[1]),
//      .dataa   (true_regB[20]),
//      .datab   (dc_readdata[20]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F41.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F41.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F41.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F41.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F41.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F41.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F20
//    (
//      .cascout (unxcascade_chainxx55[2]),
//      .dataa   (control_register_result[20]),
//      .datab   (custom_instruction_result[20]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F20.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F20.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F20.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F20.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F20.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F20.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC21
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx56[0]),
//      .clk    (clk),
//      .dataa  (true_regA[5]),
//      .datab  (true_regA[13]),
//      .datac  (sel_rota_byte_2[0]),
//      .datad  (sel_rota_byte_2[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[21])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC21.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC21.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC21.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC21.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC21.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC21.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F42
//    (
//      .cascin  (unxcascade_chainxx56[1]),
//      .cascout (unxcascade_chainxx56[0]),
//      .dataa   (true_regA[21]),
//      .datab   (true_regA[29]),
//      .datac   (sel_rota_byte_2[2]),
//      .datad   (sel_rota_byte_2[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F42.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F42.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F42.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F42.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F42.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F42.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F43
//    (
//      .cascin  (unxcascade_chainxx56[2]),
//      .cascout (unxcascade_chainxx56[1]),
//      .dataa   (true_regB[21]),
//      .datab   (dc_readdata[21]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F43.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F43.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F43.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F43.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F43.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F43.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F21
//    (
//      .cascout (unxcascade_chainxx56[2]),
//      .dataa   (control_register_result[21]),
//      .datab   (custom_instruction_result[21]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F21.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F21.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F21.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F21.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F21.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F21.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC22
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx57[0]),
//      .clk    (clk),
//      .dataa  (true_regA[6]),
//      .datab  (true_regA[14]),
//      .datac  (sel_rota_byte_2[0]),
//      .datad  (sel_rota_byte_2[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[22])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC22.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC22.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC22.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC22.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC22.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC22.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F44
//    (
//      .cascin  (unxcascade_chainxx57[1]),
//      .cascout (unxcascade_chainxx57[0]),
//      .dataa   (true_regA[22]),
//      .datab   (true_regA[30]),
//      .datac   (sel_rota_byte_2[2]),
//      .datad   (sel_rota_byte_2[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F44.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F44.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F44.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F44.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F44.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F44.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F45
//    (
//      .cascin  (unxcascade_chainxx57[2]),
//      .cascout (unxcascade_chainxx57[1]),
//      .dataa   (true_regB[22]),
//      .datab   (dc_readdata[22]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F45.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F45.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F45.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F45.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F45.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F45.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F22
//    (
//      .cascout (unxcascade_chainxx57[2]),
//      .dataa   (control_register_result[22]),
//      .datab   (custom_instruction_result[22]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F22.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F22.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F22.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F22.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F22.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F22.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC23
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx58[0]),
//      .clk    (clk),
//      .dataa  (true_regA[7]),
//      .datab  (true_regA[15]),
//      .datac  (sel_rota_byte_2[0]),
//      .datad  (sel_rota_byte_2[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[23])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC23.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC23.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC23.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC23.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC23.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC23.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F46
//    (
//      .cascin  (unxcascade_chainxx58[1]),
//      .cascout (unxcascade_chainxx58[0]),
//      .dataa   (true_regA[23]),
//      .datab   (true_regA[31]),
//      .datac   (sel_rota_byte_2[2]),
//      .datad   (sel_rota_byte_2[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F46.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F46.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F46.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F46.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F46.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F46.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F47
//    (
//      .cascin  (unxcascade_chainxx58[2]),
//      .cascout (unxcascade_chainxx58[1]),
//      .dataa   (true_regB[23]),
//      .datab   (dc_readdata[23]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F47.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F47.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F47.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F47.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F47.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F47.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F23
//    (
//      .cascout (unxcascade_chainxx58[2]),
//      .dataa   (control_register_result[23]),
//      .datab   (custom_instruction_result[23]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F23.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F23.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F23.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F23.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F23.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F23.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC24
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx59[0]),
//      .clk    (clk),
//      .dataa  (true_regA[0]),
//      .datab  (true_regA[8]),
//      .datac  (sel_rota_byte_3[0]),
//      .datad  (sel_rota_byte_3[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[24])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC24.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC24.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC24.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC24.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC24.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC24.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F48
//    (
//      .cascin  (unxcascade_chainxx59[1]),
//      .cascout (unxcascade_chainxx59[0]),
//      .dataa   (true_regA[16]),
//      .datab   (true_regA[24]),
//      .datac   (sel_rota_byte_3[2]),
//      .datad   (sel_rota_byte_3[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F48.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F48.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F48.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F48.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F48.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F48.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F49
//    (
//      .cascin  (unxcascade_chainxx59[2]),
//      .cascout (unxcascade_chainxx59[1]),
//      .dataa   (true_regB[24]),
//      .datab   (dc_readdata[24]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F49.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F49.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F49.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F49.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F49.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F49.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F24
//    (
//      .cascout (unxcascade_chainxx59[2]),
//      .dataa   (control_register_result[24]),
//      .datab   (custom_instruction_result[24]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F24.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F24.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F24.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F24.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F24.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F24.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC25
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx60[0]),
//      .clk    (clk),
//      .dataa  (true_regA[1]),
//      .datab  (true_regA[9]),
//      .datac  (sel_rota_byte_3[0]),
//      .datad  (sel_rota_byte_3[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[25])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC25.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC25.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC25.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC25.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC25.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC25.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F50
//    (
//      .cascin  (unxcascade_chainxx60[1]),
//      .cascout (unxcascade_chainxx60[0]),
//      .dataa   (true_regA[17]),
//      .datab   (true_regA[25]),
//      .datac   (sel_rota_byte_3[2]),
//      .datad   (sel_rota_byte_3[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F50.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F50.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F50.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F50.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F50.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F50.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F51
//    (
//      .cascin  (unxcascade_chainxx60[2]),
//      .cascout (unxcascade_chainxx60[1]),
//      .dataa   (true_regB[25]),
//      .datab   (dc_readdata[25]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F51.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F51.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F51.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F51.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F51.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F51.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F25
//    (
//      .cascout (unxcascade_chainxx60[2]),
//      .dataa   (control_register_result[25]),
//      .datab   (custom_instruction_result[25]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F25.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F25.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F25.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F25.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F25.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F25.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC26
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx61[0]),
//      .clk    (clk),
//      .dataa  (true_regA[2]),
//      .datab  (true_regA[10]),
//      .datac  (sel_rota_byte_3[0]),
//      .datad  (sel_rota_byte_3[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[26])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC26.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC26.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC26.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC26.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC26.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC26.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F52
//    (
//      .cascin  (unxcascade_chainxx61[1]),
//      .cascout (unxcascade_chainxx61[0]),
//      .dataa   (true_regA[18]),
//      .datab   (true_regA[26]),
//      .datac   (sel_rota_byte_3[2]),
//      .datad   (sel_rota_byte_3[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F52.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F52.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F52.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F52.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F52.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F52.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F53
//    (
//      .cascin  (unxcascade_chainxx61[2]),
//      .cascout (unxcascade_chainxx61[1]),
//      .dataa   (true_regB[26]),
//      .datab   (dc_readdata[26]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F53.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F53.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F53.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F53.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F53.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F53.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F26
//    (
//      .cascout (unxcascade_chainxx61[2]),
//      .dataa   (control_register_result[26]),
//      .datab   (custom_instruction_result[26]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F26.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F26.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F26.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F26.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F26.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F26.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC27
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx62[0]),
//      .clk    (clk),
//      .dataa  (true_regA[3]),
//      .datab  (true_regA[11]),
//      .datac  (sel_rota_byte_3[0]),
//      .datad  (sel_rota_byte_3[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[27])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC27.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC27.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC27.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC27.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC27.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC27.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F54
//    (
//      .cascin  (unxcascade_chainxx62[1]),
//      .cascout (unxcascade_chainxx62[0]),
//      .dataa   (true_regA[19]),
//      .datab   (true_regA[27]),
//      .datac   (sel_rota_byte_3[2]),
//      .datad   (sel_rota_byte_3[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F54.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F54.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F54.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F54.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F54.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F54.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F55
//    (
//      .cascin  (unxcascade_chainxx62[2]),
//      .cascout (unxcascade_chainxx62[1]),
//      .dataa   (true_regB[27]),
//      .datab   (dc_readdata[27]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F55.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F55.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F55.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F55.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F55.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F55.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F27
//    (
//      .cascout (unxcascade_chainxx62[2]),
//      .dataa   (control_register_result[27]),
//      .datab   (custom_instruction_result[27]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F27.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F27.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F27.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F27.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F27.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F27.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC28
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx63[0]),
//      .clk    (clk),
//      .dataa  (true_regA[4]),
//      .datab  (true_regA[12]),
//      .datac  (sel_rota_byte_3[0]),
//      .datad  (sel_rota_byte_3[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[28])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC28.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC28.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC28.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC28.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC28.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC28.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F56
//    (
//      .cascin  (unxcascade_chainxx63[1]),
//      .cascout (unxcascade_chainxx63[0]),
//      .dataa   (true_regA[20]),
//      .datab   (true_regA[28]),
//      .datac   (sel_rota_byte_3[2]),
//      .datad   (sel_rota_byte_3[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F56.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F56.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F56.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F56.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F56.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F56.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F57
//    (
//      .cascin  (unxcascade_chainxx63[2]),
//      .cascout (unxcascade_chainxx63[1]),
//      .dataa   (true_regB[28]),
//      .datab   (dc_readdata[28]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F57.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F57.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F57.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F57.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F57.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F57.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F28
//    (
//      .cascout (unxcascade_chainxx63[2]),
//      .dataa   (control_register_result[28]),
//      .datab   (custom_instruction_result[28]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F28.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F28.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F28.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F28.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F28.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F28.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC29
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx64[0]),
//      .clk    (clk),
//      .dataa  (true_regA[5]),
//      .datab  (true_regA[13]),
//      .datac  (sel_rota_byte_3[0]),
//      .datad  (sel_rota_byte_3[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[29])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC29.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC29.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC29.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC29.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC29.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC29.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F58
//    (
//      .cascin  (unxcascade_chainxx64[1]),
//      .cascout (unxcascade_chainxx64[0]),
//      .dataa   (true_regA[21]),
//      .datab   (true_regA[29]),
//      .datac   (sel_rota_byte_3[2]),
//      .datad   (sel_rota_byte_3[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F58.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F58.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F58.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F58.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F58.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F58.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F59
//    (
//      .cascin  (unxcascade_chainxx64[2]),
//      .cascout (unxcascade_chainxx64[1]),
//      .dataa   (true_regB[29]),
//      .datab   (dc_readdata[29]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F59.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F59.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F59.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F59.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F59.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F59.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F29
//    (
//      .cascout (unxcascade_chainxx64[2]),
//      .dataa   (control_register_result[29]),
//      .datab   (custom_instruction_result[29]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F29.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F29.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F29.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F29.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F29.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F29.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC30
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx65[0]),
//      .clk    (clk),
//      .dataa  (true_regA[6]),
//      .datab  (true_regA[14]),
//      .datac  (sel_rota_byte_3[0]),
//      .datad  (sel_rota_byte_3[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[30])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC30.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC30.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC30.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC30.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC30.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC30.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F60
//    (
//      .cascin  (unxcascade_chainxx65[1]),
//      .cascout (unxcascade_chainxx65[0]),
//      .dataa   (true_regA[22]),
//      .datab   (true_regA[30]),
//      .datac   (sel_rota_byte_3[2]),
//      .datad   (sel_rota_byte_3[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F60.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F60.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F60.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F60.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F60.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F60.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F61
//    (
//      .cascin  (unxcascade_chainxx65[2]),
//      .cascout (unxcascade_chainxx65[1]),
//      .dataa   (true_regB[30]),
//      .datab   (dc_readdata[30]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F61.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F61.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F61.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F61.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F61.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F61.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F30
//    (
//      .cascout (unxcascade_chainxx65[2]),
//      .dataa   (control_register_result[30]),
//      .datab   (custom_instruction_result[30]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F30.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F30.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F30.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F30.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F30.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F30.power_up = "low";
//
//  enet_nios_hidden_lcell_64FC the_enet_nios_hidden_lcell_64FC31
//    (
//      .aclr   (~reset_n),
//      .cascin (unxcascade_chainxx66[0]),
//      .clk    (clk),
//      .dataa  (true_regA[7]),
//      .datab  (true_regA[15]),
//      .datac  (sel_rota_byte_3[0]),
//      .datad  (sel_rota_byte_3[1]),
//      .ena    (pipe_run),
//      .regout (alulogicresult[31])
//    );
//  defparam the_enet_nios_hidden_lcell_64FC31.cin_used = "false",
//           the_enet_nios_hidden_lcell_64FC31.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_64FC31.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_64FC31.output_mode = "comb_and_reg",
//           the_enet_nios_hidden_lcell_64FC31.packed_mode = "false",
//           the_enet_nios_hidden_lcell_64FC31.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F62
//    (
//      .cascin  (unxcascade_chainxx66[1]),
//      .cascout (unxcascade_chainxx66[0]),
//      .dataa   (true_regA[23]),
//      .datab   (true_regA[31]),
//      .datac   (sel_rota_byte_3[2]),
//      .datad   (sel_rota_byte_3[3])
//    );
//  defparam the_enet_nios_hidden_lcell_30F62.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F62.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_30F62.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F62.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F62.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F62.power_up = "low";
//
//  enet_nios_hidden_lcell_30F0 the_enet_nios_hidden_lcell_30F63
//    (
//      .cascin  (unxcascade_chainxx66[2]),
//      .cascout (unxcascade_chainxx66[1]),
//      .dataa   (true_regB[31]),
//      .datab   (dc_readdata[31]),
//      .datac   (sel_notb),
//      .datad   (sel_memword)
//    );
//  defparam the_enet_nios_hidden_lcell_30F63.cin_used = "false",
//           the_enet_nios_hidden_lcell_30F63.lut_mask = "0C5F",
//           the_enet_nios_hidden_lcell_30F63.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_30F63.output_mode = "comb",
//           the_enet_nios_hidden_lcell_30F63.packed_mode = "false",
//           the_enet_nios_hidden_lcell_30F63.power_up = "low";
//
//  enet_nios_hidden_lcell_10F0 the_enet_nios_hidden_lcell_10F31
//    (
//      .cascout (unxcascade_chainxx66[2]),
//      .dataa   (control_register_result[31]),
//      .datab   (custom_instruction_result[31]),
//      .datac   (do_iRDCTL),
//      .datad   (do_custom_instruction)
//    );
//  defparam the_enet_nios_hidden_lcell_10F31.cin_used = "false",
//           the_enet_nios_hidden_lcell_10F31.lut_mask = "0CAF",
//           the_enet_nios_hidden_lcell_10F31.operation_mode = "normal",
//           the_enet_nios_hidden_lcell_10F31.output_mode = "comb",
//           the_enet_nios_hidden_lcell_10F31.packed_mode = "false",
//           the_enet_nios_hidden_lcell_10F31.power_up = "low";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_nios_alushifter (
                              // inputs:
                               clk,
                               dest_cwp_3,
                               dest_local_3,
                               instruction_3,
                               is_cancelled_3,
                               is_left_shift,
                               is_neutrino_3,
                               is_right_shift,
                               op_is_MSTEP_0,
                               pipe_run,
                               reset_n,
                               subinstruction_3,
                               true_regA,
                               true_shiftValue,

                              // outputs:
                               sel_rot1,
                               shift_cycle_2,
                               shiftresult
                            )
;

  output           sel_rot1;
  output           shift_cycle_2;
  output  [ 31: 0] shiftresult;
  input            clk;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_left_shift;
  input            is_neutrino_3;
  input            is_right_shift;
  input            op_is_MSTEP_0;
  input            pipe_run;
  input            reset_n;
  input   [  5: 0] subinstruction_3;
  input   [ 31: 0] true_regA;
  input   [  4: 0] true_shiftValue;

  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire    [ 31: 0] p_shiftresult;
  wire             pipe_state_we;
  reg              sel_rot1;
  reg              shift_cycle_2;
  reg     [ 31: 0] shiftresult;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign p_shiftresult = (true_shiftValue[2 : 1] == 2'b00)? true_regA :
    (true_shiftValue[2 : 1] == 2'b01)? {true_regA[29 : 0], true_regA[31 : 30]} :
    (true_shiftValue[2 : 1] == 2'b10)? {true_regA[27 : 0], true_regA[31 : 28]} :
    {true_regA[25 : 0], true_regA[31 : 26]};

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          shiftresult <= 0;
      else if (local_pipe_clk_en)
          shiftresult <= p_shiftresult &
                    ~({op_is_MSTEP_0,{31{1'b0}}});

    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          sel_rot1 <= 0;
      else if (pipe_state_we)
          sel_rot1 <= true_shiftValue[0] || op_is_MSTEP_0;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          shift_cycle_2 <= 0;
      else if (pipe_state_we)
          shift_cycle_2 <= is_right_shift || is_left_shift || op_is_MSTEP_0;
    end



endmodule


module enet_nios_shiftvalue_conditioner (
                                          // inputs:
                                           is_right_shift,
                                           shiftValue,

                                          // outputs:
                                           true_shiftValue,
                                           true_shift_right_by_0
                                        )
;

  output  [  4: 0] true_shiftValue;
  output           true_shift_right_by_0;
  input            is_right_shift;
  input   [  4: 0] shiftValue;

  wire    [  5: 0] extended_shift_value;
  wire    [  4: 0] inv_shiftValue;
  wire    [  4: 0] true_shiftValue;
  wire             true_shift_right_by_0;
  assign inv_shiftValue = ~shiftValue;
  assign extended_shift_value = is_right_shift ?  ({1'b0, (inv_shiftValue)} + 1) :
    (            shiftValue      );

  assign true_shiftValue = extended_shift_value[4 : 0];
  assign true_shift_right_by_0 = extended_shift_value[5];

endmodule


module enet_nios_op_override_unit (
                                    // inputs:
                                     is_right_shift,
                                     true_shiftValue,
                                     true_shift_right_by_0,

                                    // outputs:
                                     overridden_byte_compl,
                                     overridden_byte_zero,
                                     overridden_byterot_sel_hi16,
                                     overridden_byterot_sel_lo16,
                                     overridden_opB
                                  )
;

  output  [  3: 0] overridden_byte_compl;
  output  [  3: 0] overridden_byte_zero;
  output  [  3: 0] overridden_byterot_sel_hi16;
  output  [  3: 0] overridden_byterot_sel_lo16;
  output  [ 15: 0] overridden_opB;
  input            is_right_shift;
  input   [  4: 0] true_shiftValue;
  input            true_shift_right_by_0;

  wire    [  7: 0] bitmask;
  wire    [  1: 0] byte_shiftValue;
  wire    [  3: 0] flipmask;
  wire    [  3: 0] overridden_byte_compl;
  wire    [  3: 0] overridden_byte_zero;
  wire    [  3: 0] overridden_byterot_sel_hi16;
  wire    [  3: 0] overridden_byterot_sel_lo16;
  wire    [ 15: 0] overridden_opB;
  assign bitmask = ((true_shiftValue[2 : 0] == 0))? 8'b00000000 :
    ((true_shiftValue[2 : 0] == 1))? 8'b00000001 :
    ((true_shiftValue[2 : 0] == 2))? 8'b00000011 :
    ((true_shiftValue[2 : 0] == 3))? 8'b00000111 :
    ((true_shiftValue[2 : 0] == 4))? 8'b00001111 :
    ((true_shiftValue[2 : 0] == 5))? 8'b00011111 :
    ((true_shiftValue[2 : 0] == 6))? 8'b00111111 :
    8'b01111111;

  assign overridden_opB = {2 {bitmask}};
  assign flipmask = true_shift_right_by_0 ? 0 : {4{is_right_shift}};
  assign byte_shiftValue = true_shiftValue[4 : 3];
  assign overridden_byte_compl = (byte_shiftValue == 2'b00)? (4'b0000 ^ flipmask) :
    (byte_shiftValue == 2'b01)? (4'b0001 ^ flipmask) :
    (byte_shiftValue == 2'b10)? (4'b0011 ^ flipmask) :
    (4'b0111 ^ flipmask);

  assign overridden_byte_zero = (byte_shiftValue == 2'b00)? 4'b1110 :
    (byte_shiftValue == 2'b01)? 4'b1101 :
    (byte_shiftValue == 2'b10)? 4'b1011 :
    4'b0111;

  assign overridden_byterot_sel_hi16 = (byte_shiftValue == 2'b00)? 4'b0001 :
    (byte_shiftValue == 2'b01)? 4'b1000 :
    (byte_shiftValue == 2'b10)? 4'b0100 :
    4'b0010;

  assign overridden_byterot_sel_lo16 = (byte_shiftValue == 2'b00)? 4'b0001 :
    (byte_shiftValue == 2'b01)? 4'b1000 :
    (byte_shiftValue == 2'b10)? 4'b0100 :
    4'b0010;


endmodule


module enet_nios_byteshift_control_unit (
                                          // inputs:
                                           byterot_sel_hi16,
                                           byterot_sel_lo16,
                                           dest_cwp_3,
                                           dest_local_3,
                                           do_iABS,
                                           fw_op_b,
                                           instruction_3,
                                           is_cancelled_3,
                                           is_neutrino_3,
                                           op_is_MSTEP_1,
                                           p_true_regA_msb,
                                           pipe_run,
                                           sel_dynamic_ext,
                                           sel_dynamic_ext16,
                                           sel_dynamic_ext8,
                                           subinstruction_3,
                                           true_regA,

                                          // outputs:
                                           sel_rota_byte_0,
                                           sel_rota_byte_1,
                                           sel_rota_byte_2,
                                           sel_rota_byte_3
                                        )
;

  output  [  3: 0] sel_rota_byte_0;
  output  [  3: 0] sel_rota_byte_1;
  output  [  3: 0] sel_rota_byte_2;
  output  [  3: 0] sel_rota_byte_3;
  input   [  3: 0] byterot_sel_hi16;
  input   [  3: 0] byterot_sel_lo16;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_iABS;
  input   [ 31: 0] fw_op_b;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_neutrino_3;
  input            op_is_MSTEP_1;
  input            p_true_regA_msb;
  input            pipe_run;
  input            sel_dynamic_ext;
  input            sel_dynamic_ext16;
  input            sel_dynamic_ext8;
  input   [  5: 0] subinstruction_3;
  input   [ 31: 0] true_regA;

  wire             abs_switcheroo;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire    [  3: 0] dynamic_ext_byte0;
  wire    [  3: 0] dynamic_ext_byte1;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire             mstep_switcheroo;
  wire             pipe_state_we;
  wire    [  3: 0] rota0;
  wire    [  3: 0] rota1;
  wire    [  3: 0] rota2;
  wire    [  3: 0] rota3;
  wire    [  3: 0] sel_rota_byte_0;
  wire    [  3: 0] sel_rota_byte_1;
  wire    [  3: 0] sel_rota_byte_2;
  wire    [  3: 0] sel_rota_byte_3;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign dynamic_ext_byte0 = (fw_op_b[1 : 0] == 2'b00)? { 1'b0,      1'b0,      1'b0,      sel_dynamic_ext    } :
    (fw_op_b[1 : 0] == 2'b01)? { 1'b0,      1'b0,      sel_dynamic_ext8,   sel_dynamic_ext16} :
    (fw_op_b[1 : 0] == 2'b10)? { 1'b0,      sel_dynamic_ext,      1'b0,      1'b0    } :
    { sel_dynamic_ext8,   sel_dynamic_ext16,  1'b0,      1'b0    };

  assign dynamic_ext_byte1 = (fw_op_b[1 : 0] == 2'b00)? { 1'b0,      1'b0,      sel_dynamic_ext16,  sel_dynamic_ext8 } :
    (fw_op_b[1 : 0] == 2'b01)? { 1'b0,      1'b0,      sel_dynamic_ext,      1'b0    } :
    (fw_op_b[1 : 0] == 2'b10)? { sel_dynamic_ext16,  sel_dynamic_ext8,   1'b0,      1'b0    } :
    { sel_dynamic_ext,      1'b0,      1'b0,      1'b0    };

  assign rota0 = ( byterot_sel_lo16                             ) | dynamic_ext_byte0;
  assign rota1 = ({byterot_sel_lo16[2 : 0], byterot_sel_lo16[3]  }) | dynamic_ext_byte1;
  assign rota2 = ({byterot_sel_hi16[1 : 0], byterot_sel_hi16[3 : 2]}) | dynamic_ext_byte0;
  assign rota3 = ({byterot_sel_hi16[0],   byterot_sel_hi16[3 : 1]}) | dynamic_ext_byte1;
  assign mstep_switcheroo = op_is_MSTEP_1 && (~p_true_regA_msb         );
  assign abs_switcheroo = do_iABS       && (~true_regA[31]);
  assign sel_rota_byte_0 = (mstep_switcheroo)? 4'b0001 :
    (abs_switcheroo)? (rota0 | 4'b0001) :
    rota0;

  assign sel_rota_byte_1 = (mstep_switcheroo)? 4'b0010 :
    (abs_switcheroo)? (rota1 | 4'b0010) :
    rota1;

  assign sel_rota_byte_2 = (mstep_switcheroo)? 4'b0100 :
    (abs_switcheroo)? (rota2 | 4'b0100) :
    rota2;

  assign sel_rota_byte_3 = (mstep_switcheroo)? 4'b1000 :
    (abs_switcheroo)? (rota3 | 4'b1000) :
    rota3;


endmodule


module enet_nios_aluzerocheck (
                                // inputs:
                                 alu_result,

                                // outputs:
                                 Z_result
                              )
;

  output           Z_result;
  input   [ 31: 0] alu_result;

  wire             Z_result;
  assign Z_result = alu_result == 0;

endmodule


module enet_nios_skip_unit (
                             // inputs:
                              C,
                              N,
                              V,
                              Z,
                              aluaddresult,
                              alulogicresult,
                              clk,
                              dest_cwp_3,
                              dest_local_3,
                              do_iSKP0x,
                              do_iSKP1x,
                              do_iSKPRzx,
                              do_iSKPS,
                              instruction_3,
                              is_cancelled_3,
                              is_neutrino_3,
                              pipe_run,
                              reset_n,
                              skip_is_active,
                              subinstruction_3,
                              test,

                             // outputs:
                              Z_result,
                              alu_result,
                              do_skip
                           )
  /* synthesis auto_dissolve = "FALSE" */ ;

  output           Z_result;
  output  [ 31: 0] alu_result;
  output           do_skip;
  input            C;
  input            N;
  input            V;
  input            Z;
  input   [ 31: 0] aluaddresult;
  input   [ 31: 0] alulogicresult;
  input            clk;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_iSKP0x;
  input            do_iSKP1x;
  input            do_iSKPRzx;
  input            do_iSKPS;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_neutrino_3;
  input            pipe_run;
  input            reset_n;
  input            skip_is_active;
  input   [  5: 0] subinstruction_3;
  input   [  3: 0] test;

  wire             Z_result;
  wire    [ 31: 0] alu_result;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  reg              do_skip;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire             skip_result;
  wire             skip_result_based_on_flags;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign alu_result = aluaddresult ^ alulogicresult;
  enet_nios_aluzerocheck the_enet_nios_aluzerocheck
    (
      .Z_result   (Z_result),
      .alu_result (alu_result)
    );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          do_skip <= 0;
      else if (local_pipe_clk_en)
          do_skip <= (skip_result     ) &&
                    ( skip_is_active ) && 
                    (~is_neutrino    ) &&
                    (~is_cancelled   );

    end


  assign skip_result = (do_iSKPS)? skip_result_based_on_flags :
    (do_iSKP0x)? Z_result :
    (do_iSKP1x)? ~Z_result :
    (do_iSKPRzx)? Z_result :
    ~Z_result;

  assign skip_result_based_on_flags = ({1 {(test == 4'b0000)}} & ( C                 )) |
    ({1 {(test == 4'b0001)}} & (~C                 )) |
    ({1 {(test == 4'b0010)}} & (      Z            )) |
    ({1 {(test == 4'b0011)}} & (     ~Z            )) |
    ({1 {(test == 4'b0100)}} & (           N       )) |
    ({1 {(test == 4'b0101)}} & (          ~N       )) |
    ({1 {(test == 4'b0110)}} & (           N ==  V )) |
    ({1 {(test == 4'b0111)}} & (           N !=  V )) |
    ({1 {(test == 4'b1000)}} & ((      Z |( N !=  V)))) |
    ({1 {(test == 4'b1001)}} & ((     ~Z &( N ==  V)))) |
    ({1 {(test == 4'b1010)}} & (                 V )) |
    ({1 {(test == 4'b1011)}} & (                ~V )) |
    ({1 {(test == 4'b1100)}} & ( C |  Z            )) |
    ({1 {(test == 4'b1101)}} & (~C & ~Z            ));


endmodule


module enet_nios_control_register_unit (
                                         // inputs:
                                          C_result,
                                          N_result,
                                          N_update,
                                          V_result,
                                          V_update,
                                          aluaddresult,
                                          alulogicresult,
                                          clk,
                                          dest_cwp_3,
                                          dest_local_3,
                                          do_iRxC_0,
                                          do_iSKP0x,
                                          do_iSKP1x,
                                          do_iSKPRzx,
                                          do_iSKPS,
                                          do_iTRET,
                                          do_iWRCTL,
                                          instruction_3,
                                          is_cancelled_3,
                                          is_neutrino_3,
                                          op_b,
                                          op_is_TRAP_0,
                                          pipe_freeze,
                                          pipe_run,
                                          qualified_do_iRESTORE,
                                          qualified_do_iSAVE,
                                          reset_n,
                                          skip_is_active,
                                          subinstruction_3,
                                          test,
                                          trap_number,
                                          true_regA,

                                         // outputs:
                                          C,
                                          CWP,
                                          DC_EN,
                                          IC_EN,
                                          IE,
                                          IPRI,
                                          alu_result,
                                          control_register_result,
                                          do_skip,
                                          trap_if_restore,
                                          trap_if_save,
                                          trap_request_overflow,
                                          trap_request_underflow
                                       )
;

  output           C;
  output  [  4: 0] CWP;
  output           DC_EN;
  output           IC_EN;
  output           IE;
  output  [  5: 0] IPRI;
  output  [ 31: 0] alu_result;
  output  [ 31: 0] control_register_result;
  output           do_skip;
  output           trap_if_restore;
  output           trap_if_save;
  output           trap_request_overflow;
  output           trap_request_underflow;
  input            C_result;
  input            N_result;
  input            N_update;
  input            V_result;
  input            V_update;
  input   [ 31: 0] aluaddresult;
  input   [ 31: 0] alulogicresult;
  input            clk;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_iRxC_0;
  input            do_iSKP0x;
  input            do_iSKP1x;
  input            do_iSKPRzx;
  input            do_iSKPS;
  input            do_iTRET;
  input            do_iWRCTL;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_neutrino_3;
  input   [ 31: 0] op_b;
  input            op_is_TRAP_0;
  input            pipe_freeze;
  input            pipe_run;
  input            qualified_do_iRESTORE;
  input            qualified_do_iSAVE;
  input            reset_n;
  input            skip_is_active;
  input   [  5: 0] subinstruction_3;
  input   [  3: 0] test;
  input   [  5: 0] trap_number;
  input   [ 31: 0] true_regA;

  wire             C;
  wire    [  4: 0] CWP;
  reg     [  4: 0] CWP_out_pre_mask;
  wire             CWP_write_enable_4;
  wire             C_deferred_reg_in;
  wire             C_deferred_we;
  reg              C_stored;
  reg              C_stored_is_stale;
  wire             C_update;
  wire             C_write_enable_8;
  wire             DC_EN;
  reg              DC_EN_out_pre_mask;
  wire             DC_EN_write_enable_0;
  wire    [  4: 0] HI_LIMIT;
  wire             IC_EN;
  reg              IC_EN_out_pre_mask;
  wire             IC_EN_write_enable_1;
  wire             IE;
  reg              IE_out_pre_mask;
  wire             IE_write_enable_2;
  wire    [  5: 0] IPRI;
  reg     [  5: 0] IPRI_out_pre_mask;
  wire             IPRI_write_enable_3;
  wire    [ 17: 0] ISTATUS;
  wire    [ 17: 0] ISTATUS_saved_status_reg_in;
  wire             IS_16;
  wire             IS_DBG;
  wire    [  4: 0] LO_LIMIT;
  wire    [  2: 0] MAJOR;
  wire    [ 10: 0] MINOR;
  wire             N;
  wire             N_deferred_reg_in;
  wire             N_deferred_we;
  reg              N_stored;
  reg              N_stored_is_stale;
  wire             N_write_enable_5;
  wire    [ 17: 0] STATUS;
  wire    [  4: 0] STATUS_CWP_reg_in;
  wire    [  4: 0] STATUS_CWP_reg_in_masked;
  wire             STATUS_C_reg_in;
  wire             STATUS_DC_EN_reg_in;
  wire             STATUS_DC_EN_reg_in_masked;
  wire             STATUS_IC_EN_reg_in;
  wire             STATUS_IC_EN_reg_in_masked;
  wire             STATUS_IE_reg_in;
  wire             STATUS_IE_reg_in_masked;
  wire    [  5: 0] STATUS_IPRI_reg_in;
  wire    [  5: 0] STATUS_IPRI_reg_in_masked;
  wire             STATUS_N_reg_in;
  wire             STATUS_V_reg_in;
  wire             STATUS_Z_reg_in;
  wire             V;
  wire    [ 15: 0] VERSIONID;
  wire             V_deferred_reg_in;
  wire             V_deferred_we;
  reg              V_stored;
  reg              V_stored_is_stale;
  wire             V_write_enable_6;
  wire    [  9: 0] WVALID;
  wire             Z;
  wire             Z_deferred_reg_in;
  wire             Z_deferred_we;
  wire             Z_result;
  reg              Z_stored;
  reg              Z_stored_is_stale;
  wire             Z_update;
  wire             Z_write_enable_7;
  wire    [ 31: 0] alu_result;
  wire    [ 31: 0] control_register_result;
  wire    [  3: 0] ctrl_index;
  wire             cwp_pipe_en;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire             do_skip;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire             qualified_overflow_trap;
  wire             qualified_underflow_trap;
  reg     [ 17: 0] saved_status;
  wire             saved_status_write_enable_9;
  wire    [  5: 0] subinstruction;
  reg              trap_if_restore;
  reg              trap_if_save;
  reg              trap_request_overflow;
  reg              trap_request_underflow;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_skip_unit the_enet_nios_skip_unit
    (
      .C                (C),
      .N                (N),
      .V                (V),
      .Z                (Z),
      .Z_result         (Z_result),
      .alu_result       (alu_result),
      .aluaddresult     (aluaddresult),
      .alulogicresult   (alulogicresult),
      .clk              (clk),
      .dest_cwp_3       (dest_cwp_3),
      .dest_local_3     (dest_local_3),
      .do_iSKP0x        (do_iSKP0x),
      .do_iSKP1x        (do_iSKP1x),
      .do_iSKPRzx       (do_iSKPRzx),
      .do_iSKPS         (do_iSKPS),
      .do_skip          (do_skip),
      .instruction_3    (instruction_3),
      .is_cancelled_3   (is_cancelled_3),
      .is_neutrino_3    (is_neutrino_3),
      .pipe_run         (pipe_run),
      .reset_n          (reset_n),
      .skip_is_active   (skip_is_active),
      .subinstruction_3 (subinstruction_3),
      .test             (test)
    );

  assign ctrl_index = op_b[9 : 5];
  assign cwp_pipe_en = pipe_run && ((~is_cancelled && ~is_neutrino) || 
    (qualified_do_iSAVE           ) ||
    (qualified_do_iRESTORE        )  );

  //STATUS, which is an e_deferred_control_register
  assign Z_update = N_update;
  assign C_update = V_update | do_iRxC_0;
  //ISTATUS, which is an e_control_register
  assign qualified_underflow_trap = (op_is_TRAP_0  && (trap_number == 1)) && 
    (~is_cancelled && ~is_neutrino      );

  assign qualified_overflow_trap = (op_is_TRAP_0  && (trap_number == 2)) && 
    (~is_cancelled && ~is_neutrino      );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          trap_if_save <= 0;
      else if (local_pipe_clk_en)
          trap_if_save <= (CWP == LO_LIMIT) && IE && (IPRI > 1);
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          trap_if_restore <= 0;
      else if (local_pipe_clk_en)
          trap_if_restore <= (CWP == HI_LIMIT) && IE && (IPRI > 2);
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          trap_request_underflow <= 0;
      else if (local_pipe_clk_en)
          if (qualified_underflow_trap  || ~IE)
              trap_request_underflow <= 0;
          else if (qualified_do_iSAVE  && trap_if_save)
              trap_request_underflow <= -1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          trap_request_overflow <= 0;
      else if (local_pipe_clk_en)
          if (qualified_overflow_trap   || ~IE)
              trap_request_overflow <= 0;
          else if (qualified_do_iRESTORE   && trap_if_restore)
              trap_request_overflow <= -1;
    end


  //WVALID, which is an e_control_register
  //VERSIONID, which is an e_control_register
  assign control_register_result = ({32 {(ctrl_index == 0)}} & STATUS) |
    ({32 {(ctrl_index == 2)}} & WVALID) |
    ({32 {(ctrl_index == 6)}} & VERSIONID) |
    ({32 {(ctrl_index == 1)}} & ISTATUS);

  assign DC_EN = DC_EN_out_pre_mask      & 1'b0;
  assign STATUS_DC_EN_reg_in_masked = STATUS_DC_EN_reg_in & 1'b0;
  assign STATUS_DC_EN_reg_in = (do_iTRET)? ISTATUS[17] :
    true_regA[17];

  assign DC_EN_write_enable_0 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    ((do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          DC_EN_out_pre_mask <= 0;
      else if (DC_EN_write_enable_0)
          DC_EN_out_pre_mask <= STATUS_DC_EN_reg_in_masked;
    end


  assign IC_EN = IC_EN_out_pre_mask      & 1'b0;
  assign STATUS_IC_EN_reg_in_masked = STATUS_IC_EN_reg_in & 1'b0;
  assign STATUS_IC_EN_reg_in = (do_iTRET)? ISTATUS[16] :
    true_regA[16];

  assign IC_EN_write_enable_1 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    ((do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          IC_EN_out_pre_mask <= 0;
      else if (IC_EN_write_enable_1)
          IC_EN_out_pre_mask <= STATUS_IC_EN_reg_in_masked;
    end


  assign IE = IE_out_pre_mask      & 1'b1;
  assign STATUS_IE_reg_in_masked = STATUS_IE_reg_in & 1'b1;
  assign STATUS_IE_reg_in = (do_iTRET)? ISTATUS[15] :
    ((do_iWRCTL && (ctrl_index == 0)))? true_regA[15] :
    (op_is_TRAP_0)? 0 :
    (((do_iWRCTL && (ctrl_index == 8))))? 0 :
    1;

  assign IE_write_enable_2 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    ((op_is_TRAP_0) || ((do_iWRCTL && (ctrl_index == 8))) || ((do_iWRCTL && (ctrl_index == 9))) || (do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          IE_out_pre_mask <= 0;
      else if (IE_write_enable_2)
          IE_out_pre_mask <= STATUS_IE_reg_in_masked;
    end


  assign IPRI = IPRI_out_pre_mask      & 6'b111111;
  assign STATUS_IPRI_reg_in_masked = STATUS_IPRI_reg_in & 6'b111111;
  assign STATUS_IPRI_reg_in = (do_iTRET)? ISTATUS[14 : 9] :
    ((do_iWRCTL && (ctrl_index == 0)))? true_regA[14 : 9] :
    trap_number;

  assign IPRI_write_enable_3 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    ((op_is_TRAP_0) || (do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          IPRI_out_pre_mask <= 6'b111111;
      else if (IPRI_write_enable_3)
          IPRI_out_pre_mask <= STATUS_IPRI_reg_in_masked;
    end


  assign CWP = CWP_out_pre_mask      & 5'b11111;
  assign STATUS_CWP_reg_in_masked = STATUS_CWP_reg_in & 5'b11111;
  assign STATUS_CWP_reg_in = (qualified_do_iSAVE)? CWP - 1 :
    (qualified_do_iRESTORE)? (CWP + 1) :
    ((op_is_TRAP_0 && ~trap_request_underflow))? CWP - 1 :
    (do_iTRET)? ISTATUS[8 : 4] :
    true_regA[8 : 4];

  assign CWP_write_enable_4 = (cwp_pipe_en) && 
    ((qualified_do_iSAVE) || (qualified_do_iRESTORE) || ((op_is_TRAP_0 && ~trap_request_underflow)) || (do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          CWP_out_pre_mask <= 30;
      else if (CWP_write_enable_4)
          CWP_out_pre_mask <= STATUS_CWP_reg_in_masked;
    end


  assign STATUS_N_reg_in = (do_iTRET)? ISTATUS[3] :
    true_regA[3];

  assign N_write_enable_5 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    ((do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  assign N_deferred_we = (N_write_enable_5)  || (~pipe_freeze && N_stored_is_stale);
  assign N_deferred_reg_in = (N_write_enable_5) ? 
    (STATUS_N_reg_in) : N;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          N_stored <= 0;
      else if (N_deferred_we)
          N_stored <= N_deferred_reg_in;
    end


  assign N = N_stored_is_stale ? 
    N_result         :
    N_stored;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          N_stored_is_stale <= 0;
      else if (local_pipe_clk_en)
          if (N_update && 
                            ~is_cancelled       &&
                            ~is_neutrino)
              N_stored_is_stale <= -1;
          else if (1'b1)
              N_stored_is_stale <= 0;
    end


  assign STATUS_V_reg_in = (do_iTRET)? ISTATUS[2] :
    true_regA[2];

  assign V_write_enable_6 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    ((do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  assign V_deferred_we = (V_write_enable_6)  || (~pipe_freeze && V_stored_is_stale);
  assign V_deferred_reg_in = (V_write_enable_6) ? 
    (STATUS_V_reg_in) : V;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          V_stored <= 0;
      else if (V_deferred_we)
          V_stored <= V_deferred_reg_in;
    end


  assign V = V_stored_is_stale ? 
    V_result         :
    V_stored;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          V_stored_is_stale <= 0;
      else if (local_pipe_clk_en)
          if (V_update && 
                            ~is_cancelled       &&
                            ~is_neutrino)
              V_stored_is_stale <= -1;
          else if (1'b1)
              V_stored_is_stale <= 0;
    end


  assign STATUS_Z_reg_in = (do_iTRET)? ISTATUS[1] :
    true_regA[1];

  assign Z_write_enable_7 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    ((do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  assign Z_deferred_we = (Z_write_enable_7)  || (~pipe_freeze && Z_stored_is_stale);
  assign Z_deferred_reg_in = (Z_write_enable_7) ? 
    (STATUS_Z_reg_in) : Z;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          Z_stored <= 0;
      else if (Z_deferred_we)
          Z_stored <= Z_deferred_reg_in;
    end


  assign Z = Z_stored_is_stale ? 
    Z_result         :
    Z_stored;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          Z_stored_is_stale <= 0;
      else if (local_pipe_clk_en)
          if (Z_update && 
                            ~is_cancelled       &&
                            ~is_neutrino)
              Z_stored_is_stale <= -1;
          else if (1'b1)
              Z_stored_is_stale <= 0;
    end


  assign STATUS_C_reg_in = (do_iTRET)? ISTATUS[0] :
    true_regA[0];

  assign C_write_enable_8 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    ((do_iTRET) || (do_iWRCTL && (ctrl_index == 0)));

  assign C_deferred_we = (C_write_enable_8)  || (~pipe_freeze && C_stored_is_stale);
  assign C_deferred_reg_in = (C_write_enable_8) ? 
    (STATUS_C_reg_in) : C;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          C_stored <= 0;
      else if (C_deferred_we)
          C_stored <= C_deferred_reg_in;
    end


  assign C = C_stored_is_stale ? 
    C_result         :
    C_stored;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          C_stored_is_stale <= 0;
      else if (local_pipe_clk_en)
          if (C_update && 
                            ~is_cancelled       &&
                            ~is_neutrino)
              C_stored_is_stale <= -1;
          else if (1'b1)
              C_stored_is_stale <= 0;
    end


  assign STATUS = {DC_EN,
    IC_EN,
    IE,
    IPRI,
    CWP,
    N,
    V,
    Z,
    C};

  assign ISTATUS_saved_status_reg_in = (((do_iWRCTL && (ctrl_index == 1))))? true_regA[17 : 0] :
    STATUS[17 : 0];

  assign saved_status_write_enable_9 = ((pipe_run && ~is_cancelled && ~is_neutrino)) && 
    (((do_iWRCTL && (ctrl_index == 1))) || (op_is_TRAP_0));

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          saved_status <= 0;
      else if (saved_status_write_enable_9)
          saved_status <= ISTATUS_saved_status_reg_in;
    end


  assign ISTATUS = saved_status;
  assign HI_LIMIT = 30;
  assign LO_LIMIT = 1;
  assign WVALID = {HI_LIMIT,
    LO_LIMIT};

  assign IS_16 = 0;
  assign MAJOR = 3;
  assign MINOR = 36;
  assign IS_DBG = 0;
  assign VERSIONID = {IS_16,
    MAJOR,
    MINOR,
    IS_DBG};


endmodule


module enet_nios_compact_alu (
                               // inputs:
                                N_update,
                                V_update,
                                byte_complement,
                                byte_zero,
                                byterot_sel_hi16,
                                byterot_sel_lo16,
                                c_is_borrow,
                                clk,
                                custom_instruction_result,
                                custom_instruction_start_mul,
                                dc_readdata,
                                dest_cwp_3,
                                dest_local_3,
                                do_custom_instruction,
                                do_fwd_a_alu,
                                do_fwd_b_alu,
                                do_iABS,
                                do_iASRx_1,
                                do_iRDCTL,
                                do_iRLC_1,
                                do_iRRC_1,
                                do_iRxC_0,
                                do_iRxC_1,
                                do_iSEXT16,
                                do_iSEXT8,
                                do_iSKP0x,
                                do_iSKP1x,
                                do_iSKPRzx,
                                do_iSKPS,
                                do_iTRET,
                                do_iWRCTL,
                                force_carryin,
                                instruction_3,
                                is_cancelled_3,
                                is_left_shift,
                                is_neutrino_3,
                                is_right_shift,
                                op_a,
                                op_b,
                                op_b_is_overridden,
                                op_is_MSTEP_0,
                                op_is_MSTEP_1,
                                op_is_TRAP_0,
                                pipe_freeze,
                                pipe_run,
                                qualified_do_iRESTORE,
                                qualified_do_iSAVE,
                                reset_n,
                                sel_dynamic_ext,
                                sel_dynamic_ext16,
                                sel_dynamic_ext8,
                                sel_memword,
                                sel_notb_x,
                                shiftValue,
                                skip_is_active,
                                sload_add_with_b_control,
                                sreset_add_0,
                                sreset_add_x,
                                subinstruction_3,
                                test,
                                trap_number,
                                unqualified_custom_instruction_start_0,
                                unqualified_custom_instruction_start_1,
                                unqualified_custom_instruction_start_2,
                                unqualified_custom_instruction_start_3,
                                unqualified_custom_instruction_start_4,

                               // outputs:
                                CWP,
                                DC_EN,
                                IC_EN,
                                IE,
                                IPRI,
                                alu_result,
                                custom_instruction_clk_en,
                                custom_instruction_dataa,
                                custom_instruction_datab,
                                custom_instruction_reset,
                                custom_instruction_start,
                                do_skip,
                                overridden_byte_compl,
                                overridden_byte_zero,
                                overridden_byterot_sel_hi16,
                                overridden_byterot_sel_lo16,
                                overridden_opB,
                                trap_if_restore,
                                trap_if_save,
                                trap_request_overflow,
                                trap_request_underflow
                             )
;

  output  [  4: 0] CWP;
  output           DC_EN;
  output           IC_EN;
  output           IE;
  output  [  5: 0] IPRI;
  output  [ 31: 0] alu_result;
  output           custom_instruction_clk_en;
  output  [ 31: 0] custom_instruction_dataa;
  output  [ 31: 0] custom_instruction_datab;
  output           custom_instruction_reset;
  output  [  5: 0] custom_instruction_start;
  output           do_skip;
  output  [  3: 0] overridden_byte_compl;
  output  [  3: 0] overridden_byte_zero;
  output  [  3: 0] overridden_byterot_sel_hi16;
  output  [  3: 0] overridden_byterot_sel_lo16;
  output  [ 15: 0] overridden_opB;
  output           trap_if_restore;
  output           trap_if_save;
  output           trap_request_overflow;
  output           trap_request_underflow;
  input            N_update;
  input            V_update;
  input   [  3: 0] byte_complement;
  input   [  3: 0] byte_zero;
  input   [  3: 0] byterot_sel_hi16;
  input   [  3: 0] byterot_sel_lo16;
  input            c_is_borrow;
  input            clk;
  input   [ 31: 0] custom_instruction_result;
  input            custom_instruction_start_mul;
  input   [ 31: 0] dc_readdata;
  input   [  4: 0] dest_cwp_3;
  input   [  4: 0] dest_local_3;
  input            do_custom_instruction;
  input            do_fwd_a_alu;
  input            do_fwd_b_alu;
  input            do_iABS;
  input            do_iASRx_1;
  input            do_iRDCTL;
  input            do_iRLC_1;
  input            do_iRRC_1;
  input            do_iRxC_0;
  input            do_iRxC_1;
  input            do_iSEXT16;
  input            do_iSEXT8;
  input            do_iSKP0x;
  input            do_iSKP1x;
  input            do_iSKPRzx;
  input            do_iSKPS;
  input            do_iTRET;
  input            do_iWRCTL;
  input            force_carryin;
  input   [ 15: 0] instruction_3;
  input            is_cancelled_3;
  input            is_left_shift;
  input            is_neutrino_3;
  input            is_right_shift;
  input   [ 31: 0] op_a;
  input   [ 31: 0] op_b;
  input            op_b_is_overridden;
  input            op_is_MSTEP_0;
  input            op_is_MSTEP_1;
  input            op_is_TRAP_0;
  input            pipe_freeze;
  input            pipe_run;
  input            qualified_do_iRESTORE;
  input            qualified_do_iSAVE;
  input            reset_n;
  input            sel_dynamic_ext;
  input            sel_dynamic_ext16;
  input            sel_dynamic_ext8;
  input            sel_memword;
  input            sel_notb_x;
  input   [  4: 0] shiftValue;
  input            skip_is_active;
  input            sload_add_with_b_control;
  input            sreset_add_0;
  input            sreset_add_x;
  input   [  5: 0] subinstruction_3;
  input   [  3: 0] test;
  input   [  5: 0] trap_number;
  input            unqualified_custom_instruction_start_0;
  input            unqualified_custom_instruction_start_1;
  input            unqualified_custom_instruction_start_2;
  input            unqualified_custom_instruction_start_3;
  input            unqualified_custom_instruction_start_4;

  wire             C;
  wire    [  4: 0] CWP;
  wire             C_result;
  wire             DC_EN;
  wire             IC_EN;
  wire             IE;
  wire    [  5: 0] IPRI;
  wire             N_result;
  wire             V_result;
  wire    [ 31: 0] alu_result;
  wire    [ 31: 0] aluaddresult;
  wire    [ 31: 0] alulogicresult;
  wire             carryout;
  wire             computed_sreset_add;
  wire    [ 31: 0] control_register_result;
  wire             custom_instruction_clk_en;
  wire    [ 31: 0] custom_instruction_dataa;
  wire    [ 31: 0] custom_instruction_datab;
  wire             custom_instruction_reset;
  wire    [  5: 0] custom_instruction_start;
  reg              d1_C;
  reg              d1_c_is_borrow;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_local;
  wire             do_skip;
  wire    [ 31: 0] fw_op_b;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  reg              msb_xor_sum;
  wire    [  3: 0] overridden_byte_compl;
  wire    [  3: 0] overridden_byte_zero;
  wire    [  3: 0] overridden_byterot_sel_hi16;
  wire    [  3: 0] overridden_byterot_sel_lo16;
  wire    [ 15: 0] overridden_opB;
  reg              p_true_regA_lsb;
  reg              p_true_regA_msb;
  wire             pipe_state_we;
  wire             sel_rot1;
  wire    [  3: 0] sel_rota_byte_0;
  wire    [  3: 0] sel_rota_byte_1;
  wire    [  3: 0] sel_rota_byte_2;
  wire    [  3: 0] sel_rota_byte_3;
  wire             shift_cycle_2;
  wire    [ 31: 0] shiftresult;
  wire             sreset_add;
  wire    [  5: 0] subinstruction;
  wire             trap_if_restore;
  wire             trap_if_save;
  wire             trap_request_overflow;
  wire             trap_request_underflow;
  wire    [ 31: 0] true_regA;
  wire    [ 31: 0] true_regB;
  wire    [  4: 0] true_shiftValue;
  wire             true_shift_right_by_0;
  assign is_neutrino = is_neutrino_3;
  assign dest_local = dest_local_3;
  assign instruction = instruction_3;
  assign is_cancelled = is_cancelled_3;
  assign dest_cwp = dest_cwp_3;
  assign subinstruction = subinstruction_3;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_adder_logic_lock_region the_enet_nios_adder_logic_lock_region
    (
      .aluaddresult             (aluaddresult),
      .alulogicresult           (alulogicresult),
      .byte_complement          (byte_complement),
      .byte_zero                (byte_zero),
      .carryout                 (carryout),
      .clk                      (clk),
      .computed_sreset_add      (computed_sreset_add),
      .dest_cwp_3               (dest_cwp_3),
      .dest_local_3             (dest_local_3),
      .do_fwd_a_alu             (do_fwd_a_alu),
      .do_fwd_b_alu             (do_fwd_b_alu),
      .do_iASRx_1               (do_iASRx_1),
      .do_iRxC_1                (do_iRxC_1),
      .force_carryin            (force_carryin),
      .fw_op_b                  (fw_op_b),
      .instruction_3            (instruction_3),
      .is_cancelled_3           (is_cancelled_3),
      .is_neutrino_3            (is_neutrino_3),
      .op_a                     (op_a),
      .op_b                     (op_b),
      .pipe_run                 (pipe_run),
      .reset_n                  (reset_n),
      .sel_rot1                 (sel_rot1),
      .shift_cycle_2            (shift_cycle_2),
      .shiftresult              (shiftresult),
      .sload_add_with_b_control (sload_add_with_b_control),
      .subinstruction_3         (subinstruction_3),
      .true_regA                (true_regA),
      .true_regB                (true_regB)
    );

  enet_nios_alulogic the_enet_nios_alulogic
    (
      .alulogicresult                         (alulogicresult),
      .clk                                    (clk),
      .control_register_result                (control_register_result),
      .custom_instruction_clk_en              (custom_instruction_clk_en),
      .custom_instruction_dataa               (custom_instruction_dataa),
      .custom_instruction_datab               (custom_instruction_datab),
      .custom_instruction_reset               (custom_instruction_reset),
      .custom_instruction_result              (custom_instruction_result),
      .custom_instruction_start               (custom_instruction_start),
      .custom_instruction_start_mul           (custom_instruction_start_mul),
      .dc_readdata                            (dc_readdata),
      .dest_cwp_3                             (dest_cwp_3),
      .dest_local_3                           (dest_local_3),
      .do_custom_instruction                  (do_custom_instruction),
      .do_iRDCTL                              (do_iRDCTL),
      .instruction_3                          (instruction_3),
      .is_cancelled_3                         (is_cancelled_3),
      .is_neutrino_3                          (is_neutrino_3),
      .op_b_is_overridden                     (op_b_is_overridden),
      .pipe_run                               (pipe_run),
      .reset_n                                (reset_n),
      .sel_memword                            (sel_memword),
      .sel_notb_x                             (sel_notb_x),
      .sel_rota_byte_0                        (sel_rota_byte_0),
      .sel_rota_byte_1                        (sel_rota_byte_1),
      .sel_rota_byte_2                        (sel_rota_byte_2),
      .sel_rota_byte_3                        (sel_rota_byte_3),
      .subinstruction_3                       (subinstruction_3),
      .true_regA                              (true_regA),
      .true_regB                              (true_regB),
      .unqualified_custom_instruction_start_0 (unqualified_custom_instruction_start_0),
      .unqualified_custom_instruction_start_1 (unqualified_custom_instruction_start_1),
      .unqualified_custom_instruction_start_2 (unqualified_custom_instruction_start_2),
      .unqualified_custom_instruction_start_3 (unqualified_custom_instruction_start_3),
      .unqualified_custom_instruction_start_4 (unqualified_custom_instruction_start_4)
    );

  enet_nios_alushifter the_enet_nios_alushifter
    (
      .clk              (clk),
      .dest_cwp_3       (dest_cwp_3),
      .dest_local_3     (dest_local_3),
      .instruction_3    (instruction_3),
      .is_cancelled_3   (is_cancelled_3),
      .is_left_shift    (is_left_shift),
      .is_neutrino_3    (is_neutrino_3),
      .is_right_shift   (is_right_shift),
      .op_is_MSTEP_0    (op_is_MSTEP_0),
      .pipe_run         (pipe_run),
      .reset_n          (reset_n),
      .sel_rot1         (sel_rot1),
      .shift_cycle_2    (shift_cycle_2),
      .shiftresult      (shiftresult),
      .subinstruction_3 (subinstruction_3),
      .true_regA        (true_regA),
      .true_shiftValue  (true_shiftValue)
    );

  enet_nios_shiftvalue_conditioner the_enet_nios_shiftvalue_conditioner
    (
      .is_right_shift        (is_right_shift),
      .shiftValue            (shiftValue),
      .true_shiftValue       (true_shiftValue),
      .true_shift_right_by_0 (true_shift_right_by_0)
    );

  enet_nios_op_override_unit the_enet_nios_op_override_unit
    (
      .is_right_shift              (is_right_shift),
      .overridden_byte_compl       (overridden_byte_compl),
      .overridden_byte_zero        (overridden_byte_zero),
      .overridden_byterot_sel_hi16 (overridden_byterot_sel_hi16),
      .overridden_byterot_sel_lo16 (overridden_byterot_sel_lo16),
      .overridden_opB              (overridden_opB),
      .true_shiftValue             (true_shiftValue),
      .true_shift_right_by_0       (true_shift_right_by_0)
    );

  enet_nios_byteshift_control_unit the_enet_nios_byteshift_control_unit
    (
      .byterot_sel_hi16  (byterot_sel_hi16),
      .byterot_sel_lo16  (byterot_sel_lo16),
      .dest_cwp_3        (dest_cwp_3),
      .dest_local_3      (dest_local_3),
      .do_iABS           (do_iABS),
      .fw_op_b           (fw_op_b),
      .instruction_3     (instruction_3),
      .is_cancelled_3    (is_cancelled_3),
      .is_neutrino_3     (is_neutrino_3),
      .op_is_MSTEP_1     (op_is_MSTEP_1),
      .p_true_regA_msb   (p_true_regA_msb),
      .pipe_run          (pipe_run),
      .sel_dynamic_ext   (sel_dynamic_ext),
      .sel_dynamic_ext16 (sel_dynamic_ext16),
      .sel_dynamic_ext8  (sel_dynamic_ext8),
      .sel_rota_byte_0   (sel_rota_byte_0),
      .sel_rota_byte_1   (sel_rota_byte_1),
      .sel_rota_byte_2   (sel_rota_byte_2),
      .sel_rota_byte_3   (sel_rota_byte_3),
      .subinstruction_3  (subinstruction_3),
      .true_regA         (true_regA)
    );

  enet_nios_control_register_unit the_enet_nios_control_register_unit
    (
      .C                       (C),
      .CWP                     (CWP),
      .C_result                (C_result),
      .DC_EN                   (DC_EN),
      .IC_EN                   (IC_EN),
      .IE                      (IE),
      .IPRI                    (IPRI),
      .N_result                (N_result),
      .N_update                (N_update),
      .V_result                (V_result),
      .V_update                (V_update),
      .alu_result              (alu_result),
      .aluaddresult            (aluaddresult),
      .alulogicresult          (alulogicresult),
      .clk                     (clk),
      .control_register_result (control_register_result),
      .dest_cwp_3              (dest_cwp_3),
      .dest_local_3            (dest_local_3),
      .do_iRxC_0               (do_iRxC_0),
      .do_iSKP0x               (do_iSKP0x),
      .do_iSKP1x               (do_iSKP1x),
      .do_iSKPRzx              (do_iSKPRzx),
      .do_iSKPS                (do_iSKPS),
      .do_iTRET                (do_iTRET),
      .do_iWRCTL               (do_iWRCTL),
      .do_skip                 (do_skip),
      .instruction_3           (instruction_3),
      .is_cancelled_3          (is_cancelled_3),
      .is_neutrino_3           (is_neutrino_3),
      .op_b                    (op_b),
      .op_is_TRAP_0            (op_is_TRAP_0),
      .pipe_freeze             (pipe_freeze),
      .pipe_run                (pipe_run),
      .qualified_do_iRESTORE   (qualified_do_iRESTORE),
      .qualified_do_iSAVE      (qualified_do_iSAVE),
      .reset_n                 (reset_n),
      .skip_is_active          (skip_is_active),
      .subinstruction_3        (subinstruction_3),
      .test                    (test),
      .trap_if_restore         (trap_if_restore),
      .trap_if_save            (trap_if_save),
      .trap_number             (trap_number),
      .trap_request_overflow   (trap_request_overflow),
      .trap_request_underflow  (trap_request_underflow),
      .true_regA               (true_regA)
    );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p_true_regA_msb <= 0;
      else if (local_pipe_clk_en)
          p_true_regA_msb <= true_regA[31];
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p_true_regA_lsb <= 0;
      else if (local_pipe_clk_en)
          p_true_regA_lsb <= true_regA[0];
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          d1_C <= 0;
      else if (local_pipe_clk_en)
          d1_C <= C;
    end


  assign computed_sreset_add = (sreset_add                                                ) ||
    ((do_iASRx_1 || op_is_MSTEP_1) && ~p_true_regA_msb         ) ||
    ((do_iRxC_1                  ) && ~d1_C                    ) ||
    ((do_iSEXT16                 ) && ~true_regA[15]           ) ||
    ((do_iSEXT8                  ) && ~true_regA[7]           ) || 
    ((do_iABS                    ) && ~true_regA[31]);

  assign sreset_add = (sreset_add_x                    ) || 
    (sreset_add_0 && |subinstruction ) ||
    (op_is_TRAP_0                    );

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          d1_c_is_borrow <= 0;
      else if (local_pipe_clk_en)
          d1_c_is_borrow <= c_is_borrow;
    end


  assign C_result = (do_iRLC_1)? p_true_regA_msb :
    (do_iRRC_1)? p_true_regA_lsb :
    (carryout ^ d1_c_is_borrow);

  assign N_result = alu_result[31];
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          msb_xor_sum <= 0;
      else if (local_pipe_clk_en)
          msb_xor_sum <= true_regA[31] ^ true_regB[31];
    end


  assign V_result = ~msb_xor_sum && 
    (aluaddresult[31] ^ p_true_regA_msb);


endmodule


module enet_nios_cwp_adder (
                             // inputs:
                              cwp_base,
                              local_index,

                             // outputs:
                              adjusted_index
                           )
;

  output  [  8: 0] adjusted_index;
  input   [  4: 0] cwp_base;
  input   [  4: 0] local_index;

  wire    [  8: 0] adjusted_index;
  assign adjusted_index = (|(local_index[4 : 3]))? ({cwp_base , 4'b0} + local_index) :
    {2'b0 , local_index[2 : 0]};


endmodule


module enet_nios_register_bank_a_module (
                                          // inputs:
                                           data,
                                           rdaddress,
                                           rdclken,
                                           rdclock,
                                           wraddress,
                                           wrclock,
                                           wren,

                                          // outputs:
                                           q
                                        )
;

  output  [ 31: 0] q;
  input   [ 31: 0] data;
  input   [  8: 0] rdaddress;
  input            rdclken;
  input            rdclock;
  input   [  8: 0] wraddress;
  input            wrclock;
  input            wren;

  reg     [ 31: 0] d1_data;
  reg     [  8: 0] d1_rdaddress;
  reg     [  8: 0] d1_wraddress;
  reg              d1_wren;
  reg     [ 31: 0] mem_array [511: 0];
  wire    [ 31: 0] q;
  wire             wrclken;
  assign wrclken = 1'b1;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  always @(posedge rdclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_rdaddress <= 0;
      else if (rdclken)
          d1_rdaddress <= rdaddress;
    end


  assign q = mem_array[d1_rdaddress];
  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_wraddress <= 0;
      else if (1'b1)
          d1_wraddress <= wraddress;
    end


  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_wren <= 0;
      else if (1'b1)
          d1_wren <= wren;
    end


  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_data <= 0;
      else if (1'b1)
          d1_data <= data;
    end


  always @(negedge wrclock)
    begin
      // Write data
      if (d1_wren)
          mem_array[d1_wraddress] <= d1_data;
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  lpm_ram_dp the_lpm_ram_dp
//    (
//      .data (data),
//      .q (q),
//      .rdaddress (rdaddress),
//      .rdclken (rdclken),
//      .rdclock (rdclock),
//      .wraddress (wraddress),
//      .wrclock (wrclock),
//      .wren (wren)
//    );
//
//  defparam the_lpm_ram_dp.lpm_file = "UNUSED",
//           the_lpm_ram_dp.lpm_hint = "USE_EAB=ON",
//           the_lpm_ram_dp.lpm_indata = "REGISTERED",
//           the_lpm_ram_dp.lpm_outdata = "UNREGISTERED",
//           the_lpm_ram_dp.lpm_rdaddress_control = "REGISTERED",
//           the_lpm_ram_dp.lpm_width = 32,
//           the_lpm_ram_dp.lpm_widthad = 9,
//           the_lpm_ram_dp.lpm_wraddress_control = "REGISTERED",
//           the_lpm_ram_dp.suppress_memory_conversion_warnings = "ON";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_nios_register_bank_b_module (
                                          // inputs:
                                           data,
                                           rdaddress,
                                           rdclken,
                                           rdclock,
                                           wraddress,
                                           wrclock,
                                           wren,

                                          // outputs:
                                           q
                                        )
;

  output  [ 31: 0] q;
  input   [ 31: 0] data;
  input   [  8: 0] rdaddress;
  input            rdclken;
  input            rdclock;
  input   [  8: 0] wraddress;
  input            wrclock;
  input            wren;

  reg     [ 31: 0] d1_data;
  reg     [  8: 0] d1_rdaddress;
  reg     [  8: 0] d1_wraddress;
  reg              d1_wren;
  reg     [ 31: 0] mem_array [511: 0];
  wire    [ 31: 0] q;
  wire             wrclken;
  assign wrclken = 1'b1;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  always @(posedge rdclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_rdaddress <= 0;
      else if (rdclken)
          d1_rdaddress <= rdaddress;
    end


  assign q = mem_array[d1_rdaddress];
  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_wraddress <= 0;
      else if (1'b1)
          d1_wraddress <= wraddress;
    end


  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_wren <= 0;
      else if (1'b1)
          d1_wren <= wren;
    end


  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_data <= 0;
      else if (1'b1)
          d1_data <= data;
    end


  always @(negedge wrclock)
    begin
      // Write data
      if (d1_wren)
          mem_array[d1_wraddress] <= d1_data;
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  lpm_ram_dp the_lpm_ram_dp
//    (
//      .data (data),
//      .q (q),
//      .rdaddress (rdaddress),
//      .rdclken (rdclken),
//      .rdclock (rdclock),
//      .wraddress (wraddress),
//      .wrclock (wrclock),
//      .wren (wren)
//    );
//
//  defparam the_lpm_ram_dp.lpm_file = "UNUSED",
//           the_lpm_ram_dp.lpm_hint = "USE_EAB=ON",
//           the_lpm_ram_dp.lpm_indata = "REGISTERED",
//           the_lpm_ram_dp.lpm_outdata = "UNREGISTERED",
//           the_lpm_ram_dp.lpm_rdaddress_control = "REGISTERED",
//           the_lpm_ram_dp.lpm_width = 32,
//           the_lpm_ram_dp.lpm_widthad = 9,
//           the_lpm_ram_dp.lpm_wraddress_control = "REGISTERED",
//           the_lpm_ram_dp.suppress_memory_conversion_warnings = "ON";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_nios_register_file (
                                 // inputs:
                                  CWP,
                                  a_local,
                                  alu_result,
                                  b_local,
                                  clk,
                                  dest_cwp_4,
                                  dest_local_4,
                                  instruction_4,
                                  is_cancelled_4,
                                  is_neutrino_4,
                                  non_write_op,
                                  pipe_run,
                                  subinstruction_4,
                                  use_fresh_cwp,

                                 // outputs:
                                  raw_reg_a,
                                  raw_reg_b
                               )
;

  output  [ 31: 0] raw_reg_a;
  output  [ 31: 0] raw_reg_b;
  input   [  4: 0] CWP;
  input   [  4: 0] a_local;
  input   [ 31: 0] alu_result;
  input   [  4: 0] b_local;
  input            clk;
  input   [  4: 0] dest_cwp_4;
  input   [  4: 0] dest_local_4;
  input   [ 15: 0] instruction_4;
  input            is_cancelled_4;
  input            is_neutrino_4;
  input            non_write_op;
  input            pipe_run;
  input   [  5: 0] subinstruction_4;
  input            use_fresh_cwp;

  wire    [  8: 0] a_reg_index;
  wire    [  8: 0] b_reg_index;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_cwp_adder_base;
  wire    [  8: 0] dest_index;
  wire    [  4: 0] dest_local;
  wire             do_write_reg;
  wire    [ 15: 0] instruction;
  wire             is_cancelled;
  wire             is_neutrino;
  wire             local_pipe_clk_en;
  wire             pipe_state_we;
  wire    [ 31: 0] raw_reg_a;
  wire    [ 31: 0] raw_reg_b;
  wire             reg_write_enable;
  wire    [  5: 0] subinstruction;
  assign is_neutrino = is_neutrino_4;
  assign dest_local = dest_local_4;
  assign instruction = instruction_4;
  assign is_cancelled = is_cancelled_4;
  assign dest_cwp = dest_cwp_4;
  assign subinstruction = subinstruction_4;
  assign local_pipe_clk_en = pipe_run;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  enet_nios_cwp_adder the_enet_nios_cwp_adder
    (
      .adjusted_index (a_reg_index),
      .cwp_base       (CWP),
      .local_index    (a_local)
    );

  enet_nios_cwp_adder the_enet_nios_cwp_adder1
    (
      .adjusted_index (b_reg_index),
      .cwp_base       (CWP),
      .local_index    (b_local)
    );

  assign dest_cwp_adder_base = use_fresh_cwp ? (CWP) : (dest_cwp);
  enet_nios_cwp_adder the_enet_nios_cwp_adder2
    (
      .adjusted_index (dest_index),
      .cwp_base       (dest_cwp_adder_base),
      .local_index    (dest_local)
    );

  assign reg_write_enable = do_write_reg && (~is_neutrino) && (~is_cancelled);
  assign do_write_reg = ~non_write_op;
//enet_nios_register_bank_a, which is an e_dpram
enet_nios_register_bank_a_module enet_nios_register_bank_a
  (
    .data      (alu_result),
    .q         (raw_reg_a),
    .rdaddress (a_reg_index),
    .rdclken   (pipe_run),
    .rdclock   (clk),
    .wraddress (dest_index),
    .wrclock   (clk),
    .wren      (reg_write_enable)
  );

//enet_nios_register_bank_b, which is an e_dpram
enet_nios_register_bank_b_module enet_nios_register_bank_b
  (
    .data      (alu_result),
    .q         (raw_reg_b),
    .rdaddress (b_reg_index),
    .rdclken   (pipe_run),
    .rdclock   (clk),
    .wraddress (dest_index),
    .wrclock   (clk),
    .wren      (reg_write_enable)
  );


endmodule


module enet_nios_instruction_decoder_rom_module (
                                                  // inputs:
                                                   rdaddress,
                                                   rdclken,
                                                   rdclock,

                                                  // outputs:
                                                   q
                                                )
;

  output  [ 15: 0] q;
  input   [  6: 0] rdaddress;
  input            rdclken;
  input            rdclock;

  reg     [ 15: 0] d1_data;
  reg     [  6: 0] d1_rdaddress;
  reg     [  6: 0] d1_wraddress;
  reg              d1_wren;
  wire    [ 15: 0] data;
  reg     [ 15: 0] mem_array [127: 0];
  wire    [ 15: 0] q;
  wire    [  6: 0] wraddress;
  wire             wrclken;
  wire             wrclock;
  wire             wren;
  assign wrclken = 1'b1;
  assign wren = 1'b0;
  assign wrclock = 1'b0;
  assign wraddress = 7'b0;
  assign data = 16'b0;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
initial
    $readmemh("enet_nios_instruction_decoder_rom.dat", mem_array);
  always @(posedge rdclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_rdaddress <= 0;
      else if (rdclken)
          d1_rdaddress <= rdaddress;
    end


  assign q = mem_array[d1_rdaddress];
  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_wraddress <= 0;
      else if (1'b1)
          d1_wraddress <= wraddress;
    end


  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_wren <= 0;
      else if (1'b1)
          d1_wren <= wren;
    end


  always @(posedge wrclock or negedge 1'b1)
    begin
      if (1'b1 == 0)
          d1_data <= 0;
      else if (1'b1)
          d1_data <= data;
    end


  always @(negedge wrclock)
    begin
      // Write data
      if (d1_wren)
          mem_array[d1_wraddress] <= d1_data;
    end



//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on
//synthesis read_comments_as_HDL on
//  lpm_ram_dp the_lpm_ram_dp
//    (
//      .data (data),
//      .q (q),
//      .rdaddress (rdaddress),
//      .rdclken (rdclken),
//      .rdclock (rdclock),
//      .wraddress (wraddress),
//      .wrclock (wrclock),
//      .wren (wren)
//    );
//
//  defparam the_lpm_ram_dp.lpm_file = "enet_nios_instruction_decoder_rom.mif",
//           the_lpm_ram_dp.lpm_hint = "USE_EAB=ON",
//           the_lpm_ram_dp.lpm_indata = "REGISTERED",
//           the_lpm_ram_dp.lpm_outdata = "UNREGISTERED",
//           the_lpm_ram_dp.lpm_rdaddress_control = "REGISTERED",
//           the_lpm_ram_dp.lpm_width = 16,
//           the_lpm_ram_dp.lpm_widthad = 7,
//           the_lpm_ram_dp.lpm_wraddress_control = "REGISTERED",
//           the_lpm_ram_dp.suppress_memory_conversion_warnings = "ON";
//
//synthesis read_comments_as_HDL off

endmodule


module enet_nios_rom_decoder_unit (
                                    // inputs:
                                     clk,
                                     commit,
                                     is_cancelled,
                                     is_neutrino,
                                     major_opcode,
                                     minor_opcode,

                                    // outputs:
                                     p_byterot_F_control,
                                     p_do_byterot_1,
                                     p_do_custom_instruction,
                                     p_do_inv_all_b,
                                     p_do_normal_offset,
                                     p_do_not_interrupt,
                                     p_op_b_from_Ki5,
                                     p_op_b_from_reg_b,
                                     p_op_uses_a_pre_alu,
                                     p_op_uses_b_pre_alu,
                                     p_override_b_control,
                                     p_reg_not_modified,
                                     p_sel_notb_x,
                                     p_sload_add_with_b_control,
                                     p_sreset_add_x,
                                     p_zero_all_b_control
                                  )
;

  output           p_byterot_F_control;
  output           p_do_byterot_1;
  output           p_do_custom_instruction;
  output           p_do_inv_all_b;
  output           p_do_normal_offset;
  output           p_do_not_interrupt;
  output           p_op_b_from_Ki5;
  output           p_op_b_from_reg_b;
  output           p_op_uses_a_pre_alu;
  output           p_op_uses_b_pre_alu;
  output           p_override_b_control;
  output           p_reg_not_modified;
  output           p_sel_notb_x;
  output           p_sload_add_with_b_control;
  output           p_sreset_add_x;
  output           p_zero_all_b_control;
  input            clk;
  input            commit;
  input            is_cancelled;
  input            is_neutrino;
  input   [  5: 0] major_opcode;
  input   [  4: 0] minor_opcode;

  wire    [  6: 0] decoder_rom_address;
  wire    [ 15: 0] decoder_rom_q;
  wire             is_subtable_u;
  wire             is_subtable_v;
  wire             is_subtable_w;
  wire             local_pipe_clk_en;
  wire             p_byterot_F_control;
  wire             p_do_byterot_1;
  wire             p_do_custom_instruction;
  wire             p_do_inv_all_b;
  wire             p_do_normal_offset;
  wire             p_do_not_interrupt;
  wire             p_op_b_from_Ki5;
  wire             p_op_b_from_reg_b;
  wire             p_op_uses_a_pre_alu;
  wire             p_op_uses_b_pre_alu;
  wire             p_override_b_control;
  wire             p_reg_not_modified;
  wire             p_sel_notb_x;
  wire             p_sload_add_with_b_control;
  wire             p_sreset_add_x;
  wire             p_zero_all_b_control;
  wire             pipe_state_we;
  assign local_pipe_clk_en = commit;
  assign pipe_state_we = local_pipe_clk_en && 
    ~is_neutrino      && 
    ~is_cancelled;

  assign is_subtable_u = major_opcode == 6'b011101;
  assign is_subtable_v = major_opcode == 6'b011110;
  assign is_subtable_w = major_opcode == 6'b011111;
  assign decoder_rom_address = ({7 {is_subtable_w}} & {2'b11, minor_opcode}) |
    ({7 {is_subtable_u}} & ({2'b10, minor_opcode} & ~(7'b0000001))) |
    ({7 {is_subtable_v}} & ({2'b10, minor_opcode} |  (7'b0000001))) |
    ({7 {(~(is_subtable_w)) && (~(is_subtable_u)) && (~(is_subtable_v))}} & {1'b0,  major_opcode});

//enet_nios_instruction_decoder_rom, which is an e_dpram
enet_nios_instruction_decoder_rom_module enet_nios_instruction_decoder_rom
  (
    .q         (decoder_rom_q),
    .rdaddress (decoder_rom_address),
    .rdclken   (local_pipe_clk_en),
    .rdclock   (clk)
  );

  assign p_op_b_from_reg_b = decoder_rom_q[15];
  assign p_sreset_add_x = decoder_rom_q[14];
  assign p_do_normal_offset = decoder_rom_q[13];
  assign p_do_custom_instruction = decoder_rom_q[12];
  assign p_reg_not_modified = decoder_rom_q[11];
  assign p_do_not_interrupt = decoder_rom_q[10];
  assign p_op_uses_b_pre_alu = decoder_rom_q[9];
  assign p_op_b_from_Ki5 = decoder_rom_q[8];
  assign p_byterot_F_control = decoder_rom_q[7];
  assign p_sel_notb_x = decoder_rom_q[6];
  assign p_zero_all_b_control = decoder_rom_q[5];
  assign p_do_inv_all_b = decoder_rom_q[4];
  assign p_override_b_control = decoder_rom_q[3];
  assign p_op_uses_a_pre_alu = decoder_rom_q[2];
  assign p_sload_add_with_b_control = decoder_rom_q[1];
  assign p_do_byterot_1 = decoder_rom_q[0];

endmodule


module enet_nios_instruction_decoder (
                                       // inputs:
                                        clk,
                                        commit,
                                        instruction_0,
                                        instruction_1,
                                        instruction_2,
                                        instruction_3,
                                        is_cancelled,
                                        is_neutrino,
                                        pipe_run,
                                        reset_n,
                                        subinstruction_1,
                                        subinstruction_2,

                                       // outputs:
                                        N_update,
                                        V_update,
                                        a_field,
                                        a_index_from_o7,
                                        a_index_from_sp,
                                        a_index_from_zero,
                                        b_field,
                                        b_index_from_sp,
                                        b_index_from_zero,
                                        branch_offset,
                                        byte_offset,
                                        byterot_F_control,
                                        c_is_borrow,
                                        cancel_branch_delay_slot,
                                        custom_instruction_start_mul,
                                        dest_index_from_o7,
                                        dest_index_from_sp,
                                        dest_index_from_zero,
                                        do_byterot_1,
                                        do_custom_instruction,
                                        do_dynamic_narrow_write,
                                        do_fetch_vector,
                                        do_force_shift_0,
                                        do_force_shift_1,
                                        do_iABS,
                                        do_iASRx_1,
                                        do_iLDx,
                                        do_iMOVHI,
                                        do_iMOVI,
                                        do_iMSTEP_0,
                                        do_iMSTEP_1,
                                        do_iPFXx,
                                        do_iRDCTL,
                                        do_iRESTORE,
                                        do_iRLC_1,
                                        do_iRRC_1,
                                        do_iRxC_0,
                                        do_iRxC_1,
                                        do_iSAVE,
                                        do_iSEXT16,
                                        do_iSEXT8,
                                        do_iSKP0x,
                                        do_iSKP1x,
                                        do_iSKPRzx,
                                        do_iSKPS,
                                        do_iSKPx,
                                        do_iSTx,
                                        do_iSWAP,
                                        do_iTRAP_0,
                                        do_iTRAP_n,
                                        do_iTRET,
                                        do_iWRCTL,
                                        do_inv_all_b,
                                        do_narrow_stack_offset,
                                        do_normal_offset,
                                        do_normal_static_write,
                                        do_not_interrupt,
                                        do_save_offset,
                                        do_save_return_address,
                                        do_stack_offset,
                                        do_trap_offset,
                                        do_write_16,
                                        do_write_8,
                                        force_carryin,
                                        imm5,
                                        is_eightie,
                                        is_left_shift,
                                        is_right_shift,
                                        is_sixteenie,
                                        must_run_to_completion,
                                        narrow_stack_offset,
                                        non_write_op,
                                        op_b_from_2Ei5,
                                        op_b_from_Ki5,
                                        op_b_from_reg_b,
                                        op_b_from_reg_or_const,
                                        op_is_LDx,
                                        op_is_MOVHI,
                                        op_is_MSTEP_0,
                                        op_is_MSTEP_1,
                                        op_is_PFX,
                                        op_is_STx,
                                        op_is_TRAP_0,
                                        op_is_branch,
                                        op_is_jump,
                                        op_is_trap,
                                        op_jmpcall,
                                        op_save_restore,
                                        op_subroutine,
                                        op_uses_a_pre_alu,
                                        op_uses_b_pre_alu,
                                        override_b_control,
                                        p_field,
                                        prefix_payload,
                                        reg_not_modified,
                                        request_stable_op_b,
                                        save_offset,
                                        sel_dynamic_ext,
                                        sel_dynamic_ext16,
                                        sel_dynamic_ext8,
                                        sel_memword,
                                        sel_notb_x,
                                        skip_is_active,
                                        sload_add_with_b_control,
                                        sreset_add_0,
                                        sreset_add_x,
                                        stack_offset,
                                        test,
                                        trap_number,
                                        trap_offset,
                                        unqualified_custom_instruction_start_0,
                                        unqualified_custom_instruction_start_1,
                                        unqualified_custom_instruction_start_2,
                                        unqualified_custom_instruction_start_3,
                                        unqualified_custom_instruction_start_4,
                                        use_fresh_cwp,
                                        zero_all_b_control
                                     )
;

  output           N_update;
  output           V_update;
  output  [  4: 0] a_field;
  output           a_index_from_o7;
  output           a_index_from_sp;
  output           a_index_from_zero;
  output  [  4: 0] b_field;
  output           b_index_from_sp;
  output           b_index_from_zero;
  output  [ 10: 0] branch_offset;
  output  [  1: 0] byte_offset;
  output           byterot_F_control;
  output           c_is_borrow;
  output           cancel_branch_delay_slot;
  output           custom_instruction_start_mul;
  output           dest_index_from_o7;
  output           dest_index_from_sp;
  output           dest_index_from_zero;
  output           do_byterot_1;
  output           do_custom_instruction;
  output           do_dynamic_narrow_write;
  output           do_fetch_vector;
  output           do_force_shift_0;
  output           do_force_shift_1;
  output           do_iABS;
  output           do_iASRx_1;
  output           do_iLDx;
  output           do_iMOVHI;
  output           do_iMOVI;
  output           do_iMSTEP_0;
  output           do_iMSTEP_1;
  output           do_iPFXx;
  output           do_iRDCTL;
  output           do_iRESTORE;
  output           do_iRLC_1;
  output           do_iRRC_1;
  output           do_iRxC_0;
  output           do_iRxC_1;
  output           do_iSAVE;
  output           do_iSEXT16;
  output           do_iSEXT8;
  output           do_iSKP0x;
  output           do_iSKP1x;
  output           do_iSKPRzx;
  output           do_iSKPS;
  output           do_iSKPx;
  output           do_iSTx;
  output           do_iSWAP;
  output           do_iTRAP_0;
  output           do_iTRAP_n;
  output           do_iTRET;
  output           do_iWRCTL;
  output           do_inv_all_b;
  output           do_narrow_stack_offset;
  output           do_normal_offset;
  output           do_normal_static_write;
  output           do_not_interrupt;
  output           do_save_offset;
  output           do_save_return_address;
  output           do_stack_offset;
  output           do_trap_offset;
  output           do_write_16;
  output           do_write_8;
  output           force_carryin;
  output  [  4: 0] imm5;
  output           is_eightie;
  output           is_left_shift;
  output           is_right_shift;
  output           is_sixteenie;
  output           must_run_to_completion;
  output  [  9: 0] narrow_stack_offset;
  output           non_write_op;
  output           op_b_from_2Ei5;
  output           op_b_from_Ki5;
  output           op_b_from_reg_b;
  output           op_b_from_reg_or_const;
  output           op_is_LDx;
  output           op_is_MOVHI;
  output           op_is_MSTEP_0;
  output           op_is_MSTEP_1;
  output           op_is_PFX;
  output           op_is_STx;
  output           op_is_TRAP_0;
  output           op_is_branch;
  output           op_is_jump;
  output           op_is_trap;
  output           op_jmpcall;
  output           op_save_restore;
  output           op_subroutine;
  output           op_uses_a_pre_alu;
  output           op_uses_b_pre_alu;
  output           override_b_control;
  output  [  1: 0] p_field;
  output  [ 10: 0] prefix_payload;
  output           reg_not_modified;
  output           request_stable_op_b;
  output  [  7: 0] save_offset;
  output           sel_dynamic_ext;
  output           sel_dynamic_ext16;
  output           sel_dynamic_ext8;
  output           sel_memword;
  output           sel_notb_x;
  output           skip_is_active;
  output           sload_add_with_b_control;
  output           sreset_add_0;
  output           sreset_add_x;
  output  [  7: 0] stack_offset;
  output  [  3: 0] test;
  output  [  5: 0] trap_number;
  output  [  5: 0] trap_offset;
  output           unqualified_custom_instruction_start_0;
  output           unqualified_custom_instruction_start_1;
  output           unqualified_custom_instruction_start_2;
  output           unqualified_custom_instruction_start_3;
  output           unqualified_custom_instruction_start_4;
  output           use_fresh_cwp;
  output           zero_all_b_control;
  input            clk;
  input            commit;
  input   [ 15: 0] instruction_0;
  input   [ 15: 0] instruction_1;
  input   [ 15: 0] instruction_2;
  input   [ 15: 0] instruction_3;
  input            is_cancelled;
  input            is_neutrino;
  input            pipe_run;
  input            reset_n;
  input   [  5: 0] subinstruction_1;
  input   [  5: 0] subinstruction_2;

  wire             N_update;
  wire             V_update;
  wire    [  4: 0] a_field;
  wire             a_index_from_o7;
  wire             a_index_from_sp;
  wire             a_index_from_zero;
  wire    [  4: 0] b_field;
  wire             b_index_from_sp;
  wire             b_index_from_zero;
  wire    [ 10: 0] branch_offset;
  wire    [  1: 0] byte_offset;
  wire             byterot_F_control;
  wire             c_is_borrow;
  wire             cancel_branch_delay_slot;
  wire             custom_instruction_start_mul;
  wire             dest_index_from_o7;
  wire             dest_index_from_sp;
  wire             dest_index_from_zero;
  wire             do_byterot_1;
  wire             do_custom_instruction;
  wire             do_dynamic_narrow_write;
  wire             do_fetch_vector;
  wire             do_force_shift_0;
  wire             do_force_shift_1;
  wire             do_iABS;
  wire             do_iASRx_1;
  wire             do_iLDx;
  wire             do_iLDx_delayed_for_op_is_LDx;
  wire             do_iLDx_delayed_for_op_is_LDx_1;
  wire             do_iLDx_delayed_for_sel_memword;
  wire             do_iLDx_delayed_for_sel_memword_1;
  reg              do_iLDx_delayed_for_sel_memword_2;
  wire             do_iMOVHI;
  wire             do_iMOVHI_delayed_for_op_is_MOVHI;
  wire             do_iMOVHI_delayed_for_op_is_MOVHI_1;
  wire             do_iMOVI;
  wire             do_iMSTEP_0;
  wire             do_iMSTEP_0_delayed_for_do_force_shift_0;
  wire             do_iMSTEP_0_delayed_for_do_force_shift_0_2;
  wire             do_iMSTEP_0_delayed_for_op_is_MSTEP_0;
  wire             do_iMSTEP_0_delayed_for_op_is_MSTEP_0_2;
  wire             do_iMSTEP_1;
  wire             do_iMSTEP_1_delayed_for_op_is_MSTEP_1;
  wire             do_iMSTEP_1_delayed_for_op_is_MSTEP_1_2;
  wire             do_iPFXx;
  wire             do_iPFXx_delayed_for_op_is_PFX;
  wire             do_iPFXx_delayed_for_op_is_PFX_0;
  wire             do_iRDCTL;
  wire             do_iRESTORE;
  wire             do_iRLC_1;
  wire             do_iRRC_1;
  wire             do_iRxC_0;
  wire             do_iRxC_1;
  wire             do_iSAVE;
  wire             do_iSEXT16;
  wire             do_iSEXT8;
  wire             do_iSKP0x;
  wire             do_iSKP1x;
  wire             do_iSKPRzx;
  wire             do_iSKPS;
  wire             do_iSKPx;
  wire             do_iSKPx_delayed_for_must_run_to_completion;
  wire             do_iSKPx_delayed_for_must_run_to_completion_0;
  wire             do_iSTx;
  wire             do_iSTx_delayed_for_op_is_STx;
  wire             do_iSTx_delayed_for_op_is_STx_1;
  wire             do_iSWAP;
  wire             do_iTRAP_0;
  wire             do_iTRAP_0_delayed_for_op_is_TRAP_0;
  wire             do_iTRAP_0_delayed_for_op_is_TRAP_0_2;
  wire             do_iTRAP_0_delayed_for_sel_memword;
  wire             do_iTRAP_0_delayed_for_sel_memword_2;
  wire             do_iTRAP_n;
  wire             do_iTRAP_n_delayed_for_do_save_return_address;
  wire             do_iTRAP_n_delayed_for_do_save_return_address_2;
  wire             do_iTRET;
  wire             do_iWRCTL;
  wire             do_inv_all_b;
  wire             do_narrow_stack_offset;
  wire             do_normal_offset;
  wire             do_normal_static_write;
  wire             do_not_interrupt;
  wire             do_save_offset;
  wire             do_save_return_address;
  wire             do_stack_offset;
  wire             do_trap_offset;
  wire             do_trap_offset_delayed_for_do_fetch_vector;
  wire             do_trap_offset_delayed_for_do_fetch_vector_1;
  wire             do_write_16;
  wire             do_write_8;
  wire             force_carryin;
  wire    [  4: 0] imm5;
  wire             is_eightie;
  wire             is_left_shift;
  wire             is_right_shift;
  wire             is_sixteenie;
  wire    [  5: 0] major_opcode;
  wire    [  4: 0] minor_opcode;
  wire             must_run_to_completion;
  wire    [  9: 0] narrow_stack_offset;
  wire             non_write_op;
  wire             op_b_from_2Ei5;
  wire             op_b_from_Ki5;
  wire             op_b_from_reg_b;
  wire             op_b_from_reg_or_const;
  wire             op_is_LDx;
  wire             op_is_MOVHI;
  wire             op_is_MSTEP_0;
  wire             op_is_MSTEP_1;
  wire             op_is_PFX;
  wire             op_is_STx;
  wire             op_is_TRAP_0;
  wire             op_is_branch;
  wire             op_is_jump;
  wire             op_is_trap;
  wire             op_is_trap_delayed_for_use_fresh_cwp;
  wire             op_is_trap_delayed_for_use_fresh_cwp_1;
  reg              op_is_trap_delayed_for_use_fresh_cwp_2;
  reg              op_is_trap_delayed_for_use_fresh_cwp_3;
  wire             op_jmpcall;
  wire             op_jmpcall_delayed_for_op_is_jump;
  wire             op_jmpcall_delayed_for_op_is_jump_0;
  reg              op_jmpcall_delayed_for_op_is_jump_1;
  wire             op_save_restore;
  wire             op_subroutine;
  wire             op_subroutine_delayed_for_dest_index_from_o7;
  wire             op_subroutine_delayed_for_dest_index_from_o7_0;
  wire             op_subroutine_delayed_for_do_save_return_address;
  wire             op_subroutine_delayed_for_do_save_return_address_0;
  reg              op_subroutine_delayed_for_do_save_return_address_1;
  wire             op_subroutine_delayed_for_force_carryin;
  wire             op_subroutine_delayed_for_force_carryin_0;
  reg              op_subroutine_delayed_for_force_carryin_1;
  reg              op_subroutine_delayed_for_force_carryin_2;
  wire             op_uses_a_pre_alu;
  wire             op_uses_b_pre_alu;
  wire             override_b_control;
  reg              p1_cancel_branch_delay_slot;
  reg              p1_do_iLDx;
  reg              p1_do_iMOVHI;
  reg              p1_do_iRESTORE;
  reg              p1_do_iSAVE;
  reg              p1_do_iSTx;
  reg              p1_do_narrow_stack_offset;
  reg              p1_do_normal_static_write;
  reg              p1_do_save_offset;
  reg              p1_do_stack_offset;
  reg              p1_do_trap_offset;
  reg              p1_op_b_from_2Ei5;
  reg              p1_op_b_from_reg_or_const;
  reg              p1_op_is_PFX;
  reg              p1_op_is_branch;
  reg              p1_op_is_trap;
  reg              p2_byterot_F_control;
  reg              p2_do_byterot_1;
  reg              p2_do_custom_instruction;
  reg              p2_do_dynamic_narrow_write;
  reg              p2_do_fetch_vector;
  reg              p2_do_force_shift_0;
  reg              p2_do_force_shift_1;
  reg              p2_do_iMOVI;
  reg              p2_do_iMSTEP_0;
  reg              p2_do_iMSTEP_1;
  reg              p2_do_iSWAP;
  reg              p2_do_iTRAP_0;
  reg              p2_do_iTRAP_n;
  reg              p2_do_inv_all_b;
  reg              p2_do_save_return_address;
  reg              p2_do_write_16;
  reg              p2_do_write_8;
  reg              p2_is_eightie;
  reg              p2_is_sixteenie;
  reg              p2_op_is_LDx;
  reg              p2_op_is_MOVHI;
  reg              p2_op_is_STx;
  reg              p2_op_is_jump;
  reg              p2_request_stable_op_b;
  reg              p2_sel_notb_x;
  reg              p2_sload_add_with_b_control;
  reg              p2_sreset_add_x;
  reg              p2_zero_all_b_control;
  reg              p3_N_update;
  reg              p3_V_update;
  reg              p3_c_is_borrow;
  reg              p3_custom_instruction_start_mul;
  reg              p3_do_custom_instruction;
  reg              p3_do_iABS;
  reg              p3_do_iASRx_1;
  reg              p3_do_iRDCTL;
  reg              p3_do_iRLC_1;
  reg              p3_do_iRRC_1;
  reg              p3_do_iRxC_0;
  reg              p3_do_iRxC_1;
  reg              p3_do_iSEXT16;
  reg              p3_do_iSEXT8;
  reg              p3_do_iSKP0x;
  reg              p3_do_iSKP1x;
  reg              p3_do_iSKPRzx;
  reg              p3_do_iSKPS;
  reg              p3_do_iTRET;
  reg              p3_do_iWRCTL;
  reg              p3_force_carryin;
  reg              p3_is_left_shift;
  reg              p3_is_right_shift;
  reg              p3_op_is_MSTEP_0;
  reg              p3_op_is_MSTEP_1;
  reg              p3_op_is_TRAP_0;
  reg              p3_sel_dynamic_ext;
  reg              p3_sel_dynamic_ext16;
  reg              p3_sel_dynamic_ext8;
  reg              p3_sel_memword;
  reg              p3_sel_notb_x;
  reg              p3_skip_is_active;
  reg              p3_sload_add_with_b_control;
  reg              p3_sreset_add_0;
  reg              p3_sreset_add_x;
  reg              p3_unqualified_custom_instruction_start_0;
  reg              p3_unqualified_custom_instruction_start_1;
  reg              p3_unqualified_custom_instruction_start_2;
  reg              p3_unqualified_custom_instruction_start_3;
  reg              p3_unqualified_custom_instruction_start_4;
  reg              p4_non_write_op;
  reg              p4_use_fresh_cwp;
  wire             p_N_update;
  wire             p_V_update;
  wire             p_a_index_from_o7;
  wire             p_a_index_from_sp;
  wire             p_a_index_from_zero;
  wire             p_b_index_from_sp;
  wire             p_b_index_from_zero;
  wire             p_byterot_F_control;
  wire             p_c_is_borrow;
  wire             p_cancel_branch_delay_slot;
  wire             p_custom_instruction_start_mul;
  wire             p_dest_index_from_o7;
  wire             p_dest_index_from_sp;
  wire             p_dest_index_from_zero;
  wire             p_do_byterot_1;
  wire             p_do_custom_instruction;
  wire             p_do_dynamic_narrow_write;
  wire             p_do_fetch_vector;
  wire             p_do_force_shift_0;
  wire             p_do_force_shift_1;
  wire             p_do_iABS;
  wire             p_do_iASRx_1;
  wire             p_do_iLDx;
  wire             p_do_iMOVHI;
  wire             p_do_iMOVI;
  wire             p_do_iMSTEP_0;
  wire             p_do_iMSTEP_1;
  wire             p_do_iPFXx;
  wire             p_do_iRDCTL;
  wire             p_do_iRESTORE;
  wire             p_do_iRLC_1;
  wire             p_do_iRRC_1;
  wire             p_do_iRxC_0;
  wire             p_do_iRxC_1;
  wire             p_do_iSAVE;
  wire             p_do_iSEXT16;
  wire             p_do_iSEXT8;
  wire             p_do_iSKP0x;
  wire             p_do_iSKP1x;
  wire             p_do_iSKPRzx;
  wire             p_do_iSKPS;
  wire             p_do_iSKPx;
  wire             p_do_iSTx;
  wire             p_do_iSWAP;
  wire             p_do_iTRAP_0;
  wire             p_do_iTRAP_n;
  wire             p_do_iTRET;
  wire             p_do_iWRCTL;
  wire             p_do_inv_all_b;
  wire             p_do_narrow_stack_offset;
  wire             p_do_normal_offset;
  wire             p_do_normal_static_write;
  wire             p_do_not_interrupt;
  wire             p_do_save_offset;
  wire             p_do_save_return_address;
  wire             p_do_stack_offset;
  wire             p_do_trap_offset;
  wire             p_do_write_16;
  wire             p_do_write_8;
  wire    [  1: 0] p_field;
  wire             p_force_carryin;
  wire             p_is_eightie;
  wire             p_is_left_shift;
  wire             p_is_right_shift;
  wire             p_is_sixteenie;
  wire             p_must_run_to_completion;
  wire             p_non_write_op;
  wire             p_op_b_from_2Ei5;
  wire             p_op_b_from_Ki5;
  wire             p_op_b_from_reg_b;
  wire             p_op_b_from_reg_or_const;
  wire             p_op_is_LDx;
  wire             p_op_is_MOVHI;
  wire             p_op_is_MSTEP_0;
  wire             p_op_is_MSTEP_1;
  wire             p_op_is_PFX;
  wire             p_op_is_STx;
  wire             p_op_is_TRAP_0;
  wire             p_op_is_branch;
  wire             p_op_is_jump;
  wire             p_op_is_trap;
  wire             p_op_jmpcall;
  wire             p_op_save_restore;
  wire             p_op_subroutine;
  wire             p_op_uses_a_pre_alu;
  wire             p_op_uses_b_pre_alu;
  wire             p_override_b_control;
  wire             p_reg_not_modified;
  wire             p_request_stable_op_b;
  wire             p_sel_dynamic_ext;
  wire             p_sel_dynamic_ext16;
  wire             p_sel_dynamic_ext8;
  wire             p_sel_memword;
  wire             p_sel_notb_x;
  wire             p_skip_is_active;
  wire             p_sload_add_with_b_control;
  wire             p_sreset_add_0;
  wire             p_sreset_add_x;
  wire             p_unqualified_custom_instruction_start_0;
  wire             p_unqualified_custom_instruction_start_1;
  wire             p_unqualified_custom_instruction_start_2;
  wire             p_unqualified_custom_instruction_start_3;
  wire             p_unqualified_custom_instruction_start_4;
  wire             p_use_fresh_cwp;
  wire             p_zero_all_b_control;
  wire    [ 10: 0] prefix_payload;
  wire             reg_not_modified;
  wire             reg_not_modified_delayed_for_non_write_op;
  wire             reg_not_modified_delayed_for_non_write_op_1;
  reg              reg_not_modified_delayed_for_non_write_op_2;
  reg              reg_not_modified_delayed_for_non_write_op_3;
  wire             request_stable_op_b;
  wire    [  7: 0] save_offset;
  wire             sel_dynamic_ext;
  wire             sel_dynamic_ext16;
  wire             sel_dynamic_ext8;
  wire             sel_memword;
  wire             sel_notb_x;
  wire             skip_is_active;
  wire             sload_add_with_b_control;
  wire             sreset_add_0;
  wire             sreset_add_x;
  wire    [  7: 0] stack_offset;
  wire    [  3: 0] test;
  wire    [  5: 0] trap_number;
  wire    [  5: 0] trap_offset;
  wire             unqualified_custom_instruction_start_0;
  wire             unqualified_custom_instruction_start_1;
  wire             unqualified_custom_instruction_start_2;
  wire             unqualified_custom_instruction_start_3;
  wire             unqualified_custom_instruction_start_4;
  wire             use_fresh_cwp;
  wire             zero_all_b_control;
  enet_nios_rom_decoder_unit the_enet_nios_rom_decoder_unit
    (
      .clk                        (clk),
      .commit                     (commit),
      .is_cancelled               (is_cancelled),
      .is_neutrino                (is_neutrino),
      .major_opcode               (major_opcode),
      .minor_opcode               (minor_opcode),
      .p_byterot_F_control        (p_byterot_F_control),
      .p_do_byterot_1             (p_do_byterot_1),
      .p_do_custom_instruction    (p_do_custom_instruction),
      .p_do_inv_all_b             (p_do_inv_all_b),
      .p_do_normal_offset         (p_do_normal_offset),
      .p_do_not_interrupt         (p_do_not_interrupt),
      .p_op_b_from_Ki5            (p_op_b_from_Ki5),
      .p_op_b_from_reg_b          (p_op_b_from_reg_b),
      .p_op_uses_a_pre_alu        (p_op_uses_a_pre_alu),
      .p_op_uses_b_pre_alu        (p_op_uses_b_pre_alu),
      .p_override_b_control       (p_override_b_control),
      .p_reg_not_modified         (p_reg_not_modified),
      .p_sel_notb_x               (p_sel_notb_x),
      .p_sload_add_with_b_control (p_sload_add_with_b_control),
      .p_sreset_add_x             (p_sreset_add_x),
      .p_zero_all_b_control       (p_zero_all_b_control)
    );

  assign p_op_b_from_2Ei5 = 1'b0 ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] && ~instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_op_b_from_2Ei5 <= 0;
      else if (commit)
          p1_op_b_from_2Ei5 <= p_op_b_from_2Ei5;
    end


  //Control-bit op_b_from_2Ei5, set by: , SKP1_0, SKP1_1, BGEN, SKP0_0, SKP0_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 5 of 126 mnemonics, stage=1

  assign op_b_from_2Ei5 = p1_op_b_from_2Ei5;

  assign p_dest_index_from_zero = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7] &&  instruction_0[6] && ~instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7] &&  instruction_0[6] &&  instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]);

  //Control-bit dest_index_from_zero, set by: , FILL8, FILL16, MUL_0, MUL_1, MUL_2, 
  //Just one of 104 total control bits.
  //, 
  //Matches 5 of 126 mnemonics, stage=0

  assign dest_index_from_zero = p_dest_index_from_zero;

  assign reg_not_modified_delayed_for_non_write_op_1 = reg_not_modified;
  assign reg_not_modified_delayed_for_non_write_op = reg_not_modified_delayed_for_non_write_op_3;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          reg_not_modified_delayed_for_non_write_op_2 <= 0;
      else if (pipe_run)
          reg_not_modified_delayed_for_non_write_op_2 <= reg_not_modified_delayed_for_non_write_op_1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          reg_not_modified_delayed_for_non_write_op_3 <= 0;
      else if (pipe_run)
          reg_not_modified_delayed_for_non_write_op_3 <= reg_not_modified_delayed_for_non_write_op_2;
    end


  assign p_non_write_op = 1'b0 ||
    reg_not_modified_delayed_for_non_write_op;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p4_non_write_op <= 0;
      else if (pipe_run)
          p4_non_write_op <= p_non_write_op;
    end


  //Control-bit non_write_op, set by: , reg_not_modified_delayed_for_non_write_op, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=4

  assign non_write_op = p4_non_write_op;

  assign p_do_narrow_stack_offset = 1'b0 ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_narrow_stack_offset <= 0;
      else if (commit)
          p1_do_narrow_stack_offset <= p_do_narrow_stack_offset;
    end


  //Control-bit do_narrow_stack_offset, set by: , STS8s, STS16s, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=1

  assign do_narrow_stack_offset = p1_do_narrow_stack_offset;

  assign p_unqualified_custom_instruction_start_0 = 1'b0 ||
    (~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] && ~instruction_2[10] && ~subinstruction_2[0] &&  subinstruction_2[1] && ~subinstruction_2[2] && ~subinstruction_2[3] && ~subinstruction_2[4] &&  subinstruction_2[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_unqualified_custom_instruction_start_0 <= 0;
      else if (pipe_run)
          p3_unqualified_custom_instruction_start_0 <= p_unqualified_custom_instruction_start_0;
    end


  //Control-bit unqualified_custom_instruction_start_0, set by: , USR0_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign unqualified_custom_instruction_start_0 = p3_unqualified_custom_instruction_start_0;

  assign p_unqualified_custom_instruction_start_1 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_unqualified_custom_instruction_start_1 <= 0;
      else if (pipe_run)
          p3_unqualified_custom_instruction_start_1 <= p_unqualified_custom_instruction_start_1;
    end


  //Control-bit unqualified_custom_instruction_start_1, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign unqualified_custom_instruction_start_1 = p3_unqualified_custom_instruction_start_1;

  assign p_unqualified_custom_instruction_start_2 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_unqualified_custom_instruction_start_2 <= 0;
      else if (pipe_run)
          p3_unqualified_custom_instruction_start_2 <= p_unqualified_custom_instruction_start_2;
    end


  //Control-bit unqualified_custom_instruction_start_2, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign unqualified_custom_instruction_start_2 = p3_unqualified_custom_instruction_start_2;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_sload_add_with_b_control <= 0;
      else if (pipe_run)
          p2_sload_add_with_b_control <= p_sload_add_with_b_control;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_sload_add_with_b_control <= 0;
      else if (pipe_run)
          p3_sload_add_with_b_control <= p2_sload_add_with_b_control;
    end


  //Control-bit sload_add_with_b_control, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign sload_add_with_b_control = p3_sload_add_with_b_control;

  assign p_unqualified_custom_instruction_start_3 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_unqualified_custom_instruction_start_3 <= 0;
      else if (pipe_run)
          p3_unqualified_custom_instruction_start_3 <= p_unqualified_custom_instruction_start_3;
    end


  //Control-bit unqualified_custom_instruction_start_3, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign unqualified_custom_instruction_start_3 = p3_unqualified_custom_instruction_start_3;

  assign p_unqualified_custom_instruction_start_4 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_unqualified_custom_instruction_start_4 <= 0;
      else if (pipe_run)
          p3_unqualified_custom_instruction_start_4 <= p_unqualified_custom_instruction_start_4;
    end


  //Control-bit unqualified_custom_instruction_start_4, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign unqualified_custom_instruction_start_4 = p3_unqualified_custom_instruction_start_4;

  assign p_b_index_from_sp = 1'b0 ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) ||
    ( instruction_0[15] &&  instruction_0[14] && ~instruction_0[13]) ||
    ( instruction_0[15] &&  instruction_0[14] &&  instruction_0[13]);

  //Control-bit b_index_from_sp, set by: , STS8s, STS16s, STS, LDS, 
  //Just one of 104 total control bits.
  //, 
  //Matches 4 of 126 mnemonics, stage=0

  assign b_index_from_sp = p_b_index_from_sp;

  assign p_a_index_from_zero = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7] && ~instruction_0[6] && ~instruction_0[5]) ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]) ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]);

  //Control-bit a_index_from_zero, set by: , ST16s, ST16d, ST8s, ST8d, STS8s, MUL_0, MUL_1, MUL_2, STS16s, 
  //Just one of 104 total control bits.
  //, 
  //Matches 9 of 126 mnemonics, stage=0

  assign a_index_from_zero = p_a_index_from_zero;

  assign p_do_iRxC_0 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iRxC_0 <= 0;
      else if (pipe_run)
          p3_do_iRxC_0 <= p_do_iRxC_0;
    end


  //Control-bit do_iRxC_0, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign do_iRxC_0 = p3_do_iRxC_0;

  assign p_do_iRxC_1 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iRxC_1 <= 0;
      else if (pipe_run)
          p3_do_iRxC_1 <= p_do_iRxC_1;
    end


  //Control-bit do_iRxC_1, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign do_iRxC_1 = p3_do_iRxC_1;

  assign p_is_right_shift = 1'b0 ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10] &&  subinstruction_2[0]) ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10] &&  subinstruction_2[0]) ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] && ~instruction_2[11] && ~instruction_2[10] &&  subinstruction_2[0]) ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] && ~instruction_2[11] &&  instruction_2[10] &&  subinstruction_2[0]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_is_right_shift <= 0;
      else if (pipe_run)
          p3_is_right_shift <= p_is_right_shift;
    end


  //Control-bit is_right_shift, set by: , ASR_0, ASRI_0, LSR_0, LSRI_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 4 of 126 mnemonics, stage=3

  assign is_right_shift = p3_is_right_shift;

  assign op_subroutine_delayed_for_dest_index_from_o7_0 = p_op_subroutine;
  assign op_subroutine_delayed_for_dest_index_from_o7 = op_subroutine_delayed_for_dest_index_from_o7_0;
  assign p_dest_index_from_o7 = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    op_subroutine_delayed_for_dest_index_from_o7;

  //Control-bit dest_index_from_o7, set by: , TRAP_0, TRAP_1, TRAP_2, TRAP_3, op_subroutine_delayed_for_dest_index_from_o7, 
  //Just one of 104 total control bits.
  //, 
  //Matches 4 of 126 mnemonics, stage=0

  assign dest_index_from_o7 = p_dest_index_from_o7;

  assign p_op_jmpcall = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] &&  instruction_0[8] &&  instruction_0[7] &&  instruction_0[6] &&  instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] &&  instruction_0[8] &&  instruction_0[7] &&  instruction_0[6] && ~instruction_0[5]);

  //Control-bit op_jmpcall, set by: , CALL, JMP, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=0

  assign op_jmpcall = p_op_jmpcall;

  //Control-bit reg_not_modified, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign reg_not_modified = p_reg_not_modified;

  assign p_do_iSKP1x = 1'b0 ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10] &&  subinstruction_2[0]) ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10] && ~subinstruction_2[0]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iSKP1x <= 0;
      else if (pipe_run)
          p3_do_iSKP1x <= p_do_iSKP1x;
    end


  //Control-bit do_iSKP1x, set by: , SKP1_0, SKP1_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=3

  assign do_iSKP1x = p3_do_iSKP1x;

  assign p_op_subroutine = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] &&  instruction_0[8] &&  instruction_0[7] &&  instruction_0[6] &&  instruction_0[5]) ||
    ( instruction_0[15] && ~instruction_0[14] && ~instruction_0[13] && ~instruction_0[12] &&  instruction_0[11]);

  //Control-bit op_subroutine, set by: , CALL, BSR, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=0

  assign op_subroutine = p_op_subroutine;

  assign op_subroutine_delayed_for_force_carryin_0 = op_subroutine;
  assign op_subroutine_delayed_for_force_carryin = op_subroutine_delayed_for_force_carryin_2;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_subroutine_delayed_for_force_carryin_1 <= 0;
      else if (commit)
          op_subroutine_delayed_for_force_carryin_1 <= op_subroutine_delayed_for_force_carryin_0;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_subroutine_delayed_for_force_carryin_2 <= 0;
      else if (pipe_run)
          op_subroutine_delayed_for_force_carryin_2 <= op_subroutine_delayed_for_force_carryin_1;
    end


  assign p_force_carryin = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] && ~instruction_2[10]) && ~instruction_2[9] && ~instruction_2[8]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) ||
    op_subroutine_delayed_for_force_carryin ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_force_carryin <= 0;
      else if (pipe_run)
          p3_force_carryin <= p_force_carryin;
    end


  //Control-bit force_carryin, set by: , SAVE, SUBI, op_subroutine_delayed_for_force_carryin, SUB, CMP, CMPI, 
  //Just one of 104 total control bits.
  //, 
  //Matches 5 of 126 mnemonics, stage=3

  assign force_carryin = p3_force_carryin;

  assign p_do_iSEXT16 = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) && ~instruction_2[9] && ~instruction_2[8] &&  instruction_2[7] && ~instruction_2[6] && ~instruction_2[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iSEXT16 <= 0;
      else if (pipe_run)
          p3_do_iSEXT16 <= p_do_iSEXT16;
    end


  //Control-bit do_iSEXT16, set by: , SEXT16, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign do_iSEXT16 = p3_do_iSEXT16;

  assign p_do_iRESTORE = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8] &&  instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_iRESTORE <= 0;
      else if (commit)
          p1_do_iRESTORE <= p_do_iRESTORE;
    end


  //Control-bit do_iRESTORE, set by: , RESTORE, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=1

  assign do_iRESTORE = p1_do_iRESTORE;

  //Control-bit op_b_from_reg_b, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign op_b_from_reg_b = p_op_b_from_reg_b;

  assign do_iPFXx_delayed_for_op_is_PFX_0 = do_iPFXx;
  assign do_iPFXx_delayed_for_op_is_PFX = do_iPFXx_delayed_for_op_is_PFX_0;
  assign p_op_is_PFX = 1'b0 ||
    do_iPFXx_delayed_for_op_is_PFX;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_op_is_PFX <= 0;
      else if (commit)
          p1_op_is_PFX <= p_op_is_PFX;
    end


  //Control-bit op_is_PFX, set by: , do_iPFXx_delayed_for_op_is_PFX, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign op_is_PFX = p1_op_is_PFX;

  assign do_trap_offset_delayed_for_do_fetch_vector_1 = do_trap_offset;
  assign do_trap_offset_delayed_for_do_fetch_vector = do_trap_offset_delayed_for_do_fetch_vector_1;
  assign p_do_fetch_vector = 1'b0 ||
    do_trap_offset_delayed_for_do_fetch_vector;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_fetch_vector <= 0;
      else if (pipe_run)
          p2_do_fetch_vector <= p_do_fetch_vector;
    end


  //Control-bit do_fetch_vector, set by: , do_trap_offset_delayed_for_do_fetch_vector, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign do_fetch_vector = p2_do_fetch_vector;

  assign p_a_index_from_o7 = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]);

  //Control-bit a_index_from_o7, set by: , TRAP_3, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=0

  assign a_index_from_o7 = p_a_index_from_o7;

  //Control-bit do_normal_offset, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign do_normal_offset = p_do_normal_offset;

  assign p_b_index_from_zero = 1'b0;
  //Control-bit b_index_from_zero, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=0

  assign b_index_from_zero = p_b_index_from_zero;

  assign p_do_iASRx_1 = 1'b0 ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10] && ~subinstruction_2[0]) ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10] && ~subinstruction_2[0]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iASRx_1 <= 0;
      else if (pipe_run)
          p3_do_iASRx_1 <= p_do_iASRx_1;
    end


  //Control-bit do_iASRx_1, set by: , ASR_1, ASRI_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=3

  assign do_iASRx_1 = p3_do_iASRx_1;

  assign p_do_iSAVE = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] && ~instruction_0[8]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_iSAVE <= 0;
      else if (commit)
          p1_do_iSAVE <= p_do_iSAVE;
    end


  //Control-bit do_iSAVE, set by: , SAVE, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=1

  assign do_iSAVE = p1_do_iSAVE;

  assign p_do_save_offset = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] && ~instruction_0[8]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_save_offset <= 0;
      else if (commit)
          p1_do_save_offset <= p_do_save_offset;
    end


  //Control-bit do_save_offset, set by: , SAVE, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=1

  assign do_save_offset = p1_do_save_offset;

  //Control-bit op_uses_a_pre_alu, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign op_uses_a_pre_alu = p_op_uses_a_pre_alu;

  assign p_is_eightie = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] && ~instruction_1[11] &&  instruction_1[10]) && ~instruction_1[9] && ~instruction_1[8] && ~instruction_1[7]) ||
    (~instruction_1[15] &&  instruction_1[14] && ~instruction_1[13] && ~instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) && ~instruction_1[9] && ~instruction_1[8] && ~instruction_1[7] &&  instruction_1[6] &&  instruction_1[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_is_eightie <= 0;
      else if (pipe_run)
          p2_is_eightie <= p_is_eightie;
    end


  //Control-bit is_eightie, set by: , EXT8s, EXT8d, SEXT8, 
  //Just one of 104 total control bits.
  //, 
  //Matches 3 of 126 mnemonics, stage=2

  assign is_eightie = p2_is_eightie;

  assign do_iSKPx_delayed_for_must_run_to_completion_0 = p_do_iSKPx;
  assign do_iSKPx_delayed_for_must_run_to_completion = do_iSKPx_delayed_for_must_run_to_completion_0;
  assign p_must_run_to_completion = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] &&  instruction_0[8] && ~instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]) ||
    do_iSKPx_delayed_for_must_run_to_completion ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] &&  instruction_0[8] && ~instruction_0[7] && ~instruction_0[6] && ~instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8] &&  instruction_0[7] &&  instruction_0[6] && ~instruction_0[5]);

  //Control-bit must_run_to_completion, set by: , RDCTL, do_iSKPx_delayed_for_must_run_to_completion, TRAP_0, TRAP_1, TRAP_2, TRAP_3, WRCTL, TRET, 
  //Just one of 104 total control bits.
  //, 
  //Matches 7 of 126 mnemonics, stage=0

  assign must_run_to_completion = p_must_run_to_completion;

  assign p_N_update = 1'b0 ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] && ~instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]) ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] && ~instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]) ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_N_update <= 0;
      else if (pipe_run)
          p3_N_update <= p_N_update;
    end


  //Control-bit N_update, set by: , AND, ANDN, SUB, SUBI, ADD, ADDI, OR, CMP, CMPI, XOR, 
  //Just one of 104 total control bits.
  //, 
  //Matches 10 of 126 mnemonics, stage=3

  assign N_update = p3_N_update;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_sreset_add_x <= 0;
      else if (pipe_run)
          p2_sreset_add_x <= p_sreset_add_x;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_sreset_add_x <= 0;
      else if (pipe_run)
          p3_sreset_add_x <= p2_sreset_add_x;
    end


  //Control-bit sreset_add_x, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign sreset_add_x = p3_sreset_add_x;

  //Control-bit op_uses_b_pre_alu, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign op_uses_b_pre_alu = p_op_uses_b_pre_alu;

  assign p_do_iTRAP_n = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] && ~instruction_1[10]) && ~instruction_1[9] &&  instruction_1[8] && ~subinstruction_1[0] && ~subinstruction_1[1]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_iTRAP_n <= 0;
      else if (pipe_run)
          p2_do_iTRAP_n <= p_do_iTRAP_n;
    end


  //Control-bit do_iTRAP_n, set by: , TRAP_3, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=2

  assign do_iTRAP_n = p2_do_iTRAP_n;

  assign do_iMSTEP_0_delayed_for_do_force_shift_0_2 = p_do_iMSTEP_0;
  assign do_iMSTEP_0_delayed_for_do_force_shift_0 = do_iMSTEP_0_delayed_for_do_force_shift_0_2;
  assign p_do_force_shift_0 = 1'b0 ||
    do_iMSTEP_0_delayed_for_do_force_shift_0;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_force_shift_0 <= 0;
      else if (pipe_run)
          p2_do_force_shift_0 <= p_do_force_shift_0;
    end


  //Control-bit do_force_shift_0, set by: , do_iMSTEP_0_delayed_for_do_force_shift_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign do_force_shift_0 = p2_do_force_shift_0;

  assign p_do_force_shift_1 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_force_shift_1 <= 0;
      else if (pipe_run)
          p2_do_force_shift_1 <= p_do_force_shift_1;
    end


  //Control-bit do_force_shift_1, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign do_force_shift_1 = p2_do_force_shift_1;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_custom_instruction <= 0;
      else if (pipe_run)
          p2_do_custom_instruction <= p_do_custom_instruction;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_custom_instruction <= 0;
      else if (pipe_run)
          p3_do_custom_instruction <= p2_do_custom_instruction;
    end


  //Control-bit do_custom_instruction, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign do_custom_instruction = p3_do_custom_instruction;

  assign p_cancel_branch_delay_slot = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8] &&  instruction_0[7] &&  instruction_0[6] && ~instruction_0[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_cancel_branch_delay_slot <= 0;
      else if (commit)
          p1_cancel_branch_delay_slot <= p_cancel_branch_delay_slot;
    end


  //Control-bit cancel_branch_delay_slot, set by: , TRAP_0, TRAP_1, TRAP_2, TRAP_3, TRET, 
  //Just one of 104 total control bits.
  //, 
  //Matches 5 of 126 mnemonics, stage=1

  assign cancel_branch_delay_slot = p1_cancel_branch_delay_slot;

  assign p_op_save_restore = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] && ~instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8] &&  instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]);

  //Control-bit op_save_restore, set by: , SAVE, RESTORE, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=0

  assign op_save_restore = p_op_save_restore;

  assign p_request_stable_op_b = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_request_stable_op_b <= 0;
      else if (pipe_run)
          p2_request_stable_op_b <= p_request_stable_op_b;
    end


  //Control-bit request_stable_op_b, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign request_stable_op_b = p2_request_stable_op_b;

  assign do_iMOVHI_delayed_for_op_is_MOVHI_1 = do_iMOVHI;
  assign do_iMOVHI_delayed_for_op_is_MOVHI = do_iMOVHI_delayed_for_op_is_MOVHI_1;
  assign p_op_is_MOVHI = 1'b0 ||
    do_iMOVHI_delayed_for_op_is_MOVHI;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_op_is_MOVHI <= 0;
      else if (pipe_run)
          p2_op_is_MOVHI <= p_op_is_MOVHI;
    end


  //Control-bit op_is_MOVHI, set by: , do_iMOVHI_delayed_for_op_is_MOVHI, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign op_is_MOVHI = p2_op_is_MOVHI;

  assign p_do_iMOVHI = 1'b0 ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_iMOVHI <= 0;
      else if (commit)
          p1_do_iMOVHI <= p_do_iMOVHI;
    end


  //Control-bit do_iMOVHI, set by: , MOVHI, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=1

  assign do_iMOVHI = p1_do_iMOVHI;

  assign do_iMSTEP_0_delayed_for_op_is_MSTEP_0_2 = do_iMSTEP_0;
  assign do_iMSTEP_0_delayed_for_op_is_MSTEP_0 = do_iMSTEP_0_delayed_for_op_is_MSTEP_0_2;
  assign p_op_is_MSTEP_0 = 1'b0 ||
    do_iMSTEP_0_delayed_for_op_is_MSTEP_0;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_op_is_MSTEP_0 <= 0;
      else if (pipe_run)
          p3_op_is_MSTEP_0 <= p_op_is_MSTEP_0;
    end


  //Control-bit op_is_MSTEP_0, set by: , do_iMSTEP_0_delayed_for_op_is_MSTEP_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign op_is_MSTEP_0 = p3_op_is_MSTEP_0;

  assign p_is_sixteenie = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] && ~instruction_1[11] &&  instruction_1[10]) && ~instruction_1[9] && ~instruction_1[8] &&  instruction_1[7]) ||
    (~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] && ~instruction_1[12] &&  instruction_1[11] && ~instruction_1[10]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) && ~instruction_1[9] && ~instruction_1[8] &&  instruction_1[7] && ~instruction_1[6] && ~instruction_1[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_is_sixteenie <= 0;
      else if (pipe_run)
          p2_is_sixteenie <= p_is_sixteenie;
    end


  //Control-bit is_sixteenie, set by: , EXT16s, EXT16d, SEXT16, 
  //Just one of 104 total control bits.
  //, 
  //Matches 3 of 126 mnemonics, stage=2

  assign is_sixteenie = p2_is_sixteenie;

  assign do_iMSTEP_1_delayed_for_op_is_MSTEP_1_2 = do_iMSTEP_1;
  assign do_iMSTEP_1_delayed_for_op_is_MSTEP_1 = do_iMSTEP_1_delayed_for_op_is_MSTEP_1_2;
  assign p_op_is_MSTEP_1 = 1'b0 ||
    do_iMSTEP_1_delayed_for_op_is_MSTEP_1;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_op_is_MSTEP_1 <= 0;
      else if (pipe_run)
          p3_op_is_MSTEP_1 <= p_op_is_MSTEP_1;
    end


  //Control-bit op_is_MSTEP_1, set by: , do_iMSTEP_1_delayed_for_op_is_MSTEP_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign op_is_MSTEP_1 = p3_op_is_MSTEP_1;

  assign p_sel_dynamic_ext = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] && ~instruction_2[7] &&  instruction_2[6] && ~instruction_2[5]) ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] && ~instruction_2[7] &&  instruction_2[6] &&  instruction_2[5]) ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]) && ~instruction_2[9] && ~instruction_2[8] && ~instruction_2[7]) ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]) && ~instruction_2[9] && ~instruction_2[8] &&  instruction_2[7]) ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) ||
    (~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_sel_dynamic_ext <= 0;
      else if (pipe_run)
          p3_sel_dynamic_ext <= p_sel_dynamic_ext;
    end


  //Control-bit sel_dynamic_ext, set by: , FILL8, FILL16, EXT8s, EXT16s, EXT8d, EXT16d, 
  //Just one of 104 total control bits.
  //, 
  //Matches 6 of 126 mnemonics, stage=3

  assign sel_dynamic_ext = p3_sel_dynamic_ext;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_byterot_F_control <= 0;
      else if (pipe_run)
          p2_byterot_F_control <= p_byterot_F_control;
    end


  //Control-bit byterot_F_control, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign byterot_F_control = p2_byterot_F_control;

  assign p_do_iMSTEP_0 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_iMSTEP_0 <= 0;
      else if (pipe_run)
          p2_do_iMSTEP_0 <= p_do_iMSTEP_0;
    end


  //Control-bit do_iMSTEP_0, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign do_iMSTEP_0 = p2_do_iMSTEP_0;

  assign p_do_iABS = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) && ~instruction_2[9] && ~instruction_2[8] && ~instruction_2[7] &&  instruction_2[6] && ~instruction_2[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iABS <= 0;
      else if (pipe_run)
          p3_do_iABS <= p_do_iABS;
    end


  //Control-bit do_iABS, set by: , ABS, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign do_iABS = p3_do_iABS;

  assign p_do_iPFXx = 1'b0 ||
    ( instruction_0[15] && ~instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11]) ||
    ( instruction_0[15] && ~instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] &&  instruction_0[11]);

  //Control-bit do_iPFXx, set by: , PFXIO, PFX, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=0

  assign do_iPFXx = p_do_iPFXx;

  assign p_do_iMSTEP_1 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_iMSTEP_1 <= 0;
      else if (pipe_run)
          p2_do_iMSTEP_1 <= p_do_iMSTEP_1;
    end


  //Control-bit do_iMSTEP_1, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign do_iMSTEP_1 = p2_do_iMSTEP_1;

  assign p_do_iSTx = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7] && ~instruction_0[6] && ~instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7] && ~instruction_0[6] &&  instruction_0[5]) ||
    ( instruction_0[15] && ~instruction_0[14] &&  instruction_0[13] && ~instruction_0[12]) ||
    ( instruction_0[15] &&  instruction_0[14] && ~instruction_0[13]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_iSTx <= 0;
      else if (commit)
          p1_do_iSTx <= p_do_iSTx;
    end


  //Control-bit do_iSTx, set by: , ST8s, ST16s, ST, STS8s, STS16s, ST8d, ST16d, STP, STS, 
  //Just one of 104 total control bits.
  //, 
  //Matches 9 of 126 mnemonics, stage=1

  assign do_iSTx = p1_do_iSTx;

  assign p_do_normal_static_write = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] && ~instruction_0[7]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_normal_static_write <= 0;
      else if (commit)
          p1_do_normal_static_write <= p_do_normal_static_write;
    end


  //Control-bit do_normal_static_write, set by: , ST16s, ST8s, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=1

  assign do_normal_static_write = p1_do_normal_static_write;

  assign p_do_iSKPS = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] &&  instruction_2[7] &&  instruction_2[6] &&  instruction_2[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iSKPS <= 0;
      else if (pipe_run)
          p3_do_iSKPS <= p_do_iSKPS;
    end


  //Control-bit do_iSKPS, set by: , SKPS, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign do_iSKPS = p3_do_iSKPS;

  assign do_iSTx_delayed_for_op_is_STx_1 = do_iSTx;
  assign do_iSTx_delayed_for_op_is_STx = do_iSTx_delayed_for_op_is_STx_1;
  assign p_op_is_STx = 1'b0 ||
    do_iSTx_delayed_for_op_is_STx;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_op_is_STx <= 0;
      else if (pipe_run)
          p2_op_is_STx <= p_op_is_STx;
    end


  //Control-bit op_is_STx, set by: , do_iSTx_delayed_for_op_is_STx, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign op_is_STx = p2_op_is_STx;

  assign p_do_iTRET = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) && ~instruction_2[9] &&  instruction_2[8] &&  instruction_2[7] &&  instruction_2[6] && ~instruction_2[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iTRET <= 0;
      else if (pipe_run)
          p3_do_iTRET <= p_do_iTRET;
    end


  //Control-bit do_iTRET, set by: , TRET, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign do_iTRET = p3_do_iTRET;

  assign p_is_left_shift = 1'b0 ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] && ~instruction_2[10] &&  subinstruction_2[0]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10] &&  subinstruction_2[0]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_is_left_shift <= 0;
      else if (pipe_run)
          p3_is_left_shift <= p_is_left_shift;
    end


  //Control-bit is_left_shift, set by: , LSL_0, LSLI_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=3

  assign is_left_shift = p3_is_left_shift;

  assign p_do_iSKPRzx = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] &&  instruction_2[7] &&  instruction_2[6] && ~instruction_2[5] &&  subinstruction_2[0]) ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] &&  instruction_2[7] &&  instruction_2[6] && ~instruction_2[5] && ~subinstruction_2[0]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iSKPRzx <= 0;
      else if (pipe_run)
          p3_do_iSKPRzx <= p_do_iSKPRzx;
    end


  //Control-bit do_iSKPRzx, set by: , SKPRz_0, SKPRz_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=3

  assign do_iSKPRzx = p3_do_iSKPRzx;

  assign p_do_stack_offset = 1'b0 ||
    ( instruction_0[15] &&  instruction_0[14] && ~instruction_0[13]) ||
    ( instruction_0[15] &&  instruction_0[14] &&  instruction_0[13]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_stack_offset <= 0;
      else if (commit)
          p1_do_stack_offset <= p_do_stack_offset;
    end


  //Control-bit do_stack_offset, set by: , STS, LDS, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=1

  assign do_stack_offset = p1_do_stack_offset;

  assign do_iTRAP_0_delayed_for_op_is_TRAP_0_2 = do_iTRAP_0;
  assign do_iTRAP_0_delayed_for_op_is_TRAP_0 = do_iTRAP_0_delayed_for_op_is_TRAP_0_2;
  assign p_op_is_TRAP_0 = 1'b0 ||
    do_iTRAP_0_delayed_for_op_is_TRAP_0;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_op_is_TRAP_0 <= 0;
      else if (pipe_run)
          p3_op_is_TRAP_0 <= p_op_is_TRAP_0;
    end


  //Control-bit op_is_TRAP_0, set by: , do_iTRAP_0_delayed_for_op_is_TRAP_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign op_is_TRAP_0 = p3_op_is_TRAP_0;

  assign do_iLDx_delayed_for_sel_memword_1 = do_iLDx;
  assign do_iLDx_delayed_for_sel_memword = do_iLDx_delayed_for_sel_memword_2;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          do_iLDx_delayed_for_sel_memword_2 <= 0;
      else if (pipe_run)
          do_iLDx_delayed_for_sel_memword_2 <= do_iLDx_delayed_for_sel_memword_1;
    end


  assign do_iTRAP_0_delayed_for_sel_memword_2 = do_iTRAP_0;
  assign do_iTRAP_0_delayed_for_sel_memword = do_iTRAP_0_delayed_for_sel_memword_2;
  assign p_sel_memword = 1'b0 ||
    do_iLDx_delayed_for_sel_memword ||
    do_iTRAP_0_delayed_for_sel_memword;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_sel_memword <= 0;
      else if (pipe_run)
          p3_sel_memword <= p_sel_memword;
    end


  //Control-bit sel_memword, set by: , do_iLDx_delayed_for_sel_memword, do_iTRAP_0_delayed_for_sel_memword, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign sel_memword = p3_sel_memword;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_byterot_1 <= 0;
      else if (pipe_run)
          p2_do_byterot_1 <= p_do_byterot_1;
    end


  //Control-bit do_byterot_1, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign do_byterot_1 = p2_do_byterot_1;

  assign p_dest_index_from_sp = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] && ~instruction_0[8]);

  //Control-bit dest_index_from_sp, set by: , SAVE, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=0

  assign dest_index_from_sp = p_dest_index_from_sp;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_inv_all_b <= 0;
      else if (pipe_run)
          p2_do_inv_all_b <= p_do_inv_all_b;
    end


  //Control-bit do_inv_all_b, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign do_inv_all_b = p2_do_inv_all_b;

  assign p_op_is_trap = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_op_is_trap <= 0;
      else if (commit)
          p1_op_is_trap <= p_op_is_trap;
    end


  //Control-bit op_is_trap, set by: , TRAP_0, TRAP_1, TRAP_2, TRAP_3, 
  //Just one of 104 total control bits.
  //, 
  //Matches 4 of 126 mnemonics, stage=1

  assign op_is_trap = p1_op_is_trap;

  assign p_do_iRLC_1 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iRLC_1 <= 0;
      else if (pipe_run)
          p3_do_iRLC_1 <= p_do_iRLC_1;
    end


  //Control-bit do_iRLC_1, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign do_iRLC_1 = p3_do_iRLC_1;

  assign p_op_is_branch = 1'b0 ||
    ( instruction_0[15] && ~instruction_0[14] && ~instruction_0[13] && ~instruction_0[12] && ~instruction_0[11]) ||
    ( instruction_0[15] && ~instruction_0[14] && ~instruction_0[13] && ~instruction_0[12] &&  instruction_0[11]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_op_is_branch <= 0;
      else if (commit)
          p1_op_is_branch <= p_op_is_branch;
    end


  //Control-bit op_is_branch, set by: , BR, BSR, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=1

  assign op_is_branch = p1_op_is_branch;

  assign p_a_index_from_sp = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] && ~instruction_0[8]);

  //Control-bit a_index_from_sp, set by: , SAVE, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=0

  assign a_index_from_sp = p_a_index_from_sp;

  //Control-bit do_not_interrupt, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign do_not_interrupt = p_do_not_interrupt;

  assign p_c_is_borrow = 1'b0 ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_c_is_borrow <= 0;
      else if (pipe_run)
          p3_c_is_borrow <= p_c_is_borrow;
    end


  //Control-bit c_is_borrow, set by: , SUB, SUBI, CMP, CMPI, 
  //Just one of 104 total control bits.
  //, 
  //Matches 4 of 126 mnemonics, stage=3

  assign c_is_borrow = p3_c_is_borrow;

  assign p_do_dynamic_narrow_write = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] && ~instruction_1[7] && ~instruction_1[6] &&  instruction_1[5]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] && ~instruction_1[7] && ~instruction_1[6] && ~instruction_1[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_dynamic_narrow_write <= 0;
      else if (pipe_run)
          p2_do_dynamic_narrow_write <= p_do_dynamic_narrow_write;
    end


  //Control-bit do_dynamic_narrow_write, set by: , ST16d, ST8d, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=2

  assign do_dynamic_narrow_write = p2_do_dynamic_narrow_write;

  assign p_sreset_add_0 = 1'b0 ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10] &&  subinstruction_2[0]) ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10] && ~subinstruction_2[0]) ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10] &&  subinstruction_2[0]) ||
    (~instruction_2[15] && ~instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10] && ~subinstruction_2[0]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_sreset_add_0 <= 0;
      else if (pipe_run)
          p3_sreset_add_0 <= p_sreset_add_0;
    end


  //Control-bit sreset_add_0, set by: , ASR_0, ASR_1, ASRI_0, ASRI_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 4 of 126 mnemonics, stage=3

  assign sreset_add_0 = p3_sreset_add_0;

  assign p_do_iSWAP = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) && ~instruction_1[9] &&  instruction_1[8] && ~instruction_1[7] && ~instruction_1[6] && ~instruction_1[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_iSWAP <= 0;
      else if (pipe_run)
          p2_do_iSWAP <= p_do_iSWAP;
    end


  //Control-bit do_iSWAP, set by: , SWAP, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=2

  assign do_iSWAP = p2_do_iSWAP;

  assign p_sel_dynamic_ext16 = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] && ~instruction_2[7] &&  instruction_2[6] &&  instruction_2[5]) ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]) && ~instruction_2[9] && ~instruction_2[8] &&  instruction_2[7]) ||
    (~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_sel_dynamic_ext16 <= 0;
      else if (pipe_run)
          p3_sel_dynamic_ext16 <= p_sel_dynamic_ext16;
    end


  //Control-bit sel_dynamic_ext16, set by: , FILL16, EXT16s, EXT16d, 
  //Just one of 104 total control bits.
  //, 
  //Matches 3 of 126 mnemonics, stage=3

  assign sel_dynamic_ext16 = p3_sel_dynamic_ext16;

  assign p_do_iRDCTL = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] &&  instruction_2[8] && ~instruction_2[7] && ~instruction_2[6] &&  instruction_2[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iRDCTL <= 0;
      else if (pipe_run)
          p3_do_iRDCTL <= p_do_iRDCTL;
    end


  //Control-bit do_iRDCTL, set by: , RDCTL, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign do_iRDCTL = p3_do_iRDCTL;

  assign p_custom_instruction_start_mul = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] &&  instruction_2[7] && ~instruction_2[6] &&  instruction_2[5] && ~subinstruction_2[0] &&  subinstruction_2[1]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_custom_instruction_start_mul <= 0;
      else if (pipe_run)
          p3_custom_instruction_start_mul <= p_custom_instruction_start_mul;
    end


  //Control-bit custom_instruction_start_mul, set by: , MUL_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign custom_instruction_start_mul = p3_custom_instruction_start_mul;

  assign p_skip_is_active = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] &&  instruction_2[7] &&  instruction_2[6] &&  instruction_2[5]) ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] && ~instruction_2[10] && ~subinstruction_2[0]) ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10] && ~subinstruction_2[0]) ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] &&  instruction_2[7] &&  instruction_2[6] && ~instruction_2[5] && ~subinstruction_2[0]) ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] &&  instruction_2[8] && ~instruction_2[7] &&  instruction_2[6] && ~instruction_2[5] && ~subinstruction_2[0]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_skip_is_active <= 0;
      else if (pipe_run)
          p3_skip_is_active <= p_skip_is_active;
    end


  //Control-bit skip_is_active, set by: , SKPS, SKP0_1, SKP1_1, SKPRz_1, SKPRnz_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 5 of 126 mnemonics, stage=3

  assign skip_is_active = p3_skip_is_active;

  assign p_do_iSKPx = 1'b0 ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] &&  instruction_0[6] && ~instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] &&  instruction_0[6] && ~instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] && ~instruction_0[8] &&  instruction_0[7] &&  instruction_0[6] &&  instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] &&  instruction_0[8] && ~instruction_0[7] &&  instruction_0[6] && ~instruction_0[5]) ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) &&  instruction_0[9] &&  instruction_0[8] && ~instruction_0[7] &&  instruction_0[6] && ~instruction_0[5]);

  //Control-bit do_iSKPx, set by: , SKP0_0, SKP0_1, SKP1_0, SKP1_1, SKPRz_0, SKPRz_1, SKPS, SKPRnz_0, SKPRnz_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 9 of 126 mnemonics, stage=0

  assign do_iSKPx = p_do_iSKPx;

  assign p_do_iLDx = 1'b0 ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) ||
    ( instruction_0[15] && ~instruction_0[14] &&  instruction_0[13] &&  instruction_0[12]) ||
    ( instruction_0[15] &&  instruction_0[14] &&  instruction_0[13]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_iLDx <= 0;
      else if (commit)
          p1_do_iLDx <= p_do_iLDx;
    end


  //Control-bit do_iLDx, set by: , LD, LDP, LDS, 
  //Just one of 104 total control bits.
  //, 
  //Matches 3 of 126 mnemonics, stage=1

  assign do_iLDx = p1_do_iLDx;

  assign p_V_update = 1'b0 ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] && ~instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] && ~instruction_2[10]) ||
    (~instruction_2[15] && ~instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_V_update <= 0;
      else if (pipe_run)
          p3_V_update <= p_V_update;
    end


  //Control-bit V_update, set by: , SUB, SUBI, ADD, ADDI, CMP, CMPI, 
  //Just one of 104 total control bits.
  //, 
  //Matches 6 of 126 mnemonics, stage=3

  assign V_update = p3_V_update;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_sel_notb_x <= 0;
      else if (pipe_run)
          p2_sel_notb_x <= p_sel_notb_x;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_sel_notb_x <= 0;
      else if (pipe_run)
          p3_sel_notb_x <= p2_sel_notb_x;
    end


  //Control-bit sel_notb_x, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign sel_notb_x = p3_sel_notb_x;

  assign p_sel_dynamic_ext8 = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] && ~instruction_2[8] && ~instruction_2[7] &&  instruction_2[6] && ~instruction_2[5]) ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] &&  instruction_2[10]) && ~instruction_2[9] && ~instruction_2[8] && ~instruction_2[7]) ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] && ~instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_sel_dynamic_ext8 <= 0;
      else if (pipe_run)
          p3_sel_dynamic_ext8 <= p_sel_dynamic_ext8;
    end


  //Control-bit sel_dynamic_ext8, set by: , FILL8, EXT8s, EXT8d, 
  //Just one of 104 total control bits.
  //, 
  //Matches 3 of 126 mnemonics, stage=3

  assign sel_dynamic_ext8 = p3_sel_dynamic_ext8;

  assign do_iLDx_delayed_for_op_is_LDx_1 = do_iLDx;
  assign do_iLDx_delayed_for_op_is_LDx = do_iLDx_delayed_for_op_is_LDx_1;
  assign p_op_is_LDx = 1'b0 ||
    do_iLDx_delayed_for_op_is_LDx;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_op_is_LDx <= 0;
      else if (pipe_run)
          p2_op_is_LDx <= p_op_is_LDx;
    end


  //Control-bit op_is_LDx, set by: , do_iLDx_delayed_for_op_is_LDx, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign op_is_LDx = p2_op_is_LDx;

  assign op_is_trap_delayed_for_use_fresh_cwp_1 = op_is_trap;
  assign op_is_trap_delayed_for_use_fresh_cwp = op_is_trap_delayed_for_use_fresh_cwp_3;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_is_trap_delayed_for_use_fresh_cwp_2 <= 0;
      else if (pipe_run)
          op_is_trap_delayed_for_use_fresh_cwp_2 <= op_is_trap_delayed_for_use_fresh_cwp_1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_is_trap_delayed_for_use_fresh_cwp_3 <= 0;
      else if (pipe_run)
          op_is_trap_delayed_for_use_fresh_cwp_3 <= op_is_trap_delayed_for_use_fresh_cwp_2;
    end


  assign p_use_fresh_cwp = 1'b0 ||
    op_is_trap_delayed_for_use_fresh_cwp;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p4_use_fresh_cwp <= 0;
      else if (pipe_run)
          p4_use_fresh_cwp <= p_use_fresh_cwp;
    end


  //Control-bit use_fresh_cwp, set by: , op_is_trap_delayed_for_use_fresh_cwp, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=4

  assign use_fresh_cwp = p4_use_fresh_cwp;

  assign p_do_write_16 = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] && ~instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] &&  instruction_1[7]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] && ~instruction_1[7] && ~instruction_1[6] &&  instruction_1[5]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] && ~instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] &&  instruction_1[7]) ||
    (~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] && ~instruction_1[12] && ~instruction_1[11] &&  instruction_1[10]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] && ~instruction_1[7] && ~instruction_1[6] &&  instruction_1[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_write_16 <= 0;
      else if (pipe_run)
          p2_do_write_16 <= p_do_write_16;
    end


  //Control-bit do_write_16, set by: , ST16s, ST16d, ST16s, STS16s, ST16d, 
  //Just one of 104 total control bits.
  //, 
  //Matches 3 of 126 mnemonics, stage=2

  assign do_write_16 = p2_do_write_16;

  assign p_do_iTRAP_0 = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] && ~instruction_1[10]) && ~instruction_1[9] &&  instruction_1[8] &&  subinstruction_1[0] &&  subinstruction_1[1]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_iTRAP_0 <= 0;
      else if (pipe_run)
          p2_do_iTRAP_0 <= p_do_iTRAP_0;
    end


  //Control-bit do_iTRAP_0, set by: , TRAP_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=2

  assign do_iTRAP_0 = p2_do_iTRAP_0;

  assign p_do_write_8 = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] && ~instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] && ~instruction_1[7]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] && ~instruction_1[7] && ~instruction_1[6] && ~instruction_1[5]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] && ~instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] && ~instruction_1[7]) ||
    (~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] && ~instruction_1[12] && ~instruction_1[11] && ~instruction_1[10]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) &&  instruction_1[9] && ~instruction_1[8] && ~instruction_1[7] && ~instruction_1[6] && ~instruction_1[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_write_8 <= 0;
      else if (pipe_run)
          p2_do_write_8 <= p_do_write_8;
    end


  //Control-bit do_write_8, set by: , ST8s, ST8d, ST8s, STS8s, ST8d, 
  //Just one of 104 total control bits.
  //, 
  //Matches 3 of 126 mnemonics, stage=2

  assign do_write_8 = p2_do_write_8;

  assign p_do_trap_offset = 1'b0 ||
    ((~instruction_0[15] &&  instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) && ~instruction_0[9] &&  instruction_0[8]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_do_trap_offset <= 0;
      else if (commit)
          p1_do_trap_offset <= p_do_trap_offset;
    end


  //Control-bit do_trap_offset, set by: , TRAP_0, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=1

  assign do_trap_offset = p1_do_trap_offset;

  assign p_do_iSKP0x = 1'b0 ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] && ~instruction_2[10] &&  subinstruction_2[0]) ||
    (~instruction_2[15] &&  instruction_2[14] && ~instruction_2[13] &&  instruction_2[12] && ~instruction_2[11] && ~instruction_2[10] && ~subinstruction_2[0]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iSKP0x <= 0;
      else if (pipe_run)
          p3_do_iSKP0x <= p_do_iSKP0x;
    end


  //Control-bit do_iSKP0x, set by: , SKP0_0, SKP0_1, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=3

  assign do_iSKP0x = p3_do_iSKP0x;

  //Control-bit op_b_from_Ki5, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign op_b_from_Ki5 = p_op_b_from_Ki5;

  //Control-bit override_b_control, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=1

  assign override_b_control = p_override_b_control;

  assign op_subroutine_delayed_for_do_save_return_address_0 = op_subroutine;
  assign op_subroutine_delayed_for_do_save_return_address = op_subroutine_delayed_for_do_save_return_address_1;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_subroutine_delayed_for_do_save_return_address_1 <= 0;
      else if (commit)
          op_subroutine_delayed_for_do_save_return_address_1 <= op_subroutine_delayed_for_do_save_return_address_0;
    end


  assign do_iTRAP_n_delayed_for_do_save_return_address_2 = p_do_iTRAP_n;
  assign do_iTRAP_n_delayed_for_do_save_return_address = do_iTRAP_n_delayed_for_do_save_return_address_2;
  assign p_do_save_return_address = 1'b0 ||
    op_subroutine_delayed_for_do_save_return_address ||
    do_iTRAP_n_delayed_for_do_save_return_address;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_save_return_address <= 0;
      else if (pipe_run)
          p2_do_save_return_address <= p_do_save_return_address;
    end


  //Control-bit do_save_return_address, set by: , op_subroutine_delayed_for_do_save_return_address, do_iTRAP_n_delayed_for_do_save_return_address, 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign do_save_return_address = p2_do_save_return_address;

  assign p_do_iWRCTL = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) &&  instruction_2[9] &&  instruction_2[8] && ~instruction_2[7] && ~instruction_2[6] && ~instruction_2[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iWRCTL <= 0;
      else if (pipe_run)
          p3_do_iWRCTL <= p_do_iWRCTL;
    end


  //Control-bit do_iWRCTL, set by: , WRCTL, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign do_iWRCTL = p3_do_iWRCTL;

  assign op_jmpcall_delayed_for_op_is_jump_0 = op_jmpcall;
  assign op_jmpcall_delayed_for_op_is_jump = op_jmpcall_delayed_for_op_is_jump_1;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          op_jmpcall_delayed_for_op_is_jump_1 <= 0;
      else if (commit)
          op_jmpcall_delayed_for_op_is_jump_1 <= op_jmpcall_delayed_for_op_is_jump_0;
    end


  assign p_op_is_jump = 1'b0 ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] && ~instruction_1[10]) && ~instruction_1[9] &&  instruction_1[8] && ~subinstruction_1[0] && ~subinstruction_1[1]) ||
    ((~instruction_1[15] &&  instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] &&  instruction_1[11] &&  instruction_1[10]) && ~instruction_1[9] &&  instruction_1[8] &&  instruction_1[7] &&  instruction_1[6] && ~instruction_1[5]) ||
    op_jmpcall_delayed_for_op_is_jump;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_op_is_jump <= 0;
      else if (pipe_run)
          p2_op_is_jump <= p_op_is_jump;
    end


  //Control-bit op_is_jump, set by: , TRAP_3, TRET, op_jmpcall_delayed_for_op_is_jump, 
  //Just one of 104 total control bits.
  //, 
  //Matches 2 of 126 mnemonics, stage=2

  assign op_is_jump = p2_op_is_jump;

  assign p_do_iMOVI = 1'b0 ||
    (~instruction_1[15] && ~instruction_1[14] &&  instruction_1[13] &&  instruction_1[12] && ~instruction_1[11] &&  instruction_1[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_do_iMOVI <= 0;
      else if (pipe_run)
          p2_do_iMOVI <= p_do_iMOVI;
    end


  //Control-bit do_iMOVI, set by: , MOVI, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=2

  assign do_iMOVI = p2_do_iMOVI;

  assign p_do_iRRC_1 = 1'b0;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iRRC_1 <= 0;
      else if (pipe_run)
          p3_do_iRRC_1 <= p_do_iRRC_1;
    end


  //Control-bit do_iRRC_1, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=3

  assign do_iRRC_1 = p3_do_iRRC_1;

  assign p_op_b_from_reg_or_const = 1'b0 ||
    (~instruction_0[15] && ~instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] && ~instruction_0[10]) ||
    (~instruction_0[15] && ~instruction_0[14] &&  instruction_0[13] &&  instruction_0[12] &&  instruction_0[11] &&  instruction_0[10]) ||
    (~instruction_0[15] &&  instruction_0[14] && ~instruction_0[13] && ~instruction_0[12] && ~instruction_0[11] &&  instruction_0[10]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p1_op_b_from_reg_or_const <= 0;
      else if (commit)
          p1_op_b_from_reg_or_const <= p_op_b_from_reg_or_const;
    end


  //Control-bit op_b_from_reg_or_const, set by: , AND, OR, ANDN, XOR, 
  //Just one of 104 total control bits.
  //, 
  //Matches 4 of 126 mnemonics, stage=1

  assign op_b_from_reg_or_const = p1_op_b_from_reg_or_const;

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p2_zero_all_b_control <= 0;
      else if (pipe_run)
          p2_zero_all_b_control <= p_zero_all_b_control;
    end


  //Control-bit zero_all_b_control, set by: , 
  //Just one of 104 total control bits.
  //, 
  //Matches 0 of 126 mnemonics, stage=2

  assign zero_all_b_control = p2_zero_all_b_control;

  assign p_do_iSEXT8 = 1'b0 ||
    ((~instruction_2[15] &&  instruction_2[14] &&  instruction_2[13] &&  instruction_2[12] &&  instruction_2[11] &&  instruction_2[10]) && ~instruction_2[9] && ~instruction_2[8] && ~instruction_2[7] &&  instruction_2[6] &&  instruction_2[5]);

  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          p3_do_iSEXT8 <= 0;
      else if (pipe_run)
          p3_do_iSEXT8 <= p_do_iSEXT8;
    end


  //Control-bit do_iSEXT8, set by: , SEXT8, 
  //Just one of 104 total control bits.
  //, 
  //Matches 1 of 126 mnemonics, stage=3

  assign do_iSEXT8 = p3_do_iSEXT8;

  assign byte_offset = instruction_1[6 : 5];
  assign narrow_stack_offset = instruction_1[9 : 0];
  assign minor_opcode = instruction_0[9 : 5];
  assign major_opcode = instruction_0[15 : 10];
  assign a_field = instruction_0[4 : 0];
  assign stack_offset = instruction_1[12 : 5];
  assign imm5 = instruction_1[9 : 5];
  assign prefix_payload = instruction_1[10 : 0];
  assign test = instruction_3[3 : 0];
  assign trap_number = instruction_3[5 : 0];
  assign b_field = instruction_0[9 : 5];
  assign p_field = instruction_0[11 : 10];
  assign save_offset = instruction_1[7 : 0];
  assign trap_offset = instruction_1[5 : 0];
  assign branch_offset = instruction_1[10 : 0];

endmodule


module enet_nios_pipeline (
                            // inputs:
                             clk,
                             custom_instruction_result,
                             dc_readdata,
                             dc_wait,
                             ic_datavalid,
                             ic_readdata,
                             ic_wait,
                             irq,
                             irqnumber,
                             reset_n,

                            // outputs:
                             DC_EN,
                             IC_EN,
                             committed_instruction,
                             custom_instruction_clk_en,
                             custom_instruction_dataa,
                             custom_instruction_datab,
                             custom_instruction_prefix,
                             custom_instruction_reset,
                             custom_instruction_start,
                             dc_address,
                             dc_byteenable,
                             dc_read,
                             dc_write,
                             dc_writedata,
                             ic_address,
                             ic_flush,
                             ic_read
                          )
;

  output           DC_EN;
  output           IC_EN;
  output  [ 15: 0] committed_instruction;
  output           custom_instruction_clk_en;
  output  [ 31: 0] custom_instruction_dataa;
  output  [ 31: 0] custom_instruction_datab;
  output  [ 10: 0] custom_instruction_prefix;
  output           custom_instruction_reset;
  output  [  5: 0] custom_instruction_start;
  output  [ 22: 0] dc_address;
  output  [  3: 0] dc_byteenable;
  output           dc_read;
  output           dc_write;
  output  [ 31: 0] dc_writedata;
  output  [ 22: 0] ic_address;
  output           ic_flush;
  output           ic_read;
  input            clk;
  input   [ 31: 0] custom_instruction_result;
  input   [ 31: 0] dc_readdata;
  input            dc_wait;
  input            ic_datavalid;
  input   [ 15: 0] ic_readdata;
  input            ic_wait;
  input            irq;
  input   [  5: 0] irqnumber;
  input            reset_n;

  wire    [  4: 0] CWP;
  wire             DC_EN;
  wire             IC_EN;
  wire             IE;
  wire    [  5: 0] IPRI;
  wire             N_update;
  wire             V_update;
  wire    [  4: 0] a_field;
  wire             a_index_from_o7;
  wire             a_index_from_sp;
  wire             a_index_from_zero;
  wire    [  4: 0] a_local;
  wire             a_matches_dest1;
  wire             a_matches_dest2;
  wire    [ 31: 0] alu_result;
  wire    [  4: 0] b_field;
  wire             b_index_from_sp;
  wire             b_index_from_zero;
  wire    [  4: 0] b_local;
  wire             b_matches_dest1;
  wire             b_matches_dest2;
  wire    [ 22: 0] base_address;
  wire    [ 21: 0] branch_base;
  wire    [ 10: 0] branch_offset;
  wire    [  3: 0] byte_complement;
  wire    [  1: 0] byte_offset;
  wire    [  3: 0] byte_zero;
  wire             byterot_F_control;
  wire    [  3: 0] byterot_sel_hi16;
  wire    [  3: 0] byterot_sel_lo16;
  wire             c_is_borrow;
  wire             cancel_branch_delay_slot;
  wire             commit;
  wire    [ 15: 0] committed_instruction;
  wire    [ 15: 0] const;
  wire             custom_instruction_clk_en;
  wire    [ 31: 0] custom_instruction_dataa;
  wire    [ 31: 0] custom_instruction_datab;
  wire    [ 10: 0] custom_instruction_prefix;
  wire             custom_instruction_reset;
  wire    [  5: 0] custom_instruction_start;
  wire             custom_instruction_start_mul;
  wire    [ 22: 0] dc_address;
  wire    [  3: 0] dc_byteenable;
  wire             dc_read;
  wire             dc_write;
  wire    [ 31: 0] dc_writedata;
  wire    [  4: 0] dest_cwp;
  wire    [  4: 0] dest_cwp_2;
  reg     [  4: 0] dest_cwp_3;
  reg     [  4: 0] dest_cwp_4;
  wire             dest_index_from_o7;
  wire             dest_index_from_sp;
  wire             dest_index_from_zero;
  wire    [  4: 0] dest_local;
  wire    [  4: 0] dest_local_1;
  reg     [  4: 0] dest_local_2;
  reg     [  4: 0] dest_local_3;
  reg     [  4: 0] dest_local_4;
  wire             do_branch;
  wire             do_byterot_1;
  wire             do_cancel_next_instruction;
  wire             do_custom_instruction;
  wire             do_dynamic_narrow_write;
  wire             do_fetch_vector;
  wire             do_force_shift_0;
  wire             do_force_shift_1;
  wire             do_force_trap;
  wire             do_fwd_a_alu;
  wire             do_fwd_b_alu;
  wire             do_iABS;
  wire             do_iASRx_1;
  wire             do_iLDx;
  wire             do_iMOVHI;
  wire             do_iMOVI;
  wire             do_iMSTEP_0;
  wire             do_iMSTEP_1;
  wire             do_iPFXx;
  wire             do_iRDCTL;
  wire             do_iRESTORE;
  wire             do_iRLC_1;
  wire             do_iRRC_1;
  wire             do_iRxC_0;
  wire             do_iRxC_1;
  wire             do_iSAVE;
  wire             do_iSEXT16;
  wire             do_iSEXT8;
  wire             do_iSKP0x;
  wire             do_iSKP1x;
  wire             do_iSKPRzx;
  wire             do_iSKPS;
  wire             do_iSKPx;
  wire             do_iSTx;
  wire             do_iSWAP;
  wire             do_iTRAP_0;
  wire             do_iTRAP_n;
  wire             do_iTRET;
  wire             do_iWRCTL;
  wire             do_inv_all_b;
  wire             do_jump;
  wire             do_narrow_stack_offset;
  wire             do_normal_offset;
  wire             do_normal_static_write;
  wire             do_not_interrupt;
  wire             do_override_op_b;
  wire             do_save_offset;
  wire             do_save_return_address;
  wire             do_skip;
  wire             do_stack_offset;
  wire             do_trap_offset;
  wire             do_write_16;
  wire             do_write_8;
  wire             force_carryin;
  wire    [ 15: 0] forced_trap_instruction;
  wire             hold_for_hazard;
  wire    [ 22: 0] ic_address;
  wire             ic_flush;
  wire             ic_read;
  wire    [  4: 0] imm5;
  wire    [ 15: 0] instruction;
  wire    [ 15: 0] instruction_0;
  reg     [ 15: 0] instruction_1;
  reg     [ 15: 0] instruction_2;
  reg     [ 15: 0] instruction_3;
  reg     [ 15: 0] instruction_4;
  wire             is_cancelled;
  wire             is_cancelled_1;
  reg              is_cancelled_2;
  reg              is_cancelled_3;
  reg              is_cancelled_4;
  wire             is_cancelled_from_commit_stage;
  wire             is_eightie;
  wire             is_left_shift;
  wire             is_neutrino;
  wire             is_neutrino_1;
  reg              is_neutrino_2;
  reg              is_neutrino_3;
  reg              is_neutrino_4;
  wire             is_right_shift;
  wire             is_sixteenie;
  wire    [ 21: 0] jump_target_address;
  wire             must_run_to_completion;
  wire    [  9: 0] narrow_stack_offset;
  wire    [ 21: 0] next_instruction_address;
  wire             non_write_op;
  wire    [ 22: 0] offset;
  wire    [ 31: 0] op_a;
  wire    [ 31: 0] op_b;
  wire             op_b_from_2Ei5;
  wire             op_b_from_Ki5;
  wire             op_b_from_reg_b;
  wire             op_b_from_reg_or_const;
  wire             op_b_from_reg_really;
  wire             op_b_hi_from_zero;
  wire             op_b_is_overridden;
  wire             op_b_lo_from_zero;
  wire             op_is_LDx;
  wire             op_is_MOVHI;
  wire             op_is_MSTEP_0;
  wire             op_is_MSTEP_1;
  wire             op_is_PFX;
  wire             op_is_STx;
  wire             op_is_TRAP_0;
  wire             op_is_branch;
  wire             op_is_jump;
  wire             op_is_trap;
  wire             op_jmpcall;
  wire             op_save_restore;
  wire             op_subroutine;
  wire             op_uses_a_pre_alu;
  wire             op_uses_b_pre_alu;
  wire    [  3: 0] overridden_byte_compl;
  wire    [  3: 0] overridden_byte_zero;
  wire    [  3: 0] overridden_byterot_sel_hi16;
  wire    [  3: 0] overridden_byterot_sel_lo16;
  wire    [ 15: 0] overridden_opB;
  wire             override_b_control;
  wire    [  1: 0] p_field;
  wire             pipe_freeze;
  wire             pipe_run;
  wire    [ 10: 0] prefix_payload;
  wire             qualified_do_iRESTORE;
  wire             qualified_do_iSAVE;
  wire    [ 31: 0] raw_reg_a;
  wire    [ 31: 0] raw_reg_b;
  wire             reg_not_modified;
  wire             request_stable_op_b;
  wire    [  7: 0] save_offset;
  wire             sel_dynamic_ext;
  wire             sel_dynamic_ext16;
  wire             sel_dynamic_ext8;
  wire             sel_memword;
  wire             sel_notb_x;
  wire    [  4: 0] shiftValue;
  wire    [ 21: 0] signed_branch_offset;
  wire             skip_is_active;
  wire             sload_add_with_b_control;
  wire             sreset_add_0;
  wire             sreset_add_x;
  wire    [  7: 0] stack_offset;
  wire    [  5: 0] subinstruction;
  wire    [  5: 0] subinstruction_1;
  reg     [  5: 0] subinstruction_2;
  reg     [  5: 0] subinstruction_3;
  reg     [  5: 0] subinstruction_4;
  wire    [  3: 0] test;
  wire             trap_if_restore;
  wire             trap_if_save;
  wire    [  5: 0] trap_number;
  wire    [  5: 0] trap_offset;
  wire             trap_properly_received;
  wire             trap_request_overflow;
  wire             trap_request_underflow;
  wire             unqualified_custom_instruction_start_0;
  wire             unqualified_custom_instruction_start_1;
  wire             unqualified_custom_instruction_start_2;
  wire             unqualified_custom_instruction_start_3;
  wire             unqualified_custom_instruction_start_4;
  wire             use_fresh_cwp;
  wire             zero_all_b_control;
  enet_nios_interrupt_control the_enet_nios_interrupt_control
    (
      .IE                             (IE),
      .IPRI                           (IPRI),
      .cancel_branch_delay_slot       (cancel_branch_delay_slot),
      .clk                            (clk),
      .do_cancel_next_instruction     (do_cancel_next_instruction),
      .do_force_trap                  (do_force_trap),
      .do_not_interrupt               (do_not_interrupt),
      .forced_trap_instruction        (forced_trap_instruction),
      .instruction_1                  (instruction_1),
      .irq                            (irq),
      .irqnumber                      (irqnumber),
      .is_cancelled                   (is_cancelled),
      .is_cancelled_from_commit_stage (is_cancelled_from_commit_stage),
      .is_neutrino                    (is_neutrino),
      .op_is_trap                     (op_is_trap),
      .pipe_run                       (pipe_run),
      .reset_n                        (reset_n),
      .subinstruction                 (subinstruction),
      .trap_properly_received         (trap_properly_received),
      .trap_request_overflow          (trap_request_overflow),
      .trap_request_underflow         (trap_request_underflow)
    );

  enet_nios_commitment_maker the_enet_nios_commitment_maker
    (
      .branch_base                    (branch_base),
      .clk                            (clk),
      .commit                         (commit),
      .do_branch                      (do_branch),
      .do_cancel_next_instruction     (do_cancel_next_instruction),
      .do_force_trap                  (do_force_trap),
      .do_iPFXx                       (do_iPFXx),
      .do_iSKPx                       (do_iSKPx),
      .do_jump                        (do_jump),
      .do_skip                        (do_skip),
      .forced_trap_instruction        (forced_trap_instruction),
      .hold_for_hazard                (hold_for_hazard),
      .ic_address                     (ic_address),
      .ic_datavalid                   (ic_datavalid),
      .ic_flush                       (ic_flush),
      .ic_read                        (ic_read),
      .ic_readdata                    (ic_readdata),
      .ic_wait                        (ic_wait),
      .instruction                    (instruction),
      .is_cancelled                   (is_cancelled),
      .is_cancelled_from_commit_stage (is_cancelled_from_commit_stage),
      .is_neutrino                    (is_neutrino),
      .jump_target_address            (jump_target_address),
      .must_run_to_completion         (must_run_to_completion),
      .next_instruction_address       (next_instruction_address),
      .op_jmpcall                     (op_jmpcall),
      .op_save_restore                (op_save_restore),
      .pipe_freeze                    (pipe_freeze),
      .pipe_run                       (pipe_run),
      .reset_n                        (reset_n),
      .signed_branch_offset           (signed_branch_offset),
      .subinstruction                 (subinstruction),
      .trap_if_restore                (trap_if_restore),
      .trap_if_save                   (trap_if_save),
      .trap_properly_received         (trap_properly_received)
    );

  enet_nios_constant_generator the_enet_nios_constant_generator
    (
      .byte_offset            (byte_offset),
      .clk                    (clk),
      .const                  (const),
      .do_iLDx                (do_iLDx),
      .do_iMOVHI              (do_iMOVHI),
      .do_iSTx                (do_iSTx),
      .do_narrow_stack_offset (do_narrow_stack_offset),
      .do_normal_offset       (do_normal_offset),
      .do_normal_static_write (do_normal_static_write),
      .do_override_op_b       (do_override_op_b),
      .do_save_offset         (do_save_offset),
      .do_stack_offset        (do_stack_offset),
      .do_trap_offset         (do_trap_offset),
      .imm5                   (imm5),
      .instruction_1          (instruction_1),
      .is_cancelled           (is_cancelled),
      .is_neutrino            (is_neutrino),
      .narrow_stack_offset    (narrow_stack_offset),
      .offset                 (offset),
      .op_b_from_2Ei5         (op_b_from_2Ei5),
      .op_b_from_Ki5          (op_b_from_Ki5),
      .op_b_from_reg_b        (op_b_from_reg_b),
      .op_b_from_reg_or_const (op_b_from_reg_or_const),
      .op_b_from_reg_really   (op_b_from_reg_really),
      .op_b_hi_from_zero      (op_b_hi_from_zero),
      .op_b_lo_from_zero      (op_b_lo_from_zero),
      .op_is_PFX              (op_is_PFX),
      .override_b_control     (override_b_control),
      .pipe_run               (pipe_run),
      .prefix_payload         (prefix_payload),
      .reset_n                (reset_n),
      .save_offset            (save_offset),
      .stack_offset           (stack_offset),
      .subinstruction         (subinstruction),
      .trap_offset            (trap_offset)
    );

  enet_nios_reg_index_calculator the_enet_nios_reg_index_calculator
    (
      .a_field              (a_field),
      .a_index_from_o7      (a_index_from_o7),
      .a_index_from_sp      (a_index_from_sp),
      .a_index_from_zero    (a_index_from_zero),
      .a_local              (a_local),
      .b_field              (b_field),
      .b_index_from_sp      (b_index_from_sp),
      .b_index_from_zero    (b_index_from_zero),
      .b_local              (b_local),
      .clk                  (clk),
      .commit               (commit),
      .dest_index_from_o7   (dest_index_from_o7),
      .dest_index_from_sp   (dest_index_from_sp),
      .dest_index_from_zero (dest_index_from_zero),
      .dest_local           (dest_local),
      .instruction_0        (instruction_0),
      .is_cancelled         (is_cancelled),
      .is_neutrino          (is_neutrino),
      .op_subroutine        (op_subroutine),
      .p_field              (p_field),
      .reset_n              (reset_n)
    );

  enet_nios_branch_unit the_enet_nios_branch_unit
    (
      .branch_base                    (branch_base),
      .branch_offset                  (branch_offset),
      .do_branch                      (do_branch),
      .instruction_1                  (instruction_1),
      .is_cancelled                   (is_cancelled),
      .is_cancelled_from_commit_stage (is_cancelled_from_commit_stage),
      .is_neutrino                    (is_neutrino),
      .next_instruction_address       (next_instruction_address),
      .op_is_branch                   (op_is_branch),
      .pipe_run                       (pipe_run),
      .signed_branch_offset           (signed_branch_offset)
    );

  enet_nios_index_match_unit the_enet_nios_index_match_unit
    (
      .CWP                            (CWP),
      .a_local                        (a_local),
      .a_matches_dest1                (a_matches_dest1),
      .a_matches_dest2                (a_matches_dest2),
      .b_local                        (b_local),
      .b_matches_dest1                (b_matches_dest1),
      .b_matches_dest2                (b_matches_dest2),
      .clk                            (clk),
      .dest_cwp                       (dest_cwp),
      .dest_local_2                   (dest_local_2),
      .dest_local_3                   (dest_local_3),
      .do_iRESTORE                    (do_iRESTORE),
      .do_iSAVE                       (do_iSAVE),
      .hold_for_hazard                (hold_for_hazard),
      .instruction_1                  (instruction_1),
      .is_cancelled                   (is_cancelled),
      .is_cancelled_from_commit_stage (is_cancelled_from_commit_stage),
      .is_neutrino                    (is_neutrino),
      .op_uses_a_pre_alu              (op_uses_a_pre_alu),
      .op_uses_b_pre_alu              (op_uses_b_pre_alu),
      .pipe_run                       (pipe_run),
      .qualified_do_iRESTORE          (qualified_do_iRESTORE),
      .qualified_do_iSAVE             (qualified_do_iSAVE),
      .reg_not_modified               (reg_not_modified),
      .reset_n                        (reset_n),
      .subinstruction                 (subinstruction)
    );

  enet_nios_jump_unit the_enet_nios_jump_unit
    (
      .dest_local_2     (dest_local_2),
      .do_jump          (do_jump),
      .instruction_2    (instruction_2),
      .is_cancelled_2   (is_cancelled_2),
      .is_neutrino_2    (is_neutrino_2),
      .op_is_jump       (op_is_jump),
      .pipe_run         (pipe_run),
      .subinstruction_2 (subinstruction_2)
    );

  enet_nios_data_master the_enet_nios_data_master
    (
      .base_address            (base_address),
      .clk                     (clk),
      .dc_address              (dc_address),
      .dc_byteenable           (dc_byteenable),
      .dc_read                 (dc_read),
      .dc_wait                 (dc_wait),
      .dc_write                (dc_write),
      .dc_writedata            (dc_writedata),
      .dest_local_2            (dest_local_2),
      .do_dynamic_narrow_write (do_dynamic_narrow_write),
      .do_iTRAP_0              (do_iTRAP_0),
      .do_write_16             (do_write_16),
      .do_write_8              (do_write_8),
      .instruction_2           (instruction_2),
      .is_cancelled_2          (is_cancelled_2),
      .is_neutrino_2           (is_neutrino_2),
      .offset                  (offset),
      .op_a                    (op_a),
      .op_is_LDx               (op_is_LDx),
      .op_is_STx               (op_is_STx),
      .pipe_freeze             (pipe_freeze),
      .pipe_run                (pipe_run),
      .reset_n                 (reset_n),
      .subinstruction_2        (subinstruction_2)
    );

  enet_nios_op_fetch the_enet_nios_op_fetch
    (
      .a_matches_dest1             (a_matches_dest1),
      .a_matches_dest2             (a_matches_dest2),
      .alu_result                  (alu_result),
      .b_matches_dest1             (b_matches_dest1),
      .b_matches_dest2             (b_matches_dest2),
      .base_address                (base_address),
      .byte_complement             (byte_complement),
      .byte_zero                   (byte_zero),
      .byterot_F_control           (byterot_F_control),
      .byterot_sel_hi16            (byterot_sel_hi16),
      .byterot_sel_lo16            (byterot_sel_lo16),
      .clk                         (clk),
      .const                       (const),
      .custom_instruction_prefix   (custom_instruction_prefix),
      .dest_local_2                (dest_local_2),
      .do_byterot_1                (do_byterot_1),
      .do_fetch_vector             (do_fetch_vector),
      .do_force_shift_0            (do_force_shift_0),
      .do_force_shift_1            (do_force_shift_1),
      .do_fwd_a_alu                (do_fwd_a_alu),
      .do_fwd_b_alu                (do_fwd_b_alu),
      .do_iMOVI                    (do_iMOVI),
      .do_iMSTEP_0                 (do_iMSTEP_0),
      .do_iMSTEP_1                 (do_iMSTEP_1),
      .do_iSWAP                    (do_iSWAP),
      .do_iTRAP_n                  (do_iTRAP_n),
      .do_inv_all_b                (do_inv_all_b),
      .do_override_op_b            (do_override_op_b),
      .do_save_return_address      (do_save_return_address),
      .instruction_2               (instruction_2),
      .is_cancelled_2              (is_cancelled_2),
      .is_eightie                  (is_eightie),
      .is_neutrino_2               (is_neutrino_2),
      .is_sixteenie                (is_sixteenie),
      .jump_target_address         (jump_target_address),
      .next_instruction_address    (next_instruction_address),
      .op_a                        (op_a),
      .op_b                        (op_b),
      .op_b_from_reg_really        (op_b_from_reg_really),
      .op_b_hi_from_zero           (op_b_hi_from_zero),
      .op_b_is_overridden          (op_b_is_overridden),
      .op_b_lo_from_zero           (op_b_lo_from_zero),
      .op_is_MOVHI                 (op_is_MOVHI),
      .overridden_byte_compl       (overridden_byte_compl),
      .overridden_byte_zero        (overridden_byte_zero),
      .overridden_byterot_sel_hi16 (overridden_byterot_sel_hi16),
      .overridden_byterot_sel_lo16 (overridden_byterot_sel_lo16),
      .overridden_opB              (overridden_opB),
      .pipe_run                    (pipe_run),
      .raw_reg_a                   (raw_reg_a),
      .raw_reg_b                   (raw_reg_b),
      .request_stable_op_b         (request_stable_op_b),
      .reset_n                     (reset_n),
      .shiftValue                  (shiftValue),
      .subinstruction_2            (subinstruction_2),
      .zero_all_b_control          (zero_all_b_control)
    );

  enet_nios_compact_alu the_enet_nios_compact_alu
    (
      .CWP                                    (CWP),
      .DC_EN                                  (DC_EN),
      .IC_EN                                  (IC_EN),
      .IE                                     (IE),
      .IPRI                                   (IPRI),
      .N_update                               (N_update),
      .V_update                               (V_update),
      .alu_result                             (alu_result),
      .byte_complement                        (byte_complement),
      .byte_zero                              (byte_zero),
      .byterot_sel_hi16                       (byterot_sel_hi16),
      .byterot_sel_lo16                       (byterot_sel_lo16),
      .c_is_borrow                            (c_is_borrow),
      .clk                                    (clk),
      .custom_instruction_clk_en              (custom_instruction_clk_en),
      .custom_instruction_dataa               (custom_instruction_dataa),
      .custom_instruction_datab               (custom_instruction_datab),
      .custom_instruction_reset               (custom_instruction_reset),
      .custom_instruction_result              (custom_instruction_result),
      .custom_instruction_start               (custom_instruction_start),
      .custom_instruction_start_mul           (custom_instruction_start_mul),
      .dc_readdata                            (dc_readdata),
      .dest_cwp_3                             (dest_cwp_3),
      .dest_local_3                           (dest_local_3),
      .do_custom_instruction                  (do_custom_instruction),
      .do_fwd_a_alu                           (do_fwd_a_alu),
      .do_fwd_b_alu                           (do_fwd_b_alu),
      .do_iABS                                (do_iABS),
      .do_iASRx_1                             (do_iASRx_1),
      .do_iRDCTL                              (do_iRDCTL),
      .do_iRLC_1                              (do_iRLC_1),
      .do_iRRC_1                              (do_iRRC_1),
      .do_iRxC_0                              (do_iRxC_0),
      .do_iRxC_1                              (do_iRxC_1),
      .do_iSEXT16                             (do_iSEXT16),
      .do_iSEXT8                              (do_iSEXT8),
      .do_iSKP0x                              (do_iSKP0x),
      .do_iSKP1x                              (do_iSKP1x),
      .do_iSKPRzx                             (do_iSKPRzx),
      .do_iSKPS                               (do_iSKPS),
      .do_iTRET                               (do_iTRET),
      .do_iWRCTL                              (do_iWRCTL),
      .do_skip                                (do_skip),
      .force_carryin                          (force_carryin),
      .instruction_3                          (instruction_3),
      .is_cancelled_3                         (is_cancelled_3),
      .is_left_shift                          (is_left_shift),
      .is_neutrino_3                          (is_neutrino_3),
      .is_right_shift                         (is_right_shift),
      .op_a                                   (op_a),
      .op_b                                   (op_b),
      .op_b_is_overridden                     (op_b_is_overridden),
      .op_is_MSTEP_0                          (op_is_MSTEP_0),
      .op_is_MSTEP_1                          (op_is_MSTEP_1),
      .op_is_TRAP_0                           (op_is_TRAP_0),
      .overridden_byte_compl                  (overridden_byte_compl),
      .overridden_byte_zero                   (overridden_byte_zero),
      .overridden_byterot_sel_hi16            (overridden_byterot_sel_hi16),
      .overridden_byterot_sel_lo16            (overridden_byterot_sel_lo16),
      .overridden_opB                         (overridden_opB),
      .pipe_freeze                            (pipe_freeze),
      .pipe_run                               (pipe_run),
      .qualified_do_iRESTORE                  (qualified_do_iRESTORE),
      .qualified_do_iSAVE                     (qualified_do_iSAVE),
      .reset_n                                (reset_n),
      .sel_dynamic_ext                        (sel_dynamic_ext),
      .sel_dynamic_ext16                      (sel_dynamic_ext16),
      .sel_dynamic_ext8                       (sel_dynamic_ext8),
      .sel_memword                            (sel_memword),
      .sel_notb_x                             (sel_notb_x),
      .shiftValue                             (shiftValue),
      .skip_is_active                         (skip_is_active),
      .sload_add_with_b_control               (sload_add_with_b_control),
      .sreset_add_0                           (sreset_add_0),
      .sreset_add_x                           (sreset_add_x),
      .subinstruction_3                       (subinstruction_3),
      .test                                   (test),
      .trap_if_restore                        (trap_if_restore),
      .trap_if_save                           (trap_if_save),
      .trap_number                            (trap_number),
      .trap_request_overflow                  (trap_request_overflow),
      .trap_request_underflow                 (trap_request_underflow),
      .unqualified_custom_instruction_start_0 (unqualified_custom_instruction_start_0),
      .unqualified_custom_instruction_start_1 (unqualified_custom_instruction_start_1),
      .unqualified_custom_instruction_start_2 (unqualified_custom_instruction_start_2),
      .unqualified_custom_instruction_start_3 (unqualified_custom_instruction_start_3),
      .unqualified_custom_instruction_start_4 (unqualified_custom_instruction_start_4)
    );

  enet_nios_register_file the_enet_nios_register_file
    (
      .CWP              (CWP),
      .a_local          (a_local),
      .alu_result       (alu_result),
      .b_local          (b_local),
      .clk              (clk),
      .dest_cwp_4       (dest_cwp_4),
      .dest_local_4     (dest_local_4),
      .instruction_4    (instruction_4),
      .is_cancelled_4   (is_cancelled_4),
      .is_neutrino_4    (is_neutrino_4),
      .non_write_op     (non_write_op),
      .pipe_run         (pipe_run),
      .raw_reg_a        (raw_reg_a),
      .raw_reg_b        (raw_reg_b),
      .subinstruction_4 (subinstruction_4),
      .use_fresh_cwp    (use_fresh_cwp)
    );

  enet_nios_instruction_decoder the_enet_nios_instruction_decoder
    (
      .N_update                               (N_update),
      .V_update                               (V_update),
      .a_field                                (a_field),
      .a_index_from_o7                        (a_index_from_o7),
      .a_index_from_sp                        (a_index_from_sp),
      .a_index_from_zero                      (a_index_from_zero),
      .b_field                                (b_field),
      .b_index_from_sp                        (b_index_from_sp),
      .b_index_from_zero                      (b_index_from_zero),
      .branch_offset                          (branch_offset),
      .byte_offset                            (byte_offset),
      .byterot_F_control                      (byterot_F_control),
      .c_is_borrow                            (c_is_borrow),
      .cancel_branch_delay_slot               (cancel_branch_delay_slot),
      .clk                                    (clk),
      .commit                                 (commit),
      .custom_instruction_start_mul           (custom_instruction_start_mul),
      .dest_index_from_o7                     (dest_index_from_o7),
      .dest_index_from_sp                     (dest_index_from_sp),
      .dest_index_from_zero                   (dest_index_from_zero),
      .do_byterot_1                           (do_byterot_1),
      .do_custom_instruction                  (do_custom_instruction),
      .do_dynamic_narrow_write                (do_dynamic_narrow_write),
      .do_fetch_vector                        (do_fetch_vector),
      .do_force_shift_0                       (do_force_shift_0),
      .do_force_shift_1                       (do_force_shift_1),
      .do_iABS                                (do_iABS),
      .do_iASRx_1                             (do_iASRx_1),
      .do_iLDx                                (do_iLDx),
      .do_iMOVHI                              (do_iMOVHI),
      .do_iMOVI                               (do_iMOVI),
      .do_iMSTEP_0                            (do_iMSTEP_0),
      .do_iMSTEP_1                            (do_iMSTEP_1),
      .do_iPFXx                               (do_iPFXx),
      .do_iRDCTL                              (do_iRDCTL),
      .do_iRESTORE                            (do_iRESTORE),
      .do_iRLC_1                              (do_iRLC_1),
      .do_iRRC_1                              (do_iRRC_1),
      .do_iRxC_0                              (do_iRxC_0),
      .do_iRxC_1                              (do_iRxC_1),
      .do_iSAVE                               (do_iSAVE),
      .do_iSEXT16                             (do_iSEXT16),
      .do_iSEXT8                              (do_iSEXT8),
      .do_iSKP0x                              (do_iSKP0x),
      .do_iSKP1x                              (do_iSKP1x),
      .do_iSKPRzx                             (do_iSKPRzx),
      .do_iSKPS                               (do_iSKPS),
      .do_iSKPx                               (do_iSKPx),
      .do_iSTx                                (do_iSTx),
      .do_iSWAP                               (do_iSWAP),
      .do_iTRAP_0                             (do_iTRAP_0),
      .do_iTRAP_n                             (do_iTRAP_n),
      .do_iTRET                               (do_iTRET),
      .do_iWRCTL                              (do_iWRCTL),
      .do_inv_all_b                           (do_inv_all_b),
      .do_narrow_stack_offset                 (do_narrow_stack_offset),
      .do_normal_offset                       (do_normal_offset),
      .do_normal_static_write                 (do_normal_static_write),
      .do_not_interrupt                       (do_not_interrupt),
      .do_save_offset                         (do_save_offset),
      .do_save_return_address                 (do_save_return_address),
      .do_stack_offset                        (do_stack_offset),
      .do_trap_offset                         (do_trap_offset),
      .do_write_16                            (do_write_16),
      .do_write_8                             (do_write_8),
      .force_carryin                          (force_carryin),
      .imm5                                   (imm5),
      .instruction_0                          (instruction_0),
      .instruction_1                          (instruction_1),
      .instruction_2                          (instruction_2),
      .instruction_3                          (instruction_3),
      .is_cancelled                           (is_cancelled),
      .is_eightie                             (is_eightie),
      .is_left_shift                          (is_left_shift),
      .is_neutrino                            (is_neutrino),
      .is_right_shift                         (is_right_shift),
      .is_sixteenie                           (is_sixteenie),
      .must_run_to_completion                 (must_run_to_completion),
      .narrow_stack_offset                    (narrow_stack_offset),
      .non_write_op                           (non_write_op),
      .op_b_from_2Ei5                         (op_b_from_2Ei5),
      .op_b_from_Ki5                          (op_b_from_Ki5),
      .op_b_from_reg_b                        (op_b_from_reg_b),
      .op_b_from_reg_or_const                 (op_b_from_reg_or_const),
      .op_is_LDx                              (op_is_LDx),
      .op_is_MOVHI                            (op_is_MOVHI),
      .op_is_MSTEP_0                          (op_is_MSTEP_0),
      .op_is_MSTEP_1                          (op_is_MSTEP_1),
      .op_is_PFX                              (op_is_PFX),
      .op_is_STx                              (op_is_STx),
      .op_is_TRAP_0                           (op_is_TRAP_0),
      .op_is_branch                           (op_is_branch),
      .op_is_jump                             (op_is_jump),
      .op_is_trap                             (op_is_trap),
      .op_jmpcall                             (op_jmpcall),
      .op_save_restore                        (op_save_restore),
      .op_subroutine                          (op_subroutine),
      .op_uses_a_pre_alu                      (op_uses_a_pre_alu),
      .op_uses_b_pre_alu                      (op_uses_b_pre_alu),
      .override_b_control                     (override_b_control),
      .p_field                                (p_field),
      .pipe_run                               (pipe_run),
      .prefix_payload                         (prefix_payload),
      .reg_not_modified                       (reg_not_modified),
      .request_stable_op_b                    (request_stable_op_b),
      .reset_n                                (reset_n),
      .save_offset                            (save_offset),
      .sel_dynamic_ext                        (sel_dynamic_ext),
      .sel_dynamic_ext16                      (sel_dynamic_ext16),
      .sel_dynamic_ext8                       (sel_dynamic_ext8),
      .sel_memword                            (sel_memword),
      .sel_notb_x                             (sel_notb_x),
      .skip_is_active                         (skip_is_active),
      .sload_add_with_b_control               (sload_add_with_b_control),
      .sreset_add_0                           (sreset_add_0),
      .sreset_add_x                           (sreset_add_x),
      .stack_offset                           (stack_offset),
      .subinstruction_1                       (subinstruction_1),
      .subinstruction_2                       (subinstruction_2),
      .test                                   (test),
      .trap_number                            (trap_number),
      .trap_offset                            (trap_offset),
      .unqualified_custom_instruction_start_0 (unqualified_custom_instruction_start_0),
      .unqualified_custom_instruction_start_1 (unqualified_custom_instruction_start_1),
      .unqualified_custom_instruction_start_2 (unqualified_custom_instruction_start_2),
      .unqualified_custom_instruction_start_3 (unqualified_custom_instruction_start_3),
      .unqualified_custom_instruction_start_4 (unqualified_custom_instruction_start_4),
      .use_fresh_cwp                          (use_fresh_cwp),
      .zero_all_b_control                     (zero_all_b_control)
    );

  assign is_neutrino_1 = is_neutrino;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          is_neutrino_2 <= 0;
      else if (pipe_run)
          is_neutrino_2 <= is_neutrino_1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          is_neutrino_3 <= 0;
      else if (pipe_run)
          is_neutrino_3 <= is_neutrino_2;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          is_neutrino_4 <= 0;
      else if (pipe_run)
          is_neutrino_4 <= is_neutrino_3;
    end


  assign dest_local_1 = dest_local;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dest_local_2 <= 0;
      else if (pipe_run)
          dest_local_2 <= dest_local_1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dest_local_3 <= 0;
      else if (pipe_run)
          dest_local_3 <= dest_local_2;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dest_local_4 <= 0;
      else if (pipe_run)
          dest_local_4 <= dest_local_3;
    end


  assign instruction_0 = instruction;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          instruction_1 <= 0;
      else if (commit)
          instruction_1 <= instruction_0;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          instruction_2 <= 0;
      else if (pipe_run)
          instruction_2 <= instruction_1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          instruction_3 <= 0;
      else if (pipe_run)
          instruction_3 <= instruction_2;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          instruction_4 <= 0;
      else if (pipe_run)
          instruction_4 <= instruction_3;
    end


  assign is_cancelled_1 = is_cancelled;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          is_cancelled_2 <= 0;
      else if (pipe_run)
          is_cancelled_2 <= is_cancelled_1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          is_cancelled_3 <= 0;
      else if (pipe_run)
          is_cancelled_3 <= is_cancelled_2;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          is_cancelled_4 <= 0;
      else if (pipe_run)
          is_cancelled_4 <= is_cancelled_3;
    end


  assign dest_cwp_2 = dest_cwp;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dest_cwp_3 <= 0;
      else if (pipe_run)
          dest_cwp_3 <= dest_cwp_2;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          dest_cwp_4 <= 0;
      else if (pipe_run)
          dest_cwp_4 <= dest_cwp_3;
    end


  assign subinstruction_1 = subinstruction;
  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          subinstruction_2 <= 0;
      else if (pipe_run)
          subinstruction_2 <= subinstruction_1;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          subinstruction_3 <= 0;
      else if (pipe_run)
          subinstruction_3 <= subinstruction_2;
    end


  always @(posedge clk or negedge reset_n)
    begin
      if (reset_n == 0)
          subinstruction_4 <= 0;
      else if (pipe_run)
          subinstruction_4 <= subinstruction_3;
    end


  assign committed_instruction = instruction_1;

endmodule


module enet_nios (
                   // inputs:
                    clk,
                    custom_instruction_result,
                    d_irq,
                    d_irqnumber,
                    d_readdata,
                    d_wait,
                    i_datavalid,
                    i_readdata,
                    i_wait,
                    reset_n,

                   // outputs:
                    custom_instruction_clk_en,
                    custom_instruction_dataa,
                    custom_instruction_datab,
                    custom_instruction_prefix,
                    custom_instruction_reset,
                    custom_instruction_start,
                    d_address,
                    d_byteenable,
                    d_read,
                    d_write,
                    d_writedata,
                    i_address,
                    i_flush,
                    i_read
                 )
  /* synthesis auto_dissolve = "FALSE" */ ;

  output           custom_instruction_clk_en;
  output  [ 31: 0] custom_instruction_dataa;
  output  [ 31: 0] custom_instruction_datab;
  output  [ 10: 0] custom_instruction_prefix;
  output           custom_instruction_reset;
  output  [  5: 0] custom_instruction_start;
  output  [ 22: 0] d_address;
  output  [  3: 0] d_byteenable;
  output           d_read;
  output           d_write;
  output  [ 31: 0] d_writedata;
  output  [ 22: 0] i_address;
  output           i_flush;
  output           i_read;
  input            clk;
  input   [ 31: 0] custom_instruction_result;
  input            d_irq;
  input   [  5: 0] d_irqnumber;
  input   [ 31: 0] d_readdata;
  input            d_wait;
  input            i_datavalid;
  input   [ 15: 0] i_readdata;
  input            i_wait;
  input            reset_n;

  wire             DC_EN;
  wire             IC_EN;
  wire    [ 15: 0] committed_instruction;
  wire             custom_instruction_clk_en;
  wire    [ 31: 0] custom_instruction_dataa;
  wire    [ 31: 0] custom_instruction_datab;
  wire    [ 10: 0] custom_instruction_prefix;
  wire             custom_instruction_reset;
  wire    [  5: 0] custom_instruction_start;
  wire    [ 22: 0] d_address;
  wire    [  3: 0] d_byteenable;
  wire             d_read;
  wire             d_write;
  wire    [ 31: 0] d_writedata;
  wire    [ 22: 0] dc_address;
  wire    [  3: 0] dc_byteenable;
  wire             dc_read;
  wire    [ 31: 0] dc_readdata;
  wire             dc_wait;
  wire             dc_write;
  wire    [ 31: 0] dc_writedata;
  wire             dummy_sink;
  wire    [ 22: 0] i_address;
  wire             i_flush;
  wire             i_read;
  wire    [ 22: 0] ic_address;
  wire             ic_datavalid;
  wire             ic_flush;
  wire             ic_read;
  wire    [ 15: 0] ic_readdata;
  wire    [ 15: 0] ic_readdata_to_cpu;
  wire             ic_wait;
  wire    [ 15: 0] instruction;
  wire             irq;
  wire    [  5: 0] irqnumber;
  enet_nios_pipeline the_enet_nios_pipeline
    (
      .DC_EN                     (DC_EN),
      .IC_EN                     (IC_EN),
      .clk                       (clk),
      .committed_instruction     (committed_instruction),
      .custom_instruction_clk_en (custom_instruction_clk_en),
      .custom_instruction_dataa  (custom_instruction_dataa),
      .custom_instruction_datab  (custom_instruction_datab),
      .custom_instruction_prefix (custom_instruction_prefix),
      .custom_instruction_reset  (custom_instruction_reset),
      .custom_instruction_result (custom_instruction_result),
      .custom_instruction_start  (custom_instruction_start),
      .dc_address                (dc_address),
      .dc_byteenable             (dc_byteenable),
      .dc_read                   (dc_read),
      .dc_readdata               (dc_readdata),
      .dc_wait                   (dc_wait),
      .dc_write                  (dc_write),
      .dc_writedata              (dc_writedata),
      .ic_address                (ic_address),
      .ic_datavalid              (ic_datavalid),
      .ic_flush                  (ic_flush),
      .ic_read                   (ic_read),
      .ic_readdata               (ic_readdata_to_cpu),
      .ic_wait                   (ic_wait),
      .irq                       (d_irq),
      .irqnumber                 (d_irqnumber),
      .reset_n                   (reset_n)
    );

  assign irq = d_irq;
  assign irqnumber = d_irqnumber;
  //dummy sink, which is an e_mux
  assign dummy_sink = IC_EN |
    DC_EN;

  assign ic_readdata_to_cpu = ic_readdata;
  //oci_core, which is an e_avalon_slave
  //data_master, which is an e_avalon_master
  //instruction_master, which is an e_avalon_master
  assign ic_datavalid = i_datavalid;
  assign ic_wait = i_wait;
  assign ic_readdata = i_readdata;
  assign i_address = ic_address;
  assign i_read = ic_read;
  assign i_flush = ic_flush;
  assign dc_wait = d_wait;
  assign dc_readdata = d_readdata;
  assign d_writedata = dc_writedata;
  assign d_address = dc_address;
  assign d_byteenable = dc_byteenable;
  assign d_read = dc_read;
  assign d_write = dc_write;
  //custom_instruction_master, which is an e_custom_instruction_master
  assign instruction = committed_instruction;

//synthesis translate_off
//////////////// SIMULATION-ONLY CONTENTS
  //i_readdata_display, which is an e_instance
  enet_nios_opcode_display_unit i_readdata_display
    (
      .instruction (i_readdata)
    );

  //instruction_display, which is an e_instance
  enet_nios_opcode_display_unit instruction_display
    (
      .instruction (instruction)
    );


//////////////// END SIMULATION-ONLY CONTENTS

//synthesis translate_on

endmodule

