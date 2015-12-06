#include <stdint.h>

extern "C"
{
    void updateTLB(unsigned long *pageTable, int pageDirectoryIndex, int pageTableIndex, uint8_t asid);
    uint8_t switchASID(uint8_t asid);
}
