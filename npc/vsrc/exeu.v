`include "params.vh"
module exeu(
  input clk,
  input rst_n,

  input   [DYN_INST_WIDTH-1       : 0]  dyn_instr_idu_i,


  //TO idu to write back register
  output [DATA_WIDTH-1:         0]         wrtbck_val_idu_o,
  output [REG_ADDR_WIDTH-1:     0]         rd_idu_o,
  output                                   wrtbck_en_idu_o
);

reg       [DATA_WIDTH-1:        0] alu_ans;
wire      [RD_LEN_STA-1:        0] rd;
wire      [ALUOP_LEN_DYN-1:     0] alu_op;
wire      [DATA_WIDTH-1:        0] imm;
wire      [DATA_WIDTH-1:        0] rs1_val;
wire      [DATA_WIDTH-1:        0] rs2_val;
wire                               use_rd;
wire                               use_rs1;
wire                               use_rs2;
wire                               use_imm;


wire      [DATA_WIDTH-1:        0]  alu_oprnd1;
wire      [DATA_WIDTH-1:        0]  alu_oprnd2;

assign  alu_op  =   dyn_instr_idu_i[ALUOP_HI_DYNOFF:  ALUOP_LO_DYNOFF];
assign  rs1_val =   dyn_instr_idu_i[RS1VAL_HI_DYNOFF: RS1VAL_LO_DYNOFF];
assign  rs2_val =   dyn_instr_idu_i[RS2VAL_HI_DYNOFF: RS2VAL_LO_DYNOFF];
assign  imm     =   dyn_instr_idu_i[IMM_HI_DYNOFF   : IMM_LO_DYNOFF];
assign  rd      =   dyn_instr_idu_i[RD_HI_DYNOFF    : RD_LO_DYNOFF];
assign  use_rd  =   dyn_instr_idu_i[USE_RD_DYNOFF];
assign  use_rs1  =   dyn_instr_idu_i[USE_RS1_DYNOFF];
assign  use_rs2  =   dyn_instr_idu_i[USE_RS2_DYNOFF];
assign  use_imm  =   dyn_instr_idu_i[USE_IMM_DYNOFF];

assign  rd_idu_o = rd;

assign wrtbck_val_idu_o = alu_ans;
assign wrtbck_en_idu_o  = use_rd;

//TODO
assign  alu_oprnd1 =  use_rs1 ? rs1_val: {DATA_WIDTH{1'b0}};
assign  alu_oprnd2 =  use_rs2 ? rs2_val: 
                      use_imm ? imm :
                      {DATA_WIDTH{1'b0}};


always @(*) begin
  case(alu_op)
    ADD_ALUOP:
      alu_ans = alu_oprnd1 + alu_oprnd2; 
    default: alu_ans = 0;
  endcase
end



endmodule