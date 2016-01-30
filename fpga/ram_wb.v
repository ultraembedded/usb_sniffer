
//-----------------------------------------------------------------
// Module: ram_wb - Dual port block RAM
//-----------------------------------------------------------------
module ram_wb
(

    // Port A
    input         clka_i,
    input         rsta_i,
    input         stba_i,
    input         wea_i,
    input [3:0]   sela_i,
    input [31:0]  addra_i,
    input [31:0]  dataa_i,
    output [31:0] dataa_o,
    output        acka_o,
    
    // Port B
    input         clkb_i,
    input         rstb_i,
    input         stbb_i,
    input         web_i,
    input [3:0]   selb_i,
    input [31:0]  addrb_i,
    input [31:0]  datab_i,
    output [31:0] datab_o,
    output        ackb_o
    
);

//-----------------------------------------------------------------
// Params
//-----------------------------------------------------------------
// Number of 8KB blocks of internal RAM, up to 64KB (1 to 8)
parameter BLOCK_COUNT = 8;

wire [31:0] ZERO = 32'b0;

//-----------------------------------------------------------------
// Port A
//-----------------------------------------------------------------

// Address within a 8KB block (without lower two bits)
wire [10:0] block_a_addr = addra_i[12:2];

// Block Data Out
wire [31:0] block_a_do[7:0];

// Block select
reg [7:0] block_a_enable;
always @ *
begin
    case ({stba_i, addra_i[15:13]})
      4'b1000: block_a_enable = 8'b00000001;
      4'b1001: block_a_enable = 8'b00000010;
      4'b1010: block_a_enable = 8'b00000100;
      4'b1011: block_a_enable = 8'b00001000;
      4'b1100: block_a_enable = 8'b00010000;
      4'b1101: block_a_enable = 8'b00100000;
      4'b1110: block_a_enable = 8'b01000000;
      4'b1111: block_a_enable = 8'b10000000;
      default: block_a_enable = 8'b00000000;
    endcase
end

// Flop selected block
reg [2:0] block_a_q;
always @(posedge clka_i) 
    block_a_q <= addra_i[15:13];

assign dataa_o = block_a_do[block_a_q];

reg acka_q;
always @(posedge clka_i or posedge rsta_i)
    if (rsta_i)
        acka_q  <= 1'b0;
    else
        acka_q  <= stba_i;
assign acka_o = acka_q;


//-----------------------------------------------------------------
// Port B
//-----------------------------------------------------------------

// Address within a 8KB block (without lower two bits)
wire [10:0] block_b_addr = addrb_i[12:2];

// Block Data Out
wire [31:0] block_b_do[7:0];

// Block select
reg [7:0] block_b_enable;
always @ *
begin
    case ({stbb_i, addrb_i[15:13]})
      4'b1000: block_b_enable = 8'b00000001;
      4'b1001: block_b_enable = 8'b00000010;
      4'b1010: block_b_enable = 8'b00000100;
      4'b1011: block_b_enable = 8'b00001000;
      4'b1100: block_b_enable = 8'b00010000;
      4'b1101: block_b_enable = 8'b00100000;
      4'b1110: block_b_enable = 8'b01000000;
      4'b1111: block_b_enable = 8'b10000000;
      default: block_b_enable = 8'b00000000;
    endcase
end

// Flop selected block
reg [2:0] block_b_q;
always @(posedge clkb_i) 
    block_b_q <= addrb_i[15:13];

assign datab_o = block_b_do[block_b_q];

reg ackb_q;
always @(posedge clkb_i or posedge rstb_i)
    if (rstb_i)
        ackb_q  <= 1'b0;
    else
        ackb_q  <= stbb_i;
assign ackb_o = ackb_q;

generate 
if (BLOCK_COUNT > 0)
begin: BLOCK0
    RAMB16_S9_S9
    ram_byte0
    (
      .DOA(block_a_do[0][7:0]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[7:0]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[0]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[0]),
      
      .DOB(block_b_do[0][7:0]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[7:0]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[0]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[0])
    );
    RAMB16_S9_S9
    ram_byte1
    (
      .DOA(block_a_do[0][15:8]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[15:8]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[0]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[1]),
      
      .DOB(block_b_do[0][15:8]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[15:8]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[0]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[1])
    );
    RAMB16_S9_S9
    ram_byte2
    (
      .DOA(block_a_do[0][23:16]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[23:16]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[0]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[2]),
      
      .DOB(block_b_do[0][23:16]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[23:16]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[0]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[2])
    );
    RAMB16_S9_S9
    ram_byte3
    (
      .DOA(block_a_do[0][31:24]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[31:24]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[0]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[3]),
      
      .DOB(block_b_do[0][31:24]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[31:24]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[0]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[3])
    );
