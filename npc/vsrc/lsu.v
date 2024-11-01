`include "params.vh"
module lsu(
  input clk, 
  input rst_n,

// From/to dcache/memory
  output[ADDR_WIDTH-1 :   0]          addr_dcache_o,
  output[DATA_WIDTH-1:    0]          st_dat_dcache_o,
  output[LSWDTH_LSULEN-1: 0]          ls_wdth_dcache_o,
  output                              ls_dcache_o,
  output                              use_pmem_dcache_o,
  input [DATA_WIDTH-1 :   0]          dat_dcache_i,


// From decoder 
  input [LSU_PKG_WIDTH - 1: 0]      lsu_pkg_idu_i,

// To wrb
  output reg [DATA_WIDTH -1: 0]      wrtbck_dat_idu_o,
  output     [RD_LSULEN  -1: 0]      rd_idu_o,
  output                             wrtbck_en_idu_o
  // output     [PC_LSULEN  -1: 0]      pc_idu_o,

);

wire    [ADRBASE_LSULEN  -1 :0]      ls_addr_base;
wire    [ADROFF_LSULEN -1: 0]        ls_addr_off;
wire    [PC_LSULEN   -1 :0]      pc;
wire    [RD_LSULEN    -1 :0]      rd;
wire    [LSWDTH_LSULEN   -1 :0]      ls_wdth;
wire    [ST_DAT_LSULEN-1 :0]      st_dat;
wire                              use_lsu;
wire                              ls;

assign  ls_addr_base = lsu_pkg_idu_i[ADRBASE_HI_LSUOFF:     ADRBASE_LO_LSUOFF];
assign  ls_addr_off  = lsu_pkg_idu_i[ADROFF_HI_LSUOFF:      ADROFF_LO_LSUOFF];
assign  pc      = lsu_pkg_idu_i[PC_HI_LSUOFF :     PC_LO_LSUOFF];
assign  rd      = lsu_pkg_idu_i[RD_HI_LSUOFF :     RD_LO_LSUOFF];
assign  ls_wdth = lsu_pkg_idu_i[LSWDTH_HI_LSUOFF : LSWDTH_LO_LSUOFF];
assign  st_dat  = lsu_pkg_idu_i[ST_DAT_HI_LSUOFF : ST_DAT_LO_LSUOFF];
assign  use_lsu = lsu_pkg_idu_i[USE_LSU_LSUOFF];
assign  ls      = lsu_pkg_idu_i[LSU_LS_LSUOFF];


assign addr_dcache_o = ls_addr_base + {{(ADRBASE_LSULEN-ADROFF_LSULEN){1'b0}}, ls_addr_off};
assign st_dat_dcache_o = st_dat;
assign ls_wdth_dcache_o = ls_wdth;
assign ls_dcache_o      = ls;
assign use_pmem_dcache_o         = use_lsu;

assign wrtbck_dat_idu_o        = dat_dcache_i;
assign rd_idu_o                = rd;
assign wrtbck_en_idu_o         = (ls == LS_LOAD) && use_lsu; 

endmodule
