(* dont_touch = "true" *)
module System(clk_in1, tx, rx);

input clk_in1, rx;
output tx;
wire rxinterrupt, rxack, txready, txbusy, tx_irq_en, tu, restu, ecall, tucall, tuint, sret, mret, rdwe, cur, in0o, rstco, tupccs, tucspc, ipc, rpi, ramwe, stupc, stucsr, scmcsi, scmcst, scsr0, rlpccs, pc, scmpc, jalr, pcpi, spm, smr, srm, crm, smi, sri, spcr, spcrt, srpc, spca, sraa, srab, sar, scmr, srcm, srram, srr8, srr16, srr32, sramrs8, sramrs16, sramrs32, sramru8, sramru16, sramru32, scsr, srcs, spcs, scsp, scmcs, csbmr, csbmi, csor, csori, csan, csrl, csani, smrar, add, mul, lt, gt, gtu, br, eq, neq, ltu, XOR, OR, AND, sll, srl, sra, sub, sllr, scma, scmau;
wire[4:0] REG_A, REG_B, REG_D, ALU_OP;
wire[5:0] div_cnt;
wire[31:0] PC_MAR, MAR_RAM, MAR_UART, RAM_IR, IR_CM, PC_REGS, REGS_PC, REGS_ALU_A, REGS_ALU_B, ALU_REGS, CM_REGS, REGS_CM, ALU_CM, REGS_RAM, RAM_REGS, CSR_REGS, REGS_CSR, REGS_MAR, CM_CSRI, CM_CSRT, PC_CSR, CSR_PC_MTVEC, CSR_PC_MEPC, CSR_PC_TXVEC, CSR_PC_RXVEC, TU_CSR, TU_PC, UART_MAR;
wire[11:0] CM_CSR;
wire[7:0] REGS_UART, UART_REGS;
wire[19:0] CM_PC;
wire[31:0] CM_MAR;
wire[31:0] CM_ALU, PC_ALU;
reg indicators = 0;

clk_wiz_0 CW1(clk_out1, clk_in1);

(* dont_touch = "true" *)
Program_Counter PC1(clk_out1, indicators, pc, tupc, stucsrpc, pcpi, ipc, jalr, srpc, scspcecall, scspcmret, scspctxint, scspcrxint, bacm, PC_MAR, PC_ALU, CM_PC, REGS_PC, PC_REGS, CSR_PC_MTVEC, CSR_PC_MEPC, CSR_PC_TXVEC, CSR_PC_RXVEC, PC_CSR, TU_PC);

(* dont_touch = "true" *)
MAR MAR1(clk_out1, mpi, rpi, spm, srm, crm, smi, PC_MAR, MAR_RAM, REGS_MAR, CM_MAR, MAR_UART);

(* dont_touch = "true" *)
IR IR1(clk_out1, sri, srm, smi, mpi, rpi, srr8, srr16, srr32, crm, pc, IR_CM, RAM_IR);

(* dont_touch = "true" *)
RAM RAM1(clk_out1, indicators, rst, srr8, srr16, srr32, sramrs8, sramrs16, sramrs32, sramru8, sramru16, tx_irq_en, MAR_RAM, RAM_REGS, REGS_MAR, REGS_RAM, RAM_IR);

(* dont_touch = "true" *)
CM CM1(clk_out1, rst, tu, restu, ecall, sret, mret, rxinterrupt, tx_int_ack, pc, bacm, stucsrpc, pcpi, ipc, mpi, rpi, scmcsi, scmcst, scmpc, jalr, cur, smi, spm, srm, crm, sri, spcr, spcrt, srpc, spca, sraa, srab, sar, scmr, srcm, srram, srr8, srr16, srr32, sramrs8, sramrs16, sramrs32, sramru8, sramru16, scsr, srcs, spcs, scsp, scmcs, csbmr, csbmi, csor, csori, csan, csrl, csani, smrar, scma, scmau, stupc, ALU_OP, CM_PC, IR_CM, REGS_CM, CM_REGS, REG_A, REG_B, REG_D, CM_ALU, ALU_CM, CM_CSR, CM_CSRI, CM_CSRT, CM_MAR);

(* dont_touch = "true" *)
REGS REGS1(clk_out1, indicators, out, spcr, spcrt, sraa, srab, sar, scmr, sramr, scsr, sramrs8, sramrs16, sramrs32, sramru8, sramru16, REGS_UART, UART_REGS, PC_REGS, REGS_PC, CM_REGS, REGS_CM, RAM_REGS, REGS_RAM, CSR_REGS, REGS_CSR, REGS_MAR, REGS_ALU_A, REG_A, REG_B, REG_D, REGS_ALU_B, ALU_REGS);

