.data 0x10008000
a:.word 3
b:.word -4
.data 0x10010000
:.byte ''*'', '/', 
:.word 11, 12, 11, 12, 
.str1:.asciiz "18373089"
.text
la $s0, 0
sw $s0, -12($sp)
la $a0 .str1
li $v0, 4
syscall
la $v0, 11
la $a0, '\n'
syscall
li $v0, 5
syscall
sw $v0, -4($sp)
li $v0, 5
syscall
sw $v0, -8($sp)
label3 :
lw $s1, -12($sp)
la $s2, 2
bgt $s1, $s2, label4
la $s1, 2
lw $s2, -12($sp)
mul $s0, $s1, $s2
sw $s0, -16($sp)
lw $a0, -16($sp)
li $v0, 1
syscall
la $v0, 11
la $a0, '\n'
syscall
lw $s1, -12($sp)
la $s2, 1
add $s0, $s1, $s2
sw $s0, -20($sp)
lw $s0, -20($sp)
sw $s0, -12($sp)
j label3
label4 :
la $s0, 0
sw $s0, -12($sp)
label5 :
lw $s1, -12($sp)
la $s2, 3
beq $s1, $s2, label6
la $s1, 3
lw $s2, -12($sp)
mul $s0, $s1, $s2
sw $s0, -24($sp)
lw $a0, -24($sp)
li $v0, 1
syscall
la $v0, 11
la $a0, '\n'
syscall
lw $s1, -12($sp)
la $s2, 1
add $s0, $s1, $s2
sw $s0, -12($sp)
j label5
label6 :
la $s0, 3
sw $s0, -12($sp)
label7 :
lw $s1, -12($sp)
la $s2, 0
ble $s1, $s2, label8
la $s1, 4
lw $s2, -12($sp)
mul $s0, $s1, $s2
sw $s0, -28($sp)
lw $a0, -28($sp)
li $v0, 1
syscall
la $v0, 11
la $a0, '\n'
syscall
lw $s1, -12($sp)
la $s2, 1
sub $s0, $s1, $s2
sw $s0, -12($sp)
j label7
label8 :
.exit:
li $v0, 10
syscall
