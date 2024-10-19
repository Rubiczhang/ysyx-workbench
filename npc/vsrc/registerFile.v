module registerFile #(
  ADDR_WIDTH = 5, 
  DATA_WIDTH = 32) (
  
  input clk,
  input rst_n,
  input     [ADDR_WIDTH-1: 0]       rs1_i,
  input     [ADDR_WIDTH-1: 0]       rs2_i,
  input     [ADDR_WIDTH-1: 0]       rd_i,
  input     [DATA_WIDTH-1: 0]       dat_i,
  input                             w_en_i,

  output    [DATA_WIDTH-1: 0]       rs1_val_o,
  output    [DATA_WIDTH-1: 0]       rs2_val_o
);


export "DPI-C" task     readGpr;

task readGpr;
  input int idx;
  output int reg_val;
  assert(idx >= 0 && idx < 32);
  assign reg_val = regfile[idx];
endtask


  reg       [DATA_WIDTH-1: 0]       regfile   [0: 2** ADDR_WIDTH-1];

  assign rs1_val_o = regfile[rs1_i];
  assign rs2_val_o = regfile[rs2_i];

  integer i;
  always @(posedge clk) begin
    if(!rst_n) begin
      for(i = 0; i < 2**ADDR_WIDTH; i = i+1) begin :reset_regfile;
        regfile[i] <= 0;
      end 
    end else if(w_en_i && rd_i != 0) begin
        regfile[rd_i] <= dat_i;
    end 
  end


endmodule