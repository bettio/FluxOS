#include <core/systemtimer.h>

#include <mm/usermemoryops.h>
#include <task/scheduler.h>

long SystemTimer::time(long *t)
{
    long tSeconds = startupTimestamp + (systemTicks / tickFrequency); 
    if (t){
        int ret = putToUser(tSeconds, t);
        if (UNLIKELY(ret < 0)) {
            return ret;
        }
    }
    return tSeconds;
}

int SystemTimer::stime(long *t)
{
    if (UNLIKELY(Scheduler::currentThread()->parentProcess->uid != 0)) {
        return -EPERM;
    }

    long time;
    int ret = getFromUser(&time, t);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }
    startupTimestamp = time - systemTicks / tickFrequency;

    return 0;
}
