.data 0x10008000
.data 0x10010000
a:.space 4000
b:.space 4000
kernelid:.space 160

.text
jal main
li $v0, 10
syscall

main :
sw $ra, -4($sp)
sw $fp, -8($sp)
move $fp, $sp
la $s0, 0
sw $s0, -16($sp)
la $s0, 0
sw $s0, -44($sp)
la $s0, 0
sw $s0, -48($sp)
addiu $sp, $sp, -96
la $s0, 5
sw $s0, -20($fp)
la $s0, 10
sw $s0, -24($fp)
la $s0, 10
sw $s0, -28($fp)
la $s0, 100
sw $s0, -36($fp)
label45 :
lw $s1, -44($fp)
lw $s2, -36($fp)
bge $s1, $s2, label46
lw $s1, -44($fp)
li $at, 10
div $s1, $at
mflo $s0
sw $s0, -52($fp)
lw $s1, -52($fp)
mul $s0, $s1, 10
sw $s0, -56($fp)
lw $s1, -44($fp)
lw $s2, -56($fp)
subu $s0, $s1, $s2
sw $s0, -60($fp)
lw $t0, -44($fp)
sll $s0, $t0, 2
lw $s1, -60($fp)
sw $s1, a($s0)
lw $s1, -44($fp)
addiu $s0, $s1, 1
sw $s0, -64($fp)
lw $s0, -64($fp)
sw $s0, -44($fp)
j label45
label46 :
la $s0, 15
sw $s0, -32($fp)
la $s0, 0
sw $s0, -44($fp)
la $t0, 0
sll $s0, $t0, 2
la $s1, 1
sw $s1, kernelid($s0)
la $t0, 1
sll $s0, $t0, 2
la $s1, 2
sw $s1, kernelid($s0)
la $s0, 0
sw $s0, -44($fp)
label47 :
lw $s1, -44($fp)
la $s2, 2
bge $s1, $s2, label48
lw $t0, -44($fp)
sll $s0, $t0, 2
lw $s1, kernelid($s0)
sw $s1, -68($fp)
lw $s0, -68($fp)
sw $s0, -4($sp)
lw $s0, -24($fp)
sw $s0, -8($sp)
lw $s0, -28($fp)
sw $s0, -12($sp)
lw $s0, -20($fp)
sw $s0, -16($sp)
addiu $sp, $sp, -16
jal convn
addiu $sp, $sp, 16
sw $v0, -72($fp)
la $t0, 0
sll $s0, $t0, 2
lw $s1, b($s0)
sw $s1, -76($fp)
lw $a0, -76($fp)
li $v0, 1
syscall
la $v0, 11
la $a0, '\n'
syscall
lw $s1, -24($fp)
lw $s2, -28($fp)
mul $s0, $s1, $s2
sw $s0, -80($fp)
lw $s0, -80($fp)
sw $s0, -12($fp)
label43_mymemmove3 :
lw $s1, -16($fp)
lw $s2, -12($fp)
bge $s1, $s2, label44_mymemmove3
lw $t0, -16($fp)
sll $s0, $t0, 2
lw $s1, b($s0)
sw $s1, -84($fp)
lw $t0, -16($fp)
sll $s0, $t0, 2
lw $s1, -84($fp)
sw $s1, a($s0)
lw $s1, -16($fp)
addiu $s0, $s1, 1
sw $s0, -88($fp)
lw $s0, -88($fp)
sw $s0, -16($fp)
j label43_mymemmove3
label44_mymemmove3 :
j label51
label51 :
la $t0, 0
sll $s0, $t0, 2
lw $s1, a($s0)
sw $s1, -92($fp)
lw $a0, -92($fp)
li $v0, 1
syscall
la $v0, 11
la $a0, '\n'
syscall
lw $s1, -44($fp)
addiu $s0, $s1, 1
sw $s0, -96($fp)
lw $s0, -96($fp)
sw $s0, -44($fp)
j label47
label48 :
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra

