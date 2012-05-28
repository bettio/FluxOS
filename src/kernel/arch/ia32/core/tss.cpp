#include <arch/ia32/core/tss.h>

struct TSSStruct
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomapBase;
} __attribute__((packed));

TSSStruct tss;

void TSS::init(volatile uint64_t *gdtEntry)
{
    tss.ss0 = 0x10;
    tss.iomapBase = sizeof(tss);

    uint64_t tssAddr = (uint64_t) &tss;
    *gdtEntry = (tssAddr & 0x00FFFFFF) << 16 | (tssAddr & 0xFF000000) << 32 | 0x0040890000000000LL | (uint64_t) (sizeof(tss) - 1);
}

void TSS::ltr(int index)
{
    uint16_t tdesc = index*8;
    asm volatile("ltr %0" : : "r" (tdesc));
}

void TSS::setKernelStack(void *kstack)
{
    tss.esp0 = (uint32_t) kstack;
}

void *TSS::kernelStack()
{
    return (void *) tss.esp0;
}
