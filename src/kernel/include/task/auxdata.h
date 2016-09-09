#ifndef _AUXDATA_H_
#define _AUXDATA_H_

struct AuxData
{
    char *executableFileName;
    unsigned long interpreterBaseAddress;
    unsigned long entryPointAddress;
    unsigned long programHeaderAddress;
    unsigned int programHeaderEntrySize;
    unsigned int programHeaderEntriesCount;
};

#endif