myscanf :
sw $ra, -4($sp)
sw $fp, -8($sp)
move $fp, $sp
addiu $sp, $sp, -12
li $v0, 5
syscall
sw $v0, -12($fp)
lw $v0, -12($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra

myprintf :
sw $ra, -4($sp)
sw $fp, -8($sp)
move $fp, $sp
addiu $sp, $sp, -8
lw $a0, 0($fp)
li $v0, 1
syscall
la $v0, 11
la $a0, '\n'
syscall
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra

checkrange :
sw $ra, -4($sp)
sw $fp, -8($sp)
move $fp, $sp
addiu $sp, $sp, -16
label1 :
lw $s1, 0($fp)
la $s2, 1073741824
ble $s1, $s2, label2
lw $s1, 0($fp)
addiu $s0, $s1, -1073741824
sw $s0, -12($fp)
lw $s0, -12($fp)
sw $s0, 0($fp)
j label1
label2 :
label3 :
lw $s1, 0($fp)
la $s2, 0
bge $s1, $s2, label4
lw $s1, 0($fp)
addiu $s0, $s1, 1073741824
sw $s0, -16($fp)
lw $s0, -16($fp)
sw $s0, 0($fp)
j label3
label4 :
lw $v0, 0($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra

reduce :
sw $ra, -4($sp)
sw $fp, -8($sp)
move $fp, $sp
addiu $sp, $sp, -224
lw $s1, 8($fp)
la $s2, 0
bne $s1, $s2, label5
lw $s1, 4($fp)
lw $s2, 0($fp)
addu $s0, $s1, $s2
sw $s0, -24($fp)
lw $s0, -24($fp)
sw $s0, -12($fp)
label1_checkrange1 :
lw $s1, -12($fp)
la $s2, 1073741824
ble $s1, $s2, label2_checkrange1
lw $s1, -12($fp)
addiu $s0, $s1, -1073741824
sw $s0, -28($fp)
lw $s0, -28($fp)
sw $s0, -12($fp)
j label1_checkrange1
label2_checkrange1 :
label3_checkrange1 :
lw $s1, -12($fp)
la $s2, 0
bge $s1, $s2, label4_checkrange1
lw $s1, -12($fp)
addiu $s0, $s1, 1073741824
sw $s0, -32($fp)
lw $s0, -32($fp)
sw $s0, -12($fp)
j label3_checkrange1
label4_checkrange1 :
lw $s0, -12($fp)
sw $s0, -36($fp)
j label49
label49 :
lw $s0, -36($fp)
sw $s0, -44($fp)
lw $v0, -44($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label5 :
lw $s1, 8($fp)
la $s2, 1
bne $s1, $s2, label6
la $s0, 0
sw $s0, -16($fp)
la $s0, 1
sw $s0, -20($fp)
label7 :
lw $s1, -20($fp)
la $s2, 1073741824
bge $s1, $s2, label8
lw $s1, 4($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -48($fp)
lw $s1, 4($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -52($fp)
lw $s1, -52($fp)
bge $s1, $zero, noNeg1
addiu $s1, $s1, 1
noNeg1:
sra $s0, $s1, 1
sw $s0, -56($fp)
lw $s1, -56($fp)
sll $s0, $s1, 1
sw $s0, -60($fp)
lw $s1, -48($fp)
lw $s2, -60($fp)
subu $s0, $s1, $s2
sw $s0, -64($fp)
lw $s1, 0($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -68($fp)
lw $s1, 0($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -72($fp)
lw $s1, -72($fp)
bge $s1, $zero, noNeg2
addiu $s1, $s1, 1
noNeg2:
sra $s0, $s1, 1
sw $s0, -76($fp)
lw $s1, -76($fp)
sll $s0, $s1, 1
sw $s0, -80($fp)
lw $s1, -68($fp)
lw $s2, -80($fp)
subu $s0, $s1, $s2
sw $s0, -84($fp)
lw $s1, -64($fp)
lw $s2, -84($fp)
bne $s1, $s2, label9
lw $s1, -16($fp)
sll $s0, $s1, 1
sw $s0, -88($fp)
lw $s0, -88($fp)
sw $s0, -16($fp)
j label10
label9 :
lw $s1, -16($fp)
sll $s0, $s1, 1
sw $s0, -92($fp)
lw $s1, -92($fp)
addiu $s0, $s1, 1
sw $s0, -96($fp)
lw $s0, -96($fp)
sw $s0, -16($fp)
label10 :
lw $s1, -20($fp)
sll $s0, $s1, 1
sw $s0, -100($fp)
lw $s0, -100($fp)
sw $s0, -20($fp)
j label7
label8 :
lw $v0, -16($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label6 :
lw $s1, 8($fp)
la $s2, 2
bne $s1, $s2, label11
lw $s1, 4($fp)
lw $s2, 0($fp)
ble $s1, $s2, label12
lw $v0, 4($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
j label13
label12 :
lw $v0, 0($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label13 :
label11 :
lw $s1, 8($fp)
la $s2, 3
bne $s1, $s2, label14
la $s0, 0
sw $s0, -16($fp)
la $s0, 1
sw $s0, -20($fp)
label15 :
lw $s1, -20($fp)
la $s2, 1073741824
bge $s1, $s2, label16
lw $s1, 4($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -104($fp)
lw $s1, 4($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -108($fp)
lw $s1, -108($fp)
bge $s1, $zero, noNeg3
addiu $s1, $s1, 1
noNeg3:
sra $s0, $s1, 1
sw $s0, -112($fp)
lw $s1, -112($fp)
sll $s0, $s1, 1
sw $s0, -116($fp)
lw $s1, -104($fp)
lw $s2, -116($fp)
subu $s0, $s1, $s2
sw $s0, -120($fp)
lw $s1, -120($fp)
la $s2, 1
bne $s1, $s2, label17
lw $s1, -16($fp)
sll $s0, $s1, 1
sw $s0, -124($fp)
lw $s1, -124($fp)
addiu $s0, $s1, 1
sw $s0, -128($fp)
lw $s0, -128($fp)
sw $s0, -16($fp)
j label18
label17 :
lw $s1, 0($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -132($fp)
lw $s1, 0($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -136($fp)
lw $s1, -136($fp)
bge $s1, $zero, noNeg4
addiu $s1, $s1, 1
noNeg4:
sra $s0, $s1, 1
sw $s0, -140($fp)
lw $s1, -140($fp)
sll $s0, $s1, 1
sw $s0, -144($fp)
lw $s1, -132($fp)
lw $s2, -144($fp)
subu $s0, $s1, $s2
sw $s0, -148($fp)
lw $s1, -148($fp)
la $s2, 1
bne $s1, $s2, label19
lw $s1, -16($fp)
sll $s0, $s1, 1
sw $s0, -152($fp)
lw $s1, -152($fp)
addiu $s0, $s1, 1
sw $s0, -156($fp)
lw $s0, -156($fp)
sw $s0, -16($fp)
label19 :
lw $s1, -16($fp)
sll $s0, $s1, 1
sw $s0, -160($fp)
lw $s0, -160($fp)
sw $s0, -16($fp)
label18 :
lw $s1, -20($fp)
sll $s0, $s1, 1
sw $s0, -164($fp)
lw $s0, -164($fp)
sw $s0, -20($fp)
j label15
label16 :
lw $v0, -16($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label14 :
lw $s1, 8($fp)
la $s2, 4
bne $s1, $s2, label20
la $s0, 0
sw $s0, -16($fp)
la $s0, 1
sw $s0, -20($fp)
label21 :
lw $s1, -20($fp)
la $s2, 1073741824
bge $s1, $s2, label22
lw $s1, 4($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -168($fp)
lw $s1, 4($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -172($fp)
lw $s1, -172($fp)
bge $s1, $zero, noNeg5
addiu $s1, $s1, 1
noNeg5:
sra $s0, $s1, 1
sw $s0, -176($fp)
lw $s1, -176($fp)
sll $s0, $s1, 1
sw $s0, -180($fp)
lw $s1, -168($fp)
lw $s2, -180($fp)
subu $s0, $s1, $s2
sw $s0, -184($fp)
lw $s1, -184($fp)
la $s2, 1
bne $s1, $s2, label23
lw $s1, 0($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -188($fp)
lw $s1, 0($fp)
lw $s2, -20($fp)
div $s1, $s2
mflo $s0
sw $s0, -192($fp)
lw $s1, -192($fp)
bge $s1, $zero, noNeg6
addiu $s1, $s1, 1
noNeg6:
sra $s0, $s1, 1
sw $s0, -196($fp)
lw $s1, -196($fp)
sll $s0, $s1, 1
sw $s0, -200($fp)
lw $s1, -188($fp)
lw $s2, -200($fp)
subu $s0, $s1, $s2
sw $s0, -204($fp)
lw $s1, -204($fp)
la $s2, 1
bne $s1, $s2, label24
lw $s1, -16($fp)
sll $s0, $s1, 1
sw $s0, -208($fp)
lw $s1, -208($fp)
addiu $s0, $s1, 1
sw $s0, -212($fp)
lw $s0, -212($fp)
sw $s0, -16($fp)
j label25
label24 :
lw $s1, -16($fp)
sll $s0, $s1, 1
sw $s0, -216($fp)
lw $s0, -216($fp)
sw $s0, -16($fp)
label25 :
j label26
label23 :
lw $s1, -16($fp)
sll $s0, $s1, 1
sw $s0, -220($fp)
lw $s0, -220($fp)
sw $s0, -16($fp)
label26 :
lw $s1, -20($fp)
sll $s0, $s1, 1
sw $s0, -224($fp)
lw $s0, -224($fp)
sw $s0, -20($fp)
j label21
label22 :
lw $v0, -16($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label20 :
la $v0, 0
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra

getvalue :
sw $ra, -4($sp)
sw $fp, -8($sp)
move $fp, $sp
addiu $sp, $sp, -28
lw $s1, 4($fp)
la $s2, 0
bge $s1, $s2, label27
la $v0, 0
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label27 :
lw $s1, 0($fp)
la $s2, 0
bge $s1, $s2, label28
la $v0, 0
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label28 :
lw $s1, 4($fp)
lw $s2, 12($fp)
blt $s1, $s2, label29
la $v0, 0
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label29 :
lw $s1, 0($fp)
lw $s2, 8($fp)
blt $s1, $s2, label30
la $v0, 0
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
label30 :
lw $s1, 4($fp)
lw $s2, 8($fp)
mul $s0, $s1, $s2
sw $s0, -12($fp)
lw $s1, -12($fp)
lw $s2, 0($fp)
addu $s0, $s1, $s2
sw $s0, -20($fp)
lw $t0, -20($fp)
sll $s0, $t0, 2
lw $s1, a($s0)
sw $s1, -28($fp)
lw $v0, -28($fp)
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra

convn :
sw $ra, -4($sp)
sw $fp, -8($sp)
move $fp, $sp
la $s0, 0
sw $s0, -28($sp)
la $s0, 0
sw $s0, -32($sp)
la $s0, 1
sw $s0, -48($sp)
la $s0, 1
sw $s0, -52($sp)
la $s0, 1
sw $s0, -56($sp)
la $s0, 1
sw $s0, -60($sp)
addiu $sp, $sp, -164
label31 :
lw $s1, -48($fp)
la $s2, 1
bne $s1, $s2, label32
la $s0, 0
sw $s0, -32($fp)
label33 :
lw $s1, -52($fp)
la $s2, 1
bne $s1, $s2, label34
la $s0, 0
sw $s0, -44($fp)
lw $s1, 0($fp)
bge $s1, $zero, noNeg7
addiu $s1, $s1, 1
noNeg7:
sra $s0, $s1, 1
sw $s0, -64($fp)
lw $s1, -28($fp)
lw $s2, -64($fp)
subu $s0, $s1, $s2
sw $s0, -68($fp)
lw $s0, -68($fp)
sw $s0, -36($fp)
label35 :
lw $s1, -56($fp)
la $s2, 1
bne $s1, $s2, label36
lw $s1, 0($fp)
bge $s1, $zero, noNeg8
addiu $s1, $s1, 1
noNeg8:
sra $s0, $s1, 1
sw $s0, -72($fp)
lw $s1, -32($fp)
lw $s2, -72($fp)
subu $s0, $s1, $s2
sw $s0, -76($fp)
lw $s0, -76($fp)
sw $s0, -40($fp)
label37 :
lw $s1, -60($fp)
la $s2, 1
bne $s1, $s2, label38
lw $s0, 8($fp)
sw $s0, -12($fp)
lw $s0, 4($fp)
sw $s0, -16($fp)
lw $s0, -36($fp)
sw $s0, -20($fp)
lw $s0, -40($fp)
sw $s0, -24($fp)
lw $s1, -20($fp)
la $s2, 0
bge $s1, $s2, label27_getvalue2
la $s0, 0
sw $s0, -112($fp)
j label50
label27_getvalue2 :
lw $s1, -24($fp)
la $s2, 0
bge $s1, $s2, label28_getvalue2
la $s0, 0
sw $s0, -112($fp)
j label50
label28_getvalue2 :
lw $s1, -20($fp)
lw $s2, -12($fp)
blt $s1, $s2, label29_getvalue2
la $s0, 0
sw $s0, -112($fp)
j label50
label29_getvalue2 :
lw $s1, -24($fp)
lw $s2, -16($fp)
blt $s1, $s2, label30_getvalue2
la $s0, 0
sw $s0, -112($fp)
j label50
label30_getvalue2 :
lw $s1, -20($fp)
lw $s2, -16($fp)
mul $s0, $s1, $s2
sw $s0, -96($fp)
lw $s1, -96($fp)
lw $s2, -24($fp)
addu $s0, $s1, $s2
sw $s0, -104($fp)
lw $t0, -104($fp)
sll $s0, $t0, 2
lw $s1, a($s0)
sw $s1, -108($fp)
lw $s0, -108($fp)
sw $s0, -112($fp)
j label50
label50 :
lw $s0, -112($fp)
sw $s0, -116($fp)
lw $s0, 12($fp)
sw $s0, -4($sp)
lw $s0, -44($fp)
sw $s0, -8($sp)
lw $s0, -116($fp)
sw $s0, -12($sp)
addiu $sp, $sp, -12
jal reduce
addiu $sp, $sp, 12
sw $v0, -120($fp)
lw $s0, -120($fp)
sw $s0, -44($fp)
lw $s1, -40($fp)
addiu $s0, $s1, 1
sw $s0, -124($fp)
lw $s0, -124($fp)
sw $s0, -40($fp)
lw $s1, 0($fp)
bge $s1, $zero, noNeg9
addiu $s1, $s1, 1
noNeg9:
sra $s0, $s1, 1
sw $s0, -128($fp)
lw $s1, -32($fp)
lw $s2, -128($fp)
addu $s0, $s1, $s2
sw $s0, -132($fp)
lw $s1, -40($fp)
lw $s2, -132($fp)
blt $s1, $s2, label39
la $s0, 0
sw $s0, -60($fp)
label39 :
j label37
label38 :
la $s0, 1
sw $s0, -60($fp)
lw $s1, -36($fp)
addiu $s0, $s1, 1
sw $s0, -136($fp)
lw $s0, -136($fp)
sw $s0, -36($fp)
lw $s1, 0($fp)
bge $s1, $zero, noNeg10
addiu $s1, $s1, 1
noNeg10:
sra $s0, $s1, 1
sw $s0, -140($fp)
lw $s1, -28($fp)
lw $s2, -140($fp)
addu $s0, $s1, $s2
sw $s0, -144($fp)
lw $s1, -36($fp)
lw $s2, -144($fp)
blt $s1, $s2, label40
la $s0, 0
sw $s0, -56($fp)
label40 :
j label35
label36 :
la $s0, 1
sw $s0, -56($fp)
lw $s1, -28($fp)
lw $s2, 4($fp)
mul $s0, $s1, $s2
sw $s0, -148($fp)
lw $s1, -148($fp)
lw $s2, -32($fp)
addu $s0, $s1, $s2
sw $s0, -156($fp)
lw $t0, -156($fp)
sll $s0, $t0, 2
lw $s1, -44($fp)
sw $s1, b($s0)
lw $s1, -32($fp)
addiu $s0, $s1, 1
sw $s0, -160($fp)
lw $s0, -160($fp)
sw $s0, -32($fp)
lw $s1, -32($fp)
lw $s2, 4($fp)
blt $s1, $s2, label41
la $s0, 0
sw $s0, -52($fp)
label41 :
j label33
label34 :
la $s0, 1
sw $s0, -52($fp)
lw $s1, -28($fp)
addiu $s0, $s1, 1
sw $s0, -164($fp)
lw $s0, -164($fp)
sw $s0, -28($fp)
lw $s1, -28($fp)
lw $s2, 8($fp)
blt $s1, $s2, label42
la $s0, 0
sw $s0, -48($fp)
label42 :
j label31
label32 :
la $v0, 0
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra

mymemmove :
sw $ra, -4($sp)
sw $fp, -8($sp)
move $fp, $sp
la $s0, 0
sw $s0, -12($sp)
addiu $sp, $sp, -20
label43 :
lw $s1, -12($fp)
lw $s2, 0($fp)
bge $s1, $s2, label44
lw $t0, -12($fp)
sll $s0, $t0, 2
lw $s1, b($s0)
sw $s1, -16($fp)
lw $t0, -12($fp)
sll $s0, $t0, 2
lw $s1, -16($fp)
sw $s1, a($s0)
lw $s1, -12($fp)
addiu $s0, $s1, 1
sw $s0, -20($fp)
lw $s0, -20($fp)
sw $s0, -12($fp)
j label43
label44 :
lw $ra, -4($fp)
move $sp, $fp
lw $fp, -8($fp)
jr $ra
