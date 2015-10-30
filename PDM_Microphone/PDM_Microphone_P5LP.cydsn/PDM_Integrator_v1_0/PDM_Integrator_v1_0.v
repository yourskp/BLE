
//`#start header` -- edit after this line, do not edit this line
// ========================================
//
// Copyright YOUR COMPANY, THE YEAR
// All Rights Reserved
// UNPUBLISHED, LICENSED SOFTWARE.
//
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// WHICH IS THE PROPERTY OF your company.
//
// ========================================
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 07/22/2013 at 16:49
// Component: PDM_Integrator_v1_0
module PDM_Integrator_v1_0 (
	output wire  data_ready_l,
	output wire  data_ready_r,
	output reg   fifo_ov_l,
	output reg   fifo_ov_r,
	output reg   pdm_clk,
	input wire   clock,
	input wire   enable,
	input wire   pdm_data
);
	parameter ClockingFactor = 0;
	parameter DecimationRate = 64;
	parameter MicrophoneChannel = 2;

//`#start body` -- edit after this line, do not edit this line

//        Your code goes here

/* Below signal definitions commented out to add to module definition
   if the verilog code is regenerated */
//output wire  data_ready_l,
//output wire  data_ready_r,
//output reg   fifo_ov_l,
//output reg   fifo_ov_r,
//output reg   pdm_clk,
//input wire   clock,
//input wire   enable,
//input wire   pdm_data

/* Count7 period register is one less than the decimation rate */
localparam [6:0] DECIMATION_COUNTER_PERIOD = DecimationRate - 1;

/* Microphone Channel definitions - From the Symbol parameters defines */
localparam RIGHT_CHANNEL   = 2'd0;
localparam LEFT_CHANNEL    = 2'd1;
localparam STEREO_CHANNEL  = 2'd2;

/* Clocking factor definitions - ratio between input integrator clock frequency and pdm output clock frequency */
localparam CLOCK_FACTOR_SIX    = 1'd0;
localparam CLOCK_FACTOR_EIGHT  = 1'd1;

/* State machine states */
localparam PDM_INTEGRATOR_STATE_RESET			   = 3'd0;
localparam PDM_INTEGRATOR_STATE_START			   = 3'd1;
localparam PDM_INTEGRATOR_STATE_FIRSTSTAGE		   = 3'd2;
localparam PDM_INTEGRATOR_STATE_SECONDSTAGE	       = 3'd3;
localparam PDM_INTEGRATOR_STATE_THIRDSTAGE	       = 3'd4;
localparam PDM_INTEGRATOR_STATE_FOURTHSTAGE	       = 3'd5;
localparam PDM_INTEGRATOR_STATE_FIFTHSTAGE	       = 3'd6;
localparam PDM_INTEGRATOR_STATE_CLOCKSTRETCH       = 3'd7;

reg [2:0] cs_addr; /* Datapath state machine */

reg pdm_data_reg_l;  /* registered version of PDM input - left channel */
reg pdm_data_reg_r;  /* registered version of PDM input - right channel */

/* FIFO load signals. F0 is output FIFO,
 and F1 is used as a dynamic FIFO */
reg f0_load;
reg f1_load;

/* startup counter to address count7 startup behavior, dynamic FIFO  F1 reset
   on startup */
reg[1:0] first_count;

/* Flag used to stretch clocking ratio between input clock and pdm clock to eight
   from the default value of 6 */
reg toggle_flag;

/* Datapath FIFO block, bus status signals */
wire [3:0] f0_bus_stat_l;
wire [3:0] f0_bus_stat_r;
wire [3:0] f0_blk_stat_l;
wire [3:0] f0_blk_stat_r;

/* Registered version of FIFO block status signal */
reg out_fifo_full_l;
reg out_fifo_full_r;

/* Take the MSB datapath FIFO status signal for data ready status */
assign data_ready_l = f0_bus_stat_l[3];
assign data_ready_r = f0_bus_stat_r[3];

wire reset      = ~enable;     /* used as a global reset for the component */

wire cnt_tc; /* Count7 terminal count */
reg cnt_enable; /* Count7 enable */

