default:
	make img

ipl.bin: ipl.asm Makefile
	nasm -o ipl.bin -l ipl.lst ipl.asm

haribote.sys: haribote.asm Makefile
	nasm -o haribote.sys -l haribote.lst haribote.asm

haribote.img: ipl.bin haribote.sys Makefile
	mformat -f 1440 -C -B ipl.bin -i haribote.img
	mcopy -i haribote.img haribote.sys ::haribote.sys

img:
	make -r haribote.img

run:
	make img
	qemu-system-i386 -drive file=haribote.img,if=floppy,format=raw -boot a

clean:
	rm -rf *.bin *.lst *.img *.sys
