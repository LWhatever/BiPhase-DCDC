module delay(en,clk,ed,ok);
	input en,clk;
	input [7:0] ed;
	output ok;
	reg [7:0] count;
	reg ok;
	always@(posedge clk)
	begin
		if(en)
		begin
			ok <= 1'b0;
			count <= count+8'b0001;
		end
		if(count == ed)
			ok <= 1'b1;
	end
endmodule