module lfsr_rand #(
    parameter WIDTH = 8
) (
    input clk,
    input rst,
    input  [WIDTH-1: 0] rst_val,
    output [WIDTH-1: 0] out
);

reg [WIDTH-1:0] data;
assign out = data;

reg new_bit = 0;

assign new_bit = data[4] ^ data[3] ^ data[2] ^ data[0];

always @(posedge clk) begin
    if(rst) begin
        data <= rst_val;
    end else begin
        data <= {new_bit, data[WIDTH-1:1]};
    end
end

endmodule