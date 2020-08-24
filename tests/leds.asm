main:
	jal $imm, $zero, $zero, leds		# start the func leds
	halt $zero, $zero, $zero, 0		#halt execution
leds:
	add $t1, $zero, $imm, 255		# $t1 = 255
	out $t1, $zero, $imm, 13		# timermax = 252
	
	add $t2, $zero, $imm, 1		# $t2 = 1
	out $t2, $zero, $imm, 11		# turn on the timerenable
	out $t2, $zero, $imm, 9		# turn on the first led

onesec:
	in $t0, $zero, $imm, 3		# turn on irq0status 
	beq  $imm, $t0, $zero, onesec		# if $t0 = irq0status  = 0 : go again to "onesec", else : continue to loop

loop:
	out $zero, $zero,  $imm, 3		# turn off irq0status
	add $t2, $t2, $t2, $zero		# $t1 = 2 * $t1
	out $t2, $zero, $imm, 9		# turn on the next led
	beq $ra, $zero, $t2, 0		# if $t2 = 0 : finish 
	add $zero, $zero, $zero, 0		# we need 256 cycles between 2 leds
	beq  $imm, $zero, $zero, onesec 	# go back to "onesec"  : for the 1 second we need
	

