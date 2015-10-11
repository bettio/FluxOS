#include <core/printk.h>

extern "C" void addressLoadExceptionISR(unsigned long address, unsigned long epc)
{
    printk("addressLoadException: 0x%x (EPC: 0x%x)\n", address, epc);
    while(1);
}

