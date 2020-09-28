OBJS_BOOTPACK = bootpack.obj nasmfunc.obj hankaku.obj mystdio.obj mystring.obj mystdlib.obj graphic.obj dsctbl.obj int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj mtask.obj window.obj console.obj file.obj
HDRS_BOOTPACK = bootpack.h mystdio.h mystring.h mystdlib.h
APPS = hello.hrb hello2.hrb hello3.hrb hello4.hrb hello5.hrb a.hrb bug1.hrb bug2.hrb bug3.hrb winhelo.hrb winhelo2.hrb winhelo3.hrb star1.hrb stars.hrb stars2.hrb lines.hrb

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

hello4.hrb: hello4.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o hello4.hrb hello4.obj a_nasm.obj

hello5.hrb: hello5.asm Makefile
	nasm -f elf32 -o hello5.obj -l hello5.lst hello5.asm
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o hello5.hrb hello5.obj

bug1.hrb: bug1.obj a_nasm.obj Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o bug1.hrb bug1.obj a_nasm.obj

bug2.hrb: bug2.obj Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o bug2.hrb bug2.obj

bug3.hrb: bug3.obj a_nasm.obj Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o bug3.hrb bug3.obj a_nasm.obj

winhelo.hrb: winhelo.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o winhelo.hrb winhelo.obj a_nasm.obj

winhelo2.hrb: winhelo2.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o winhelo2.hrb winhelo2.obj a_nasm.obj

winhelo3.hrb: winhelo3.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o winhelo3.hrb winhelo3.obj a_nasm.obj

star1.hrb: star1.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o star1.hrb star1.obj a_nasm.obj

stars.hrb: stars.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o stars.hrb stars.obj a_nasm.obj

stars2.hrb: stars2.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o stars2.hrb stars2.obj a_nasm.obj

lines.hrb: lines.obj a_nasm.obj app.ld Makefile
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o lines.hrb lines.obj a_nasm.obj

haribote.sys: asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb > haribote.sys

haribote.img: ipl10.bin haribote.sys $(APPS) Makefile
	mformat -f 1440 -C -B ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote.sys ::haribote.sys
	mcopy -i haribote.img $(APPS) ::

img:
	make -r haribote.img

run:
	make img
	qemu-system-i386 -m 32M -drive file=haribote.img,if=floppy,format=raw -boot a

clean:
	rm -rf *.bin *.lst *.img *.sys *.obj *.hrb *.map
