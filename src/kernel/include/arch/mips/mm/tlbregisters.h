#include <stdint.h>

#define TLB_MISSING_PAGE_MASK 0x80000000

extern "C"
{
    void updateTLB(unsigned long *pageTable, int pageDirectoryIndex, int pageTableIndex, uint8_t asid);
    uint8_t switchASID(uint8_t asid);
    int probeTLB(const void *addr);
}
