# SacramentuOS Makefile
# Recommended compiler for real OSDev work: i686-elf-gcc.
# For the учебный запуск в WSL/Ubuntu можно использовать: make CC=gcc ...

OS_NAME := SacramentuOS
BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/isodir
KERNEL := $(BUILD_DIR)/sacramentuos.kernel
ISO := $(BUILD_DIR)/SacramentuOS.iso
PREBUILT_KERNEL := prebuilt/sacramentuos.kernel

CC ?= i686-elf-gcc
GRUB_MKRESCUE ?= grub-mkrescue
QEMU ?= qemu-system-i386
QEMU_UEFI ?= qemu-system-x86_64
UEFI_CC ?= clang
UEFI_IMG := $(BUILD_DIR)/SacramentuOS_UEFI.img
UEFI_TREE := flash
UEFI_APP := $(BUILD_DIR)/BOOTX64.EFI
PREBUILT_UEFI := prebuilt/BOOTX64.EFI
FLASH_SYSTEM := $(UEFI_TREE)/SacramentuOS/SYSTEM
FLASH_LEGACY := $(UEFI_TREE)/SacramentuOS/LEGACY_BIOS_KERNEL

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude \
          -fno-stack-protector -fno-pie -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables -m32
LDFLAGS := -T linker.ld -ffreestanding -O2 -nostdlib -m32 -no-pie -Wl,--build-id=none
QEMUFLAGS := -machine pc -m 256M -serial stdio -no-reboot -no-shutdown
UEFI_CFLAGS := -target x86_64-pc-win32 -fuse-ld=lld -ffreestanding -fshort-wchar \
               -fno-stack-protector -fno-builtin -mno-red-zone -Wall -Wextra -nostdlib \
               -Wl,/entry:efi_main -Wl,/subsystem:efi_application -Wl,/nodefaultlib
UEFI_QEMUFLAGS := -m 256M -serial stdio -no-reboot -no-shutdown