(* dont_touch = "true" *)
ALU ALU1(clk_out1, spca, sraa, srab, scma, scmau, bacm, div_cnt, ALU_OP, REGS_ALU_A, REGS_ALU_B, ALU_REGS, CM_ALU, PC_ALU, ALU_CM);

(* dont_touch = "true" *)
CSR CSR1(clk_out1, ecall, mret, tucall, tuint, tupccs, stucsr, scmcsi, scmcst, scsr0, srcs, spcs, scmcs, csbmr, csbmi, csor, csori, csan, csrl, csani, CSR_REGS, REGS_CSR, CSR_PC_MTVEC, CSR_PC_MEPC, CSR_PC_TXVEC, CSR_PC_RXVEC, PC_CSR, CM_CSR, CM_CSRI, CM_CSRT, TU_CSR);

(* dont_touch = "true" *)
Trap_Unit TU1(clk_out1, restu, tucall, tuint, tupc, rxinterrupt, tx_irq_en, rxpending, rxack, txready, tx_int_ack, txbusy, ecall, sret, mret, tupccs, tucspc, stupc, stucsr, stucsrpc, scspcecall, scspcmret, scspctxint, scspcrxint, scsr0, TU_PC, TU_CSR);

(* dont_touch = "true" *)
UART UART1(clk_out1, indicators, cur, tx, rx, rxack, srm, mpi, srr8, srr32, rxinterrupt, txready, txbusy, UART_REGS, REGS_RAM, REGS_MAR, UART_MAR, MAR_UART);

endmodule


module Program_Counter(clk, indicators, pc, tupc, stucsrpc, pcpi, ipc, jalr, srpc, scspcecall, scspcmret, scspctxint, scspcrxint, bacm, PC_MAR, PC_ALU, CM_PC, REGS_PC, PC_REGS, CSR_PC_MTVEC, CSR_PC_MEPC, CSR_PC_TXVEC, CSR_PC_RXVEC, PC_CSR, TU_PC);

input clk, indicators, pc, ipc, tupc, stucsrpc, pcpi, jalr, srpc, scspcecall, scspcmret, scspctxint, scspcrxint, bacm;
input[31:0] REGS_PC, CSR_PC_MTVEC, CSR_PC_MEPC, CSR_PC_TXVEC, CSR_PC_RXVEC, TU_PC;
input[19:0] CM_PC;
output[31:0] PC_REGS, PC_CSR, PC_ALU;
output reg[31:0] PC_MAR;
reg[33:0] program_counter;

initial
begin
	program_counter = 34'b0;
end

always @*
if (indicators)
	$display("PC/4 = %d", program_counter/4);

always @(posedge clk)
begin
/*if (stucsrpc)
begin
	program_counter <= CSR_PC;
end
else*/ if (srpc)
begin
	program_counter <= REGS_PC;
end
else if (pcpi)
begin
	if (jalr)
	begin
		program_counter <= program_counter + ({ {14{CM_PC[19]}}, CM_PC[19:0]} & ~1);
	end
	else
	begin
		program_counter <= program_counter + ({ {14{CM_PC[19]}}, CM_PC[19:0]} << 1);
	end
end
else if (bacm)
begin
	program_counter <= program_counter + ({ {22{CM_PC[11]}}, CM_PC[11:0]} << 1);
end
else if (pc)
begin
	program_counter <= program_counter + 4;
end 
else if (tupc)
	program_counter <= program_counter + 4;
else if (scspcecall)
begin
	program_counter <= CSR_PC_MTVEC;
end
else if (scspcmret)
begin
	program_counter <= CSR_PC_MEPC;
end
else if (scspctxint)
begin
	program_counter <= CSR_PC_TXVEC;
end
else if (scspcrxint)
begin
	program_counter <= CSR_PC_RXVEC;
end
end

always @*
if (!ipc)
	PC_MAR = program_counter;
else
	PC_MAR = 34'b0;

assign PC_ALU = program_counter;
assign PC_REGS = program_counter;
assign PC_CSR = program_counter;

endmodule


module MAR(clk, mpi, rpi, spm, srm, crm, smi, PC_MAR, MAR_RAM, REGS_MAR, CM_MAR, MAR_UART);

input clk, mpi, rpi, spm, srm, crm, smi;
input[31:0] PC_MAR, REGS_MAR;
input[31:0] CM_MAR;
output[31:0] MAR_RAM, MAR_UART;
reg[31:0] MAR; 

