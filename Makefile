NASM = nasm
CC = gcc

CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector \
         -Wall -Wextra -O0 -g -std=c11

all: os-image.bin

bootloader.bin: bootloader.asm
	$(NASM) -f bin $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel_entry.o: kernel_entry.asm
	$(NASM) -f elf32 $< -o $@

kernel.bin: kernel_entry.o kernel.o linker.ld
	$(CC) -m32 -T linker.ld -nostdlib -ffreestanding -o kernel.elf kernel_entry.o kernel.o
	objcopy -O binary kernel.elf $@

os-image.bin: bootloader.bin kernel.bin
	dd if=/dev/zero of=$@ bs=1024 count=1440
	dd if=bootloader.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=kernel.bin of=$@ bs=512 seek=1 conv=notrunc

clean:
	rm -f *.o *.bin *.elf os-image.bin

run: os-image.bin
	qemu-system-i386 -drive format=raw,file=os-image.bin -boot order=a

.PHONY: all clean run