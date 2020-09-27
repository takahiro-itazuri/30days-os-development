OBJS_BOOTPACK = bootpack.obj nasmfunc.obj hankaku.obj mystdio.obj mystring.obj graphic.obj dsctbl.obj int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj mtask.obj window.obj console.obj file.obj
HDRS_BOOTPACK = bootpack.h mystdio.h mystring.h

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
	i386-elf-gcc -march=i486 -m32 -nostdlib -T hrb.ld -Wl,-Map=bootpack.map -o bootpack.hrb $(OBJS_BOOTPACK)

hello.hrb: hello.asm Makefile
	nasm -o hello.hrb -l hello.lst hello.asm

hello2.hrb: hello2.asm Makefile
	nasm -o hello2.hrb -l hello2.lst hello2.asm

a_nasm.obj: a_nasm.asm Makefile
	nasm -f elf32 -o a_nasm.obj -l a_nasm.lst a_nasm.asm

a.hrb: a.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o a.hrb a.obj a_nasm.obj

hello3.hrb: hello3.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o hello3.hrb hello3.obj a_nasm.obj

crack1.hrb: crack1.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o crack1.hrb crack1.obj a_nasm.obj

crack2.hrb: crack2.asm Makefile
	nasm -o crack2.hrb -l crack2.lst crack2.asm

crack3.hrb: crack3.asm Makefile
	nasm -o crack3.hrb -l crack2.lst crack3.asm

crack4.hrb: crack4.asm Makefile
	nasm -o crack4.hrb -l crack4.lst crack4.asm

crack5.hrb: crack5.asm Makefile
	nasm -o crack5.hrb -l crack5.lst crack5.asm

crack6.hrb: crack6.asm Makefile
	nasm -o crack6.hrb -l crack6.lst crack6.asm

haribote.sys: asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb > haribote.sys

haribote.img: ipl10.bin haribote.sys hello.hrb hello2.hrb a.hrb hello3.hrb crack1.hrb crack2.hrb crack3.hrb crack4.hrb crack5.hrb crack6.hrb Makefile
	mformat -f 1440 -C -B ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote.sys ::haribote.sys
	mcopy -i haribote.img hello.hrb ::hello.hrb
	mcopy -i haribote.img hello2.hrb ::hello2.hrb
	mcopy -i haribote.img a.hrb ::a.hrb
	mcopy -i haribote.img hello3.hrb ::hello3.hrb
	mcopy -i haribote.img crack1.hrb ::crack1.hrb
	mcopy -i haribote.img crack2.hrb ::crack2.hrb
	mcopy -i haribote.img crack3.hrb ::crack3.hrb
	mcopy -i haribote.img crack4.hrb ::crack4.hrb
	mcopy -i haribote.img crack5.hrb ::crack5.hrb
	mcopy -i haribote.img crack6.hrb ::crack6.hrb

img:
	make -r haribote.img

run:
	make img
	qemu-system-i386 -m 32M -drive file=haribote.img,if=floppy,format=raw -boot a

clean:
	rm -rf *.bin *.lst *.img *.sys *.obj *.hrb *.map
