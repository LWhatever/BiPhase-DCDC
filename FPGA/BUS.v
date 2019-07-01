module BUS
(
input clk,rst_n,
input [11:0]ADDR,
input RD,WR,
inout [15:0]DATA,
output cs0,//cs1,cs2,cs3//cs4,cs5,cs6,cs7,
//output [7:0]addr,
//output [23:0]addr24,
input [15:0]rddat0,rddat1,rddat2,rddat3,rddat4,rddat5,//rddat6,//rddat7,
output reg [15:0] otdata0 = 15'd10000,
output reg [15:0] otdata1,otdata2
);

assign cs0=(ADDR[11:8]==4'd0);
//assign cs1=(ADDR[11:8]==4'd1);
//assign cs2=(ADDR[11:8]==4'd2);
//assign cs3=(ADDR[11:8]==4'd3);

//wire ADDR_H_cs=(ADDR[11:8]==4'd15);

reg [15:0]rdmux;
always @(posedge clk)
	case(ADDR[11:8])					//此处ADDR用于读入传入的数据,赋值给rdmux,只有当RD置高时,才会传出数据,不置高时高8位用于片选
	4'd0:rdmux <= rddat0;
	4'd1:rdmux <= rddat1;
	4'd2:rdmux <= rddat2;
	4'd3:rdmux <= rddat3;
	4'd4:rdmux <= rddat4;
	4'd5:rdmux <= rddat5;
//	4'd6:rdmux<=rddat6;
	default:;
	endcase

//reg [15:0] wrdat;
//always @(posedge WR)
//begin
//	if(WR)
//		wrdat <= DATA;
//end
always @(posedge WR)
begin
	if(WR)
		case(ADDR[11:8])
			4'd0:otdata0 <= DATA;
			4'd1:otdata1 <= DATA;
			4'd2:otdata2 <= DATA;
			default:;
		endcase
end

assign DATA = RD? rdmux : 16'hzzzz;				//将rdmux中数据传至总线上,
endmodule

	