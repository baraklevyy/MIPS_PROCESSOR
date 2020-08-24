main:
		.word 0x100 2				#values of matrice 1= A
		.word 0x101 4
		.word 0x102 6
		.word 0x103 8
		.word 0x104 10
		.word 0x105 12
		.word 0x106 14
		.word 0x107 16
		.word 0x108 18
		.word 0x109 20
		.word 0x10A 22
		.word 0x10B 24
		.word 0x10C 26
		.word 0x10D 28
		.word 0x10E 30
		.word 0x10F 32
		.word 0x110 1				#values of matrice 2 = B
		.word 0x111 3
		.word 0x112 5
		.word 0x113 7
		.word 0x114 9
		.word 0x115 11
		.word 0x116 13
		.word 0x117 15
		.word 0x118 17
		.word 0x119 19
		.word 0x11A 21
		.word 0x11B 23
		.word 0x11C 25
		.word 0x11D 27
		.word 0x11E 29
		.word 0x11F 31				
		
	
		add $sp , $zero , $imm, 0x130
		add $t0 , $zero , $imm, -3			# t0 = -3
		add $sp , $sp , $t0, 0  			# sp = sp - 3 (define space in stack)
		sw $s0 , $sp , $imm, 0 			# store s0 in stack
		sw $s1 , $sp , $imm, 1 			# store s1 in stack
		sw $s2 , $sp , $imm, 2 			# store s2 in stack
		
		add $s0 , $zero , $imm , 0x100 		# set $s0 to the address of the first argument of matrix A.
		add $s1 , $zero , $imm , 0x110 		# set $s1 to the address of the first argument of matrix B.
		add $s2 , $zero , $imm , 0x120			# set $s2 to the address of the first argument of matrix C - the sum matrix.
		
		
for:	
		lw $t0 , $s0 , $zero, $zero 			# t0 = A[s0] 
		lw $t1 , $s1 , $zero	, $zero			# t1 = B[s1]
		add $t0 , $t0 , $t1 , $zero			# t0 = t0+t1 = A[s0] + B[s1]
		sw $t0 , $s2 , $zero , $zero			# save result to the matrix. 
		
		add $s2 , $s2 , $imm , 1   			# s2 is now pointing to the next argument of matrix C.
		add $s1 , $s1 , $imm , 1  			# s1 is now pointing to the next argument of matrix B.
		add $s0 , $s0 , $imm , 1  			# s0 is now pointing to the next argument of matrix A.

		add $t0 , $s0, $imm , 0			# $t0 = $s0
		add $t1 , $zero , $imm , 0x110		#$t1=0x110
		ble $imm, $t0, $t1, for			#if we didnt done: go again to for with the next arguments
		bgt $imm, $t0, $t1, END			#else - go to END
		
END:	
		lw $s0 , $sp , $imm, 0 			# restore values from stack
		lw $s1 , $sp , $imm, 1 			# restore values from stack
		lw $s2 , $sp , $imm, 2 			# restore values from stack
		add $sp , $sp , $imm , 3			# sp = sp + 3 (release space in stack).
		halt  $zero, $zero , $zero ,0			#halt