`include "params.vh"
import "DPI-C" function int paddr_read(input int addr, input int len);
import "DPI-C" function void paddr_write(input int addr, input int len, input int data);

module pmem(
  input clk,
  input rst_n,

//instrution
  input       [ADDR_WIDTH-1: 0]       instr_addr_ifu_i,
  output reg  [DATA_WIDTH-1: 0]       instr_ifu_o,

  // input       [DATA_WIDTH: 0]       pmem_reset_i [0: MEM_SIZE -1]
  // input       [7:             0]      pmem_reset_i [0: MEM_SIZE - 1],

 
  input  [ADDR_WIDTH-1 :   0]          addr_npc_i,
  input  [DATA_WIDTH-1:    0]          st_dat_npc_i,
  input  [LSWDTH_LSULEN-1: 0]          ls_wdth_npc_i,
  input                                ls_npc_i  ,
  input                                npc_use_pmem_npc_i,  
  output [DATA_WIDTH-1:    0]          dcache_dat_npc_o
);



// reg [DATA_WIDTH-1: 0]         mem[0:  MEM_SIZE-1];
reg [7: 0]         mem[0:  MEM_SIZE-1];
// wire [ADDR_WIDTH-1: 0]         mem_idx;

// assign mem_idx = (instr_addr_ifu_i - MEM_BASE )/ 4;

// assign instr_ifu_o = mem[mem_idx];
// assign instr_ifu_o = {mem[mem_idx*4+3], mem[mem_idx*4 + 2],  mem[mem_idx*4+1], mem[mem_idx*4]};
// $display()
// assign instr_ifu_o = (!rst_n) ? 32'b0 : paddr_read( $unsigned(instr_addr_ifu_i), 4);

always @(*) begin
  // $display("instr_addr_ifu_i %0x", instr_addr_ifu_i);
  instr_ifu_o =  paddr_read( $unsigned(instr_addr_ifu_i), 4);
end


always @(*) begin
  if(npc_use_pmem_npc_i && ls_npc_i == LS_LOAD) begin
    dcache_dat_npc_o = paddr_read($unsigned(addr_npc_i), {29'b0, 3'b001<<ls_wdth_npc_i});
  end else
    dcache_dat_npc_o = 32'b0;
end

always @(*) begin
  if(npc_use_pmem_npc_i && ls_npc_i == LS_STOR) begin
    // $display("Second parameter value: %b", {29'b0, 3'b001 << ls_wdth_npc_i});
    paddr_write($unsigned(addr_npc_i), {29'b0, 3'b001<<ls_wdth_npc_i}, st_dat_npc_i);
  end
end

// always @(posedge clk) begin
//   instr_ifu_o <= mem[instr_addr_ifu_i];
// end


// always @(posedge clk or rst_n) begin
//   if(!rst_n) 
//     mem = pmem_reset_i;
// end
endmodule
