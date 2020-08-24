#####################################
# $to store the current time        #
# $t1 arithmetic usage              #
# $t2 indicating on 9 - corner case #       
#####################################
Main:			add $t2 , $zero , $imm , 9           # $s2 = 9
				.word 1024 0x195955                  # starting time
                .word 1025 0x1fffff                  # one second before the switch of 200000
				add $t0 , $zero , $imm , 254         # $t0 =  254
			    out $t0 , $zero , $imm , 13          # timermax = 254 clock cycles
				jal $imm , $zero , $zero , clockRun  # (unconditional jump) starting the function of clock counting from 19:59:00 to 20:00:05
				halt $zero , $zero , $zero , 0       # finishing clock-test
clockRun:
			    lw  $t0 , $zero , $imm , 1024        # storing the starting time (19:59:55)to $t0
			    out $t0 , $zero , $imm , 10          # enabling the display 
			    add $t1 , $zero , $imm , 1           # $t1 = 1
			    out $t1 , $zero , $imm , 11          # timerenable = 1 (turning in timer-enable)
CoolDown:
			    in  $t1  , $zero  , $imm , 3         # $t1 = irq0status = ---> 0: when timercurrent = timermax, timercurrent : otherwise
			    beq $imm, $t1, $zero, CoolDown       # staying in this loop until timermax reached.
Display:	  
				out $zero, $zero, $imm, 3            # irq0status = 0
			    add $t0 , $t0 , $imm , 1             # next second increment
			    out $t0 , $zero , $imm , 10          # display the next second. here we finish 256 clock cycles which equally to 1 sec
			    and $t1 , $t0 , $imm , 15            # masking the 4 LSB bits of the time with 15 mask (bin(15)=000..->..1111) 
			    beq $imm , $t1 , $t2 , CornerCase    # fixing corner case of 19:59:59 - the only time last digit is in clock should be 9
			    and $t1 , $t0 , $imm , 15            # masking the 4 LSB bits with 1's mask 
			    beq $ra , $t1 , $imm 5               # cheacking if the last digit(4LSB's) is 5 if yes break. 
			    beq $imm , $zero , $zero , CoolDown  # back to count cpu cycles in order to accomplish 1 second.
CornerCase:
			    lw  $t0 , $zero , $imm , 1025        # loading the time second before 200000
			    beq $imm , $zero , $zero , CoolDown  # return from Interrunpt