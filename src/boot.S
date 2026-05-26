/* SacramentuOS boot entry: Multiboot v1 header + 32-bit entry point. */
.set ALIGN,    1 << 0
.set MEMINFO,  1 << 1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack, "aw", @nobits
.align 16
stack_bottom:
.skip 32768
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    cli
    mov $stack_top, %esp

    /* C ABI: kernel_main(magic, multiboot_info). */
    push %ebx
    push %eax
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
.size _start, . - _start
