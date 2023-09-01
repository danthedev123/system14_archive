echo Building debug module...
rm a.out -f
rm syscall.o -f
nasm -felf64 syscall.asm
ld syscall.o
echo OK