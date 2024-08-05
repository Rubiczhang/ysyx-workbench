module bcd7seg(
    input [3:0] b,
    input en,
    output reg[7:0] out
);

wire [7:0] segs [10:0];
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
assign segs[10] =8'b00000000;


always @(b or en) begin
    if(en) begin
        if(b <= 4'd9)
            out = segs[b];
        else
            out = segs[10];
    end else
        out = segs[10];
end

endmodule