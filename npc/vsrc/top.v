module top
(
    input clk,
    input rst,
    output reg[15:0] ledr
);

light my_light(
    .clk(clk),
    .rst(rst),
    .led(ledr)
);

initial begin
    if ($test$plusargs("trace") != 0) begin
      $display("[%0t] Tracing to logs/vlt_dump.fst...\n", $time);
      $dumpfile("logs/vlt_dump.fst");
      $dumpvars();
    end
    $display("[%0t] Model running...\n", $time);
end

endmodule