module DCDC(
input CLK_50M,
output CS,
output SCLK,
output SDI,
input SDO,
output RST,
input rst_n,
input [11:0]ADDR,
input RD,WR,
inout [15:0]DATA,
inout [3:0]KEY_H,KEY_V,
output PWM1,
output PWM2
);


wire clk_400M;
wire clk_100M;
wire cs0;//cs1,cs2,cs3;//cs4,cs5,cs6,cs7,
wire [7:0] rddat0;
wire [15:0] data0,data1,data2,outRMS;
wire irq;
wire [15:0]wrdat;
reg clk_263;

pll	pll_inst (
	.inclk0 ( CLK_50M ),
	.c0 ( clk_400M )
	);
	
clkdiv clkdiv_inst
(
	.clk(clk_400M) ,	// input  clk_sig
	.div(4'd3) ,	// input [11:0] div_sig
	.clkout(clk_100M) 	// output  clkdiv_sig
);
	
reg [18:0] cnt2 = 0;
always @(posedge CLK_50M)
begin
	cnt2 <= cnt2 + 1'b1;
	if(cnt2 == 18'd95057)
		clk_263 <= 1'b1;
	else if(cnt2 >= 18'd190114)
	begin
		clk_263 <= 1'b0;
		cnt2 <= 0;
	end
end

reg [14:0] cnt1 = 0;
wire [14:0] pwm_cycle;			// = 15'd10000
reg pwm1,pwm2;
wire pwm_en;
assign PWM1 = pwm1&pwm_en;
assign PWM2 = pwm2&pwm_en;

//reg [9:0] cnt3, cnt4;
//always @(posedge clk_400M)
//begin
//	cnt1 <= cnt1 + 1'b1;
//	if(cnt1 <= pwm_cycle)
//	begin
//		pwm1 <= 1'b0;
//		cnt4 <= 0;
//		cnt3 <= cnt3 + 1'b1;
//		if(cnt3 >= 8'd162)
//			pwm2 <= 1'b1;
//	end
//	else
//	begin
//		pwm2 <= 1'b0;
//		cnt3 <= 0;
//		cnt4 <= cnt4 + 1'b1;
//		if(cnt4 >= 8'd162)
//			pwm1 <= 1'b1;
//		if(cnt1 == 15'd20000)
//			cnt1 <= 0;
//	end
//end
reg en;
wire ok;
reg pclk;
delay delay_inst
(
	.en(en) ,	// input  en_sig
	.clk(clk_400M) ,	// input  clk_sig
	.ed(8'd162) ,	// input [4:0] ed_sig
	.ok(ok) 	// output  ok_sig
);
always @(posedge clk_400M)
begin
	cnt1 <= cnt1 + 1'b1;
	if(cnt1 == pwm_cycle)
	begin
		pclk <= 1'b1;
		en <= 1'b1;
	end
	else if(cnt1 == 15'd26667)
	begin
		pclk <= 1'b0;
		en <= 1'b1;
		cnt1 <= 15'd0;
	end
	
	if(pclk == 1'b1)
	begin
		pwm1 <= 0;
		if(ok)
		begin
			pwm2 <= 1'b1;
			en <= 0;
		end
	end
	else if(pclk == 1'b0)
	begin
		pwm2 <= 0;
		if(ok)
		begin
			pwm1 <= 1'b1;
			en <= 0;
		end
	end
end

ADS8688 ADS8688_inst
(
	.rst_n(rst_n) ,	// input  rst_n_sig
	.CLK_50M(CLK_50M) ,	// input  CLK_50M_sig
	.clk_263(clk_263) ,	// input  clk_263_sig
	.CS(CS) ,	// output  CS_sig
	.SCLK(SCLK) ,	// output  SCLK_sig
	.SDI(SDI) ,	// output  SDI_sig
	.SDO(SDO) ,	// input  SDO_sig
	.RST(RST) ,	// output  RST_sig
	.rddat0(data0) ,	// output [15:0] rddat0_sig
	.rddat1(data1) ,	// output [15:0] rddat1_sig
	.rddat2(data2) ,	// output [15:0] rddat2_sig
	.outRMS(outRMS) ,	// output [15:0] outRMS_sig
	.dataAddr(ADDR) ,	// input [11:0] dataAddr_sig
	.RDdata(RD) 	// input  RDdata_sig
);

BUS BUS_inst
(
	.clk(CLK_50M) ,	// input  clk_sig
	.rst_n(rst_n) ,	// input  rst_n_sig
	.ADDR(ADDR) ,	// input [11:0] ADDR_sig
	.RD(RD) ,	// input  RD_sig
	.WR(WR) ,	// input  WR_sig
	.DATA(DATA) ,	// inout [15:0] DATA_sig
	.cs0(cs0) ,	// output  cs0_sig
	.rddat0(rddat0) ,	// input [15:0] rddat0_sig
	.rddat1(data0) ,	// input [15:0] rddat1_sig
	.rddat2(data1) ,	// input [15:0] rddat2_sig
	.rddat3(data2) ,	// input [15:0] rddat3_sig
	.rddat4(outRMS) ,	// input [15:0] rddat4_sig
	.rddat5() ,	// input [15:0] rddat5_sig
	.otdata0(pwm_cycle) ,	// output [15:0] otdata0_sig
	.otdata1(pwm_en) ,	// output [15:0] otdata1_sig
	.otdata2() 	// output [15:0] otdata2_sig
);

KEY KEY_inst
(
	.clk(clk_100M) ,	// input  clk_sig
	.rst_n(rst_n) ,	// input  rst_n_sig
	.rddat(rddat0) ,	// output [7:0] rddat_sig
	.irq(irq) ,	// output  irq_sig
	.cs(cs0) ,	// input  cs_sig
	.KEY_H(KEY_H) ,	// inout [3:0] KEY_H_sig
	.KEY_V(KEY_V) 	// inout [3:0] KEY_V_sig
);

endmodule
