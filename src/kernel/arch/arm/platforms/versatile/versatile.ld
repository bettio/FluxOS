ENTRY(_start)
SECTIONS
{
 . = 0x10000;
 .startup . : { CMakeFiles/vmflux.dir/arch/arm/platforms/versatile/startup.s.o }
 .text : { *(.text) }

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) }
   __exidx_start = .;
  .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
   __exidx_end = .;


 .data : { *(.data) }
 .bss : { *(.bss) }
 . = . + 0x1000;
 stack_top = .;
}
