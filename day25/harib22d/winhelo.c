int api_openwin(char *buf, int xsize, int ysize, int col_inv, char *title);
void api_closewin(int win);
int api_getkey(int mode);
void api_end(void);

char buf[150 * 50];

void HariMain(void)
{
	int win;
	win = api_openwin(buf, 150, 50, -1, "hello");
	for (;;) {
		if (api_getkey(1) == 0x0a) { break; } /* Enter */
	}
	api_closewin(win);
	api_end();
}