			add $sp , $zero , $imm , 500
			add $a0 , $zero , $imm ,1024                       # loading the address of the first element of the array
			add $a1 , $zero , $imm , 16                        # loading the number of elements in the array
			jal  $imm , $zero , $zero , BubbleSort             # unconditional jump to BubbleSort
			halt $zero, $zero, $zero, 0
BubbleSort:		
			add  $sp, $sp, $imm, -6                            # space for 6 slots
			sw   $s0 , $sp , $zero, 0 
			sw   $s1 , $sp , $imm , 1
			sw   $s2 , $sp , $imm , 2
			sw   $a0 , $sp , $imm , 3
			sw   $a1 , $sp , $imm , 4
			sw   $v0 , $sp , $imm , 5
			sub  $a1 , $a1 , $imm , 1                          # $a1 = 15
OuterLoop:                                                     # if zero swapps in inner loop - break and return
			add $v0 , $zero , $imm , 0                         # $v0 stores: 0 if no swaps 1 otherwise
			add $s2 , $zero , $imm, -1                         # set j = -1
InnerLoop:
			add $s2 , $s2 , $imm , 1                           # j++
			bge $imm , $s2 , $a1 , Condition                   # checking if j>=15
			add $t0 , $a0 , $s2, 0                             # array + j
			lw  $s0 , $t0 , $imm , 0                           # $s0 = array[j]
			lw  $s1 , $t0 , $imm , 1                           # $s1 = array[j+1]
			bgt $imm, $s0 , $s1 , Swap                         # if(array[j] > array[j+1])
			beq $imm , $zero , $zero , InnerLoop               # unconditional jump
Swap:
			sw  $s1 , $t0 , $imm , 0                           #set array[j+1] = array[j]
			sw  $s0 , $t0 , $imm , 1                           #set A[j] = A[j+1]
			add $v0 , $zero , $imm, 1                          #set v0 = 1 (true) in other words, swap occured
			beq $imm , $zero, $zero, InnerLoop                 #unconditional jump
Condition:
			bne $imm , $v0 , $zero , OuterLoop                 #if swaps occurred back to outerLoop
			beq $imm , $zero , $zero , Finish                  #if no swaps - function execution can stop ---> break
Finish:
			lw   $s0 , $sp , $zero , 0                         # restore $s0 from stack
			lw   $s1 , $sp , $zero , 1                         # restore $s1 from stack
			lw   $s2 , $sp , $zero , 2                         # restore $s2 from stack
			lw   $a0 , $sp , $zero , 3                         # restore array address
			lw   $a1 , $sp , $zero , 4                         # restore argument - number of elements
			lw   $v0 , $sp , $zero , 5                         # restore return address
			add  $sp , $sp , $imm, 6                           # release allocated space
			beq  $ra, $zero, $zero, 0                          # return fron function
			

		

            .word 1024 2                
			.word 1025 -56              
			.word 1026 1                
			.word 1027 10               
			.word 1028 -6               
			.word 1029 4                
			.word 1030 94               
			.word 1031 12               
			.word 1032 -1               
			.word 1033 0                
			.word 1034 712              
			.word 1035 222              
			.word 1036 3334             
			.word 1037 -333             
			.word 1038 543              
			.word 1039 225              
			


			
			
			

