module ADS8688
(
input wire rst_n,
input CLK_50M,
input clk_263,
output reg CS,
output wire SCLK,
output reg SDI,
input SDO,
output reg RST=1,
output reg [15:0] rddat0,rddat1,rddat2,outRMS,
input [11:0] dataAddr,
input RDdata
);

reg [6:0] cnt;
reg [2:0] order=0;
reg [15:0] PRC=16'h0B00;     //program register configure
reg [15:0] ch_sel;		//channel select
reg [15:0] rddat;

assign SCLK=~CS&CLK_16_7M;

wire CLK_16_7M;
clkdiv clk_inst
(
	.clk(CLK_50M),
	.div(15'd3),
	.clkout(CLK_16_7M)
);
////////////msp430 read data//////////////////
always@(posedge CLK_50M or negedge rst_n)
begin
	if(!rst_n)
		outRMS <= 0;
	else if(RDdata)
	begin
		case(dataAddr[7:0])				//dataAddr决定取出的V_RMS数据段
		8'd0:outRMS <= result0[47:32];
		8'd1:outRMS <= result0[31:16];
		8'd2:outRMS <= result0[15:0];
		8'd3:outRMS <= result1[47:32];
		8'd4:outRMS <= result1[31:16];
		8'd5:outRMS <= result1[15:0];
		8'd6:outRMS <= max_cnt[13:0];
		8'd7:outRMS <= result2[47:32];
		8'd8:outRMS <= result2[31:16];
		8'd9:outRMS <= result2[15:0];
		default: outRMS<=0;
		endcase
	end
end

////////////////SPI/////////////////
always@(posedge CLK_16_7M or negedge rst_n)
begin
	if(!rst_n)
	begin
		cnt <= 1'd0;
		CS <= 1'd0;
		RST <= 0;
		order <= 3'd0;
	end
	else 
	begin
		RST<=1'd1;
		if(cnt<7'd50)
		begin
			CS<=1'd1;
			cnt<=cnt+1'd1;
		end
		else if(cnt>=7'd50&&cnt<=7'd82)
		begin
			CS<=0;
			cnt<=cnt+1'd1;
		end
		if(cnt>7'd82)
		begin
			if(order==3'd0)
			begin
				PRC<=16'h0B01;					//channel0 0 +-1.25*Vref
				order<=3'd1;
			end
			if(order==3'd1)
			begin
				PRC<=16'h0D01;					//channel1 0 +-1.25*Vref
				order<=3'd2;
			end
			if(order==3'd2)
			begin
				 PRC<=16'h0F01;            //channel2 0 +-1.25*Vref
				 order<=3'd3;
			end
			else if(order==3'd3)
			begin 
				ch_sel<=16'hC000;				//channel0
				order<=3'd4;
			end
			else if(order==3'd4)
			begin 
				ch_sel<=16'hC400;				//channel1
				order<=3'd5;
				rddat0<=rddat;
			end
			else if(order==3'd5)
			begin 
				ch_sel<=16'hC800;          //channel2
				order<=3'd6;
				rddat1<=rddat;
			end
			else if(order==3'd6)
			begin 
				ch_sel<=16'hC000;          //channel0
				order<=3'd4;
				rddat2<=rddat;
			end
			CS<=1'd1;
			cnt<=7'd40;
		end
	end
end

always@(posedge CLK_16_7M or negedge rst_n) 
begin
	if(!rst_n)
	begin
		SDI<=0;
	end
	else if(order==3'd0||order==3'd1||order==3'd2||order==3'd3)
	begin
		case(cnt)
			7'd50:SDI <= PRC[15];
			7'd51:SDI <= PRC[14];
			7'd52:SDI <= PRC[13];
			7'd53:SDI <= PRC[12];
			7'd54:SDI <= PRC[11];
			7'd55:SDI <= PRC[10];
			7'd56:SDI <= PRC[9];
			7'd57:SDI <= PRC[8];
			7'd58:SDI <= PRC[7];
			7'd59:SDI <= PRC[6];
			7'd60:SDI <= PRC[5];
			7'd61:SDI <= PRC[4];
			7'd62:SDI <= PRC[3];
			7'd63:SDI <= PRC[2];
			7'd64:SDI <= PRC[1];
			7'd65:SDI <= PRC[0];
		endcase
	end
	else 
	begin
		case(cnt)
			7'd50:SDI <= ch_sel[15];
			7'd51:SDI <= ch_sel[14];
			7'd52:SDI <= ch_sel[13];
			7'd53:SDI <= ch_sel[12];
			7'd54:SDI <= ch_sel[11];
			7'd55:SDI <= ch_sel[10];
			7'd56:SDI <= ch_sel[9];
			7'd57:SDI <= ch_sel[8];
			7'd58:SDI <= ch_sel[7];
			7'd59:SDI <= ch_sel[6];
			7'd60:SDI <= ch_sel[5];
			7'd61:SDI <= ch_sel[4];
			7'd62:SDI <= ch_sel[3];
			7'd63:SDI <= ch_sel[2];
			7'd64:SDI <= ch_sel[1];
			7'd65:SDI <= ch_sel[0];
		endcase
	end
end

always@(negedge CLK_16_7M or negedge rst_n) 
begin
	if(!rst_n)
	begin
		rddat<=0;
	end
	else if(order>3'd3)
	begin
		case(cnt)
			7'd67:rddat[15]=SDO;
			7'd68:rddat[14]=SDO;
			7'd69:rddat[13]=SDO;
			7'd70:rddat[12]=SDO;
			7'd71:rddat[11]=SDO;
			7'd72:rddat[10]=SDO;
			7'd73:rddat[9]=SDO;
			7'd74:rddat[8]=SDO;
			7'd75:rddat[7]=SDO;
			7'd76:rddat[6]=SDO;
			7'd77:rddat[5]=SDO;
			7'd78:rddat[4]=SDO;
			7'd79:rddat[3]=SDO;
			7'd80:rddat[2]=SDO;
			7'd81:rddat[1]=SDO;
			7'd82:rddat[0]=SDO;
		endcase
	end	
end

// calculate the RMS
reg [13:0] cnt50 = 0;
reg [47:0] sum0;
reg [47:0] sum1,sum2;
reg [47:0] result0/*synthesis noprune*/;
reg [47:0] result1/*synthesis noprune*/;
reg [47:0] result2/*synthesis noprune*/;
reg [13:0] max_cnt = 0;
reg r1,r2/*synthesis noprune*/;
reg [15:0] data0_abs,data1_abs,data2_abs;
reg sig2;

always@(posedge CS )
begin
	r1 <= clk_263;
	r2 <= r1; 
	if(r1==1&&r2==0)									//132540/263 = 504 find the posedge of 50Hz
	begin
		result0 <= sum0;
		result1 <= sum1;
		result2 <= sum2;
		sum0 <= 0;
		sum1 <= 0;
		sum2 <= 0;
		max_cnt <= cnt50;
		cnt50 <= 0;
	end
	else
	begin
		data0_abs = rddat0 - 16'd32768;
		sum0 <= sum0 + data0_abs * data0_abs;
		
		data1_abs = rddat1 - 16'd32768;
		sum1 <= sum1 + data1_abs * data1_abs;
		
		sig2 = (rddat2 > 16'd32768);
		if(sig2)
			data2_abs = rddat2 - 16'd32768;
		else
			data2_abs = 16'd32768 - rddat2;
		sum2 <= sum2 + data2_abs * data2_abs;
		
		cnt50 <= cnt50 + 1'b1;
	end
end

endmodule