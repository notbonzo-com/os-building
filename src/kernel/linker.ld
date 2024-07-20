ENTRY(_start)
OUTPUT_FORMAT("elf32-i386")
phys = 0x00100000;

PHDRS
{
    text PT_LOAD FLAGS(5);    /* R and X */
    data PT_LOAD FLAGS(6);    /* R and W */
    rodata PT_LOAD FLAGS(4);  /* R */
}

SECTIONS
{
    . = phys;

    __kernel = .;

    .entry :
    {
        __entry_start = .;
        *(.entry)
        __entry_end = .;
    } :text

    .text :
    {
        __text_start = .;
        *(.text)
        __text_end = .;
    } :text

    .rodata :
    {
        __rodata_start = .;
        *(.rodata)
        __rodata_end = .;
    } :rodata

    .data :
    {
        __data_start = .;
        *(.data)
        __data_end = .;
    } :data

    .bss :
    {
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    } :data

    __end = .;
}
