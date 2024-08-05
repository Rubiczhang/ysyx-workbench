module lfsr_rand_seg#(
    parameter WIDTH=8,
    parameter SEG_NUM = (WIDTH-1)/4 + 1
) (
    input clk,
    input rst,
    input  [WIDTH-1: 0] rst_val,
    output [7:0]        segs[0: SEG_NUM-1]
);

wire [WIDTH-1:0] lfsr_out;

lfsr_rand #(
    .WIDTH(WIDTH)
) lfsr(
    .clk(clk),
    .rst(rst),
    .rst_val(rst_val),
    .out(lfsr_out)
);

generate
    genvar i;
    for(i = 0; i < SEG_NUM; i++) begin: gen_segs
        hex7seg cvrt(
            .b(lfsr_out[i*4+3 : i*4]),
            .en(1),
            .out(segs[i])
        );
    end

endgenerate

endmodule