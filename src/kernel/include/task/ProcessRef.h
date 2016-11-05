#ifndef _PROCESSREF_H_
#define _PROCESSREF_H_

class ProcessControlBlock;

class ProcessRef
{
    public:
        inline ProcessRef(ProcessControlBlock *process);
        inline ProcessRef(const ProcessRef &procRef);
        inline ~ProcessRef();
        inline ProcessControlBlock *operator->();

        inline bool isValid() const;

    private:
        ProcessControlBlock *m_process;
};


#endif
