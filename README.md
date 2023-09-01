# System/14
# ARCHIVE ARCHIVE ARCHIVE ARCHIVE ARCHIVE ARHIVE ARCHIVE
# A rewrite (System/28) is underway.

## What?
- A 'kernel' written in pure C.
- The aim is for it to one day become a functional workstation system.
- I believe in the simplicity of C in its pure form, which is why system14 is written in C and Assembly exclusively.

## Goals
- Compatibility with most real hardware.
- Being able to emulate a unix-like environment.
- Being advanced enough to the point where System/14 can self-host.

## Cross compilation
- It is recommended to use a cross compiler to compile System/14. I recommend using [crosstool-ng](https://crosstool-ng.github.io/) with a **bare metal** target with **no** libc.

## Progress
- [ ] ⚠️ Priority: Fix broken bootloader handling code
- [x] Serial terminal
- [x] Page frame (4KiB) memory allocation
- [x] Global Descriptor Table
- [x] Interrupt Descriptor Table
- [x] Keyboard handler
- [x] Paging
- [x] Kernel heap manager
- [x] ACPI
- [x] PCI-e
- [x] APIC
- [ ] HPET
- [ ] USB (for modern keyboards)
- [ ] Hard disk read/write
- [ ] FAT16
- [x] Ramdisk for fonts, etc
- [x] Proper framebuffer terminal (flanterm)
- [ ] FAT32
- [ ] Kernel system calls
- [x] Kernel scheduler
- [x] Process exception handling
- [ ] Kernel modules
- [ ] Userspace
- [ ] POSIX-compliance
- [ ] Port bash to system14
- [ ] Port useful workstation applications

## Libraries used
- Flanterm (https://github.com/mintsuki/flanterm) by mintsuki, used as terminal emulator.# system14_archive
# system14_archive
