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
  output reg [DYN_INST_WIDTH-1 : 0] dyn_instr_exu_o,

  //from/to lsu
  input  [RD_LEN_STA-1     : 0] rd_lsu_i,
  input  [DATA_WIDTH-1     : 0] wrtbck_dat_lsu_i,
  input                         wrtbck_en_lsu_i,
  output reg [LSU_PKG_WIDTH -1 : 0] lsu_pkg_lsu_o

);

import "DPI-C" function void handle_ebreak();

// export "DPI-C" task     readGpr;

// task readGpr;
//   input int idx;
//   output int reg_val;
//   assign reg_val = u_regFile
// endtask

wire    [OPCD_LEN_STA-1   : 0]     opcode;
wire    [FUNC3_LEN_STA-1  : 0]     func3;
wire    [FUNC7_LEN_STA-1  : 0]     func7;
wire                               is_func7_0x20;
wire                               is_func7_allzr;

wire    [RS1_LEN_STA -1   : 0]      rs1;
wire    [REG_DATA_WIDTH-1 : 0]      rs1_val;
reg                                 use_rs1;
// reg                                 use_pc;   
wire    [RS2_LEN_STA -1   : 0]      rs2;
wire    [REG_DATA_WIDTH-1 : 0]      rs2_val;
wire    [RD_LEN_STA - 1   : 0]      rd;
reg                                 exeu_use_rd;
reg                                 use_rs2;
reg   [DATA_WIDTH  -1   : 0]        imm;   
// reg                                 use_imm;
wire                                invld_instr;

wire    [DATA_WIDTH  -1   :   0]      imm_i_compu;     //imm computation i_type
// wire    [DATA_WIDTH  -1     : 0]      imm_i_compu;     //imm computation i_type
wire    [DATA_WIDTH  -1   :   0]      imm_i_shamt;     // 添加 imm_i_shamt 的定义
wire    [DATA_WIDTH  -1   :   0]      imm_u;           // 添加 imm_u 的定义
wire    [DATA_WIDTH  -1   :   0]      imm_j;           // 添加 imm_j 的定义
wire    [DATA_WIDTH  -1   :   0]      imm_b;           // 添加 imm_b 的定义
wire    [DATA_WIDTH  -1   :   0]      imm_s;           // 添加 imm_s 的定义

wire    [DATA_WIDTH - 1   :   0]     wrb_dat;
wire                                 wrb_en;
wire    [RD_LEN_STA - 1   :   0]     wrb_rd;


reg     [INST_TYPE_WIDTH-1:   0]      instr_type;     
wire                                  is_i_type;
reg     [ALUOP_LEN_DYN-1:     0]      alu_op;
reg     [FUID_LEN_DYN -1:     0]      fu_id;
reg     [ALU_OPRD1_LEN_DYN-1: 0]      alu_oprd1_type;
reg     [ALU_OPRD2_LEN_DYN-1: 0]      alu_oprd2_type;

reg                                   lsu_ls;
reg                                   use_lsu;


assign opcode   = instr_ifu_i[OPCD_HI_STAOFF : OPCD_LO_STAOFF];
assign func3    = instr_ifu_i[FUNC3_HI_STAOFF: FUNC3_LO_STAOFF];
assign func7    = instr_ifu_i[FUNC7_HI_STAOFF: FUNC7_LO_STAOFF];
assign rs1      = instr_ifu_i[RS1_HI_STAOFF  : RS1_LO_STAOFF];
assign rs2      = instr_ifu_i[RS2_HI_STAOFF  : RS2_LO_STAOFF];
assign rd       = instr_ifu_i[RD_HI_STAOFF   : RD_LO_STAOFF];
assign invld_instr = (instr_type ==  INVLD_INSTR);

assign lsu_pkg_lsu_o[ADRBASE_HI_LSUOFF:ADRBASE_LO_LSUOFF] = rs1_val;
assign lsu_pkg_lsu_o[ADROFF_HI_LSUOFF:  ADROFF_LO_LSUOFF] = imm[11:0];
assign lsu_pkg_lsu_o[PC_HI_LSUOFF:          PC_LO_LSUOFF] = pc_ifu_i;
assign lsu_pkg_lsu_o[RD_HI_LSUOFF:          RD_LO_LSUOFF] = rd;
assign lsu_pkg_lsu_o[LSWDTH_HI_LSUOFF:  LSWDTH_LO_LSUOFF] = instr_ifu_i[LSWDTH_HI_STAOFF: LSWDTH_LO_STAOFF];
assign lsu_pkg_lsu_o[ST_DAT_HI_LSUOFF:  ST_DAT_LO_LSUOFF] = rs2_val;
assign lsu_pkg_lsu_o[USE_LSU_LSUOFF                     ] = use_lsu;
assign lsu_pkg_lsu_o[LSU_LS_LSUOFF                      ] = lsu_ls;
assign lsu_pkg_lsu_o[SIGNED_LOAD_LSUOFF                 ] = instr_ifu_i[SIGN_LOAD_STAOFF];

