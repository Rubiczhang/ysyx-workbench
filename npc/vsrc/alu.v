`include "params.vh"

module alu #(
  parameter ALU_DATA_WIDTH = 32
) (
  input [ALU_DATA_WIDTH-1: 0] alu_oprd1,
  input [ALU_DATA_WIDTH-1: 0] alu_oprd2,
  input [ALUOP_LEN_DYN -1: 0] alu_op,
  output reg [ALU_DATA_WIDTH-1:0] alu_ans
);

always @(*) begin
  case(alu_op)
    ADD_ALUOP:
      alu_ans = alu_oprd1 + alu_oprd2; 
    SUB_ALUOP:
      alu_ans = alu_oprd1 - alu_oprd2;
    SLL_ALUOP: 
      alu_ans = alu_oprd1 << alu_oprd2[4:0];
    SLT_ALUOP:
      alu_ans = $signed(alu_oprd1) < $signed(alu_oprd2) ? 32'b0001: 32'b0;
    SLTU_ALUOP:
      alu_ans = $unsigned(alu_oprd1) < $unsigned(alu_oprd2) ? 32'b0001: 32'b0;
    XOR_ALUOP:
      alu_ans = alu_oprd1 ^ alu_oprd2;
    SRL_ALUOP:
      alu_ans = $unsigned(alu_oprd1) >> alu_oprd2[4:0];
    SRA_ALUOP:
      alu_ans = $signed(alu_oprd1) >>> alu_oprd2[4:0];
    OR_ALUOP:
      alu_ans = alu_oprd1 | alu_oprd2;
    AND_ALUOP:
      alu_ans = alu_oprd1 & alu_oprd2;
    default: alu_ans = 0;
  endcase
end

endmodule