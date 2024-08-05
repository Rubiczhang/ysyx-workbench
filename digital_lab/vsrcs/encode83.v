module encode83(
    input [7:0] x,
    input en,
    output reg [2:0] y,
    output reg outEn
);

integer i;

always @(x or en) begin
    if(en) begin
        y = 0;
        for(i = 0; i <=7 ; i++) begin
            if(x[i] == 1)
                y = i[2:0];
        end
        outEn = |x;
    end else begin
        y = 0;
        outEn = 0;
    end
end

endmodule