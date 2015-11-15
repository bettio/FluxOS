set(KERNEL_SRCS ${KERNEL_SRCS}
    task/eventsmanager.cpp
    task/task.cpp
    task/threadsmanager.cpp
)

if (NOT UMM)
    set(KERNEL_SRCS ${KERNEL_SRCS}
        task/scheduler.cpp
        task/userprocessimage.cpp
    )
endif (NOT UMM)

if (NOT UMM)
if (NOT NATIVE_IA32)
    set(KERNEL_SRCS ${KERNEL_SRCS}
        task/userprocessimage.cpp
    )
endif (NOT NATIVE_IA32)
endif (NOT UMM)
