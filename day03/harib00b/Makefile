default:
	make img

ipl.bin: ipl.asm Makefile
	nasm -o ipl.bin -l ipl.lst ipl.asm

helloos.img: ipl.bin Makefile
	mformat -f 1440 -C -B ipl.bin -i helloos.img

img:
	make -r helloos.img

run:
	make img
	qemu-system-i386 -drive file=helloos.img,if=floppy,format=raw -boot a

clean:
	rm -rf *.bin *.lst *.img
