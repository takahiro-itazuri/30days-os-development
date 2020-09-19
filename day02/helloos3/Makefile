default:
	make img

helloos.img: helloos.asm Makefile
	nasm -o helloos.img helloos.asm

img:
	make -r helloos.img

run:
	make img
	qemu-system-i386 -drive file=helloos.img,if=floppy,format=raw -boot a

clean:
	rm -rf *.img