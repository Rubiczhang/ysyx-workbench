`include "params.vh"
module top(
  input clk,
  input rst_n,
  // input [DATA_WIDTH-1:0] pmm_reset [0: MEM_SIZE-1]
  input [7:0] pmm_reset [0: MEM_SIZE-1]

);

wire [ADDR_WIDTH-1: 0]     npc_icache_instr_addr;
wire [DATA_WIDTH-1 : 0]    icache_npc_instr;


npc u_npc(
  .clk(clk),
  .rst_n(rst_n),

  .instr_addr_icache_o(npc_icache_instr_addr),
  .instr_icache_i(icache_npc_instr)
);

pmem u_pmem(
  .clk(clk),
  .rst_n(rst_n),

  .instr_addr_ifu_i(npc_icache_instr_addr),
  .instr_ifu_o(icache_npc_instr),
  .pmem_reset_i(pmm_reset)
);

endmodule