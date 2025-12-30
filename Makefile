CC = gcc
LD = ld
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra

all: kernel.bin iso

kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel.o

mouse.o: kernel/mouse.c
	$(CC) $(CFLAGS) -c kernel/mouse.c -o mouse.o

keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o keyboard.o

boot.o: kernel/boot.s
	$(CC) $(CFLAGS) -c kernel/boot.s -o boot.o




kernel.bin: boot.o kernel.o mouse.o keyboard.o
	$(LD) -m elf_i386 -T kernel/linker.ld -o kernel.bin boot.o kernel.o mouse.o keyboard.o


iso:
	cp kernel.bin iso/boot/kernel.bin
	cp iso/boot/grub/grub.cfg iso/boot/grub/grub.cfg

clean:
	rm -f *.o kernel.bin

buildiso:
	cp kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o chios.iso iso

run:
	qemu-system-i386 -cdrom chios.iso
