
OSNAME = RedX

# Build Stuff
GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kernel.ld
CC = gcc
AS = nasm
LD = ld

CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -ggdb
ASFLAGS =
LDFLAGS = -T $(LDS) -static -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi
FONTFILE := font.psf

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
ASSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.o, $(ASSRC))
DIRS = $(wildcard $(SRCDIR)/*)

# Flash stuff
BLK_DEV := NULL

# Qemu Stuff
QEMU-MEM = 256M

QEMU-FLAGS = -m $(QEMU-MEM) -cpu qemu64 -net none -no-reboot -no-shutdown

kernel: $(OBJS) link
$(OBJDIR)/interrupts/interrupts.o: $(SRCDIR)/interrupts/interrupts.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@ -lm1048488

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@ -lm

$(OBJDIR)/%.o: $(SRCDIR)/%.asm
	@ echo !==== ASSEMBLING $^
	@ mkdir -p $(@D)
	$(AS) $(ASFLAGS) $^ -f elf64 -o $@

link:
	@ echo !==== LINKING $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel $(OBJS)

setup: 
	-@mkdir $(BUILDDIR)
	-@mkdir $(SRCDIR)
	-@mkdir $(OBJDIR)

clean:
	rm -r $(BUILDDIR)
	rm -r $(OBJDIR)

buildiso:
	-@ rm $(BUILDDIR)/$(OSNAME).iso
	-@ rm $(BUILDDIR)/$(OSNAME)-fat.iso
	-@ rm $(BUILDDIR)/$(OSNAME)-mbr
	@ dd if=../bootloader/bin/mbr.bin of=$(BUILDDIR)/$(OSNAME)-mbr bs=512
	@ dd if=/dev/zero bs=512 count=93750 >> $(BUILDDIR)/$(OSNAME)-fat.iso
	@ parted $(BUILDDIR)/$(OSNAME)-fat.iso mktable msdos
	@ parted $(BUILDDIR)/$(OSNAME)-fat.iso mkpart primary fat32 1 45M
	@ mkfs.fat -F32 $(BUILDDIR)/$(OSNAME)-fat.iso
	@ mmd -i $(BUILDDIR)/$(OSNAME)-fat.iso ::/EFI
	@ mmd -i $(BUILDDIR)/$(OSNAME)-fat.iso ::/EFI/BOOT
	@ mcopy -i $(BUILDDIR)/$(OSNAME)-fat.iso $(BOOTEFI) ::/EFI/BOOT/BOOTX64.EFI
	@ mcopy -i $(BUILDDIR)/$(OSNAME)-fat.iso startup.nsh ::
	@ mcopy -i $(BUILDDIR)/$(OSNAME)-fat.iso $(BUILDDIR)/kernel ::
	@ mcopy -i $(BUILDDIR)/$(OSNAME)-fat.iso $(FONTFILE) ::
	@ dd if=$(BUILDDIR)/$(OSNAME)-mbr of=$(BUILDDIR)/$(OSNAME).iso bs=512
	@ dd if=$(BUILDDIR)/$(OSNAME)-fat.iso bs=512 >> $(BUILDDIR)/$(OSNAME).iso

partition-drive:
	sudo dd if=../bootloader/bin/mbr.bin of=$(BLK_DEV)
	sudo parted $(BLK_DEV) mklabel msdos
	sudo parted $(BLK_DEV) mkpart primary fat32 1 45M
	sudo mkfs.fat -F32 $(BLK_DEV)1
	- sudo mount $(BLK_DEV)1 /mnt

flash: partition-drive
	sudo mkdir -p /mnt/EFI/BOOT
	sudo cp $(BOOTEFI) /mnt/EFI/BOOT/BOOTX64.EFI
	sudo cp startup.nsh /mnt
	sudo cp $(BUILDDIR)/kernel /mnt
	sudo cp $(FONTFILE) /mnt/font.psf
	sudo umount $(BLK_DEV)1

#-d int -M smm=off

run-efi:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME)-fat.iso $(QEMU-FLAGS) -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd"

run-efi-debug:
	qemu-system-x86_64 -s -S -drive file=$(BUILDDIR)/$(OSNAME)-fat.iso $(QEMU-FLAGS) -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd"

run-bios:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).iso $(QEMU-FLAGS)