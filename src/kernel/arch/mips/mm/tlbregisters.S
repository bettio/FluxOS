
#include <cp0registers.h>

#define REGISTERSSTORAGE_SIZE 0x4000

.text
.globl writeTLBRegisters

writeTLBRegisters:
    mtc0 $4, CP0_PageMask_Register /* 0 | Mask | 0 */
    mtc0 $5, CP0_EntryHi_Register /* VPN2 | 0 | ASID */
    mtc0 $6, CP0_EntryLo0_Register /* R | 0 | PFN | C | D | V | G */
    mtc0 $7, CP0_EntryLo1_Register /* R | 0 | PFN | C | D | V | G */
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
    nop
