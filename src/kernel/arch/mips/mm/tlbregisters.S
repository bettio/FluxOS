
#include <cp0registers.h>

.text

.globl probeTLB
probeTLB:
    mtc0 $a0, CP0_EntryHi_Register
    tlbp
    nop
    nop
    mfc0 $v0, CP0_Index_Register
    jr $ra

/*
  $a0 -> new ASID
  $v0 -> return old ASID
*/
.globl switchASID
switchASID:
    mfc0 $t0, CP0_EntryHi_Register
    ori $t1, $zero, 0xFF
    andi $v0, $t0, 0xFF
    not $t1, $t1
    and $t0, $t0, $t1
    or $t0, $t0, $a0
    mtc0 $t0, CP0_EntryHi_Register
    jr $ra

/*
  $a0 -> page table ptr
  $a1 -> page dir index
  $a2 -> page table index
  $a3 -> asid
*/
.globl updateTLB
updateTLB:
  /*
   registers usage:
   $t0 -> entry address
   $t1 -> page table offset
   $t2 -> loaded entry
   $t3 -> virtual address
   $t4 -> found index (CP0_Index_Register copy)
   $t5 -> temporary
   $t6 -> temporary
  */

  probeTLBForVAddr:
    sll $t5, $a1, 22
    sll $t6, $a2, 12
    or $t3, $t5, $t6
    or $t3, $t3, $a3
    mtc0 $t3, CP0_EntryHi_Register
    tlbp
    nop
    nop
    mfc0 $t4, CP0_Index_Register

  calculatePageTableOffset:
    /* offset = (table_index / 2) * 8 */
    srl $t1, $a2, 1
    sll $t1, $t1, 3

  loadEntryLo0:
    add  $t0, $a0, $t1
    lw $t2, 0($t0)
    beq $zero, $t2, loadInvalidEntryLo0
    srl $t2, $t2, 6
    ori $t2, $t2, 0x2 /* Valid */
    ori $t2, $t2, 0x4 /* We should not force dirty here */
    mtc0 $t2, CP0_EntryLo0_Register
    b loadEntryLo1

  loadInvalidEntryLo0:
    mtc0 $zero, CP0_EntryLo0_Register

  loadEntryLo1:
    lw $t2, 4($t0)
    beq $zero, $t2, loadInvalidEntryLo1
    srl $t2, $t2, 6
    ori $t2, $t2, 0x2 /* Valid */
    ori $t2, $t2, 0x4 /* We should not force dirty here */
    mtc0 $t2, CP0_EntryLo1_Register
    b writeTLB

  loadInvalidEntryLo1:
    mtc0 $zero, CP0_EntryLo1_Register

  writeTLB:
    srl $t4, $t4, 31
    bne $zero, $t4, writeRandomTLB

  writeIndexTLB:
    tlbwi
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    jr $ra

  writeRandomTLB:
    tlbwr
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    jr $ra