/* Decimation rate counter using Count7 cell */
cy_psoc3_count7 #(.cy_period(DECIMATION_COUNTER_PERIOD), .cy_route_ld(`FALSE), .cy_route_en(`TRUE))
Counter7 (
/* input */ .clock (clock), // Clock
/* input */ .reset(reset), // Reset
/* input */ .load(1'b0), // Load signal used if cy_route_ld = TRUE
/* input */ .enable(cnt_enable), // Enable signal used if cy_route_en = TRUE
/* output [6:0] */ .count(), // Counter value output
/* output */ .tc(cnt_tc) // Terminal Count output
);

always @(posedge clock or posedge reset)
begin
	if(reset)
	begin	
		pdm_clk <= 0;
		first_count <= 0;
		cnt_enable <= 0;
		f0_load <= 0;
	end
	else
	begin	
		/* Generate a 50% dutyc cycle PDM clock by having the rising edge
  		   and the falling edge of the pdm clock spaced equally apart in
           the FSM */
	
		if(cs_addr == PDM_INTEGRATOR_STATE_SECONDSTAGE)	        pdm_clk <= 1;
		else if(cs_addr == PDM_INTEGRATOR_STATE_FIFTHSTAGE)		pdm_clk <= 0;
		else                                                    pdm_clk <= pdm_clk;	
	
		/* Startup counter after reset to reset FIFO, count7 properly  */
		if(first_count == 2'd3)	first_count <= first_count;	
		else                    first_count <= first_count + 1;	
		
		/* Enable count7 one extra time during the first FSM cycle to overcome
		   one clock cycle tc delay after starting the counter. After that,
		   enable once during each pass of the FSM */
		if((cs_addr == PDM_INTEGRATOR_STATE_FOURTHSTAGE) || (first_count[1:0] == 2'd2))		cnt_enable <= 1;	
		else                                        cnt_enable <= 0;	
		
		/* Load the output FIFO F0 on terminal count and completion of FSM */
		if((cnt_tc == 1) && (cs_addr == PDM_INTEGRATOR_STATE_FIFTHSTAGE))		f0_load <= 1;	
		else            f0_load <= 0;
	end
end

/* FSM Logic, Dynamic FIFO F1 loading logic based on the ratio selection between
   the integrator input clock and the PDM clock frequencies */
generate
	if(ClockingFactor == CLOCK_FACTOR_SIX)
	begin
	
		/* FSM for clocking ratio of six */
		always @(posedge clock or posedge reset)
		begin
			if(reset)
				cs_addr <= PDM_INTEGRATOR_STATE_RESET;
			else
			begin
				case (cs_addr)	
				
					/* After release of reset, stay in this state for 4 cycles for
					   initializing FIFO F1 to all zeros, and also to address cnt7
					   startup behavior */				
					PDM_INTEGRATOR_STATE_RESET:
						if(first_count == 2'd3)    cs_addr <= PDM_INTEGRATOR_STATE_START;
						else                       cs_addr <= PDM_INTEGRATOR_STATE_RESET;						
					
					/* Transitions below are all unconditional sequential flows */
					
					PDM_INTEGRATOR_STATE_START:
						cs_addr <= PDM_INTEGRATOR_STATE_FIRSTSTAGE;
					
					PDM_INTEGRATOR_STATE_FIRSTSTAGE:
						cs_addr <= PDM_INTEGRATOR_STATE_SECONDSTAGE;
						
					PDM_INTEGRATOR_STATE_SECONDSTAGE:
						cs_addr <= PDM_INTEGRATOR_STATE_THIRDSTAGE; 
					
					PDM_INTEGRATOR_STATE_THIRDSTAGE:
						cs_addr <= PDM_INTEGRATOR_STATE_FOURTHSTAGE;
						
					PDM_INTEGRATOR_STATE_FOURTHSTAGE:
						cs_addr <= PDM_INTEGRATOR_STATE_FIFTHSTAGE;	
						
					PDM_INTEGRATOR_STATE_FIFTHSTAGE:
						cs_addr <= PDM_INTEGRATOR_STATE_START; /* Back to start of FSM */						
						
					default: cs_addr <= PDM_INTEGRATOR_STATE_RESET;
				
				endcase
			end
		end
		
		/* Dynamic FIFO F1 loading */		
		always @(posedge clock or posedge reset)
		begin
			if(reset)	f1_load <= 0;			
			else
			begin				
				/* Dynamic FIFO loading for circularly moving the integrator memory elements.
				   F1 is not loaded only in the PDM_INTEGRATOR_STATE_START state */
				if(cs_addr == PDM_INTEGRATOR_STATE_START )		f1_load <= 0;	
				else                                            f1_load <= 1;
			end
		end		
	end
	else
	begin	
		
		/* FSM for clocking ratio of eight */
		always @(posedge clock or posedge reset)
		begin
			if(reset)
			begin
				cs_addr <= PDM_INTEGRATOR_STATE_RESET;
				toggle_flag <= 0;
			end
			else
			begin
				case (cs_addr)	
				
					/* After release of reset, stay in this state for 4 cycles for
					   initializing FIFO F1 to all zeros, and also to address cnt7
					   startup behavior */				
				
					PDM_INTEGRATOR_STATE_RESET:
						if(first_count == 2'd3)    cs_addr <= PDM_INTEGRATOR_STATE_START;
						else                       cs_addr <= PDM_INTEGRATOR_STATE_RESET;	
						
						/* Transitions below are all unconditional sequential flows except the state
						   PDM_INTEGRATOR_STATE_CLOCKSTRETCH used to stretch the FSM to 8 states from
						   the default 6 states */
					
					PDM_INTEGRATOR_STATE_START:
						cs_addr <= PDM_INTEGRATOR_STATE_FIRSTSTAGE;
					
					PDM_INTEGRATOR_STATE_FIRSTSTAGE:
					begin
						toggle_flag <= 0;
						cs_addr <= PDM_INTEGRATOR_STATE_CLOCKSTRETCH;
					end
						
					PDM_INTEGRATOR_STATE_SECONDSTAGE:
						cs_addr <= PDM_INTEGRATOR_STATE_THIRDSTAGE; 
					
					PDM_INTEGRATOR_STATE_THIRDSTAGE:
						cs_addr <= PDM_INTEGRATOR_STATE_FOURTHSTAGE;
						
					PDM_INTEGRATOR_STATE_FOURTHSTAGE:
					begin
						toggle_flag <= 1;
						cs_addr <= PDM_INTEGRATOR_STATE_CLOCKSTRETCH;	
					end
						
					PDM_INTEGRATOR_STATE_FIFTHSTAGE:
						cs_addr <= PDM_INTEGRATOR_STATE_START; /* Back to start of FSM */
						
					PDM_INTEGRATOR_STATE_CLOCKSTRETCH:
					begin
						if(toggle_flag == 1'b0) cs_addr <= PDM_INTEGRATOR_STATE_SECONDSTAGE;
						else                    cs_addr <= PDM_INTEGRATOR_STATE_FIFTHSTAGE;                  
					end
						
					default: cs_addr <= PDM_INTEGRATOR_STATE_RESET;
				
				endcase
			end
		end
		
		/* Dynamic FIFO F1 loading */	
		always @(posedge clock or posedge reset)
		begin
			if(reset)	f1_load <= 0;			
			else
			begin		
				/* Dynamic FIFO loading for circularly moving the integrator memory elements 
				    F1 is not loaded only in the PDM_INTEGRATOR_STATE_START, PDM_INTEGRATOR_STATE_CLOCKSTRETCH
					states */
				if((cs_addr == PDM_INTEGRATOR_STATE_START ) || ( cs_addr == PDM_INTEGRATOR_STATE_CLOCKSTRETCH ))
					f1_load <= 0;	
				else
					f1_load <= 1;
			end
		end
	end
	
	
	/* Separate datapath, data sampling for each microphone channel */
	if(( MicrophoneChannel == LEFT_CHANNEL ) || ( MicrophoneChannel == STEREO_CHANNEL ))
	begin
	
		always @(posedge clock or posedge reset)
		begin
			if(reset)
			begin
				pdm_data_reg_l  <= 0;
				out_fifo_full_l <= 0;
				fifo_ov_l       <= 0;
			end
			else
			begin
				/* Sample PDM mic data only during the relevant clock transition FSM state.
				   See Mic datasheet for clock-data timing. For left channel, sample the mic
				   data when generating the falling edge on pdm_clk */
				if(cs_addr == PDM_INTEGRATOR_STATE_FIFTHSTAGE)		pdm_data_reg_l <= pdm_data;	
				else                                                pdm_data_reg_l <= pdm_data_reg_l;				
				
				out_fifo_full_l <= f0_blk_stat_l[3]; /* Sampled version of FIFO full status */
				
				/* Overflow status generation - when FIFO is full, and load is done when full */
				fifo_ov_l <= out_fifo_full_l & f0_load; 				
			end
		end	
		
		/* Separate datapath for each microphone channel */
		/* 32-bit version of the datapath for left channel */
		cy_psoc3_dp32 #(.cy_dpconfig_a(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0:       PDM_INTEGRATOR_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1:       PDM_INTEGRATOR_STATE_START*/
		    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGB, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2:       PDM_INTEGRATOR_STATE_FIRSTSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3:       PDM_INTEGRATOR_STATE_SECONDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4:       PDM_INTEGRATOR_STATE_THIRDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5:       PDM_INTEGRATOR_STATE_FOURTHSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6:       PDM_INTEGRATOR_STATE_FIFTHSTAGE*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7:       PDM_INTEGRATOR_STATE_CLOCKSTRETCH*/
		    8'hFF, 8'h00,  /*CFG9:       */
		    8'hFF, 8'hFF,  /*CFG11-10:       */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ROUTE,
		    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:       */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:       */
		    6'h00, `SC_FIFO1_DYN_ON,3'h00,
		    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,`SC_FIFO_LEVEL,
		    `SC_FIFO__SYNC /*CFG17-16:       */
		,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_b(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0:       PDM_INTEGRATOR_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1:       PDM_INTEGRATOR_STATE_START*/
		    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2:       PDM_INTEGRATOR_STATE_FIRSTSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3:       PDM_INTEGRATOR_STATE_SECONDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4:       PDM_INTEGRATOR_STATE_THIRDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5:       PDM_INTEGRATOR_STATE_FOURTHSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6:       PDM_INTEGRATOR_STATE_FIFTHSTAGE*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7:       PDM_INTEGRATOR_STATE_CLOCKSTRETCH*/
		    8'hFF, 8'h00,  /*CFG9:       */
		    8'hFF, 8'hFF,  /*CFG11-10:       */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:       */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:       */
		    6'h00, `SC_FIFO1_DYN_ON,3'h00,
		    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,`SC_FIFO_LEVEL,
		    `SC_FIFO__SYNC /*CFG17-16:       */
		,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_c(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0:       PDM_INTEGRATOR_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1:       PDM_INTEGRATOR_STATE_START*/
		    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2:       PDM_INTEGRATOR_STATE_FIRSTSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3:       PDM_INTEGRATOR_STATE_SECONDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4:       PDM_INTEGRATOR_STATE_THIRDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5:       PDM_INTEGRATOR_STATE_FOURTHSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6:       PDM_INTEGRATOR_STATE_FIFTHSTAGE*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7:       PDM_INTEGRATOR_STATE_CLOCKSTRETCH*/
		    8'hFF, 8'h00,  /*CFG9:       */
		    8'hFF, 8'hFF,  /*CFG11-10:       */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:       */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:       */
		    6'h00, `SC_FIFO1_DYN_ON,3'h00,
		    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,`SC_FIFO_LEVEL,
		    `SC_FIFO__SYNC /*CFG17-16:       */
		,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_d(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0:       PDM_INTEGRATOR_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1:       PDM_INTEGRATOR_STATE_START*/
		    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2:       PDM_INTEGRATOR_STATE_FIRSTSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3:       PDM_INTEGRATOR_STATE_SECONDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4:       PDM_INTEGRATOR_STATE_THIRDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5:       PDM_INTEGRATOR_STATE_FOURTHSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6:       PDM_INTEGRATOR_STATE_FIFTHSTAGE*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7:       PDM_INTEGRATOR_STATE_CLOCKSTRETCH*/
		    8'hFF, 8'h00,  /*CFG9:       */
		    8'hFF, 8'hFF,  /*CFG11-10:       */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:       */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:       */
		    6'h00, `SC_FIFO1_DYN_ON,3'h00,
		    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,`SC_FIFO_LEVEL,
		    `SC_FIFO__SYNC /*CFG17-16:       */
		,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		)) dp32_l(
		        /*  input                   */  .reset(1'b0),
		        /*  input                   */  .clk(clock),
		        /*  input   [02:00]         */  .cs_addr(cs_addr),
		        /*  input                   */  .route_si(1'b0),
		        /*  input                   */  .route_ci(pdm_data_reg_l),
		        /*  input                   */  .f0_load(f0_load),
		        /*  input                   */  .f1_load(f1_load),
		        /*  input                   */  .d0_load(1'b0),
		        /*  input                   */  .d1_load(1'b0),
		        /*  output  [03:00]                  */  .ce0(),
		        /*  output  [03:00]                  */  .cl0(),
		        /*  output  [03:00]                  */  .z0(),
		        /*  output  [03:00]                  */  .ff0(),
		        /*  output  [03:00]                  */  .ce1(),
		        /*  output  [03:00]                  */  .cl1(),
		        /*  output  [03:00]                  */  .z1(),
		        /*  output  [03:00]                  */  .ff1(),
		        /*  output  [03:00]                  */  .ov_msb(),
		        /*  output  [03:00]                  */  .co_msb(),
		        /*  output  [03:00]                  */  .cmsb(),
		        /*  output  [03:00]                  */  .so(),
		        /*  output  [03:00]                  */  .f0_bus_stat(f0_bus_stat_l),
		        /*  output  [03:00]                  */  .f0_blk_stat(f0_blk_stat_l),
		        /*  output  [03:00]                  */  .f1_bus_stat(),
		        /*  output  [03:00]                  */  .f1_blk_stat()
		);
	end	
	
	/* Separate datapath, data sampling for each microphone channel */	
	if(( MicrophoneChannel == RIGHT_CHANNEL ) || ( MicrophoneChannel == STEREO_CHANNEL ))
	begin
	
		always @(posedge clock or posedge reset)
		begin
			if(reset)
			begin
				pdm_data_reg_r <= 0;
				out_fifo_full_r <= 0;
				fifo_ov_r       <= 0;				
			end
			else
			begin
				/* Sample PDM mic data only during the relevant clock transition FSM state.
				   See Mic datasheet for clock-data timing. For right channel, sample the mic
				   data when generating the rising edge on pdm_clk */
				if(cs_addr == PDM_INTEGRATOR_STATE_SECONDSTAGE)			pdm_data_reg_r <= pdm_data;	
				else                                                    pdm_data_reg_r <= pdm_data_reg_r;
				
				out_fifo_full_r <= f0_blk_stat_r[3]; /* Sampled version of FIFO full status */
				
				/* Overflow status generation - when FIFO is full, and load is done when full */
				fifo_ov_r <= out_fifo_full_r & f0_load; 
				
			end
		end		
		
		/* Separate datapath for each microphone channel */
		/* 32-bit version of the datapath for right channel */
		cy_psoc3_dp32 #(.cy_dpconfig_a(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0:       PDM_INTEGRATOR_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1:       PDM_INTEGRATOR_STATE_START*/
		    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGB, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2:       PDM_INTEGRATOR_STATE_FIRSTSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3:       PDM_INTEGRATOR_STATE_SECONDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4:       PDM_INTEGRATOR_STATE_THIRDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5:       PDM_INTEGRATOR_STATE_FOURTHSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6:       PDM_INTEGRATOR_STATE_FIFTHSTAGE*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7:       PDM_INTEGRATOR_STATE_CLOCKSTRETCH*/
		    8'hFF, 8'h00,  /*CFG9:       */
		    8'hFF, 8'hFF,  /*CFG11-10:       */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ROUTE,
		    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:       */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:       */
		    6'h00, `SC_FIFO1_DYN_ON,3'h00,
		    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,`SC_FIFO_LEVEL,
		    `SC_FIFO__SYNC /*CFG17-16:       */
		,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_b(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0:       PDM_INTEGRATOR_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1:       PDM_INTEGRATOR_STATE_START*/
		    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2:       PDM_INTEGRATOR_STATE_FIRSTSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3:       PDM_INTEGRATOR_STATE_SECONDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4:       PDM_INTEGRATOR_STATE_THIRDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5:       PDM_INTEGRATOR_STATE_FOURTHSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6:       PDM_INTEGRATOR_STATE_FIFTHSTAGE*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7:       PDM_INTEGRATOR_STATE_CLOCKSTRETCH*/
		    8'hFF, 8'h00,  /*CFG9:       */
		    8'hFF, 8'hFF,  /*CFG11-10:       */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:       */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:       */
		    6'h00, `SC_FIFO1_DYN_ON,3'h00,
		    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,`SC_FIFO_LEVEL,
		    `SC_FIFO__SYNC /*CFG17-16:       */
		,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_c(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0:       PDM_INTEGRATOR_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1:       PDM_INTEGRATOR_STATE_START*/
		    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2:       PDM_INTEGRATOR_STATE_FIRSTSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3:       PDM_INTEGRATOR_STATE_SECONDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4:       PDM_INTEGRATOR_STATE_THIRDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5:       PDM_INTEGRATOR_STATE_FOURTHSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6:       PDM_INTEGRATOR_STATE_FIFTHSTAGE*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7:       PDM_INTEGRATOR_STATE_CLOCKSTRETCH*/
		    8'hFF, 8'h00,  /*CFG9:       */
		    8'hFF, 8'hFF,  /*CFG11-10:       */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:       */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:       */
		    6'h00, `SC_FIFO1_DYN_ON,3'h00,
		    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,`SC_FIFO_LEVEL,
		    `SC_FIFO__SYNC /*CFG17-16:       */
		,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		), .cy_dpconfig_d(
		{
		    `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC__ALU,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM0:       PDM_INTEGRATOR_STATE_RESET*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM1:       PDM_INTEGRATOR_STATE_START*/
		    `CS_ALU_OP__INC, `CS_SRCA_A1, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM2:       PDM_INTEGRATOR_STATE_FIRSTSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM3:       PDM_INTEGRATOR_STATE_SECONDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM4:       PDM_INTEGRATOR_STATE_THIRDSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM5:       PDM_INTEGRATOR_STATE_FOURTHSTAGE*/
		    `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC___F1,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM6:       PDM_INTEGRATOR_STATE_FIFTHSTAGE*/
		    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
		    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
		    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
		    `CS_CMP_SEL_CFGA, /*CFGRAM7:       PDM_INTEGRATOR_STATE_CLOCKSTRETCH*/
		    8'hFF, 8'h00,  /*CFG9:       */
		    8'hFF, 8'hFF,  /*CFG11-10:       */
		    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
		    `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
		    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
		    `SC_SI_A_DEFSI, /*CFG13-12:       */
		    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
		    1'h0, `SC_FIFO1__A0, `SC_FIFO0__A0,
		    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
		    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
		    `SC_CMP0_NOCHN, /*CFG15-14:       */
		    6'h00, `SC_FIFO1_DYN_ON,3'h00,
		    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,`SC_FIFO_LEVEL,
		    `SC_FIFO__SYNC /*CFG17-16:       */
		,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL}
		)) dp32_r(
		        /*  input                   */  .reset(1'b0),
		        /*  input                   */  .clk(clock),
		        /*  input   [02:00]         */  .cs_addr(cs_addr),
		        /*  input                   */  .route_si(1'b0),
		        /*  input                   */  .route_ci(pdm_data_reg_r),
		        /*  input                   */  .f0_load(f0_load),
		        /*  input                   */  .f1_load(f1_load),
		        /*  input                   */  .d0_load(1'b0),
		        /*  input                   */  .d1_load(1'b0),
		        /*  output  [03:00]                  */  .ce0(),
		        /*  output  [03:00]                  */  .cl0(),
		        /*  output  [03:00]                  */  .z0(),
		        /*  output  [03:00]                  */  .ff0(),
		        /*  output  [03:00]                  */  .ce1(),
		        /*  output  [03:00]                  */  .cl1(),
		        /*  output  [03:00]                  */  .z1(),
		        /*  output  [03:00]                  */  .ff1(),
		        /*  output  [03:00]                  */  .ov_msb(),
		        /*  output  [03:00]                  */  .co_msb(),
		        /*  output  [03:00]                  */  .cmsb(),
		        /*  output  [03:00]                  */  .so(),
		        /*  output  [03:00]                  */  .f0_bus_stat(f0_bus_stat_r),
		        /*  output  [03:00]                  */  .f0_blk_stat(f0_blk_stat_r),
		        /*  output  [03:00]                  */  .f1_bus_stat(),
		        /*  output  [03:00]                  */  .f1_blk_stat()
		);
	end		
endgenerate

//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line


