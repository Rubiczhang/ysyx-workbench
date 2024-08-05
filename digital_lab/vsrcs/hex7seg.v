module hex7seg(
    input [3:0] b,
    input en,
    output reg[7:0] out
);

wire [7:0] segs [16:0];
assign segs[0] = 8'b11111101;
assign segs[1] = 8'b01100000;
assign segs[2] = 8'b11011010;
assign segs[3] = 8'b11110010;
assign segs[4] = 8'b01100110;
assign segs[5] = 8'b10110110;
assign segs[6] = 8'b10111110;
assign segs[7] = 8'b11100000;
assign segs[8] = 8'b11111110;
assign segs[9] = 8'b11110110;
assign segs[10] =8'b11100110;
assign segs[11] =8'b00111110;
assign segs[12] =8'b10011100;
assign segs[13] =8'b01111010;
assign segs[14] =8'b10011110;
assign segs[15] =8'b10001110;
assign segs[16] =8'b00000000;


always @(b or en) begin
    if(en) begin
        out = ~segs[{1'b0,b}];
    end else
        out = ~segs[16];
end

endmodule