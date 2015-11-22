#ifndef _MM_USERSPACEMEMORYMANAGER_H_
#define _MM_USERSPACEMEMORYMANAGER_H_

class UserspaceMemoryManager
{
    public:
        enum MemoryOperation
        {
            ExecuteOperation,
            ReadOperation,
            WriteOperation
        };

        enum PageFaultFlags
        {
            NoFlags,
            MissingPageFault
        };
};

#endif
