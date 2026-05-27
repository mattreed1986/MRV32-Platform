.globl write_handler
#.align 4
write_handler:
    addi	a2, x0, 0		# set a2 to 0
    lui 	a3, 2			# prepare for a3 = 1997*4
    addi	a3, a3, -204		# ...
    lui 	a4, 2			# prepare for UART_TX_BUF 1795*4
    addi	a4, a4, -1012		# ...
    lui 	a5, 2			# prepare for UART_TRIG 1797*4
    addi	a5, a5, -1004		# ...
    addi	a6, x0, 1		# 1 utility register 
    sw 		a1, 0(a3)		# store count in 1997*4
    bge 	a1, a6, continue	# branch away from return if count > 0
    addi	a0, a2, 0		# set a0 to iterations
    jalr 	x0, 0(ra)		# ret
    
    continue:
    lbu  	t0, 0(a0)    		# load byte FROM buffer
    sb   	t0, 0(a4)    		# write that byte to UART TX
    addi	a0, a0, 1		# increment a0
    sb		a6, 0(a5)		# store 1 in UART_TRIG
    sb		x0, 0(a5)		# store 0 in UART_TRIG
    addi	a2, a2, 1		# increment iterations
    addi	a1, a1, -1		# decrement size(count)
    wfi
