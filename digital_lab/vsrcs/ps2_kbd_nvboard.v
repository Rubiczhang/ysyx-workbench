module ps2_kbd_nvboard(
    input clk, clrn,
    input ps2_clk, ps2_data,
    input nextdata_n,

    output [7:0] kbd_segcode[0:1],
    output [7:0] asci_segcode[0:1],
    output [7:0] cntr_segcode[0:1],
    output ready,
    output overflow
);

//clk, clrn: set by top->xxx
//nextdata_n = 0
//ps2_clk, ps2_data: from nvboard

//kbd_segcode: if ps2_keyboard->data == F0, (not shown), wait the next and show
// else directly show

wire ps2_rdy_w;
wire overflow_w;

reg         seg_show_en; //seg_show_en contral seg0 and seg1
reg [1:0]   seg_en_cur_st, seg_en_nxt_st; //seg_enable current status, ** next **

wire [7:0] ps2_kbd_out;
reg  [7:0] kbd_code_buf;
reg  [7:0] asci_code_buf;
reg [7:0] kbd_segcode_buf[0:1];


assign kbd_segcode = kbd_segcode_buf;

reg [7:0] press_cntr;

typedef enum reg[1:0]{
    S0 = 2'b00,
    S_OUT_EN = 2'b01,
    S_F0 = 2'b10,
    S_F0_KEY = 2'b11
} seg_en_state_t;

// FSM of seg enable
always @(posedge clk) begin
    if(!clrn) 
        seg_en_cur_st <= 0;
    else
        seg_en_cur_st <= seg_en_nxt_st;
end

always @(*) begin
    case (seg_en_cur_st)
        S0: begin
            if(ps2_rdy_w && ps2_kbd_out == 8'hf0)   //F0 code
                seg_en_nxt_st = S_F0;
            else if(ps2_rdy_w)                      //valid kbd code
                seg_en_nxt_st = S_OUT_EN;
            else seg_en_nxt_st = seg_en_cur_st;
        end
        S_OUT_EN: begin
            if(ps2_rdy_w)
                seg_en_nxt_st = S_F0;
            else
                seg_en_nxt_st = seg_en_cur_st;
        end
        S_F0: begin
            if(ps2_rdy_w)
                seg_en_nxt_st = S_F0_KEY;
            else seg_en_nxt_st = seg_en_cur_st;
        end
        S_F0_KEY: begin
            if(!nextdata_n)
                seg_en_nxt_st = S0;
            else
                seg_en_nxt_st = seg_en_cur_st;
        end
    endcase
end

always @(*) begin
    case (seg_en_cur_st)
        S0:
            seg_show_en = 1'b0;
        S_OUT_EN:     
            seg_show_en = 1'b1;
        S_F0:         
            seg_show_en = 1'b0;
        S_F0_KEY:     
            seg_show_en = 1'b1;
    endcase
end

always @(posedge clk) begin
    if(ps2_rdy_w && ps2_kbd_out != 8'hf0)
        kbd_code_buf <= ps2_kbd_out;
    else
        kbd_code_buf <= kbd_code_buf;
end


assign ready = ps2_rdy_w;
assign overflow = overflow_w;

ps2_keyboard ps2_kbd(
    .clk(clk),
    .clrn(clrn),
    .ps2_clk(ps2_clk),
    .ps2_data(ps2_data),
    .nextdata_n(nextdata_n),
    .data(ps2_kbd_out),
    .ready(ps2_rdy_w),
    .overflow(overflow_w)
);

genvar i;
generate
//cvt kbd_code_buf, and put result into kbd_segcode
    for(i = 0; i < 2; i++)
    begin: gen_cvt
        hex7seg cvt(
            .b(kbd_code_buf[i*4 + 3: i*4]),
            .en(seg_show_en),
            .out(kbd_segcode_buf[i])
        );

    end
endgenerate

generate 
//cvt kbd_code_buf[1], and put result into kbd_segcode
    for(i = 0; i < 2; i++)
    begin: gen_cvt_acsi
        hex7seg cvt(
            .b(asci_code_buf[i*4 + 3: i*4]),
            .en(seg_show_en),
            .out(asci_segcode[i])
        );
    end
endgenerate

generate 
//cvt kbd_code_buf[1], and put result into kbd_segcode
    for(i = 0; i < 2; i++)
    begin: gen_cvt_cntr
        hex7seg cvt(
            .b(press_cntr[i*4 + 3: i*4]),
            .en(1),
            .out(cntr_segcode[i])
        );
    end
endgenerate
ps2_asci_cvt pacvt(
    .scan_code(kbd_code_buf),
    .asci_code(asci_code_buf)
);

always @(posedge clk) begin
    if(!clrn)
        press_cntr <= 0;
    else begin
        if(seg_en_nxt_st == S_OUT_EN && seg_en_cur_st == S0)
            press_cntr <= press_cntr + 1;
        else 
            press_cntr <= press_cntr;
    end
end

endmodule
