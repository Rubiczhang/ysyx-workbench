`include "params.vh"

module pmem(
  input clk,
  input rst_n,

//instrution
  input       [ADDR_WIDTH-1: 0]       instr_addr_ifu_i,
  output reg  [DATA_WIDTH-1: 0]     instr_ifu_o,

  input       [DATA_WIDTH-1: 0]       pmem_reset_i [0: MEM_SIZE -1]
);

reg [DATA_WIDTH-1: 0]         mem[0:  MEM_SIZE-1];
wire [ADDR_WIDTH-1: 0]         mem_idx;

assign mem_idx = (instr_addr_ifu_i - MEM_BASE )/ 4;

assign instr_ifu_o = mem[mem_idx];
// always @(posedge clk) begin
//   instr_ifu_o <= mem[instr_addr_ifu_i];
// end


always @(posedge clk or rst_n) begin
  if(!rst_n) 
    mem = pmem_reset_i;
end
endmodule