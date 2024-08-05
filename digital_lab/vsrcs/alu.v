module alu(
    input [3:0] A,
    input [3:0] B,
    input [2:0] sel,
    output [3:0] ans
);

reg [3:0] w_ans;
assign ans = w_ans;

reg overflow;

always @(*) begin
    w_ans = A - B;
    // overflow when sub, (A * B < 0) && (ans * A < 0)
    overflow = (A[3] != B[3]) && (w_ans[3] != A[3]);
    case (sel)
        3'b000: w_ans = A+B;
        3'b010: w_ans = ~A;
        3'b011: w_ans = A & B;
        3'b100: w_ans = A | B;
        3'b101: w_ans = A ^ B;
        //A<B <=>  (if(of): A<0) or (if(!of): A-B < 0)
        3'b110: w_ans = {3'b00, ((overflow & A[3]) | ((!overflow) & w_ans[3]))};
        3'b111: w_ans = |w_ans ? 0: 1;
        default: w_ans = w_ans;
    endcase
end

endmodule