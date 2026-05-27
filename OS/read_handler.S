.globl read_handler
#.align 4
read_handler:
    lui 	a3, 2			# prepare for UART_RX_BUF 1798*4
    addi	a3, a3, -1000		# ...
    lui 	a4, 2			# prepare for UART_TX_BUF 1795*4
    addi	a4, a4, -1012		# ...
    lui 	a5, 2			# prepare for UART_TRIG 1797*4
    addi	a5, a5, -1004		# ...
    addi	x6, x0, 1		# 1 utility register 
    addi	x7, x0, 0x7F		# top ascii utility register
    addi	a7, x0, 0		# set up i
    addi	a6, x0, 13		# set up '\r' character
    wfi
    beq 	a2, x0, canonical	# branch to canonical mode if flag == 0
    lbu		x5, 0(a3)		# load UART_RX_BUF into x5
    bgt		x5, x7, wait_for_interrupt
    sb		x5, 0(a0)		# store x5 in buf
    addi	a7, a7, 1		# increment i
    addi 	a0, a7, 0		# return i
    jalr 	x0, 0(ra)		# ret
    
canonical:
    lbu		x5, 0(a3)		# load UART_RX_BUF into x5
    bgt		x5, x7, wait_for_interrupt
    sb		x5, 0(a0)		# store x5 in buf
    beq		a6, x5, return_i	# branch to return i
    sb	 	x5, 0(a4)		# store c in UART_TX_BUF
    addi	a0, a0, 1		# increment buf
    addi	a7, a7, 1		# increment i
    sb		x6, 0(a5)		# set UART_TRIG to 1
    sb		x0, 0(a5)		# set UART_TRIG to 0
    wfi
    
wait_for_interrupt:
    wfi					#wfi
    
return_i:
    addi	a0, a7, 0		# return i
    jalr	x0, 0(ra)		# ret	
