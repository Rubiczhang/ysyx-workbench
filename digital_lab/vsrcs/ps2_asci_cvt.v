module ps2_asci_cvt(
    input   [7:0] scan_code,
    output  reg [7:0] asci_code
);

//
always @(*) begin
    case (scan_code) 
//can be down by awk the table: https://www.osfree.org/docs/cmdref/cmdref.2.0476.php
// awk '{ \
//       if($1 ~ /[A-Z]/){ \
//           printf"8'\''d%s: asci_code = 8'\''d%s;\n", $2, $3 \
//       } else { \
//           printf"8'\''d%s: asci_code = 8'\''d%s;\n", $3, $4  }}'  temp.txt  
		8'h05:	asci_code = 8'ha1;		// F1
		8'h06:	asci_code = 8'ha2;		// F2
		8'h04:	asci_code = 8'ha3;		// F3
		8'h0C:	asci_code = 8'ha4;		// F4
		8'h03:	asci_code = 8'ha5;		// F5
		8'h0B:	asci_code = 8'ha6;		// F6
		8'h83:	asci_code = 8'hA7;		// F7
		8'h0A:	asci_code = 8'hA8;		// F8
		8'h01:	asci_code = 8'hA9;		// F9
		8'h09:	asci_code = 8'hAA;		// F10
		8'h78:	asci_code = 8'hAB;		// F11
		8'h07:	asci_code = 8'hAC;		// F12
		8'h0d: asci_code = 8'h09;	// tab
		8'h0e: asci_code = 8'h60;	// `
		8'h15: asci_code = 8'h71;	// q
		8'h16: asci_code = 8'h31;	// 1
		8'h1a: asci_code = 8'h7a;	// z
		8'h1b: asci_code = 8'h73;	// s
		8'h1c: asci_code = 8'h61;	// a
		8'h1d: asci_code = 8'h77;	// w
		8'h1e: asci_code = 8'h32;	// 2
		8'h21: asci_code = 8'h63;	// c
		8'h22: asci_code = 8'h78;	// x
		8'h23: asci_code = 8'h64;	// d
		8'h24: asci_code = 8'h65;	// e
		8'h25: asci_code = 8'h34;	// 4
		8'h26: asci_code = 8'h33;	// 3
		8'h29: asci_code = 8'h20;	// space
		8'h2a: asci_code = 8'h76;	// v
		8'h2b: asci_code = 8'h66;	// f
		8'h2c: asci_code = 8'h74;	// t
		8'h2d: asci_code = 8'h72;	// r
		8'h2e: asci_code = 8'h35;	// 5
		8'h31: asci_code = 8'h6e;	// n
		8'h32: asci_code = 8'h62;	// b
		8'h33: asci_code = 8'h68;	// h
		8'h34: asci_code = 8'h67;	// g
		8'h35: asci_code = 8'h79;	// y
		8'h36: asci_code = 8'h36;	// 6
		8'h3a: asci_code = 8'h6d;	// m
		8'h3b: asci_code = 8'h6a;	// j
		8'h3c: asci_code = 8'h75;	// u
		8'h3d: asci_code = 8'h37;	// 7
		8'h3e: asci_code = 8'h38;	// 8
		8'h41: asci_code = 8'h2c;	// ,
		8'h42: asci_code = 8'h6b;	// k
		8'h43: asci_code = 8'h69;	// i
		8'h44: asci_code = 8'h6f;	// o
		8'h45: asci_code = 8'h30;	// 0
		8'h46: asci_code = 8'h39;	// 9
		8'h49: asci_code = 8'h2e;	// .
		8'h4a: asci_code = 8'h2f;	// /
		8'h4b: asci_code = 8'h6c;	// l
		8'h4c: asci_code = 8'h3b;	// ;
		8'h4d: asci_code = 8'h70;	// p
		8'h4e: asci_code = 8'h2d;	// -
		8'h52: asci_code = 8'h27;	// '
		8'h54: asci_code = 8'h5b;	// [
		8'h55: asci_code = 8'h3d;	// =
		8'h58: asci_code = 8'hAD;	// CAPS lock
		8'h7E: asci_code = 8'hAE;	// Scroll lock
		8'h77: asci_code = 8'hAF;	// Num lock
		8'h5a: asci_code = 8'h0d;	// carriage return
		8'h5b: asci_code = 8'h5d;	// ]
		8'h5d: asci_code = 8'h5c;	// \
		8'h66: asci_code = 8'h08;	// backspace
		8'h69:	asci_code = 8'h95;		// end
		8'h6b:	asci_code = 8'h93;		// left
		8'h6c:	asci_code = 8'h94;		// home
		8'h70:	asci_code = 8'h98;		// insert
		8'h71: asci_code = 8'h7f;	// del
		8'h72:	asci_code = 8'h92;		// down
		8'h74:  asci_code = 8'h91;		// right
		8'h75:  asci_code = 8'h90;		// up
		8'h76: asci_code = 8'h1b;	// escape
		8'h7a:	asci_code = 8'h97;		// pg down
		8'h7d:	asci_code = 8'h96;		// pg up
		8'hFA:	asci_code = 8'hFA;		// keyboard ACK code
		default: asci_code = 8'h2e;  // '.' used for unlisted characters.
    endcase
end

endmodule
