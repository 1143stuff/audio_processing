`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    15:52:13 11/12/2014 
// Design Name: 
// Module Name:    fft_wrapper 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module fft_wrapper(
input clk,
input start,
input unload,
input [15 : 0] xn_re,
input [15 : 0] xn_im,
input fwd_inv,
input fwd_inv_we,
output [6 : 0] xn_index,
output[31:0]Magnitude,
output[6:0] xk_index
);
wire dv;
wire rfd;
wire busy;
wire done;
wire edone;	 
//wire[6:0] xk_index;
wire[15:0]xk_re;
wire[15:0]xk_im;
wire[4:0]blk_exp;
	 
fft_core your_instance_name (
  .clk(clk), // input clk
  .start(start), // input start
  .unload(unload), // input unload
  .xn_re(xn_re), // input [15 : 0] xn_re
  .xn_im(xn_im), // input [15 : 0] xn_im
  .fwd_inv(fwd_inv), // input fwd_inv
  .fwd_inv_we(fwd_inv_we), // input fwd_inv_we
  .rfd(rfd), // output rfd
  .xn_index(xn_index), // output [9 : 0] xn_index
  .busy(busy), // output busy
  .edone(edone), // output edone
  .done(done), // output done
  .dv(dv), // output dv
  .xk_index(xk_index), // output [9 : 0] xk_index
  .xk_re(xk_re), // output [15 : 0] xk_re
  .xk_im(xk_im), // output [15 : 0] xk_im
  .blk_exp(blk_exp) // output [4 : 0] blk_exp
);

reg[15:0] temp_re;
reg[15:0] temp_im;
reg[31:0] mag_out;
assign Magnitude = mag_out;

always@(xk_index,dv) begin
	if (dv ==1'b1) begin
	temp_re = (xk_re[15]==1'b1)?-xk_re:xk_re;
	temp_im =(xk_im[15]==1'b1)?-xk_im:xk_im;
	mag_out=(temp_re*temp_re)+(temp_im*temp_im);
	end
end

endmodule
