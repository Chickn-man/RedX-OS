
PROGNAME = RedX-bootloader

LDS = rxb.ld
CC = gcc
AS = nasm
LD = ld

CFLAGS = -ffreestanding
ASFLAGS =
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

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

bootloader: MBR $(OBJS) link

MBR: 
	@ echo !==== ASSEMBLING mbr.asm
	@ mkdir -p $(@D)
	$(AS) -o $(BUILDDIR)/mbr.bin mbr.asm

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
	-@ mkdir $(BUILDDIR)
	-@ mkdir $(SRCDIR)
	-@ mkdir $(OBJDIR)

clean:
	rm -r $(BUILDDIR)
	rm -r $(OBJDIR)