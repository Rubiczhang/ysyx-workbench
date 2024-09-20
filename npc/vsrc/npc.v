// import "DPI-C" function string getenv (input string env_name);

module npc
(
  input clk,
  input rst_n,
  output [ADDR_WIDTH-1: 0]     instr_addr_icache_o,
  input  [DATA_WIDTH-1 : 0]    instr_icache_i

);

wire     [ADDR_WIDTH-1: 0]     ifu_idu_pc;
wire     [INST_WIDTH-1: 0]     ifu_idu_instr;

wire     [DYN_INST_WIDTH-1: 0] idu_exeu_dyn_instr;
wire     [DATA_WIDTH-1: 0]     exeu_idu_wrtbck_val;
wire     [REG_ADDR_WIDTH-1: 0] exeu_idu_rd;
wire                           exeu_idu_wrtbck_en;

ifu u_ifu(
  .clk(clk),
  .rst_n(rst_n),

  .instr_addr_icache_o(instr_addr_icache_o),
  .instr_icache_i(instr_icache_i),
  .pc_idu_o(ifu_idu_pc),
  .instr_idu_o(ifu_idu_instr)
);


idu u_idu(
  .clk(clk),
  .rst_n(rst_n),
  //from execute unit
  .rd_exeu_i(exeu_idu_rd),
  .wrtbck_dat_exeu_i(exeu_idu_wrtbck_val),
  .wrtbck_en_exeu_i(exeu_idu_wrtbck_en),

  //from ifu
  .instr_ifu_i(ifu_idu_instr),
  .pc_ifu_i(ifu_idu_pc),
    
  //to execute unit
  .dyn_instr_exu_o(idu_exeu_dyn_instr)
);

exeu u_exeu(
  .clk(clk),
  .rst_n(rst_n),

  //from idu
  .dyn_instr_idu_i(idu_exeu_dyn_instr),

  //to idu to write back register
  .wrtbck_val_idu_o(exeu_idu_wrtbck_val),
  .rd_idu_o(exeu_idu_rd),
  .wrtbck_en_idu_o(exeu_idu_wrtbck_en)
);

endmodule
