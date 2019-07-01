module KEY
(
input clk,rst_n,
output [7:0]rddat,
output irq,
input cs,
inout [3:0]KEY_H,KEY_V
	);
reg [23:0]cnt;
reg clk_100;


always @(posedge clk or negedge rst_n)
	if(!rst_n)begin
		cnt<=24'd0;
		clk_100<=1'b0;
	end
	else if(cnt<24'd2000_000)
		cnt<=cnt+1'b1;
	else begin
		cnt<=1'b0;
		clk_100<=~clk_100;
	end
	
reg i;	
reg [3:0]rKEY_H,rKEY_V;
always @(posedge clk_100)begin
		i<=~i;
		rKEY_H<=KEY_H;
		rKEY_V<=KEY_V;
end
reg [7:0]keyvalue;
always @(posedge clk_100)
	case(i)
	1'b1:keyvalue[7:4]<=~rKEY_H;
	1'b0:keyvalue[3:0]<=~rKEY_V;
	endcase
	
assign KEY_H=i?4'h0:4'hz;
assign KEY_V=!i?4'h0:4'hz;

assign rddat=cs?keyvalue:8'b0;
assign irq=keyvalue[7:4]&&keyvalue[3:0];
endmodule

		
		
		
		

	
		
