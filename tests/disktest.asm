#disktest
#############################################
# $t0 = sector number                       #
# $t1 = read/write mode to diskcmd          #
# $t2 == 1  - indicating mode if(t1 != t2)  #
# $t3 = diskstatus                          #
#############################################

Main:				
					add $a0 , $zero , $imm , 1024							# $a0 = 1024
					out $a0 , $zero , $imm , 16								#setting diskbuffer to 1024
					add $t2 , $zero , $imm , 1								# $t2 = 1
					jal $imm , $zero , $zero , DiskInitialized				#jump to diskfunction
					halt $zero , $zero , $zero , 0							#finish program
DiskInitialized:
					add $t0 , $zero , $zero , 0								# $t0=0 - sector number initialized to 0
					add $t1 , $zero , $imm ,1								# $t1=1 - initialized for reading mode
DiskReadWrite:
					out $t0 , $zero , $imm , 15                             #setting disksector
					out $t1 , $zero , $imm , 14                             #setting diskcmd as $t1
Cool-down:													                
					in  $t3  , $zero , $imm , 17                            #set $t3 = diskstatus
					bne $imm , $zero , $t3 , Cool-down                      #continue looping until diskstatus = 0
					out $zero, $imm, $zero, 1                               # turn off irq1status
Reading:													                
                    add $t2 , $zero , $imm , 1                              # $t2 = 1
					bne $imm , $t1 , $t2 , Writing                          # jump to the Reading-switch epoch becasue the last diskcmd was writing ---> bench if the last diskcmd was writing
					add $t1 , $zero , $imm , 2                              # $t1 = 2 next time writing mode
					add $t0 , $t0 , $imm , 4                                # $t0+=4
					beq $imm , $zero , $zero , DiskReadWrite                # return to DiskReadWrite for next operation
Writing:													                
					add $t1 , $zero , $imm , 1                              # $t1 = 1 indicating on reading mode
					sub $t0 , $t0 , $imm , 3                                # $t0-=3
					beq $ra , $t0 , $imm , 4                                # if we reached to read from sector number 4 - break!
					beq $imm , $zero , $zero , DiskReadWrite                # return to DiskReadWrite for next operation