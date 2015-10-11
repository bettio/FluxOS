#define NO_C_CODE 1

#include <cp0registers.h>
#include <arch/mips/core/cpuregistersframe.h>

#define INITIALEXCEPTIONSTACK_SIZE 0x2000

#define CAUSE_REGISTER_MASK 0x7C
#define TLB_LOAD_EXCEPTION 8
#define TLB_STORE_EXCEPTION 12

.text
.globl generalExceptionsHandler
.globl kernelStackPointer
.globl interruptReturn
.globl generalExceptionHandling

generalExceptionsHandler:
    /* check for tlb refill exception */
    mfc0 $k0, CP0_Cause_Register
    andi $k0, $k0, CAUSE_REGISTER_MASK
    li $k1, TLB_LOAD_EXCEPTION
    beq $k0, $k1, tlbRefillException
    li $k1, TLB_STORE_EXCEPTION
    beq $k0, $k1, tlbRefillException

generalExceptionHandling:
    /* general exception handling */
    /* load stack pointer and registers frame */
    la	$k1, kernelStackPointer
    lw $k0, 0($k1)
    addi $k0, $k0, -CPU_REGISTERS_FRAME_SIZE
    sw $k0, 0($k1)

   .set noat
    sw $1, 0($k0)
   .set at
    sw $2, 4($k0)
    sw $3, 8($k0)
    sw $4, 16($k0)
    sw $5, 20($k0)
    sw $6, 24($k0)
    sw $7, 28($k0)
    sw $8, 32($k0)
    sw $9, 36($k0)
    sw $10, 40($k0)
    sw $11, 44($k0)
    sw $12, 48($k0)
    sw $13, 52($k0)
    sw $14, 56($k0)
    sw $15, 60($k0)
    sw $16, 64($k0)
    sw $16, 68($k0)
    sw $17, 72($k0)
    sw $18, 76($k0)
    sw $19, 80($k0)
    sw $20, 84($k0)
    sw $21, 88($k0)
    sw $22, 92($k0)
    sw $23, 96($k0)
    sw $24, 100($k0)
    sw $25, 104($k0)
    sw $28, 108($k0)
    sw $29, 112($k0)
    sw $30, 116($k0)
    sw $31, 120($k0)
    mfc0 $k1, CP0_EPC_Register
    sw $k1, 124($k0)

    addi $sp, $k0, -4

    mfc0 $k0, CP0_Cause_Register
    andi $k0, $k0, 0x7C
    sll $k0, $k0, 1
    la $k1, exceptionsTable
    add $k1, $k1, $k0
    jr $k1

    .set noreorder
  exceptionsTable:
    j unknownException
      nop
    j callTLBModificationExceptionISR
      nop
    j callTLBLoadExceptionISR
      nop
    j callTLBStoreExceptionISR
      nop
    j callAddressLoadExceptionISR
      nop
    j unknownException
      nop
    j unknownException
      nop
    j unknownException
      nop
    j callSyscallISR
      nop
    j unknownException
      nop
    j unknownException
      nop
    .set reorder

  callTLBModificationExceptionISR:
    mfc0 $4, CP0_BadVAddr_Register
    mfc0 $5, CP0_EPC_Register
    mtc0 $6, CP0_EntryHi_Register
    addi $sp, $sp, -12
    jal tlbModificationExceptionISR
    j exitGeneralExceptionHandler

  callTLBLoadExceptionISR:
    mfc0 $4, CP0_BadVAddr_Register
    mfc0 $5, CP0_EPC_Register
    mtc0 $6, CP0_EntryHi_Register
    addi $sp, $sp, -12
    jal tlbLoadExceptionISR
    j exitGeneralExceptionHandler

  callTLBStoreExceptionISR:
    mfc0 $4, CP0_BadVAddr_Register
    mfc0 $5, CP0_EPC_Register
    addi $sp, $sp, -8
    jal tlbStoreExceptionISR
    j exitGeneralExceptionHandler

  callAddressLoadExceptionISR:
    mfc0 $4, CP0_BadVAddr_Register
    mfc0 $5, CP0_EPC_Register
    addi $sp, $sp, -8
    jal addressLoadExceptionISR
    j exitGeneralExceptionHandler

  callSyscallISR:
    addi $7, $a2, 0
    addi $6, $a1, 0
    addi $5, $a0, 0
    addi $4, $v0, 0
    addi $sp, $sp, -16
    jal syscallISR
    la	$k0, kernelStackPointer
    /* save syscall return value */
    lw $k0, 0($k0)
    sw $v0, 4($k0)
    /* saved EPC will point to the instruction EPC + 4 */
    lw $k1, 124($k0)
    addi $k1, $k1, 4
    sw $k1, 124($k0)
    j exitGeneralExceptionHandler

  unknownException:
    srl $k0, $k0, 3
    addi $4, $k0, 0
    jal panicISR

  exitGeneralExceptionHandler:
    la	$k1, kernelStackPointer
    lw $k0, 0($k1)
    addi $k0, $k0, CPU_REGISTERS_FRAME_SIZE
    sw $k0, 0($k1)
    addi $k0, $k0, -CPU_REGISTERS_FRAME_SIZE

   .set noat
    lw $1, 0($k0)
   .set at
    lw $2, 4($k0)
    lw $3, 8($k0)
    lw $4, 16($k0)
    lw $5, 20($k0)
    lw $6, 24($k0)
    lw $7, 28($k0)
    lw $8, 32($k0)
    lw $9, 36($k0)
    lw $10, 40($k0)
    lw $11, 44($k0)
    lw $12, 48($k0)
    lw $13, 52($k0)
    lw $14, 56($k0)
    lw $15, 60($k0)
    lw $16, 64($k0)
    lw $16, 68($k0)
    lw $17, 72($k0)
    lw $18, 76($k0)
    lw $19, 80($k0)
    lw $20, 84($k0)
    lw $21, 88($k0)
    lw $22, 92($k0)
    lw $23, 96($k0)
    lw $24, 100($k0)
    lw $25, 104($k0)
    lw $28, 108($k0)
    lw $29, 112($k0)
    lw $30, 116($k0)
    lw $31, 120($k0)
    lw $k1, 124($k0)
    mtc0 $k1, CP0_EPC_Register

  interruptReturn:
    mtc0 $0, CP0_Cause_Register
    mfc0 $k0, CP0_Status_Register
    andi $k0, 0xFFFD
    ori  $k0, STATUS_REGISTER_EXL_SET
    mtc0 $k0, CP0_Status_Register

    eret
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

.data
kernelStackPointer:
.long initialKernelStack
.align 16
.comm   initialKernelStack, INITIALEXCEPTIONSTACK_SIZE
