NASM = nasm
CC = gcc
CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector -Wall -Wextra -O0 -g -std=c11

all: disk.img

bootloader.bin: bootloader.asm
	$(NASM) -f bin $< -o $@

kernel_entry.o: kernel_entry.asm
	$(NASM) -f elf32 $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: kernel_entry.o kernel.o linker.ld
	$(CC) -m32 -T linker.ld -nostdlib -ffreestanding -o kernel.elf kernel_entry.o kernel.o
	objcopy -O binary kernel.elf $@

disk.img: bootloader.bin kernel.bin
	dd if=/dev/zero of=$@ bs=1M count=64
	dd if=bootloader.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=kernel.bin of=$@ bs=512 seek=1 conv=notrunc

clean:
	rm -f *.o *.bin *.elf disk.img

run: disk.img
	qemu-system-i386 -drive file=disk.img,format=raw,index=0,media=disk -m 256

.PHONY: all clean run