C_SOURCES := $(wildcard src/*.c)
ASM_SOURCES := $(wildcard src/*.S)
OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) \
           $(patsubst src/%.S,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

.PHONY: all kernel prebuilt iso iso-prebuilt run run-prebuilt run-kernel run-kernel-built run-iso uefi uefi-prebuilt uefi-usb uefi-flash uefi-img run-uefi run-uefi-prebuilt check-uefi check check-iso clean help

all: iso

kernel: $(KERNEL)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.S | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)
	@if command -v grub-file >/dev/null 2>&1; then \
		grub-file --is-x86-multiboot $@ || (echo "Kernel is not Multiboot-compatible" && exit 1); \
	else \
		echo "grub-file not found; skipping Multiboot check"; \
	fi

prebuilt: $(KERNEL)
	mkdir -p prebuilt
	cp $(KERNEL) $(PREBUILT_KERNEL)
	@echo "Updated $(PREBUILT_KERNEL)"

iso: $(KERNEL)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_DIR)/boot/sacramentuos.kernel
	cp boot/grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO) $(ISO_DIR)
	@echo "Built $(ISO)"
	@$(MAKE) --no-print-directory check-iso

iso-prebuilt:
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(PREBUILT_KERNEL) $(ISO_DIR)/boot/sacramentuos.kernel
	cp boot/grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO) $(ISO_DIR)
	@echo "Built $(ISO) from $(PREBUILT_KERNEL)"
	@$(MAKE) --no-print-directory check-iso

# Primary Windows 11 / WSL fallback: boots the Multiboot kernel directly, without ISO/GRUB.
# Use this when QEMU freezes at "Booting from DVD/CD...".
run-kernel:
	$(QEMU) $(QEMUFLAGS) -kernel $(PREBUILT_KERNEL)

run-kernel-built: $(KERNEL)
	$(QEMU) $(QEMUFLAGS) -kernel $(KERNEL)

# ISO boot. -boot d forces BIOS to try the CD/DVD first.
run-iso: iso-prebuilt
	$(QEMU) $(QEMUFLAGS) -boot d -cdrom $(ISO)

run: iso
	$(QEMU) $(QEMUFLAGS) -boot d -cdrom $(ISO)

run-prebuilt: iso-prebuilt
	$(QEMU) $(QEMUFLAGS) -boot d -cdrom $(ISO)


# Native pure UEFI build. This does not use BIOS, CSM, GRUB, VGA 0xB8000 or PS/2.
# It builds BOOTX64.EFI, a firmware-console edition of SacramentuOS.
uefi: $(UEFI_APP)

$(UEFI_APP): src/uefi/uefi_main.c | $(BUILD_DIR)
	$(UEFI_CC) $(UEFI_CFLAGS) -o $@ $<
	@file $@ || true

uefi-prebuilt: $(UEFI_APP)
	mkdir -p prebuilt
	cp $(UEFI_APP) $(PREBUILT_UEFI)
	@echo "Updated $(PREBUILT_UEFI)"

uefi-usb: uefi-flash

uefi-flash:
	mkdir -p $(UEFI_TREE)/EFI/BOOT $(FLASH_SYSTEM) $(FLASH_LEGACY)
	cp $(PREBUILT_UEFI) $(UEFI_TREE)/EFI/BOOT/BOOTX64.EFI
	@if [ -f "$(PREBUILT_KERNEL)" ]; then cp $(PREBUILT_KERNEL) $(FLASH_LEGACY)/sacramentuos.kernel; fi
	@printf 'SacramentuOS 0.3.5\r\nCopy the CONTENTS of this flash directory to a FAT32 USB drive.\r\nMain system: EFI/BOOT/BOOTX64.EFI\r\n' > $(UEFI_TREE)/README_FIRST_RU.txt
	@printf '0.3.5\r\n' > $(FLASH_SYSTEM)/VERSION.txt
	@printf 'help docs commands fetch mode edit files show cat rm color whoami time date clear clean reboot shutdown exit\r\n' > $(FLASH_SYSTEM)/TEST_COMMANDS.txt
	@printf 'BOOTX64.EFI contains the native SacramentuOS shell and RAM editor. No ISO is required for UEFI boot.\r\n' > $(FLASH_SYSTEM)/WHAT_IS_SYSTEM.txt
	@echo "Prepared flash tree: $(UEFI_TREE)"
	@find $(UEFI_TREE) -maxdepth 5 -type f | sort

uefi-img: uefi-usb
	@if ! command -v mkfs.vfat >/dev/null 2>&1 || ! command -v mcopy >/dev/null 2>&1; then \
		echo "Install FAT tools first: sudo apt install -y dosfstools mtools"; exit 1; \
	fi
	rm -f $(UEFI_IMG)
	dd if=/dev/zero of=$(UEFI_IMG) bs=1M count=64 status=none
	mkfs.vfat -F 32 $(UEFI_IMG) >/dev/null
	mmd -i $(UEFI_IMG) ::/EFI ::/EFI/BOOT ::/SacramentuOS ::/SacramentuOS/SYSTEM
	mcopy -i $(UEFI_IMG) $(PREBUILT_UEFI) ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i $(UEFI_IMG) $(UEFI_TREE)/README_FIRST_RU.txt ::/README_FIRST_RU.txt
	mcopy -i $(UEFI_IMG) $(FLASH_SYSTEM)/VERSION.txt ::/SacramentuOS/SYSTEM/VERSION.txt
	@echo "Built UEFI FAT image: $(UEFI_IMG)"

run-uefi-prebuilt: uefi-usb
	@if [ -f /usr/share/OVMF/OVMF_CODE.fd ]; then OVMF=/usr/share/OVMF/OVMF_CODE.fd; \
	elif [ -f /usr/share/ovmf/OVMF.fd ]; then OVMF=/usr/share/ovmf/OVMF.fd; \
	else echo "OVMF not found. Install it: sudo apt install -y ovmf"; exit 1; fi; \
	$(QEMU_UEFI) $(UEFI_QEMUFLAGS) -bios $$OVMF -drive format=raw,file=fat:rw:$(UEFI_TREE)

run-uefi: uefi-prebuilt run-uefi-prebuilt

check-uefi:
	@if [ ! -f "$(PREBUILT_UEFI)" ]; then echo "Missing $(PREBUILT_UEFI)"; exit 1; fi
	@file $(PREBUILT_UEFI)

check: $(KERNEL)
	@if command -v grub-file >/dev/null 2>&1; then \
		grub-file --is-x86-multiboot $(KERNEL) && echo "OK: Multiboot kernel"; \
	else \
		echo "grub-file not found; install GRUB tools to run this check"; \
	fi

check-iso:
	@if [ ! -f "$(ISO)" ]; then echo "ISO not found: $(ISO)"; exit 1; fi
	@echo "ISO size:"; ls -lh $(ISO)
	@if command -v xorriso >/dev/null 2>&1; then \
		xorriso -indev $(ISO) -report_el_torito plain 2>/dev/null | sed -n '1,80p'; \
	else \
		echo "xorriso not found; skipping El Torito boot catalog check"; \
	fi

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "Targets:"
	@echo "  kernel             build legacy BIOS/Multiboot kernel from source"
	@echo "  prebuilt           refresh prebuilt/sacramentuos.kernel"
	@echo "  iso                build legacy GRUB ISO from source"
	@echo "  run-kernel         run legacy prebuilt kernel directly in QEMU"
	@echo "  run-kernel-built   build legacy kernel and run it directly in QEMU"
	@echo "  uefi              build native BOOTX64.EFI from source"
	@echo "  uefi-prebuilt     refresh prebuilt/BOOTX64.EFI"
	@echo "  uefi-flash        prepare ./flash for FAT32 USB copying"
	@echo "  uefi-img          build 64MB FAT32 UEFI disk image"
	@echo "  run-uefi          build and run native UEFI app in QEMU/OVMF"
	@echo "  run-uefi-prebuilt run prebuilt BOOTX64.EFI in QEMU/OVMF"
	@echo "  check-uefi        inspect prebuilt/BOOTX64.EFI"
	@echo "  clean             remove build directory"
	@echo ""
	@echo "Real PC UEFI flash: make uefi-prebuilt uefi-flash, then copy CONTENTS of ./flash to FAT32 USB."
