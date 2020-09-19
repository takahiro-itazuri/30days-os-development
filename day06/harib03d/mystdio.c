#include <stdarg.h>
#include "mystdio.h"

/* 10進数からASCIIコードに変換 */
int dec2asc(char *str, int dec) {
	int len, len_buf = 0;
	int buf[10];
	
	for (;;) {
		buf[len_buf++] = dec % 10;
		if (dec < 10) break;
		dec /= 10;
	}
	len = len_buf;

	while (len_buf) {
		*(str++) = buf[--len_buf] + 0x30;
	}

	return len;
}

/* 16進数からASCIIコードに変換 */
int hex2asc(char *str, int dec) {
	int len, len_buf = 0;
	int buf[10];

	for (;;) {
		buf[len_buf++] = dec % 16;
		if (dec < 16) break;
		dec /= 16;
	}
	len = len_buf;

	while (len_buf) {
		--len_buf;
		*(str++) = (buf[len_buf] < 10) ? (buf[len_buf] + 0x30) : (buf[len_buf] - 9 + 0x60);
	}

	return len;
}

void mysprintf(char *str, char *fmt, ...) {
	va_list args;
	int i, len;

	va_start(args, fmt);

	while (*fmt) {
		if (*fmt == '%') {
			++fmt;
			switch (*fmt) {
			case 'd':
				len = dec2asc(str, va_arg(args, int));
				break;
			case 'x':
				len = hex2asc(str, va_arg(args, int));
				break;
			}
			str += len;
			++fmt;
		} else {
			*(str++) = *(fmt++);
		}
	}
	*str = 0x00;
	va_end(args);
	return;
}