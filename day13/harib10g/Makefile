OBJS_BOOTPACK = bootpack.obj nasmfunc.obj hankaku.obj mystdio.obj graphic.obj dsctbl.obj int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj
HDRS_BOOTPACK = mystdio.h bootpack.h

default:
	make img

ipl10.bin: ipl10.asm Makefile
	nasm -o ipl10.bin -l ipl10.lst ipl10.asm

asmhead.bin: asmhead.asm Makefile
	nasm -o asmhead.bin -l asmhead.lst asmhead.asm

nasmfunc.obj: nasmfunc.asm Makefile
	nasm -f elf32 -o nasmfunc.obj -l nasmfunc.lst nasmfunc.asm

%.obj: %.c Makefile
	i386-elf-gcc -c -march=i486 -m32 -nostdlib -o $@ $<

bootpack.hrb: $(OBJS_BOOTPACK) $(HDRS_BOOTPACK) hrb.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T hrb.ld -o bootpack.hrb $(OBJS_BOOTPACK)

haribote.sys: asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb > haribote.sys

haribote.img: ipl10.bin haribote.sys Makefile
	mformat -f 1440 -C -B ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote.sys ::haribote.sys

img:
	make -r haribote.img

run:
	make img
	qemu-system-i386 -m 32M -drive file=haribote.img,if=floppy,format=raw -boot a

clean:
	rm -rf *.bin *.lst *.img *.sys *.obj *.hrb
