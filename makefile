
GCCP1 = -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc  -fno-stack-protector -nostdinc -Os

target/boot/bootblock.bin : boot/bootblock.S
	nasm -Iboot/ -Ilibs -o $@ $^

target/boot/bootmain.bin : boot/bootmain.S
	nasm -Iboot/ -Ilibs/  -o $@ $^


target/kern/kernel.bin : obj/kern/kernel.o
	ld -m elf_i386 $^ -Ttext 0x100000 -e main -o $@

obj/kern/kernel.o : kern/init.c
	gcc -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc  -fno-stack-protector -no-pie -fno-pic -c $^ -o $@
#target/boot/bootmain.bin : obj/boot/bootmain.o
#	gcc -Iboot/ $(GCCP1) -Ilibs/ -c $^ -o $@
#
#obj/boot/bootmain.o : boot/bootmain.c
#	gcc -Iboot/ $(GCCP1) -Ilibs/ -c $^ -o $@

install:
	dd if=target/boot/bootblock.bin of=xxos.img bs=512 count=1 conv=notrunc
	dd if=target/boot/bootmain.bin of=xxos.img bs=512 seek=2 count=1 conv=notrunc
	dd if=target/kern/kernel.bin of=xxos.img bs=512 seek=4 count=10 conv=notrunc

run:
	bochs -f bochsrc