end
endgenerate

generate 
if (BLOCK_COUNT > 1)
begin: BLOCK1
    RAMB16_S9_S9
    ram_byte0
    (
      .DOA(block_a_do[1][7:0]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[7:0]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[1]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[0]),
      
      .DOB(block_b_do[1][7:0]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[7:0]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[1]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[0])
    );
    RAMB16_S9_S9
    ram_byte1
    (
      .DOA(block_a_do[1][15:8]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[15:8]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[1]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[1]),
      
      .DOB(block_b_do[1][15:8]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[15:8]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[1]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[1])
    );
    RAMB16_S9_S9
    ram_byte2
    (
      .DOA(block_a_do[1][23:16]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[23:16]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[1]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[2]),
      
      .DOB(block_b_do[1][23:16]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[23:16]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[1]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[2])
    );
    RAMB16_S9_S9
    ram_byte3
    (
      .DOA(block_a_do[1][31:24]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[31:24]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[1]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[3]),
      
      .DOB(block_b_do[1][31:24]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[31:24]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[1]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[3])
    );
end
endgenerate

generate 
if (BLOCK_COUNT > 2)
begin: BLOCK2
    RAMB16_S9_S9
    ram_byte0
    (
      .DOA(block_a_do[2][7:0]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[7:0]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[2]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[0]),
      
      .DOB(block_b_do[2][7:0]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[7:0]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[2]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[0])
    );
    RAMB16_S9_S9
    ram_byte1
    (
      .DOA(block_a_do[2][15:8]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[15:8]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[2]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[1]),
      
      .DOB(block_b_do[2][15:8]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[15:8]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[2]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[1])
    );
    RAMB16_S9_S9
    ram_byte2
    (
      .DOA(block_a_do[2][23:16]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[23:16]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[2]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[2]),
      
      .DOB(block_b_do[2][23:16]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[23:16]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[2]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[2])
    );
    RAMB16_S9_S9
    ram_byte3
    (
      .DOA(block_a_do[2][31:24]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[31:24]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[2]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[3]),
      
      .DOB(block_b_do[2][31:24]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[31:24]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[2]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[3])
    );
end
endgenerate

generate 
if (BLOCK_COUNT > 3)
begin: BLOCK3
    RAMB16_S9_S9
    ram_byte0
    (
      .DOA(block_a_do[3][7:0]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[7:0]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[3]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[0]),
      
      .DOB(block_b_do[3][7:0]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[7:0]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[3]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[0])
    );
    RAMB16_S9_S9
    ram_byte1
    (
      .DOA(block_a_do[3][15:8]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[15:8]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[3]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[1]),
      
      .DOB(block_b_do[3][15:8]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[15:8]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[3]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[1])
    );
    RAMB16_S9_S9
    ram_byte2
    (
      .DOA(block_a_do[3][23:16]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[23:16]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[3]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[2]),
      
      .DOB(block_b_do[3][23:16]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[23:16]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[3]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[2])
    );
    RAMB16_S9_S9
    ram_byte3
    (
      .DOA(block_a_do[3][31:24]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[31:24]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[3]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[3]),
      
      .DOB(block_b_do[3][31:24]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[31:24]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[3]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[3])
    );
end
endgenerate

generate 
if (BLOCK_COUNT > 4)
begin: BLOCK4
    RAMB16_S9_S9
    ram_byte0
    (
      .DOA(block_a_do[4][7:0]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[7:0]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[4]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[0]),
      
      .DOB(block_b_do[4][7:0]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[7:0]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[4]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[0])
    );
    RAMB16_S9_S9
    ram_byte1
    (
      .DOA(block_a_do[4][15:8]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[15:8]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[4]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[1]),
      
      .DOB(block_b_do[4][15:8]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[15:8]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[4]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[1])
    );
    RAMB16_S9_S9
    ram_byte2
    (
      .DOA(block_a_do[4][23:16]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[23:16]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[4]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[2]),
      
      .DOB(block_b_do[4][23:16]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[23:16]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[4]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[2])
    );
    RAMB16_S9_S9
    ram_byte3
    (
      .DOA(block_a_do[4][31:24]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[31:24]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[4]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[3]),
      
      .DOB(block_b_do[4][31:24]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[31:24]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[4]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[3])
    );
