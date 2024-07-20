ENTRY(start)
OUTPUT_FORMAT("binary")
phys = 0x7C00;                  /* BIOS (or MBR) puts us at 0x7C00 */

SECTIONS
{
    . = phys;

    .fsjump             : { *(.fsjump)           }

    .fsheaders          : { *(.fsheaders)       }

    .entry              : { __entry_start = .;
                            *(.entry)   
                          }

    .text               : { *(.text)            }

    .data               : { *(.data)            }

    .rodata             : { *(.rodata)          }

    .bios_footer 0x7DFE : { SHORT(0xAA55)       }

    /* Uninitialized memory (declared with resb/resw/resq). We can use memory
       beyond this binary for that. 
    */
    .bss                : { *(.bss)             }
}