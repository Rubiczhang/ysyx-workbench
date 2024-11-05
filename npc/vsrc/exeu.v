`include "params.vh"
module exeu(
  input clk,
  input rst_n,

  input   [DYN_INST_WIDTH-1       : 0]  dyn_instr_idu_i,


  //TO idu to write back register
  output [DATA_WIDTH-1:         0]         wrtbck_val_idu_o,
  output [REG_ADDR_WIDTH-1:     0]         rd_idu_o,
  output                                   wrtbck_en_idu_o,
  
  //TO ifu to update pc and btb
  output [ADDR_WIDTH-1:         0]         pc_act_trgt_ifu_o
);

`include "include/exeu_dpi.vh"

// export  "DPI-C" task readExeuPc;
// task readExeuPc;
//   output int ret_exeu_pc;
//   assign ret_exeu_pc = exeu_pc;
// endtask

reg       [DATA_WIDTH-1:        0] alu_ans;
reg       [ADDR_WIDTH-1:        0] pc_act_trgt;
reg       [ADDR_WIDTH-1:        0] pc_trgt;
reg       [ADDR_WIDTH-1:        0] base_addr;
reg                                pick_pc_trgt;
wire      [FUNC3_LEN-1:         0] func3;
wire      [RD_LEN_STA-1:        0] rd;
wire      [ALUOP_LEN_DYN-1:     0] alu_op;
wire      [DATA_WIDTH-1:        0] imm;
wire      [PC_WIDTH-1:        0]   exeu_pc;
wire      [PC_WIDTH-1:        0]   exeu_snpc;
wire      [DATA_WIDTH-1:        0] rs1_val;
wire      [DATA_WIDTH-1:        0] rs2_val;
wire                               exeu_use_rd;
wire                               use_rs1;
wire                               use_rs2;
// wire                               use_pc;
// wire                               use_imm;
wire     [INST_TYPE_WIDTH-1:    0] instr_type;
reg                                 instr_j_or_jalr;

reg                                 cmp_result;
reg                                 br_taken;

reg       [DATA_WIDTH-1:        0]  alu_oprd1;
reg       [DATA_WIDTH-1:        0]  alu_oprd2;

reg       [ALU_OPRD1_LEN_DYN-1: 0]  alu_oprd1_type;
reg       [ALU_OPRD2_LEN_DYN-1: 0]  alu_oprd2_type;

assign  alu_op  =   dyn_instr_idu_i[ALUOP_HI_DYNOFF:  ALUOP_LO_DYNOFF];
assign  rs1_val =   dyn_instr_idu_i[RS1VAL_HI_DYNOFF: RS1VAL_LO_DYNOFF];
assign  rs2_val =   dyn_instr_idu_i[RS2VAL_HI_DYNOFF: RS2VAL_LO_DYNOFF];
assign  imm     =   dyn_instr_idu_i[IMM_HI_DYNOFF   : IMM_LO_DYNOFF];
assign  exeu_pc     =   dyn_instr_idu_i[PC_HI_DYNOFF   : PC_LO_DYNOFF];
assign  rd      =   dyn_instr_idu_i[RD_HI_DYNOFF    : RD_LO_DYNOFF];
assign  func3    =   dyn_instr_idu_i[FUNC3_HI_DYNOFF:  FUNC3_LO_DYNOFF];
assign  instr_type = dyn_instr_idu_i[INSTYPE_HI_DYNOFF: INSTYPE_LO_DYNOFF];
assign  exeu_use_rd  =   dyn_instr_idu_i[USE_RD_DYNOFF];
assign  use_rs1  =   dyn_instr_idu_i[USE_RS1_DYNOFF];
assign  use_rs2  =   dyn_instr_idu_i[USE_RS2_DYNOFF];
// assign  use_pc  =   dyn_instr_idu_i[USE_PC_DYNOFF];
assign  alu_oprd1_type = dyn_instr_idu_i[ALU_OPRD1_HI_DYNOFF: ALU_OPRD1_LO_DYNOFF];
assign  alu_oprd2_type = dyn_instr_idu_i[ALU_OPRD2_HI_DYNOFF: ALU_OPRD2_LO_DYNOFF];
// assign  use_imm  =   dyn_instr_idu_i[USE_IMM_DYNOFF];


