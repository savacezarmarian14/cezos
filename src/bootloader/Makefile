all: clean build

boot.o: boot.asm
	nasm -f elf32 boot.asm -o boot.o
	
main.o: main.c
	gcc -m32 -fno-pie -I inc -c -g main.c -o main.o

build: boot.o main.o
	ld -m elf_i386 -T linker.ld -o kernel.bin main.o boot.o

clean:
	rm -rf boot.o main.o kernel.bin


