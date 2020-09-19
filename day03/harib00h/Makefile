default:
	make img

ipl10.bin: ipl10.asm Makefile
	nasm -o ipl10.bin -l ipl10.lst ipl10.asm

haribote.sys: haribote.asm Makefile
	nasm -o haribote.sys -l haribote.lst haribote.asm

haribote.img: ipl10.bin haribote.sys Makefile
	mformat -f 1440 -C -B ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote.sys ::haribote.sys

img:
	make -r haribote.img

run:
	make img
	qemu-system-i386 -drive file=haribote.img,if=floppy,format=raw -boot a

clean:
	rm -rf *.bin *.lst *.img *.sys
