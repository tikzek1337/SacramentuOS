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

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude \
          -fno-stack-protector -fno-pie -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables -m32
LDFLAGS := -T linker.ld -ffreestanding -O2 -nostdlib -m32 -no-pie -Wl,--build-id=none
QEMUFLAGS := -machine pc -m 256M -serial stdio -no-reboot -no-shutdown

C_SOURCES := $(wildcard src/*.c)
ASM_SOURCES := $(wildcard src/*.S)
OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) \
           $(patsubst src/%.S,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

.PHONY: all kernel prebuilt iso iso-prebuilt run run-prebuilt run-kernel run-kernel-built run-iso check check-iso clean help

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
	@echo "  kernel            build kernel from source"
	@echo "  prebuilt          rebuild and refresh prebuilt/sacramentuos.kernel"
	@echo "  iso               build GRUB ISO from source"
	@echo "  iso-prebuilt      build GRUB ISO from prebuilt kernel"
	@echo "  run-kernel        run prebuilt kernel directly in QEMU, no ISO/GRUB"
	@echo "  run-kernel-built  build kernel and run it directly in QEMU"
	@echo "  run-iso           build ISO from prebuilt kernel and run it"
	@echo "  check             check Multiboot header"
	@echo "  check-iso         print ISO and El Torito diagnostics"
	@echo "  clean             remove build directory"
	@echo ""
	@echo "Windows/WSL fastest boot fix: make run-kernel"
	@echo "Windows/WSL full source test: make CC=gcc run-kernel-built"
