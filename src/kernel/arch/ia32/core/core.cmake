set(KERNEL_SRCS ${KERNEL_SRCS}
    arch/ia32/core/archmanager.cpp
    arch/ia32/core/contextswitcher.cpp
    arch/ia32/core/archthreadsmanager.cpp
    arch/ia32/core/gdt.cpp
    arch/ia32/core/idt.cpp
    arch/ia32/core/inthandler.cpp
    arch/ia32/core/irq.cpp
    arch/ia32/core/pci.cpp
    arch/ia32/core/tss.cpp
    arch/ia32/core/syscallsmanager.cpp
    arch/ia32/core/userprocsmanager.cpp
)
