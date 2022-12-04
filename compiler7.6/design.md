#### 所有使用到的`mips`指令
```mips
sw $t1, imm($t2)
sw $t1, labelSet($t2)
sw $t1, labelSet + number
lw $t1,imm($t2)
lw $t1, labelSet($t2)
lw $t1,labelSet + number
li $t1, imm
addi $t1, $t2, imm
add $t1, $t2, $t3
subi $t1, $t2, imm
sub $t1, $t2, $t3
mul $t1, $t2, $t3
div $t1, $t2, $t3
moveIN $t1,$t2
la $t1, labelSet
sll $t1, $t2, imm
bgez
bgtz
blez
bltz
beqz
bnez
bge
bgt
blt
ble
beq
bne
jal labelSet
j labelSet
jr $t1
syscall
```

所有变量可能被重新定义的语句：
read
funccall
operation
assign


