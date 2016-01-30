module clkgen_pll
(
  // Clock in
  input         CLKREF_IN,

  // Clock out (from BUFG)
  output        CLKOUT0G
);

  wire CLKREF_INB;
  wire CLKOUT0;
  wire CLKOUT0_INT;

  // Input buffering
  IBUFG IBUFG_IN
  (
    .I (CLKREF_IN),
    .O (CLKREF_INB)
  );

  // Clocking primitive
  PLL_BASE
  #(
    .BANDWIDTH          ("OPTIMIZED"),
    .CLK_FEEDBACK       ("CLKOUT0"),
    .COMPENSATION       ("SOURCE_SYNCHRONOUS"),
    .DIVCLK_DIVIDE      (1),
    .CLKFBOUT_MULT      (1),
    .CLKFBOUT_PHASE     (0.000),
    .CLKOUT0_DIVIDE     (13),
    .CLKOUT0_PHASE      (0.0),
    .CLKOUT0_DUTY_CYCLE (0.500),  
    .CLKIN_PERIOD       (16.666),
    .REF_JITTER         (0.010)
  )
  pll_base_inst
  (
    .CLKFBOUT(),
    .CLKOUT0(CLKOUT0),
    .CLKOUT1(),
    .CLKOUT2(),
    .CLKOUT3(),
    .CLKOUT4(),
    .CLKOUT5(),
    .RST(1'b0),
    .CLKFBIN(CLKOUT0_INT),
    .CLKIN(CLKREF_INB)
  );

  // Output buffering
  BUFG BUFG_OUT
  (
    .I(CLKOUT0),
    .O(CLKOUT0_INT)
  );

  assign CLKOUT0G = CLKOUT0_INT;

endmodule
