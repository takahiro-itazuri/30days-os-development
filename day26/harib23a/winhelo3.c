int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_putstrwin(int win, int x, int y, int col, int len, char *str);
void api_boxfillwin(int win, int x0, int y0, int x1, int y1, int col);
void api_initmalloc(void);
char *api_malloc(int size);
void api_closewin(int win);
int api_getkey(int mode);
void api_end(void);

void HariMain(void)
{
	char *buf;
	int win;

	api_initmalloc();
	buf = api_malloc(150 * 50);
	win = api_openwin(buf, 150, 50, -1, "hello");
	api_boxfillwin(win,  8, 36, 141, 43, 6);
	api_putstrwin(win, 28, 28, 0, 12, "hello, world");
	for (;;) {
		if (api_getkey(1) == 0x0a) { break; } /* Enter */
	}
	api_closewin(win);
	api_end();
}