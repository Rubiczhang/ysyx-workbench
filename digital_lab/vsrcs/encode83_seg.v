module encode83_seg(
    input [7:0] x,
    input en,
    output reg [2:0] y,
    output reg outEn,
    output reg [7:0] seg
);
wire [2:0] y_val;
assign y = y_val;

wire w_outEn;
assign outEn = w_outEn;

wire [7:0] seg_out;
assign seg = ~seg_out;

encode83 enc(
    .x(x),
    .en(en),
    .y(y_val),
    .outEn(w_outEn)
);

bcd7seg bcd(
    .b({1'b0,y_val}),
    .en(w_outEn),
    .out(seg_out)
);

endmodule