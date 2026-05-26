/* SacramentuOS boot entry: Multiboot v1 header + 32-bit entry point. */
.set ALIGN,    1 << 0
.set MEMINFO,  1 << 1
.set VIDEO,    1 << 2
.set FLAGS,    ALIGN | MEMINFO | VIDEO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
/* Multiboot v1 video request: linear graphics, 1024x768x32. GRUB may choose a close mode. */
.long 0          /* mode_type: 0 = graphics */
.long 1024       /* width */
.long 768        /* height */
.long 32         /* depth */

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
