module UART(clk, indicatorsin, cur, tx, rx, rxack, srm, mpi, srr8, srr32, rxinterrupt, txready, txbusy, UART_REGS, REGS_UART, REGS_MAR, UART_MAR, MAR_UART);

input clk, indicatorsin, cur, rx, srm, mpi, srr8, srr32, rxack;
input[31:0] REGS_UART, REGS_MAR, MAR_UART;
output[31:0] UART_MAR;
output reg[7:0] UART_REGS;
output reg tx, rxinterrupt, txready, txbusy;

reg[13:0] baudcnt, txbaudcnt;
reg[13:0] interbitcount;
reg[7:0] UART_MEM_OUT;
reg[7:0] UART_MEM_IN;
reg[1:0] txstate; 
reg[3:0] rxstate;
reg[3:0] rxbitcnt;
reg[4:0] txbitcnt;
reg txstart, txdone, rxinterrupttrig, rxbusy, rxready;

initial
begin
	UART_MEM_OUT = 8'b0;
	UART_REGS = 8'b0;
	tx = 1'b1;
	txstate = 2'b00;
	rxstate = 2'b00;
	rxbitcnt = 0;
	rxinterrupt = 1'b0;
	rxbusy = 0;
	baudcnt = 14'b0;
	txready = 1;
	txdone = 1'b0;
	txbusy = 1'b0;
end

wire indicators = indicatorsin;
	
always @(posedge clk)
if (srr8 && MAR_UART == 1795*4)
begin
    UART_MEM_IN <= REGS_UART[7:0];
end

always @*
if ((srr8 || srr32) && MAR_UART == 1797*4 && !txbusy && !rxbusy)
	txstart = 1;
else
	txstart = 0;

always @* 
begin
    txbusy = (txstate == 2'b01 || txstate == 2'b10 || txstate == 2'b11);
end 
   
//TRANSMISSION
always @(posedge clk)
begin
	if (txstart && txstate == 2'b00)
	begin
	    tx <= 1'b0;
	    txstate <= 2'b01;
	    txready <= 0;
	    txbitcnt <= 0;
	    txbaudcnt <= 0;
	end
	else if (txstate == 2'b01)
	begin
	    if (txbaudcnt == 5208)
	    begin
    		txbaudcnt <= 0;
    		tx <= UART_MEM_IN[txbitcnt];
	       	txbitcnt <= txbitcnt + 1;
		    if (indicators)
	    		$display("TX IS %b and bit = %d", tx, txbitcnt);
		    if (txbitcnt == 4'd7)
		    begin
			    txstate <= 2'b10;
			    txbitcnt <= 0;
		    end
	    end
	    else
	    begin
	        txbaudcnt <= txbaudcnt + 1;
		end
	end
	else if (txstate == 2'b10)
	begin
	    if (txbaudcnt == 5208)
	    begin
	    	if (indicators)
	    			$display("TX IS %b and bit = %d", tx, txbitcnt);
	        tx <= 1;
	        txbaudcnt <= 0;
	        txstate <= 2'b11;
	    end
	    else
	    txbaudcnt <= txbaudcnt + 1;
	end
	else if (txstate == 2'b11)
	begin
	    if (txbaudcnt == 5208)
	    begin
	    	if (indicators)
	    			$display("TX IS %b and bit = %d", tx, txbitcnt);
	        txbaudcnt <= 0;
	        txstate <= 2'b00;
	        txready <= 1;
	    end
	    else
	    txbaudcnt <= txbaudcnt + 1;
	end
end

always @*
    if (rxstate == 3'b100)
    begin
        rxinterrupt = 1'b1;
    end
    else
    begin
        rxinterrupt = 1'b0;
    end

//RECEPTION

always @(posedge clk)
begin
    if (cur)
    	UART_REGS[7:0] <= 0;
    if (rxstate == 3'b000 && !rxinterrupt)
    begin
        if (!rx && !txbusy)
        begin
            rxbusy <= 1;
            rxstate <= 3'b001;
            interbitcount <= 0;
        end
    end
  	else if (rxstate == 3'b001) 
	begin
		if (interbitcount == 2604)
		begin
		    if (!rx)
        	    begin
		      	rxstate <= 3'b010;
		       	interbitcount <= 0;
		      	rxbitcnt <= 0;
		      	UART_MEM_OUT <= 0;
		    end
		    else
		        rxstate <= 3'b000;
		end
		interbitcount <= interbitcount + 1;   
	end
	else if (rxstate == 3'b010)
	begin
	    if (interbitcount == 5208)
		    begin
           	    UART_MEM_OUT[rxbitcnt] <= rx;
		        if (indicators)
	    			$display("rx = %b", rx);
			    interbitcount <= 0;
			    rxbitcnt <= rxbitcnt + 1;
		        if (rxbitcnt == 3'd7)
		        begin
			        rxstate <= 3'b011;
			    end
		    end
		else
    	    interbitcount <= interbitcount + 1; 
       
	end
	else if (rxstate == 3'b011)
	begin
		if (interbitcount == 5208)
		begin
			UART_REGS[7:0] <= UART_MEM_OUT;
			rxstate <= 3'b100;
   			interbitcount <= 0;
   		end
   		interbitcount <= interbitcount + 1;
   	end
   	else if (rxstate == 3'b100)
	begin
		rxstate <= 3'b000;
   		rxbusy <= 0;
   	end
end

	
endmodule

