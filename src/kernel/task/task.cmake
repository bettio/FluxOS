set(KERNEL_SRCS ${KERNEL_SRCS}
    task/eventsmanager.cpp
    task/task.cpp
    task/threadsmanager.cpp
)

if (NOT UMM)
    set(KERNEL_SRCS ${KERNEL_SRCS}
        task/scheduler.cpp
    )
endif (NOT UMM)