assign imm_i_compu    = {{20{instr_ifu_i[31]}}, instr_ifu_i[31: 20]};    
assign imm_i_shamt    = {27'b0, instr_ifu_i[24:20]};
assign imm_u          = {instr_ifu_i[31:12], 12'b0};
assign imm_j          = {{12{instr_ifu_i[31]}}, instr_ifu_i[19:12], instr_ifu_i[20], instr_ifu_i[30:21], 1'b0};
assign imm_b          = {{20{instr_ifu_i[31]}}, instr_ifu_i[7], instr_ifu_i[30:25], instr_ifu_i[11:8], 1'b0};
assign imm_s          = {{20{instr_ifu_i[31]}}, instr_ifu_i[31:25], instr_ifu_i[11:7]};
assign is_func7_0x20 = (func7 == FUNC7_0x20);
assign is_func7_allzr = (func7 == FUNC7_ALLZR);

assign is_i_type = (instr_type == I_COMPU_INSTR || instr_type == I_SHIFT_INSTR
                    || instr_type == I_ECALL_INSTR || instr_type == I_EBREAK_INSTR);

//set dyn_instr_exu_o
always @(*) begin
  if(!rst_n) begin
    dyn_instr_exu_o = {DYN_INST_WIDTH{1'b0}};
  end else begin
    dyn_instr_exu_o[RS1_HI_DYNOFF: RS1_LO_DYNOFF] = rs1;
    dyn_instr_exu_o[RS2_HI_DYNOFF: RS2_LO_DYNOFF] = rs2;
    dyn_instr_exu_o[RD_HI_DYNOFF: RD_LO_DYNOFF] =   rd;
    dyn_instr_exu_o[FUNC3_HI_DYNOFF: FUNC3_LO_DYNOFF] = func3;
    dyn_instr_exu_o[RS1VAL_HI_DYNOFF     :   RS1VAL_LO_DYNOFF]  = rs1_val;
    dyn_instr_exu_o[RS2VAL_HI_DYNOFF     :   RS2VAL_LO_DYNOFF]  = rs2_val;
    dyn_instr_exu_o[IMM_HI_DYNOFF        :   IMM_LO_DYNOFF   ]  = imm;
    dyn_instr_exu_o[PC_HI_DYNOFF        :    PC_LO_DYNOFF   ]  =  pc_ifu_i;
    dyn_instr_exu_o[FUID_HI_DYNOFF       :   FUID_LO_DYNOFF ]  = fu_id;
    dyn_instr_exu_o[INSTYPE_HI_DYNOFF: INSTYPE_LO_DYNOFF]      = instr_type;
    dyn_instr_exu_o[USE_RS1_DYNOFF]  =              use_rs1;
    dyn_instr_exu_o[USE_RS2_DYNOFF]  =              use_rs2;
    dyn_instr_exu_o[USE_RD_DYNOFF]  =               exeu_use_rd;
    // dyn_instr_exu_o[USE_PC_DYNOFF]  =             use_pc;
    // dyn_instr_exu_o[USE_IMM_DYNOFF]  =             use_imm;
    dyn_instr_exu_o[INVLD_INSTR_DYNOFF]  =         invld_instr;
    dyn_instr_exu_o[ALUOP_HI_DYNOFF      :   ALUOP_LO_DYNOFF]  = alu_op;
    dyn_instr_exu_o[ALU_OPRD1_HI_DYNOFF  :    ALU_OPRD1_LO_DYNOFF]  =alu_oprd1_type;
    dyn_instr_exu_o[ALU_OPRD2_HI_DYNOFF  :    ALU_OPRD2_LO_DYNOFF]  =alu_oprd2_type;
  end
end

//typedecoder 
always @(*) begin

  instr_type = INVLD_INSTR;

  case (opcode)
    R_COMPU_OPCODE: begin
      if( (is_func7_0x20 && (func3 == SRA_FUNC3 || func3 == SUB_FUNC3))
          || is_func7_allzr)
        instr_type = R_COMPU_INSTR;
    end

    I_COMPU_OPCODE: begin
      if((func3 == SLLI_FUNC3 && is_func7_allzr) || 
        (func3 ==  SRAI_FUNC3 && is_func7_0x20)  ||
        (func3 ==  SRLI_FUNC3 && is_func7_allzr))
        instr_type = I_SHIFT_INSTR;
      else if(func3 != SLLI_FUNC3 && func3 != SRAI_FUNC3) begin
        instr_type = I_COMPU_INSTR;
      end
    end

    I_E_OPCODE:     begin
      if(instr_ifu_i[31:7] == I_ECALL_HIGHER)
        instr_type = I_ECALL_INSTR;
      else if(instr_ifu_i[31:7] == I_EBREAK_HIGHER) begin
        instr_type = I_EBREAK_INSTR;
        handle_ebreak();
      end
    end

    U_LUI_OPCODE: begin
      instr_type = U_LUI_INSTR;
    end


    U_AUIPC_OPCODE: begin
      instr_type = U_AUIPC_INSTR;
    end

    J_OPCODE: begin
      instr_type = J_INSTR;
    end

    I_JALR_OPCODE: begin
      instr_type = I_JALR_INSTR;
    end

    I_LOAD_OPCODE: begin
      instr_type = I_LOAD_INSTR;
    end

    S_OPCODE: begin
      instr_type = S_INSTR;
    end

    B_OPCODE: begin
      instr_type = B_INSTR;
    end
    default:
      instr_type = INVLD_INSTR;
  endcase
end


//LSU operation:
//  lsu_ls, use_lsu
always @(*) begin
  lsu_ls =  0;
  use_lsu = 0;
  if(instr_type == I_LOAD_INSTR) begin
    lsu_ls = LS_LOAD;
    use_lsu = 1;
  end else if(instr_type == S_INSTR) begin
    lsu_ls = LS_STOR;
    use_lsu = 1;
  end
end

//ALU operation:
always @(*) begin

  if(instr_type == I_SHIFT_INSTR || instr_type == R_COMPU_INSTR) 
    alu_op = {{instr_ifu_i[30]}, instr_ifu_i[FUNC3_HI_STAOFF: FUNC3_LO_STAOFF]};
  else if(instr_type == I_COMPU_INSTR)
    alu_op = {1'b0, instr_ifu_i[FUNC3_HI_STAOFF: FUNC3_LO_STAOFF]};
  else //load, store, alu_ans is ls address
       //branch, jalr, jal: alu_ans = trgt_pc
    alu_op = {ALUOP_LEN_DYN{1'b0}};
end



// immidiate
always @(*) begin
  case (instr_type)
    I_COMPU_INSTR, I_JALR_INSTR, I_LOAD_INSTR:
                                      imm = imm_i_compu;
    I_SHIFT_INSTR:                    imm = imm_i_shamt;
    U_AUIPC_INSTR:                    imm = imm_u;
    U_LUI_INSTR:                      imm = imm_u;
    J_INSTR:                          imm = imm_j;
    S_INSTR:                          imm = imm_s;
    B_INSTR:                          imm = imm_b;
    default:                          imm = 32'b0;
  endcase
end


// alu_oprd(1/2)_type
always @(*) begin
  exeu_use_rd = 1;
  fu_id = FU_ALU;
  alu_oprd1_type = {ALU_OPRD1_LEN_DYN{1'b0}};
  alu_oprd2_type = {ALU_OPRD2_LEN_DYN{1'b0}};
  {use_rs1, use_rs2} = 2'b00;

  case (instr_type)
    I_COMPU_INSTR, I_SHIFT_INSTR: begin
      {use_rs1, use_rs2} = 2'b10;
      alu_oprd1_type = ALU_OPRD1_RS1;
      alu_oprd2_type = ALU_OPRD2_IMM;
    end

    U_LUI_INSTR: begin
      {use_rs1, use_rs2} = 2'b00;
      alu_oprd1_type = ALU_OPRD1_ZR;
      alu_oprd2_type = ALU_OPRD2_IMM;
    end

    U_AUIPC_INSTR: begin
      {use_rs1, use_rs2} = 2'b00;
      alu_oprd1_type = ALU_OPRD1_PC;
      alu_oprd2_type = ALU_OPRD2_IMM;
    end

    I_JALR_INSTR: begin
      {use_rs1, use_rs2} = 2'b10;
      alu_oprd1_type = ALU_OPRD1_RS1;
      alu_oprd2_type = ALU_OPRD2_IMM;
    end

    J_INSTR: begin
      {use_rs1, use_rs2} = 2'b00;
      alu_oprd1_type = ALU_OPRD1_PC;
      alu_oprd2_type = ALU_OPRD2_IMM;
    end
    B_INSTR: begin
      {use_rs1, use_rs2} = 2'b11;
      alu_oprd1_type = ALU_OPRD1_PC;
      alu_oprd2_type = ALU_OPRD2_IMM;
      exeu_use_rd = 0;
    end
    S_INSTR, I_LOAD_INSTR: begin
      exeu_use_rd = 0;
    end

    // INVLD_INSTR: begin
    //   {use_rs1, use_rs2, use_imm} = 3'b000;
    //   exeu_use_rd = 0;
    // end
    default: begin
      {use_rs1, use_rs2} = 2'b00;
    end
  endcase
  
end

assign wrb_dat = wrtbck_en_exeu_i ? wrtbck_dat_exeu_i : wrtbck_dat_lsu_i;
assign wrb_en  = wrtbck_en_lsu_i | wrtbck_en_exeu_i;
assign wrb_rd  =  wrtbck_en_exeu_i ? rd_exeu_i : rd_lsu_i;
          

registerFile #(
  .ADDR_WIDTH(REG_ADDR_WIDTH),
  .DATA_WIDTH(REG_DATA_WIDTH)
) u_regFile (
  .clk(clk),
  .rst_n(rst_n),
  .rs1_i(rs1),
  .rs2_i(rs2),
  .rd_i(wrb_rd),
  .dat_i(wrb_dat),
  .w_en_i(wrb_en),

  .rs1_val_o(rs1_val),
  .rs2_val_o(rs2_val)
);

  
endmodule

