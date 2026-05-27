(* dont_touch = "true" *)
module System(clk_in1, tx, rx);

input clk_in1, rx;
output tx;
wire rxinterrupt, rxack, txready, txbusy, tu, restu, ecall, tucall, tuint, sret, mret, rdwe, cur, in0o, rstco, tupccs, tucspc, ipc, rpi, ramwe, stupc, stucsr, scmcsi, scmcst, scsr0, pc, scmpc, jalr, pcpi, spm, smr, srm, crm, smi, sri, spcr, spcrt, srpc, spca, sraa, srab, sar, scmr, srcm, srram, srr8, srr16, srr32, sramrs8, sramrs16, sramrs32, sramru8, sramru16, sramru32, scsr, srcs, spcs, scsp, scmcs, csbmr, csbmi, csor, csan, smrar, add, mul, lt, gt, gtu, br, eq, neq, ltu, XOR, OR, AND, sll, srl, sra, sub, sllr, scma, scmau;
wire[4:0] REG_A, REG_B, REG_D, ALU_OP;
wire[5:0] div_cnt;
wire[31:0] PC_MAR, MAR_RAM, MAR_UART, RAM_IR, IR_CM, PC_REGS, REGS_PC, REGS_ALU_A, REGS_ALU_B, ALU_REGS, CM_REGS, REGS_CM, ALU_CM, REGS_RAM, RAM_REGS, CSR_REGS, REGS_CSR, REGS_MAR, CM_CSR, CM_CSRI, CM_CSRT, PC_CSR, CSR_PC, TU_CSR, TU_PC, UART_MAR;
wire[7:0] REGS_UART, UART_REGS;
wire[19:0] CM_PC;
wire[31:0] CM_MAR;
wire[31:0] CM_ALU, PC_ALU;
reg indicators = 0;

clk_wiz_0 instance_name(.clk_out1(clk_out1),.clk_in1(clk_in1));

(* dont_touch = "true" *)
Program_Counter PC1(clk_out1, indicators, pc, tupc, stupc, stucsrpc, pcpi, ipc, jalr, srpc, bacm, PC_MAR, PC_ALU, CM_PC, REGS_PC, PC_REGS, CSR_PC, PC_CSR, TU_PC);

(* dont_touch = "true" *)
MAR MAR1(clk_out1, mpi, rpi, spm, srm, crm, smi, PC_MAR, MAR_RAM, REGS_MAR, CM_MAR, MAR_UART);

(* dont_touch = "true" *)
IR IR1(clk_out1, sri, srm, smi, mpi, rpi, srr8, srr16, srr32, crm, pc, IR_CM, RAM_IR);

(* dont_touch = "true" *)
RAM RAM1(clk_out1, indicators, rst, srr8, srr16, srr32, sramrs8, sramrs16, sramrs32, sramru8, sramru16, tx_irq_en, MAR_RAM, RAM_REGS, REGS_MAR, REGS_RAM, RAM_IR);

(* dont_touch = "true" *)
CM CM1(clk_out1, rst, tu, restu, ecall, sret, mret, rxinterrupt, tx_int_ack, pc, bacm, stucsrpc, pcpi, ipc, mpi, rpi, scmcsi, scmcst, scmpc, jalr, cur, smi, spm, srm, crm, sri, spcr, spcrt, srpc, spca, sraa, srab, sar, scmr, srcm, srram, srr8, srr16, srr32, sramrs8, sramrs16, sramrs32, sramru8, sramru16, scsr, srcs, spcs, scsp, scmcs, csbmr, csbmi, csor, csan, smrar, scma, scmau, stupc, ALU_OP, CM_PC, IR_CM, REGS_CM, CM_REGS, REG_A, REG_B, REG_D, CM_ALU, ALU_CM, CM_CSR, CM_CSRI, CM_CSRT, CM_MAR);

(* dont_touch = "true" *)
REGS REGS1(clk_out1, indicators, out, spcr, spcrt, sraa, srab, sar, scmr, sramr, scsr, sramrs8, sramrs16, sramrs32, sramru8, sramru16, REGS_UART, UART_REGS, PC_REGS, REGS_PC, CM_REGS, REGS_CM, RAM_REGS, REGS_RAM, CSR_REGS, REGS_CSR, REGS_MAR, REGS_ALU_A, REG_A, REG_B, REG_D, REGS_ALU_B, ALU_REGS);

(* dont_touch = "true" *)
ALU ALU1(clk_out1, spca, sraa, srab, scma, scmau, bacm, div_cnt, ALU_OP, REGS_ALU_A, REGS_ALU_B, ALU_REGS, CM_ALU, PC_ALU, ALU_CM);

(* dont_touch = "true" *)
CSR CSR1(clk_out1, ecall, tucall, tuint, tupccs, stucsr, scmcsi, scmcst, scsr0, srcs, spcs, scmcs, csbmr, csbmi, csor, csan, CSR_REGS, REGS_CSR, CSR_PC, PC_CSR, CM_CSR, CM_CSRI, CM_CSRT, TU_CSR);

(* dont_touch = "true" *)
Trap_Unit TU1(clk_out1, restu, tucall, tuint, tupc, rxinterrupt, tx_irq_en, rxpending, rxack, txready, tx_int_ack, txbusy, ecall, sret, mret, tupccs, tucspc, stupc, stucsr, stucsrpc, scsr0, TU_PC, TU_CSR);

(* dont_touch = "true" *)
UART UART1(clk_out1, indicators, cur, tx, rx, rxack, srm, mpi, srr8, srr32, rxinterrupt, txready, txbusy, UART_REGS, REGS_RAM, REGS_MAR, UART_MAR, MAR_UART);

