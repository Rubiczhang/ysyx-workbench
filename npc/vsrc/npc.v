// import "DPI-C" function string getenv (input string env_name);

module npc
(
  input clk,
  input rst_n,
  output [ADDR_WIDTH-1: 0]     instr_addr_icache_o,
  input  [DATA_WIDTH-1 : 0]    instr_icache_i,

  output[ADDR_WIDTH-1 :   0]          addr_dcache_o,
  output[DATA_WIDTH-1:    0]          st_dat_dcache_o,
  output[LSWDTH_LSULEN-1: 0]          ls_wdth_dcache_o,
  output                              npc_use_pmem_dcache_o,
  output                              ls_dcache_o,
  input [DATA_WIDTH-1 :   0]          dat_dcache_i
);

wire     [ADDR_WIDTH-1: 0]     ifu_idu_pc;
wire     [INST_WIDTH-1: 0]     ifu_idu_instr;

wire     [DYN_INST_WIDTH-1: 0] idu_exeu_dyn_instr;
wire     [DATA_WIDTH-1: 0]     exeu_idu_wrtbck_val;
wire     [REG_ADDR_WIDTH-1: 0] exeu_idu_rd;
wire                           exeu_idu_wrtbck_en;
wire     [ADDR_WIDTH-1: 0]     exeu_idu_pc_act_trgt;

wire     [RD_LEN_STA-1: 0]     ifu_idu_rd;
wire     [DATA_WIDTH-1     : 0] lsu_idu_wrtbck_dat;
wire                            lsu_idu_wrtbck_en;
wire     [LSU_PKG_WIDTH -1 : 0] idu_lsu_lsu_pkg;

ifu u_ifu(
  .clk(clk),
  .rst_n(rst_n),

  .instr_addr_icache_o(instr_addr_icache_o),
  .instr_icache_i(instr_icache_i),

  .pc_act_trgt_exeu_i(exeu_idu_pc_act_trgt),
  
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
  .dyn_instr_exu_o(idu_exeu_dyn_instr),

  //from/to lsu
  .rd_lsu_i(ifu_idu_rd),
  .wrtbck_dat_lsu_i(lsu_idu_wrtbck_dat),
  .wrtbck_en_lsu_i(lsu_idu_wrtbck_en),
  .lsu_pkg_lsu_o(idu_lsu_lsu_pkg)
`ifdef VERILATOR

`endif
);

exeu u_exeu(
  .clk(clk),
  .rst_n(rst_n),

  //from idu
  .dyn_instr_idu_i(idu_exeu_dyn_instr),

  //to idu to write back register
  .wrtbck_val_idu_o(exeu_idu_wrtbck_val),
  .rd_idu_o(exeu_idu_rd),
  .wrtbck_en_idu_o(exeu_idu_wrtbck_en),

  //TO ifu to update pc and btb
  .pc_act_trgt_ifu_o(exeu_idu_pc_act_trgt)
`ifdef VERILATOR

`endif
);

lsu u_lsu(
  .clk(clk),
  .rst_n(rst_n), 


  .addr_dcache_o(addr_dcache_o),
  .st_dat_dcache_o(st_dat_dcache_o),
  .ls_wdth_dcache_o(ls_wdth_dcache_o),
  .use_pmem_dcache_o(npc_use_pmem_dcache_o),
  .ls_dcache_o(ls_dcache_o),
  .dat_dcache_i(dat_dcache_i),

  .lsu_pkg_idu_i(idu_lsu_lsu_pkg),
  
  
  .wrtbck_dat_idu_o(lsu_idu_wrtbck_dat),
  .rd_idu_o(ifu_idu_rd),
  .wrtbck_en_idu_o(lsu_idu_wrtbck_en)

);

endmodule
