`include "params.vh"
module top(
  input clk,
  input rst_n
  // input [DATA_WIDTH-1:0] pmm_reset [0: MEM_SIZE-1]
  // input [7:0] pmm_reset [0: MEM_SIZE-1]

);

wire [ADDR_WIDTH-1: 0]     npc_icache_instr_addr;
wire [DATA_WIDTH-1 : 0]    icache_npc_instr;

wire  [ADDR_WIDTH-1 :   0]          npc_dcache_addr;
wire  [DATA_WIDTH-1:    0]          npc_dcache_st_dat;
wire  [LSWDTH_LSULEN-1: 0]          npc_dcache_ls_wdth;
wire                                npc_dcache_ls;
wire  [DATA_WIDTH-1 :   0]          dcache_npc_dat;
wire                                npc_dcache_use_pmem;

npc u_npc(
  .clk(clk),
  .rst_n(rst_n),

  .instr_addr_icache_o(npc_icache_instr_addr),
  .instr_icache_i(icache_npc_instr),

  .addr_dcache_o(npc_dcache_addr),
  .st_dat_dcache_o(npc_dcache_st_dat),
  .ls_wdth_dcache_o(npc_dcache_ls_wdth),
  .npc_use_pmem_dcache_o(npc_dcache_use_pmem),
  .ls_dcache_o(npc_dcache_ls),
  .dat_dcache_i(dcache_npc_dat)

);

pmem u_pmem(
  .clk(clk),
  .rst_n(rst_n),

  .instr_addr_ifu_i(npc_icache_instr_addr),
  .instr_ifu_o(icache_npc_instr),
  // .pmem_reset_i(pmm_reset),
  .addr_npc_i(npc_dcache_addr),
  .st_dat_npc_i(npc_dcache_st_dat),
  .ls_wdth_npc_i(npc_dcache_ls_wdth),
  .ls_npc_i(npc_dcache_ls),
  .npc_use_pmem_npc_i(npc_dcache_use_pmem),  
  .dcache_dat_npc_o(dcache_npc_dat)
  
);

endmodule
