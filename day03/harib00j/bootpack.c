/* 他のファイルで作った関数がありますとCコンパイラに教える */

void io_hlt(void);

/* 関数宣言なのに、{}がなくていきなり;を書くと
	他のファイルにあるからよろしくね、という意味になるのです。 */

void HariMain(void)
{

fin:
	io_hlt();	/* これでnasmfunc.asmのio_hltが実行されます */
	goto fin;

}