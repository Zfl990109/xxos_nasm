
GCCP1 = -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc -fno-stack-protector -no-pie -fno-pic

obj = obj/kern/init/init.o obj/kern/trap/interrupt.o obj/kern/debug/assert.o \
      	obj/kern/mm/memory.o obj/kern/driver/timer.o obj/kern/process/thread.o obj/kern/process/switch.o \
      	obj/kern/trap/vector.o obj/libs/bitmap.o obj/libs/string.o  obj/libs/print.o obj/libs/list.o

target = target/boot/bootblock.bin target/boot/bootmain.bin target/kern/kernel.bin

all: $(target)
	@echo "compile done"
	make install
	make run

target/boot/bootblock.bin : boot/bootblock.S
	nasm -Iboot/ -Ilibs -o $@ $^

target/boot/bootmain.bin : boot/bootmain.S
	nasm -Iboot/ -Ilibs/  -o $@ $^


target/kern/kernel.bin : $(obj)
	ld -m elf_i386 $^ -Ttext 0xc0001500 -e main -o $@

obj/libs/print.o : libs/print.S
	nasm -Iboot/ -f elf -o $@ $^

obj/libs/bitmap.o : libs/bitmap.c
	gcc -Ilibs/ -Ikern/trap/ $(GCCP1) -c $^ -o $@

obj/libs/list.o : libs/list.c
	gcc -Ilibs/ -Ikern/trap/ $(GCCP1) -c $^ -o $@

obj/libs/string.o : libs/string.c
	gcc -Ilibs/ -Ikern/trap/ $(GCCP1) -c $^ -o $@

obj/kern/trap/vector.o : kern/trap/vectors.S
	nasm -Iboot/ -f elf -o $@ $^

obj/kern/process/thread.o : kern/process/thread.c
	gcc -Ilibs/ -Ikern/trap/ $(GCCP1) -c $^ -o $@

obj/kern/process/switch.o : kern/process/switch.S
	nasm -Ilibs/ -f elf -o $@ $^

obj/kern/mm/memory.o : kern/mm/memory.c
	gcc -Ilibs/ -Ikern/trap/ $(GCCP1) -c $^ -o $@

obj/kern/debug/assert.o : kern/debug/assert.c
	gcc -Ilibs/ -Ikern/trap/ $(GCCP1) -c $^ -o $@

obj/kern/trap/interrupt.o : kern/trap/interrupt.c
	gcc -Ilibs/ -Ikern/trap/ $(GCCP1) -c $^ -o $@

obj/kern/driver/timer.o : kern/driver/timer.c
	gcc -Ilibs/ $(GCCP1) -c $^ -o $@

obj/kern/init/init.o : kern/init/init.c
	gcc -Ilibs/ $(GCCP1) -c $^ -o $@

install:
	dd if=target/boot/bootblock.bin of=xxos.img bs=512 count=1 conv=notrunc
	dd if=target/boot/bootmain.bin of=xxos.img bs=512 seek=2 count=3 conv=notrunc
	dd if=target/kern/kernel.bin of=xxos.img bs=512 seek=5 count=200 conv=notrunc

run:
	bochs -f bochsrc


.PHONY:
clean:
	rm $(obj) $(target)
