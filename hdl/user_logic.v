//----------------------------------------------------------------------------
// user_logic.v - module
//----------------------------------------------------------------------------
//
// ***************************************************************************
// ** Copyright (c) 1995-2012 Xilinx, Inc.  All rights reserved.            **
// **                                                                       **
// ** Xilinx, Inc.                                                          **
// ** XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"         **
// ** AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND       **
// ** SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,        **
// ** OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,        **
// ** APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION           **
// ** THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,     **
// ** AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE      **
// ** FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY              **
// ** WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE               **
// ** IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR        **
// ** REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF       **
// ** INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       **
// ** FOR A PARTICULAR PURPOSE.                                             **
// **                                                                       **
// ***************************************************************************
//
//----------------------------------------------------------------------------
// Filename:          user_logic.v
// Version:           1.00.a
// Description:       User logic module.
// Date:              Wed Oct 15 20:23:49 2014 (by Create and Import Peripheral Wizard)
// Verilog Standard:  Verilog-2001
//----------------------------------------------------------------------------
// Naming Conventions:
//   active low signals:                    "*_n"
//   clock signals:                         "clk", "clk_div#", "clk_#x"
//   reset signals:                         "rst", "rst_n"
//   generics:                              "C_*"
//   user defined types:                    "*_TYPE"
//   state machine next state:              "*_ns"
//   state machine current state:           "*_cs"
//   combinatorial signals:                 "*_com"
//   pipelined or register delay signals:   "*_d#"
//   counter signals:                       "*cnt*"
//   clock enable signals:                  "*_ce"
//   internal version of output port:       "*_i"
//   device pins:                           "*_pin"
//   ports:                                 "- Names begin with Uppercase"
//   processes:                             "*_PROCESS"
//   component instantiations:              "<ENTITY_>I_<#|FUNC>"
//----------------------------------------------------------------------------
module user_logic
(
  // -- ADD USER PORTS BELOW THIS LINE ---------------
  // --USER ports added here 
  // -- ADD USER PORTS ABOVE THIS LINE ---------------

  // -- DO NOT EDIT BELOW THIS LINE ------------------
  // -- Bus protocol ports, do not add to or delete 
  Bus2IP_Clk,                     // Bus to IP clock
  Bus2IP_Resetn,                  // Bus to IP reset
  Bus2IP_Addr,                    // Bus to IP address bus
  Bus2IP_CS,                      // Bus to IP chip select for user logic memory selection
  Bus2IP_RNW,                     // Bus to IP read/not write
  Bus2IP_Data,                    // Bus to IP data bus
  Bus2IP_BE,                      // Bus to IP byte enables
  Bus2IP_RdCE,                    // Bus to IP read chip enable
  Bus2IP_WrCE,                    // Bus to IP write chip enable
  Bus2IP_Burst,                   // Bus to IP burst-mode qualifier
  Bus2IP_BurstLength,             // Bus to IP burst length
  Bus2IP_RdReq,                   // Bus to IP read request
  Bus2IP_WrReq,                   // Bus to IP write request
  IP2Bus_AddrAck,                 // IP to Bus address acknowledgement
  IP2Bus_Data,                    // IP to Bus data bus
  IP2Bus_RdAck,                   // IP to Bus read transfer acknowledgement
  IP2Bus_WrAck,                   // IP to Bus write transfer acknowledgement
  IP2Bus_Error,                   // IP to Bus error response
  Type_of_xfer                    // Transfer Type
  // -- DO NOT EDIT ABOVE THIS LINE ------------------
); // user_logic

// -- ADD USER PARAMETERS BELOW THIS LINE ------------
// --USER parameters added here 
// -- ADD USER PARAMETERS ABOVE THIS LINE ------------

// -- DO NOT EDIT BELOW THIS LINE --------------------
// -- Bus protocol parameters, do not add to or delete
parameter C_SLV_AWIDTH                   = 32;
parameter C_SLV_DWIDTH                   = 32;
parameter C_NUM_MEM                      = 1;
// -- DO NOT EDIT ABOVE THIS LINE --------------------