endmodule


module Program_Counter(clk, indicators, pc, tupc, stupc, stucsrpc, pcpi, ipc, jalr, srpc, bacm, PC_MAR, PC_ALU, CM_PC, REGS_PC, PC_REGS, CSR_PC, PC_CSR, TU_PC);

input clk, indicators, pc, ipc, tupc, stupc, stucsrpc, pcpi, jalr, srpc, bacm;
input[31:0] REGS_PC, CSR_PC, TU_PC;
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
if (stucsrpc)
begin
	program_counter <= CSR_PC;
end
else if (srpc)
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
else if (stupc)
begin
	program_counter <= (TU_PC);
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


module CSR(clk, ecall, tucall, tuint, tupccs, stucsr, scmcsi, scmcst, scsr0, srcs, spcs, scmcs, csbmr, csbmi, csor, csan, CSR_REGS, REGS_CSR, CSR_PC, PC_CSR, CM_CSR, CM_CSRI, CM_CSRT, TU_CSR);

input clk, ecall, tucall, tuint, tupccs, stucsr, scsr0, srcs, spcs, scmcs, scmcsi, scmcst, csbmr, csbmi, csor, csan;
input[31:0] REGS_CSR, PC_CSR, CM_CSR, CM_CSRI, CM_CSRT, TU_CSR;
output[31:0] CSR_REGS, CSR_PC;
reg[31:0] SEPC;
reg[2:0] csr_enum;

always @(posedge clk)
begin
if (tupccs)
begin
	SEPC <= PC_CSR + 4;
end
else if (srcs)
begin
	SEPC <= REGS_CSR;
end
end

assign	CSR_REGS = SEPC;
assign	CSR_PC = SEPC;

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


module Trap_Unit(clk, restu, tucall, tuint, tupc, rxinterrupt, tx_irq_en, rxpending, rxack, txready, tx_int_ack, txbusy, ecall, sret, mret, tupccs, tucspc, stupc, stucsr, stucsrpc, scsr0, TU_PC, TU_CSR);

input clk, restu, ecall, sret, mret, rxinterrupt, tx_irq_en, txready, txbusy;
output reg  tucall, tuint, tupc, tupccs, tucspc, stupc, stucsr, stucsrpc, scsr0, rxpending, rxack, tx_int_ack;
output reg[31:0] TU_PC, TU_CSR;
reg takeirq, resirq, tx_fsm_cont, supmode;
reg[1:0] rxintstate, irqstate, ecallstate, sretstate;
reg[2:0] txintstate;
reg[31:0] mtvec, rxinterruptvec, txinterruptvec;
reg[15:0] fsm_cont_count;

initial
begin
	mtvec = 152*4;
	rxinterruptvec = 292*4;
	txinterruptvec = 269*4;
	rxintstate = 0;
	irqstate = 0;
	resirq = 0;
	tx_int_ack = 0;
	tx_fsm_cont = 0;
	txintstate = 0;
	ecallstate = 0;
	sretstate = 0;
end

always @(posedge clk)
begin
if (ecall && ecallstate == 2'b00)
begin
	ecallstate <= 2'b01;
	TU_CSR <= mtvec;
	TU_PC <= mtvec;
	ecallstate <= 2'b01;
	supmode <= 1;
	tupccs <= 1;
end
else if (ecallstate == 2'b01)
begin
	stucsr <= 0;
	tupccs <= 1;
	ecallstate <= 2'b10;
end
else if (ecallstate == 2'b10)	
begin
	tucall <= 0;
	tupccs <= 0;
	stupc <= 1;
	stucsr <= 0;
	ecallstate <= 2'b11;
end
else if (ecallstate == 2'b11)
begin
	stupc <= 0;
	ecallstate <= 2'b00;
end
if (sret && sretstate == 2'b00)
	begin
		stucsrpc <= 1;
		supmode <= 0;
		sretstate <= 2'b01;
	end
else if (sretstate == 2'b01)
	begin
		stucsrpc <= 1;
		sretstate <= 2'b10;
	end
else if (sretstate == 2'b10)
	begin
		stucsrpc <= 0;
		sretstate <= 2'b00;
	end
if (mret)
	begin
		stucsrpc <= 1;
		tupc <= 1;
	end	

else if (irqstate == 2'b00)
begin
    	if (rxinterrupt)
			irqstate <= 2'b01;
end
else if (irqstate == 2'b01)
begin
    TU_CSR <= rxinterruptvec;
	TU_PC <= rxinterruptvec;	
	irqstate <= 2'b10;
end
else if (irqstate == 2'b10)
begin
	if (!supmode)
	begin
		stucsr <= 1;
		tupccs <= 1;
	end	
	stupc <= 1;
	tuint <= 1;
	irqstate <= 2'b11;
end
else if (irqstate == 2'b11)
begin
	tuint <= 0;
	tupccs <= 0;
	stucsr <= 0;
	stupc <= 0;
	irqstate <= 2'b00;
end
if (txintstate == 3'b000 && tx_irq_en && !txbusy)
begin
    	TU_CSR <= txinterruptvec;
    	TU_PC <= txinterruptvec;
    	txintstate <= 3'b001;
        tx_fsm_cont <= 1;    	
end
else if (txintstate == 3'b001)
begin
	if (!supmode)
	begin
		stucsr <= 1;
		tupccs <= 1;
	end	
	tuint <= 1;
	stupc <= 1;
        tx_int_ack <= 1;
	txintstate <= 3'b010;
end
else if (txintstate == 3'b010)
begin
	tuint <= 0;
	tupccs <= 0;
	stucsr <= 0;
	stupc <= 0;
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



