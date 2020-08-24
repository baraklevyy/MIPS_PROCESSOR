main:
	.word 1024 12				# set A[0] = 12
	.word 1025 6				# set A[1] = 6
	.word 1026 5				# set A[2] = 5	
	.word 1027 11				# set A[3] = 11
	.word 1028 20 				# set A[4] = 20
	.word 1029 23				# set A[5] = 23
	.word 1030 34 				# set A[6] = 34
	.word 1031 34				# set A[7] = 34
	.word 1032 -3 				# set A[8] = -3
	.word 1033 22 				# set A[9] = 22
	.word 1034 0 				# set A[10] = 0
	.word 1035 8 				# set A[11] = 8
	.word 1036 -1 				# set A[12] =-1
	.word 1037 2 				# set A[13] = 2
	.word 1038 40 				# set A[14] = 40
	.word 1039 1 				# set A[15] = 1
	add $sp , $zero , $imm, 1022
	add $s0 , $zero , $imm, 1024 			# the address of the start of the array - A into $s0.
	add $a0 , $zero , $imm, 0			# low = 0
	add $a1 , $zero , $imm, 15			# high = 15
	jal $imm, $zero , $zero , quicksort		# go to quicksort
	halt $zero, $zero, $zero, 0			# halt execution

quicksort:				
	add $sp, $sp, $imm, -3			# space in stack
	sw $a0, $sp, $imm, 0			# store a0
	sw $a1, $sp, $imm, 1			# store a1
	sw $ra, $sp, $imm, 2				# return address

	blt $imm, $a0, $a1, partition 			# if low < high : go to partition
	beq $imm, $zero, $zero, END			#  else : go to END

partition: 			
	add $t1, $a0, $imm, -1			# t1 = i
	add $t2, $a1, $imm, 1			# t2 = j 

pivot: 
	lw $t0, $s0, $a0, 0                  			# $t0=pivot
	add $t2, $t2, $imm, -1       			# $t2 = $t2 -1
	lw $t3, $t2, $s0, 0                			#  $t3=A[j]
	bgt $imm, $t3, $t0, pivot    			# if A[j]>pivot : go again to "pivot" 
 
loop:
	add $t1, $t1, $imm, 1            			# $t1 = $t1 +1
	lw $t3, $s0, $t1, 0                 			# $t0=A[i]
	blt $imm, $t3, $t0, loop     			# if A[j]>=pivot : go agin to loop
	blt $imm, $t1, $t2, swap        			# else: if i<j : go to swap with A[j],A[I]
	add $a1, $t2, $imm, 0           			# $a1= $t2= j
	jal $imm, $zero, $zero, quicksort 		# go to quicksort with A, p, j
	lw $a1, $sp, $imm, 1               			# restore $a1 = r
	add $a0, $t2, $imm, 1              			# $a0 = j+1
	jal $imm, $zero, $zero, quicksort  		# go to quicksort with A, j+1, r

END:
 	lw $a0, $sp, $imm, 0				# release s0
 	lw $a1, $sp, $imm, 1				# release a0
 	lw $ra, $sp, $imm, 2				# release a1
 	add $sp, $sp, $imm, 3			# restore the stack
 	beq $ra, $zero, $zero, 0			# return $ra

swap:
	lw $t0, $t2, $s0, 0				# t0 = A[j]
	sw $t3, $t2, $s0, 0				# A[j] = A[i] 
	sw $t0, $t1, $s0, 0				# A[i] = A[j] 
	beq  $imm, $zero, $zero, pivot		# go back to "pivot"



