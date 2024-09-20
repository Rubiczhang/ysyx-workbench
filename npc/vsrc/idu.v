`include "params.vh"

module idu(
  input clk,
  input rst_n,
  //from execute unit
  input  [RD_LEN_STA-1     : 0] rd_exeu_i,
  input  [DATA_WIDTH-1     : 0] wrtbck_dat_exeu_i,
  input                         wrtbck_en_exeu_i,

  //from ifu
  input  [INST_WIDTH-1     : 0] instr_ifu_i,
  input  [ADDR_WIDTH-1     : 0] pc_ifu_i,
  //to execute unit
  output [DYN_INST_WIDTH-1 : 0] dyn_instr_exu_o
);




wire    [OPCD_LEN_STA-1   : 0]     opcode;
wire    [FUNC3_LEN_STA-1  : 0]     func3;
wire    [FUNC7_LEN_STA-1  : 0]     func7;
wire                               is_func7_0x20;
wire                               is_func7_allzr;

wire    [RS1_LEN_STA -1   : 0]     rs1;
wire    [REG_DATA_WIDTH-1 : 0]     rs1_val;
reg                               use_rs1;
wire    [RS2_LEN_STA -1   : 0]     rs2;
wire    [REG_DATA_WIDTH-1 : 0]     rs2_val;
wire    [RD_LEN_STA - 1   : 0]     rd;
reg                               use_rd;
reg                               use_rs2;
reg   [DATA_WIDTH  -1   : 0]      imm;   
reg                               use_imm;
wire                               invld_instr;

wire    [DATA_WIDTH  -1   : 0]     imm_i_compu;     //imm computation i_type
wire    [DATA_WIDTH  -1   : 0]     imm_i_compu;     //imm computation i_type
wire    [DATA_WIDTH  -1   : 0]     imm_i_shamt;     // 添加 imm_i_shamt 的定义
wire    [DATA_WIDTH  -1   : 0]     imm_u;           // 添加 imm_u 的定义
wire    [DATA_WIDTH  -1   : 0]     imm_j;           // 添加 imm_j 的定义
wire    [DATA_WIDTH  -1   : 0]     imm_b;           // 添加 imm_b 的定义
wire    [DATA_WIDTH  -1   : 0]     imm_s;           // 添加 imm_s 的定义


reg     [INST_TYPE_WIDTH-1: 0]     instr_type;     
wire                               is_i_type;
reg    [ALUOP_LEN_DYN-1:   0]     alu_op;
reg    [FUID_LEN_DYN -1:   0] fu_id;





assign opcode = instr_ifu_i[OPCD_HI_STAOFF : OPCD_LO_STAOFF];
assign func3  = instr_ifu_i[FUNC3_HI_STAOFF: FUNC3_LO_STAOFF];
assign func7  = instr_ifu_i[FUNC7_HI_STAOFF: FUNC7_LO_STAOFF];
assign rs1    = instr_ifu_i[RS1_HI_STAOFF  : RS1_LO_STAOFF];
assign rs2    = instr_ifu_i[RS2_HI_STAOFF  : RS2_LO_STAOFF];
assign rd     = instr_ifu_i[RD_HI_STAOFF   : RD_LO_STAOFF];
assign invld_instr = (instr_type ==  INVLD_INSTR);

                          
assign imm_i_compu    = {{20{instr_ifu_i[31]}}, instr_ifu_i[31: 20]};    
assign imm_i_shamt    = {27'b0, instr_ifu_i[24:20]};
assign imm_u          = {instr_ifu_i[31:12], 12'b0};
assign imm_j          = {{12{instr_ifu_i[31]}}, instr_ifu_i[19:12], instr_ifu_i[20], instr_ifu_i[30:21], 1'b0};
assign imm_b          = {{20{instr_ifu_i[31]}}, instr_ifu_i[7], instr_ifu_i[30:25], instr_ifu_i[11:8], 1'b0};
assign imm_s          = {{20{instr_ifu_i[31]}}, instr_ifu_i[31:25], instr_ifu_i[11:7]};
assign is_func7_0x20 = (func7 == FUNC7_0x20);
assign is_func7_allzr = (func7 == FUNC7_ALLZR);

assign is_i_type = (instr_type == I_COMPU_INSTR || instr_type == I_SHIFT_INSTR);

assign dyn_instr_exu_o[RS1_HI_DYNOFF: RS1_LO_DYNOFF] = rs1;
assign dyn_instr_exu_o[RS2_HI_DYNOFF: RS2_LO_DYNOFF] = rs2;
assign dyn_instr_exu_o[RD_HI_DYNOFF: RD_LO_DYNOFF] =   rd;
assign dyn_instr_exu_o[RS1VAL_HI_DYNOFF     :   RS1VAL_LO_DYNOFF]  = rs1_val;
assign dyn_instr_exu_o[RS2VAL_HI_DYNOFF     :   RS2VAL_LO_DYNOFF]  = rs2_val;
assign dyn_instr_exu_o[IMM_HI_DYNOFF        :   IMM_LO_DYNOFF   ]  = imm;
assign dyn_instr_exu_o[FUID_HI_DYNOFF       :   FUID_LO_DYNOFF ]  = fu_id;
assign dyn_instr_exu_o[USE_RS1_DYNOFF]  =             use_rs1;
assign dyn_instr_exu_o[USE_RS2_DYNOFF]  =             use_rs2;
assign dyn_instr_exu_o[USE_IMM_DYNOFF]  =             use_imm;
assign dyn_instr_exu_o[INVLD_INSTR_DYNOFF]  =         invld_instr;
assign dyn_instr_exu_o[ALUOP_HI_DYNOFF      :   ALUOP_LO_DYNOFF]  = alu_op;
assign dyn_instr_exu_o[USE_RD_DYNOFF]  =    use_rd;
//typedecoder
always @(*) begin
  case (opcode)
    R_COMPU_OPCODE: begin
      if( (is_func7_0x20 && (func3 == SRA_FUNC3 || func3 == SUB_FUNC3))
          || is_func7_allzr)
        instr_type = R_COMPU_INSTR;
    end

    I_COMPU_OPCODE: begin
      if((func3 == SLLI_FUNC3 && is_func7_allzr) || 
        (func3 ==  SRAI_FUNC3 && is_func7_0x20))
        instr_type = I_SHIFT_INSTR;
      else if(func3 != SLLI_FUNC3 && func3 != SRAI_FUNC3) begin
        instr_type = I_COMPU_INSTR;
      end
    end

    default:
      instr_type = INVLD_INSTR;
  endcase
end



//ALU operation:
always @(*) begin
  alu_op = {ALUOP_LEN_DYN{1'b0}};
  if(instr_type == I_SHIFT_INSTR || instr_type == R_COMPU_INSTR) 
    alu_op = {{instr_ifu_i[30]}, instr_ifu_i[FUNC3_HI_STAOFF: FUNC3_LO_STAOFF]};
  else if(instr_type == I_COMPU_INSTR)
    alu_op = {1'b0, instr_ifu_i[FUNC3_HI_STAOFF: FUNC3_LO_STAOFF]};
end



// immidiate
always @(*) begin
  case (instr_type)
    I_COMPU_INSTR:      imm = imm_i_compu;
    I_SHIFT_INSTR:      imm = imm_i_shamt;
    default:            imm = 32'b0;
  endcase
end

// use_rs1, use_rs2, use_imm, fu_id, use_rd
always @(*) begin
  use_rd = 1;
  fu_id = FU_ALU;

  case (instr_type)
    I_COMPU_INSTR, I_SHIFT_INSTR: begin
      {use_rs1, use_rs2, use_imm} = 3'b101;
    end

    INVLD_INSTR: begin
      {use_rs1, use_rs2, use_imm} = 3'b000;
      use_rd = 0;
    end
    default:  {use_rs1, use_rs2, use_imm} = 3'b000;
  endcase
end

registerFile #(
  .ADDR_WIDTH(REG_ADDR_WIDTH),
  .DATA_WIDTH(REG_DATA_WIDTH)
) u_regFile (
  .clk(clk),
  .rst_n(rst_n),
  .rs1_i(rs1),
  .rs2_i(rs2),
  .rd_i(rd_exeu_i),
  .dat_i(wrtbck_dat_exeu_i),
  .w_en_i(wrtbck_en_exeu_i),

  .rs1_val_o(rs1_val),
  .rs2_val_o(rs2_val)
);

  
endmodule

