.text
.globl __start

__start:
    /* $a0 <- argc */
    lw $a0, 0($sp)

    /* $a1 <- argv */
    addi $a1, $sp, 4

    /* $a2 <- envp */
    sll $a2, $a0, 2
    add $a2, $a2, $a1
    add $a2, $a2, 4

    add $sp, $sp, -12
    jal main

    jal _exit