end
endgenerate

generate 
if (BLOCK_COUNT > 5)
begin: BLOCK5
    RAMB16_S9_S9
    ram_byte0
    (
      .DOA(block_a_do[5][7:0]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[7:0]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[5]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[0]),
      
      .DOB(block_b_do[5][7:0]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[7:0]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[5]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[0])
    );
    RAMB16_S9_S9
    ram_byte1
    (
      .DOA(block_a_do[5][15:8]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[15:8]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[5]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[1]),
      
      .DOB(block_b_do[5][15:8]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[15:8]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[5]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[1])
    );
    RAMB16_S9_S9
    ram_byte2
    (
      .DOA(block_a_do[5][23:16]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[23:16]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[5]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[2]),
      
      .DOB(block_b_do[5][23:16]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[23:16]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[5]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[2])
    );
    RAMB16_S9_S9
    ram_byte3
    (
      .DOA(block_a_do[5][31:24]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[31:24]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[5]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[3]),
      
      .DOB(block_b_do[5][31:24]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[31:24]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[5]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[3])
    );
end
endgenerate

generate 
if (BLOCK_COUNT > 6)
begin: BLOCK6
    RAMB16_S9_S9
    ram_byte0
    (
      .DOA(block_a_do[6][7:0]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[7:0]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[6]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[0]),
      
      .DOB(block_b_do[6][7:0]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[7:0]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[6]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[0])
    );
    RAMB16_S9_S9
    ram_byte1
    (
      .DOA(block_a_do[6][15:8]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[15:8]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[6]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[1]),
      
      .DOB(block_b_do[6][15:8]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[15:8]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[6]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[1])
    );
    RAMB16_S9_S9
    ram_byte2
    (
      .DOA(block_a_do[6][23:16]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[23:16]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[6]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[2]),
      
      .DOB(block_b_do[6][23:16]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[23:16]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[6]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[2])
    );
    RAMB16_S9_S9
    ram_byte3
    (
      .DOA(block_a_do[6][31:24]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[31:24]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[6]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[3]),
      
      .DOB(block_b_do[6][31:24]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[31:24]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[6]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[3])
    );
end
endgenerate

generate 
if (BLOCK_COUNT > 7)
begin: BLOCK7
    RAMB16_S9_S9
    ram_byte0
    (
      .DOA(block_a_do[7][7:0]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[7:0]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[7]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[0]),
      
      .DOB(block_b_do[7][7:0]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[7:0]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[7]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[0])
    );
    RAMB16_S9_S9
    ram_byte1
    (
      .DOA(block_a_do[7][15:8]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[15:8]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[7]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[1]),
      
      .DOB(block_b_do[7][15:8]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[15:8]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[7]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[1])
    );
    RAMB16_S9_S9
    ram_byte2
    (
      .DOA(block_a_do[7][23:16]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[23:16]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[7]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[2]),
      
      .DOB(block_b_do[7][23:16]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[23:16]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[7]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[2])
    );
    RAMB16_S9_S9
    ram_byte3
    (
      .DOA(block_a_do[7][31:24]),
      .DOPA(),
      .ADDRA(block_a_addr),
      .CLKA(clka_i),
      .DIA(dataa_i[31:24]),
      .DIPA(ZERO[0:0]),
      .ENA(block_a_enable[7]),
      .SSRA(ZERO[0]),
      .WEA(stba_i & wea_i & sela_i[3]),
      
      .DOB(block_b_do[7][31:24]),
      .DOPB(), 
      .ADDRB(block_b_addr),
      .CLKB(clkb_i), 
      .DIB(datab_i[31:24]),
      .DIPB(ZERO[0:0]),
      .ENB(block_b_enable[7]),
      .SSRB(ZERO[0]),
      .WEB(stbb_i & web_i & selb_i[3])
    );
end
endgenerate

endmodule