wire immediate = smi ? $signed(CM_MAR) : 34'd0;

always @(posedge clk)
if (srm)
begin
	
	MAR <= (REGS_MAR);
end
else if (crm)
begin
   	MAR <= 34'b0;
end
else if (mpi)
begin
	MAR <= MAR + immediate;
end
else if (spm)
begin
	MAR <= PC_MAR;
end
else if (rpi)
begin
	MAR <= $signed(CM_MAR) + REGS_MAR;
end

assign MAR_RAM = MAR;
assign MAR_UART = MAR;

endmodule


module CSR(clk, ecall, mret, tucall, tuint, tupccs, stucsr, scmcsi, scmcst, scsr0, srcs, spcs, scmcs, csbmr, csbmi, csor, csori, csan, csrl, csani, CSR_REGS, REGS_CSR, CSR_PC_MTVEC, CSR_PC_MEPC, CSR_PC_TXVEC, CSR_PC_RXVEC, PC_CSR, CM_CSR, CM_CSRI, CM_CSRT, TU_CSR);

input clk, ecall, mret, tucall, tuint, tupccs, stucsr, scsr0, srcs, spcs, scmcs, scmcsi, scmcst, csbmr, csbmi, csor, csori, csan, csrl, csani;
input[31:0] REGS_CSR, PC_CSR, CM_CSRI, CM_CSRT, TU_CSR;
input[11:0] CM_CSR;
output[31:0] CSR_REGS, CSR_PC_MTVEC, CSR_PC_MEPC, CSR_PC_TXVEC, CSR_PC_RXVEC;
reg[31:0] CSR, bitmask, misa, mstatus, mtvec, mepc, mcause, mscratch, mtval, mie, mip, cycle, instret, mhartid, txvec, rxvec;
reg[4:0] immediate;

initial
begin
	misa 	= 32'h40001104;
	mstatus = 32'h0;
	mtvec	= 32'h0;
	mepc 	= 32'h0;
	mcause 	= 32'h0;
	mtval	= 32'h0;
	mie	= 32'h0;
	mip	= 32'h0;
	cycle	= 32'h0;
	instret	= 32'h0;
	mhartid	= 32'h0;
	txvec = 32'h0;
	rxvec = 32'h0;
end
				
always @(posedge clk)
if (scmcs)
	immediate <= CM_CSRI[4:0];

always @(posedge clk)
if (CM_CSR != 12'h301 && CM_CSR != 12'hF14)
begin
if (srcs)
begin
	CSR <= REGS_CSR;
end
else if (csor)
begin
	CSR <= CSR | REGS_CSR;
end
else if (csan)
begin
	CSR <= CSR & ~REGS_CSR;
