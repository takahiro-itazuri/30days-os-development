#include <stdarg.h>
#include "mystdio.h"

/* 10進数からASCIIコードに変換 */
int dec2asc(char *str, int dec, int len) {
	int len_buf = 0, tmp;
	int buf[10];
	
	for (;;) {
		buf[len_buf++] = dec % 10;
		if (dec < 10) break;
		dec /= 10;
	}
	
	if (len == 0) {
		len = len_buf;

		while (len_buf) {
			*(str++) = buf[--len_buf] + '0';
		}
	}
	else {
		tmp = len;

		while (tmp) {
			if (tmp > len_buf) {
				--tmp;
				*(str++) = 0x30;
			}
			else {
				*(str++) = buf[--tmp] + '0';
			}
		}
	}

	return len;
}

/* 16進数からASCIIコードに変換 */
int hex2asc(char *str, int dec, int len) {
	int len_buf = 0, tmp;
	int buf[10];

	for (;;) {
		buf[len_buf++] = dec % 16;
		if (dec < 16) break;
		dec /= 16;
	}

	if (len == 0) {
		len = len_buf;

		while (len_buf) {
			--len_buf;
			*(str++) = (buf[len_buf] < 10) ? (buf[len_buf] + '0') : (buf[len_buf] + 'a');
		}
	}
	else {
		tmp = len;

		while (tmp) {
			if (tmp > len_buf) {
				--tmp;
				*(str++) = '0';
			}
			else {
				--tmp;
				*(str++) = (buf[tmp] < 10) ? (buf[tmp] + '0') : (buf[tmp] - 10 + 'a');
			}
		}
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
			len = 0;
			while ('0' <= *fmt && *fmt <= '9') {
				len = len * 10 + (*fmt - '0');
				++fmt;
			}
			switch(*fmt) {
			case 'd':
				len = dec2asc(str, va_arg(args, int), len);
				break;
			case 'x':
				len = hex2asc(str, va_arg(args, int), len);
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