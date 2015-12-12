#include <core/printk.h>
#include <core/systemerrors.h>

extern "C" void addressLoadExceptionISR(unsigned long address, unsigned long epc)
{
    printk("addressLoadException: 0x%x (EPC: 0x%x)\n", address, epc);
    while(1);
}

extern "C" void addressStoreExceptionISR(unsigned long address, unsigned long epc)
{
    printk("address store exception at address 0x%x (EPC: 0x%x)\n", address, epc);
    kernelPanic("");
}