end
else if (scmcsi)
begin
	CSR <= { 27'b0 , immediate };
end
else if (csori)
begin
	CSR <= CSR | { 27'b0 , immediate };
end
else if (csani)
begin
	CSR <= CSR & ~{ 27'b0 , immediate };
end
else if (scmcs)
begin
case (CM_CSR)
	12'h301 : CSR <= misa;
	12'h300 : CSR <= mstatus;
	12'h305 : CSR <= mtvec;
	12'h340 : CSR <= mscratch;			 
	12'h341 : CSR <= mepc;
	12'h342 : CSR <= mcause;
	12'h343 : CSR <= mtval;
	12'h304 : CSR <= mie;
	12'h344 : CSR <= mip;
	12'h7C0 : CSR <= txvec;
	12'h7C1 : CSR <= rxvec;
	12'hC00 : CSR <= cycle;
	12'hC02 : CSR <= instret;
	12'hF14 : CSR <= mhartid;
	default : CSR <= 0;
endcase
end
else if (stucsr)
	CSR <= mtvec;
end

always @(posedge clk)
begin
if (csrl)
begin
case (CM_CSR)
	12'h301 : misa <= CSR;
	12'h300 : mstatus <= CSR;
	12'h305 : mtvec <= CSR;
	12'h340 : mscratch <= CSR;
	12'h341 : mepc <= CSR;
	12'h342 : mcause <= CSR;
	12'h343 : mtval <= CSR;
	12'h304 : mie <= CSR;
	12'h344 : mip <= CSR;
	12'h7C0 : txvec <= CSR;
	12'h7C1 : rxvec <= CSR;
	12'hC00 : cycle <= CSR;
	12'hC02 : instret <= CSR;
	12'hF14 : mhartid <= CSR;
endcase
end
if (tupccs)
begin
	mepc <= PC_CSR + 4;
end
end

assign	CSR_REGS = CSR;
assign	CSR_PC_MTVEC = mtvec;
assign	CSR_PC_MEPC = mepc;
assign	CSR_PC_TXVEC = txvec;
assign	CSR_PC_RXVEC = rxvec;

endmodule


module IR(clk, sri, srm, smi, mpi, rpi, srr8, srr16, srr32, crm, pc, IR_CM, RAM_IR);

input clk, sri, srm, smi, mpi, rpi, srr8, srr16, srr32, crm, pc;
input[31:0] RAM_IR;
output[31:0] IR_CM;
reg[31:0] instruction;

always @(posedge clk)
if (sri && !srm && !smi && !mpi && !rpi && !srr8 && !srr16 && !srr32 && !crm && !pc)
begin
	instruction <= RAM_IR;
end

assign	IR_CM = instruction;

endmodule


module Trap_Unit(clk, restu, tucall, tuint, tupc, rxinterrupt, tx_irq_en, rxpending, rxack, txready, tx_int_ack, txbusy, ecall, sret, mret, tupccs, tucspc, stupc, stucsr, stucsrpc, scspcecall, scspcmret, scspctxint, scspcrxint, scsr0, TU_PC, TU_CSR);

input clk, restu, ecall, sret, mret, rxinterrupt, tx_irq_en, txready, txbusy;
output reg  tucall, tuint, tupc, tupccs, tucspc, stupc, stucsr, stucsrpc, scspcecall, scspcmret, scspctxint, scspcrxint, scsr0, rxpending, rxack, tx_int_ack;
output reg[31:0] TU_PC, TU_CSR;
reg takeirq, resirq, tx_fsm_cont, supmode;
reg[1:0] rxintstate, irqstate, mretstate;
reg[2:0] txintstate, ecallstate;
reg[31:0] mtvec, rxinterruptvec, txinterruptvec;
reg[15:0] fsm_cont_count;

initial
begin
	rxintstate = 0;
	irqstate = 0;
	resirq = 0;
	tx_int_ack = 0;
	tx_fsm_cont = 0;
	txintstate = 0;
	ecallstate = 0;
	mretstate = 0;
end

always @(posedge clk)
begin
if (ecall && ecallstate == 3'b000)
begin
	ecallstate <= 3'b001;
	tupccs <= 1;
end
else if (ecallstate == 3'b001)
begin
	stucsr <= 0;
	tupccs <= 0;
	ecallstate <= 3'b010;
end
else if (ecallstate == 3'b010)	
begin
	tupccs <= 0;
	scspcecall <= 0;
	ecallstate <= 3'b011;
end
else if (ecallstate == 3'b011)
begin
	scspcecall <= 1;
	ecallstate <= 3'b100;
end
else if (ecallstate == 3'b100)
begin
	scspcecall <= 0;
	ecallstate <= 3'b000;
end
if (mret && mretstate == 2'b00)
	begin
		scspcmret <= 1;
		supmode <= 0;
		mretstate <= 2'b01;
	end
else if (mretstate == 2'b01)
	begin
		scspcmret <= 0;
		mretstate <= 2'b00;
	end
if (sret)
	begin
		stucsrpc <= 1;
		tupc <= 1;
	end	

else if (irqstate == 2'b00 && rxinterrupt)
begin
	scspcrxint <= 1;
	irqstate <= 2'b01;
end
else if (irqstate == 2'b01)
begin
    	scspcrxint <= 0;	
	irqstate <= 2'b0;
end
if (txintstate == 3'b000 && tx_irq_en && !txbusy)
begin
	scspctxint <= 1;
    	txintstate <= 3'b001;
end
else if (txintstate == 3'b001)
begin
	scspctxint <= 0;
        tx_int_ack <= 1;
	txintstate <= 3'b010;
end
else if (txintstate == 3'b010)
begin
    	tx_int_ack <= 0;
	txintstate <= 3'b011;
	fsm_cont_count <= 0;
end
else if (txintstate == 3'b011)
begin
    if (txbusy || !tx_irq_en)
    begin
        tx_fsm_cont <= 0;
        txintstate <= 3'b000;
    end
end
if (restu)
begin
	stucsr 	<= 0;
	stupc 	<= 0;
	tupccs 	<= 0;
	scsr0 	<= 0;
	stucsrpc<= 0;
	tupc 	<= 0;
	resirq 	<= 0;
end
end

endmodule



