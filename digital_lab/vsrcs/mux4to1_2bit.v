module  mux4to1_2bit(
    output reg [1:0] out,
    input [1:0] x[0:3],
    input [1:0] sel
);

MuxKeyWithDefault #(4, 2, 2) i0(
    .out(out),
    .key(sel),
    .default_out(2'b0),
    .lut({
        2'b00, x[0],
        2'b01, x[1],
        2'b10, x[2],
        2'b11, x[3]
    })
    
);
endmodule

