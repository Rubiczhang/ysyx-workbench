`include  "params.vh"
module ifu (
  input clk, 
  input rst_n,


// For icache/memory
  output[ADDR_WIDTH-1 : 0]    instr_addr_icache_o,
  input [DATA_WIDTH-1 : 0]    instr_icache_i,

// From backend
//  input  [ADDR_WIDTH-1: 0]    target_pc_i;

// To idu
  output [ADDR_WIDTH-1: 0]    pc_idu_o,
  output [INST_WIDTH-1: 0]    instr_idu_o

);

  reg   [ADDR_WIDTH-1: 0] pc;
  wire  [INST_WIDTH-1: 0] instr;

  assign instr = instr_icache_i;
  assign instr_idu_o = instr;
  assign pc_idu_o     = pc;
  assign instr_addr_icache_o = pc;

  always @( posedge clk ) begin
    if(!rst_n) begin pc <= 32'h80000000; end
    // if(!rst_n) begin pc <= rst_pc_i; end
    else begin
      pc <= pc+4;
    end
  end




endmodule
