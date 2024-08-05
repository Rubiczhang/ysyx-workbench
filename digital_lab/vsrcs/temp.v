module temp();
    reg[7:0] r1;
/* verilator lint_off LITENDIAN */
    reg[0:7] r2;
    reg r3[0:7];

    initial begin
        assign r1 = {7'b0,1};
        assign r2 = 1;
        assign r3 = {0,0,0,0,0,0,0,1};
        if(r1[0] == r3[0]) begin
            $display("YES! They are equal\n");
        end else
            $display("NO! They are unequal\n");
        $stop;
    end
endmodule