assign  exeu_snpc   = exeu_pc + 4;
assign  rd_idu_o = rd;
assign  instr_j_or_jalr = (instr_type == I_JALR_INSTR || instr_type == J_INSTR);

assign wrtbck_val_idu_o = instr_j_or_jalr ? exeu_snpc : alu_ans;
assign wrtbck_en_idu_o  = exeu_use_rd;
assign pc_act_trgt_ifu_o = pc_act_trgt;

// alu_oprd1, alu_oprd2
always @(*) begin
  case (alu_oprd1_type)
    ALU_OPRD1_RS1:
      alu_oprd1 = rs1_val;
    ALU_OPRD1_PC:
      alu_oprd1 = exeu_pc;
    default: 
      alu_oprd1 = {DATA_WIDTH{1'b0}};
  endcase

  case (alu_oprd2_type)
    ALU_OPRD2_RS2:
      alu_oprd2 = rs2_val;
    ALU_OPRD2_IMM:
      alu_oprd2 = imm;
    ALU_OPRD2_4:
      alu_oprd2 = {{(DATA_WIDTH-3){1'b0}}, 3'b100};
    default:
      alu_oprd2 = {DATA_WIDTH{1'b0}};
  endcase
  // assign  alu_oprd1_type =  ALU_OPRD1_RS1 ? rs1_val :
  //                       use_pc  ? exeu_pc : {DATA_WIDTH{1'b0}};
  // assign  alu_oprd2_type =  use_rs2 ? rs2_val: 
  //                       use_imm ? imm :
  //                       {DATA_WIDTH{1'b0}};

end

//branch decision module
always @(*) begin
  case(func3[2:1])
    B_CMPOP_EQ:
      cmp_result = (rs1_val == rs2_val);
    B_CMPOP_LT:
      cmp_result = ($signed(rs1_val) < $signed(rs2_val));
    B_CMPOP_LTU:
      cmp_result = ($unsigned(rs1_val) < $unsigned(rs2_val));
    default:
      cmp_result = 0;
  endcase 
  if(func3[0] == B_TK_CMP) begin  //Branch take cmp_result
    br_taken = cmp_result;
  end else begin
    br_taken = !cmp_result;
  end
end


alu u_alu(
  .alu_oprd1(alu_oprd1),
  .alu_oprd2(alu_oprd2),
  .alu_op(alu_op),
  .alu_ans(alu_ans)
);

// //ALU module
// always @(*) begin
//   case(alu_op)
//     ADD_ALUOP:
//       alu_ans = alu_oprd1 + alu_oprd2; 
//     SUB_ALUOP:
//       alu_ans = alu_oprd1 - alu_oprd2;
//     SLL_ALUOP: 
//       alu_ans = alu_oprd1 << alu_oprd2[4:0];
//     SLT_ALUOP:
//       alu_ans = $signed(alu_oprd1) < $signed(alu_oprd2) ? 32'b0001: 32'b0;
//     SLTU_ALUOP:
//       alu_ans = $unsigned(alu_oprd1) < $unsigned(alu_oprd2) ? 32'b0001: 32'b0;
//     XOR_ALUOP:
//       alu_ans = alu_oprd1 ^ alu_oprd2;
//     SRL_ALUOP:
//       alu_ans = $unsigned(alu_oprd1) >> alu_oprd2[4:0];
//     SRA_ALUOP:
//       alu_ans = $signed(alu_oprd1) >>> alu_oprd2[4:0];
//     OR_ALUOP:
//       alu_ans = alu_oprd1 | alu_oprd2;
//     AND_ALUOP:
//       alu_ans = alu_oprd1 & alu_oprd2;
//     default: alu_ans = 0;
//   endcase
// end

//Branch address module
//  pick_pc_trgt
always @(*) begin
  pick_pc_trgt = 0;
  if(instr_j_or_jalr) begin
    pick_pc_trgt = 1;
  end else if(instr_type == B_INSTR) begin
    pick_pc_trgt = br_taken;
  end
end

//  pc_trgt
assign pc_act_trgt = pick_pc_trgt ? alu_ans : exeu_snpc;

endmodule