// -- ADD USER PORTS BELOW THIS LINE -----------------
// --USER ports added here 
// -- ADD USER PORTS ABOVE THIS LINE -----------------

// -- DO NOT EDIT BELOW THIS LINE --------------------
// -- Bus protocol ports, do not add to or delete
input                                     Bus2IP_Clk;
input                                     Bus2IP_Resetn;
input      [C_SLV_AWIDTH-1 : 0]           Bus2IP_Addr;
input      [C_NUM_MEM-1 : 0]              Bus2IP_CS;
input                                     Bus2IP_RNW;
input      [C_SLV_DWIDTH-1 : 0]           Bus2IP_Data;
input      [C_SLV_DWIDTH/8-1 : 0]         Bus2IP_BE;
input      [C_NUM_MEM-1 : 0]              Bus2IP_RdCE;
input      [C_NUM_MEM-1 : 0]              Bus2IP_WrCE;
input                                     Bus2IP_Burst;
input      [7 : 0]                        Bus2IP_BurstLength;
input                                     Bus2IP_RdReq;
input                                     Bus2IP_WrReq;
output                                    IP2Bus_AddrAck;
output     [C_SLV_DWIDTH-1 : 0]           IP2Bus_Data;
output                                    IP2Bus_RdAck;
output                                    IP2Bus_WrAck;
output                                    IP2Bus_Error;
output                                    Type_of_xfer;
// -- DO NOT EDIT ABOVE THIS LINE --------------------

//----------------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------------
  // --USER nets declarations added here, as needed for user logic

parameter NUM_BYTE_LANES = (C_SLV_DWIDTH+7)/8;
reg [C_SLV_DWIDTH-1 : 0] mem_data_out [0 : C_NUM_MEM-1];

wire [7:0] mem_address;
wire mem_select;
wire mem_read_enable;
reg  [C_SLV_DWIDTH-1 : 0] mem_ip2bus_data;
reg mem_read_ack_dly1;
reg mem_read_ack_dly2; 
wire mem_read_ack; 
wire mem_write_ack; 

reg [7 : 0] INPUT [C_NUM_MEM-1 : 0][NUM_BYTE_LANES-1 : 0][0 :255];
reg [7 : 0] OUTPUT [C_NUM_MEM-1 : 0][NUM_BYTE_LANES-1 : 0][0 :127];
reg [NUM_BYTE_LANES-1 : 0] write_enable [C_NUM_MEM-1 : 0];
reg [7 : 0] data_in [C_NUM_MEM-1 : 0][NUM_BYTE_LANES-1 : 0];
reg [7 : 0] data_out [C_NUM_MEM-1 : 0][NUM_BYTE_LANES-1 : 0];
reg [7 : 0] read_address;


integer byte_index;
integer j,i=0;

wire clk;
wire start;
wire fwd_inv;
wire fwd_inv_we;
wire rfd;
wire busy;
wire edone;
wire done;
wire dv;
wire [15 : 0] xn_re;
wire [15 : 0] xn_im;
wire [6 : 0] xn_index;
wire [6 : 0] xk_index;
wire [23 : 0] xk_re;
wire [23 : 0] xk_im;

wire [31:0] OLED_OUT;


// ------------------------------------------------------
// Example code to read/write user memory space
assign mem_select = Bus2IP_CS;
assign mem_read_enable = Bus2IP_RdCE[0];
assign mem_read_ack = (mem_read_ack_dly1 && (!mem_read_ack_dly2));
assign mem_write_ack = Bus2IP_WrCE[0];
assign mem_address = Bus2IP_Addr[9:2]; 


//assigning values fft module 
assign	xn_re =  {INPUT[0][1][xn_index],INPUT[0][0][xn_index]};
assign	xn_im =  0;
assign start = 1;    
assign fwd_inv = 1;
assign fwd_inv_we =1;
assign unload=1;

