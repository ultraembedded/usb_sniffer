//-----------------------------------------------------------------
// TOP
//-----------------------------------------------------------------
module top
(
    // 50MHz clock
    input           clk,

    // LED
    output [7:0]    leds,

    // SPI Flash
    output          flash_cs,
    output          flash_cclk,
    output          flash_mosi,
    input           flash_miso,

    // ADC
    output          ad_cs,
    output          ad_sclk,
    output          ad_din,
    input           ad_dout,

    // DIP Switches
    input [3:0]     sw,    

    // SD card
    output          sd_clk,
    output          sd_cd_dat3,
    inout           sd_dat0,
    inout           sd_dat1,
    inout           sd_dat2,  
    output          sd_cmd,

    // SDRAM
    inout           udqm,
    inout           sdram_clk,
    inout           cke,
    inout           bs1,
    inout           bs0,
    inout           sdram_csn,
    inout           rasn,
    inout           casn,
    inout           wen,
    inout           ldqm,
    inout [12:0]    a,
    inout [15:0]    d,

    output          audio1,
    output          audio2,
 
    // I2C
    inout           fpga_scl,
    inout           fpga_sda,

    // FTDI
    inout           ftdi_rxf,
    inout           ftdi_txe,
    inout           ftdi_siwua,
    inout           ftdi_wr,
    inout           ftdi_rd,
    inout [7:0]     ftdi_d, // TXD, RXD

    // HDMI In (J2)
    //input           tmds_in_p0,
    //input           tmds_in_n0,
    //input           tmds_in_p1,
    //input           tmds_in_n1,
    //input           tmds_in_p2,
    //input           tmds_in_n2,
    //input           tmds_in_clk_p,
    //input           tmds_in_clk_n,

    // HDMI Out (J3)
    //output          tmds_out_p0,
    //output          tmds_out_n0,
    //output          tmds_out_p1,
    //output          tmds_out_n1,
    //output          tmds_out_p2,
    //output          tmds_out_n2,
    //output          tmds_out_clk_p,
    //output          tmds_out_clk_n,

    // Port a i/o pins
    inout           porta0,
    inout           porta1,
    inout           porta2,
    inout           porta3,
    inout           porta4,
    inout           porta5,
    inout           porta6,
    inout           porta7,
    inout           porta8,
    inout           porta9,
    inout           porta10,
    inout           porta11,

    // Port b i/o pins
    inout           portb0,
    inout           portb1,
    inout           portb2,
    inout           portb3,
    inout           portb4,
    inout           portb5,
    inout           portb6,
    inout           portb7,
    inout           portb8,
    inout           portb9,
    inout           portb10,
    inout           portb11,

    // USB ULPI Interface
    input           usb_clk60,
    output          usb_ulpi_stp,
    input           usb_ulpi_dir,
    input           usb_ulpi_nxt,
    inout [7:0]     usb_ulpi_data,

    // Port d i/o pins
    inout           portd0,
    inout           portd1,
    inout           portd2,
    inout           portd3,

    // Port e i/o pins
    inout           porte0,
    inout           porte1,
    inout           porte2,
    inout           porte3,
    inout           porte4,
    inout           porte5,
    inout           porte6,
    inout           porte7,
    inout           porte8,
    inout           porte9,
    inout           porte10,
    inout           porte11,

    // Port f i/o pins
    inout           portf0,
    inout           portf1,
    inout           portf2,
    inout           portf3,
    inout           portf4,
    inout           portf5,
    inout           portf6,
    inout           portf7,
    inout           portf8,
    inout           portf9,
    inout           portf10,
    inout           portf11
);

//-----------------------------------------------------------------
// Clocking
//-----------------------------------------------------------------
wire USB_CLK60G;

clkgen_pll
u_pll
(
    .CLKREF_IN(usb_clk60),
    .CLKOUT0G(USB_CLK60G)
);

//-----------------------------------------------------------------
// Reset
//-----------------------------------------------------------------
reg reset       = 1'b1;
reg rst_next    = 1'b1;

always @(posedge USB_CLK60G) 
if (rst_next == 1'b0)
    reset       <= 1'b0;
else 
    rst_next    <= 1'b0;

//-----------------------------------------------------------------
// IO Primitives
//-----------------------------------------------------------------
wire [7:0] ulpi_out_w;
wire [7:0] ulpi_in_w;
wire       ulpi_stp_w;

genvar i;
generate  
for (i=0; i < 8; i=i+1)  
begin: gen_buf
    IOBUF 
    #(
        .DRIVE(12),
        .IOSTANDARD("DEFAULT"),
        .SLEW("FAST")
    )
    IOBUF_inst
    (
        .T(usb_ulpi_dir),
        .I(ulpi_out_w[i]),
        .O(ulpi_in_w[i]),
        .IO(usb_ulpi_data[i])
    );
end  
endgenerate  

OBUF 
#(
    .DRIVE(12),
    .IOSTANDARD("DEFAULT"),
    .SLEW("FAST")
)
OBUF_stp
(
    .I(ulpi_stp_w),
    .O(usb_ulpi_stp)
);

//-----------------------------------------------------------------
// Core
//-----------------------------------------------------------------
top_core
u_core
(
    // ULPI PHY clock (60MHz)
    .clk_i(USB_CLK60G),
    .rst_i(reset),

    // ULPI Interface
    .ulpi_data_i(ulpi_in_w),
    .ulpi_data_o(ulpi_out_w),
    .ulpi_dir_i(usb_ulpi_dir),
    .ulpi_nxt_i(usb_ulpi_nxt),
    .ulpi_stp_o(ulpi_stp_w),

    // FTDI (async FIFO interface)
    .ftdi_rxf(ftdi_rxf),
    .ftdi_txe(ftdi_txe),
    .ftdi_siwua(ftdi_siwua),
    .ftdi_wr(ftdi_wr),
    .ftdi_rd(ftdi_rd),
    .ftdi_d(ftdi_d),

    // MISC
    .leds(leds)
);

//-----------------------------------------------------------------
// Tie-offs
//-----------------------------------------------------------------
// SPI Flash
assign flash_cs   = 1'b0;
assign flash_cclk = 1'b0;
assign flash_mosi = 1'b0;

// ADC
assign ad_cs = 1'b0;
assign ad_sclk = 1'b0;
assign ad_din = 1'b0;

// SD card
assign sd_clk = 1'b0;
assign sd_cmd = 1'b0;
assign sd_cd_dat3 = 1'b0;

// Audio
assign audio1 = 1'b0;
assign audio2 = 1'b0;

endmodule
