`include  "params.vh"
module ifu (
  input clk, 
  input rst_n,


// From/to icache/memory
  output[ADDR_WIDTH-1 : 0]    instr_addr_icache_o,
  input [DATA_WIDTH-1 : 0]    instr_icache_i,

//# From/to backend
  input  [ADDR_WIDTH-1: 0]     pc_act_trgt_exeu_i, //pc actual target

//## From/to brach predictor
// input  [ADDR_WIDTH-1: 0]     pc_nxt_seq_exeu_i;  //pc next sequantially
// input  [ADDR_WIDTH-1: 0]     pc_trgt_exeu_i;     //pc target 
//  input                        nxt_pc_is_br_exeu_i;  //not used here

//# To idu
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
      pc <= pc_act_trgt_exeu_i;
    end
  end




endmodule
