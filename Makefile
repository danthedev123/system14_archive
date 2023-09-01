rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d)) # recursive wildcard

cc := x86_64-unknown-elf-gcc # gcc cross-compiler
ld := x86_64-unknown-elf-ld
kbin := kernel.elf

CFLAGS += \
    -Wall \
    -Wextra \
    -Werror \
    -std=gnu11 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fno-PIE \
    -fno-PIC \
    -m64 \
    -march=x86-64 \
    -mabi=sysv \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel

LDFLAGS += \
    -nostdlib \
    -static \
    -m elf_x86_64 \
    -z max-page-size=0x1000 \
    -T config/kernel.ld

ASMFLAGS += \
    -Wall \
    -f elf64

kcsources = $(call rwildcard,src,*.c)
kcsourcesnasm = $(call rwildcard,src,*.asm)
kobj = $(kcsources:.c=.o) $(kcsourcesnasm:.asm=.o)

all: $(kbin)

$(kbin): $(kobj)
	@echo LD $(kbin)
	@$(ld) $(kobj) $(LDFLAGS) -o $@

src/%.o: src/%.c
	@echo CC $<
	@$(cc) $(CFLAGS) -c $< -o $@ -g

src/%.o: src/%.asm
	@echo AS $<
	@nasm $(ASMFLAGS) $< -o $@

iso: all limine src/flanterm
	rm -rf iso_root
	mkdir -p iso_root
	
	cp $(kbin) config/limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin ramdisk.tar iso_root/
	
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o root.iso
	
	limine/limine-deploy root.iso
	rm -rf iso_root

run: iso
	qemu-system-x86_64 -d int -D QEMU.log -m 500M -no-shutdown -no-reboot -cdrom root.iso -machine q35 -cpu max -serial stdio

rungdb: iso
	@echo -----------------------------------------------------------------------------
	@echo   Hello! Launch GDB session from VS code, it will automagically connect to QEMU.
	@echo -----------------------------------------------------------------------------
	qemu-system-x86_64 -d int -m 500M -no-shutdown -no-reboot -cdrom root.iso -machine q35 -cpu max -s -S -serial stdio

src/flanterm:
	git clone https://github.com/mintsuki/flanterm.git src/flanterm

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

clean:
	-rm $(kobj)

# eclean (Elf Clean)
eclean: clean
	rm $(kbin)

# fclean (Full clean)
fclean: eclean
	rm -f root.iso
	rm -f qemu.log

fixclockskew:
	find . -type f | xargs -n 5 touch