fft_wrapper your_instance_name (
  .clk(Bus2IP_Clk), // input clk
  .start(start), // input start
  .unload(unload), // input unload
  .xn_re(xn_re), // input [15 : 0] xn_re
  .xn_im(xn_im), // input [15 : 0] xn_im
  .fwd_inv(fwd_inv), // input fwd_inv
  .fwd_inv_we(fwd_inv_we), // input fwd_inv_we
  .xn_index(xn_index), // output [6 : 0] xn_index
  .Magnitude(OLED_OUT),
  .xk_index(xk_index) // output [6 : 0] xk_index
);

integer k=0;
always @ (posedge Bus2IP_Clk) begin
		OUTPUT[0][0][xk_index] = OLED_OUT[7:0] ;	
		OUTPUT[0][1][xk_index] = OLED_OUT[15:8];	
		OUTPUT[0][2][xk_index] = OLED_OUT[23:16];	
		OUTPUT[0][3][xk_index] = OLED_OUT[31:24];
end

/**
 * ported from userlogic example.
 *
 */
always @( posedge Bus2IP_Clk)
  begin
    if(Bus2IP_Resetn == 0) 
    begin
      mem_read_ack_dly1 <= 0;
      mem_read_ack_dly2 <= 0;
    end
    else
    begin
      mem_read_ack_dly1 <= mem_read_enable;
      mem_read_ack_dly2 <= mem_read_ack_dly1;
    end
  end

always @(*) begin
  for (i=0;i<=C_NUM_MEM-1;i=i+1) begin
    for (byte_index=0;byte_index<=NUM_BYTE_LANES-1;byte_index=byte_index+1) begin
      write_enable[i][byte_index] <= Bus2IP_WrCE[i] && Bus2IP_BE[byte_index];
      data_in[i][byte_index] <= Bus2IP_Data[(byte_index*8) +: 8];
    end
  end
end

always @(posedge Bus2IP_Clk) 
begin
  for (i=0;i<=C_NUM_MEM-1;i=i+1)
  begin
    for (byte_index=0;byte_index<=NUM_BYTE_LANES-1;byte_index=byte_index+1)
    begin
      if(write_enable[i][byte_index] == 1)
      begin
			INPUT[i][byte_index][mem_address] <= data_in[i][byte_index];
			//if(mem_address == 0) begin
			//	Noise_Flag = INPUT[0][0][0];
			//end
      end
      read_address <= mem_address;
    end
  end
end
// output values are read from OUTPUT buffer
always @(*)
begin
  for (i=0;i<=C_NUM_MEM-1;i=i+1)
  begin
    for (byte_index=0;byte_index<=NUM_BYTE_LANES-1;byte_index=byte_index+1)
    begin
		data_out[i][byte_index] <= OUTPUT[i][byte_index][read_address];
    end
  end
end //always @(*) begin

always @(*)
begin
  for (i=0;i<=C_NUM_MEM-1;i=i+1)
  begin
    for (byte_index=0;byte_index<=NUM_BYTE_LANES-1;byte_index=byte_index+1)
    begin
      mem_data_out[i][(byte_index*8) +: 8] <= data_out[i][byte_index];//MUL_OUT_Reg;//
    end
  end
end //always @(*) begin

always @(*)
begin
  
  case (mem_select) 
    1 : mem_ip2bus_data <= mem_data_out[0];
    default : mem_ip2bus_data <= 0;
  endcase
end

  assign IP2Bus_Data  = (mem_read_ack == 1'b1) ? mem_ip2bus_data : 0;
  assign IP2Bus_AddrAck = (mem_write_ack || (mem_read_enable && mem_read_ack));
  assign IP2Bus_WrAck = mem_write_ack;
  assign IP2Bus_RdAck = mem_read_ack;
  assign IP2Bus_Error = 0;
  
  // ------------------------------------------------------------
  // Example code to drive IP to Bus signals
  // ------------------------------------------------------------


//Saving the output in the array
endmodule  