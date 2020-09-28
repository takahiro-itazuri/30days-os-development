# Memo
## Memory Map
|変数名等|開始アドレス|終了アドレス|サイズ|説明|
|:--|:--|:--|:--|:--|
|			|`0x0000_0000`|`0x0000_03ff`|`0x0000_0400` (1[KB])		|リアルモード用割り込みベクタテーブル|
|			|`0x0000_7c00`|`0x0000_7dff`|`0x0000_0200` (512[B])		|IPL (先頭セクタ)|
|`DSKCAC0`	|`0x0000_8000`|`0x0003_5fff`|`0x0002_d000` (180[KB])	|ディスクキャッシュ (10 シリンダ分)|
|`shtctl`	|`0x0000_0fe4`|`0x0000_0fe7`|`0x0000_0004` (4[B])		|画面描画用の変数|
|`BOOTINFO`	|`0x0000_0ff0`|`0x0000_0ffb`|`0x0000_000c` (12[B])		|ブート関連の情報|
|`[VRAM]`	|`0x000a_0000`|`0x000b_ffff`|`0x0002_0000` (128[KB])	|VRAM (320 x 200 x 8bit カラー)|
|			|`0x000c_0000`|`0x000c_7fff`|`0x0000_8000` (32[KB])		|ビデオ BIOS|
|			|`0x000c_8000`|`0x000e_ffff`|`0x0002_8000` (160[KB])	|拡張 BIOS|
|			|`0x000f_0000`|`0x000f_ffff`|`0x0001_0000` (64[KB])		|マザーボード BIOS|
|`DSKCAC`	|`0x0010_0000`|`0x0026_7fff`|`0x0016_8000` (1440[KB])	|ディスクキャッシュ (FD の大きさ分)|
|`ADR_IDT`	|`0x0026_8000`|`0x0027_ffff`|`0x0000_0800` (2[KB])		|IDT|
|`ADR_GDT`	|`0x0027_0000`|`0x0027_ffff`|`0x0001_0000` (64[KB])		|GDT|
|`BOTPAK`	|`0x0028_0000`|`0x002f_ffff`|`0x0008_0000` (512[KB])	|OS 本体 (bootpack.hrb)|
|			|`0x0030_0000`|`0x0030_ffff`|`0x0001_0000` (64[KB])		|スタック|
|			|`0x0031_0000`|`0x003b_ffff`|`0x000b_0000` (704[KB])	|空き領域|
|			|`0x003c_0000`|`0x003c_7fff`|`0x0000_8000` (32[KB])		|メモリマネージャ|
|			|`0x003c_8000`|`0x003f_ffff`|`0x0003_8000` (224[KB])	|空き領域|
|			|`0x0040_0000`|				|							|空き領域|
|`[VRAM]`	|`0xfd00_0000`|`0xfdff_ffff`|`0x0100_0000` (16[MB])		|VRAM (VESA BIOS Extension)|




# Source Code Reference
## sheet.c (Under Construction)
### `shtctl` 構造体
```c
#define MAX_SHEETS		256

struct SHTCTL {
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
```

- フィールド
	- `unsigned char *vram`: VRAM の開始アドレス
	- `unsigned char *map`: 各ピクセルの位置でどのシートが表示されているかを保持するマップの開始アドレス
	- `int xsize`: 画面の横方向のピクセル数
	- `int ysize`: 画面の縦方向のピクセル数
	- `int top`: `sheets` フィールドの一番上
	- `struct SHEET *sheets[MAX_SHEETS]`: 画面における上下関係を保持
	- `struct SHEET sheets0[MAX_SHEETS]`: 全てのシートを保持
- その他
	- bootpack.c で `shtctl_init` 関数によって 1 つだけ実態が作成される
	- `0x0fe4` に保管しておく


### `shtctl_init` 関数
```c
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
```
- 処理内容
	- `SHTCTL` 構造体用のメモリ領域の確保
	- `SHTCTL` 構造体の `vram` フィールドの設定
	- `SHTCTL` 構造体の `map` フィールドのメモリ領域の確保と設定
	- `SHTCTL` 構造体の `xsize` フィールドと `ysize` フィールドの設定
	- `SHTCTL` 構造体の `top` フィールドをとりあえず `-1` に設定
	- `SHTCTL` 構造体の `sheets0` フィールドを全て初期化
- その他
	- bootpack.c の `shtctl` 変数の初期化を行う


### `SHEET` 構造体
```c
struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
	struct TASK *task;
};
```



## fifo.c
### `FIFO32` 構造体
```c
/* fifo.c */
struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};
```

- フィールド
	- `int *buf`: データを入れるバッファ
	- `int p`: 書き込み位置
	- `int q`: 読み込み位置
	- `int size`: バッファのサイズ
	- `int free`: 空きサイズ
	- `int flags`: フラグ
		- `#define FLAGS_OVERRUN 0x0001`: オーバーラン
	- `struct TASK *task`: データが入ったときに起こすタスク
- その他
	- `bootpack.c` では、`fifo` 変数と `keycmd` 変数が定義されている
		- `fifo`
			- キーボード、マウス、タイマの割り込み時に、データが入れられる
				- キーボードデータ: 256 ~ 511
				- マウスデータ: 512 ~ 767
				- タイマ: 上記以外
			- バッファは `fifobuf` 変数で、サイズは 128
		- `keycmd`
			- キーボードコントローラに送るためのデータが入れられる
			- 具体的にはキーボードの LED を設定するために、キーボードコントローラに情報を送る必要がある
			- バッファは `keycmd_buf` 変数で、サイズは 32


### `fifo32_init` 関数
```c
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK * task);
```
- 処理内容
	- `FIFO32` 構造体の `size`、`buf`、`free`、`task` フィールドを与えられた引数に基づいて設定
	- `FIFO32` 構造体の `p`、`q`、`flags` フィールドを `0` で初期化


### `fifo32_put` 関数
```c
int fifo32_put(struct FIFO32 *fifo, int data);
```
- 処理内容
	- `FIFO32` 構造体の `buf` フィールドにデータを入れる
	- オーバーランした場合、`FIFO32` 構造体の `flags` に `FLAGS_OVERRUN` を設定して `-1` を返す
	- もし `FIFO32` 構造体の `task` フィールドにタスクが紐づけられていて、それが `TASK_READY` の状態じゃない場合、そのタスクを起こす


### `fifo32_get` 関数
```c
int fifo32_get(struct FIFO32 *fifo);
```
- 処理内容
	- `FIFO32` 構造体の `buf` フィールドからデータをとってくる
	- 空の場合は `-1` が返ってくる


### `fifo32_status` 関数
```c
int fifo32_status(struct FIFO32 *fifo);
```
- 処理内容
	- `FIFO32` 構造体の `size`、`free` フィールドから、データがバッファにいくつ溜まっているか確認



## graphic.c
### `init_palette` 関数
```c
void init_palette(void)
```
- 処理内容
	- パレットの初期化


### `set_palette` 関数
```c
void set_palette(int start, int end, unsigned char *rgb);
```
- 処理内容
	- 割り込み禁止にして、パレットの設定を行う
- その他
	- `init_palette` 関数のサブルーチン


### `boxfill8` 関数
```c
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
```
- 処理内容
	- 画面にボックスを表示する


### `init_screen8` 関数
```c
void init_screen8(char *vram, int x, int y);
```
- 処理内容
	- デスクトップ画面を作成する


### `putfonts8` 関数
```c
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
```
- 処理内容
	- 8 x 16 のフォントデータを画面に描画する


### `putfonts8_asc` 関数
```c
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
```
- 処理内容
	- ASCII コードの文字列を画面に描画する
- その他
	- ASCII コードからフォントデータへの変換は、`hankaku` 変数で行える


### `init_mouse_cursor8` 関数
```c
void init_mouse_cursor8(char *mouse, char bc)
```
- 処理内容
	- マウスデータを引数として与えられた `char *mouse` に設定する


### `putblock8_8` 関数
```c
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);
```
- 処理内容
	- ブロックを表示する
- その他
	- 最終的にほとんど使用されない



## timer.c
### `TIMERCTL` 構造体
```c
#define MAX_TIMER		500

struct TIMERCTL {
	unsigned int count, next;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};
```
- フィールド
	- `unsigned int count`: カウンタ
		- タイマ割り込み (10[ms] 毎) が発生する度にインクリメントされる
	- `unsigned int next`: 次にタイムアウトするタイマのタイムアウト値
	- `struct TIMER *t0`: もっともタイムアウト値が短いタイマへのポインタ
		- `TIMER` 構造体が次にタイムアウト値が短いタイマへのポインタ (`next`) を持っているため、タイムアウト値が短い順に評価できる
	- `struct TIMER timers0[MAX_TIMER]`: タイマを保管する配列
- その他
	- `timer.c` で実態を作成し、その他のファイルへは `extern` で共有


### `init_pit` 関数
```c
void init_pit(void);
```
- 処理内容
	- PIT (IRQ0) の初期化。割り込み周期は 100[Hz] に設定する
	- `TIMERCTL` 構造体の `count` を `0` に設定する
	- `TIMERCTL` 構造体の `next` を `0xffffffff` (番兵のタイムアウト値) に設定する
	- `TIMERCTL` 構造体の `timers0` フィールドを全て未使用状態として初期化する
	- 番兵として使用するタイマを設定する


### `TIMER` 構造体
```c
struct TIMER {
	struct TIMER *next;
	unsigned int timeout, flags;
	struct FIFO32 *fifo;
	int data;
};
```
- フィールド
	- `struct TIMER *next`: 次にタイムアウト値が短いタイマへのポインタ。次がない場合は `0`
	- `unsigned int timeout`: タイムアウト値
	- `unsigned int flags`: タイマのフラグ
		- `#define TIMER_FLAGS_UNUSE 0`: 未使用
		- `#define TIMER_FLAGS_ALLOC 1`; タイマを確保した状態。`timer_alloc` 関数で割り当てた後や、タイムアウト後にこの状態になる
		- `#define TIMER_FLAGS_USING 2`: 使用中
	- `struct FIFO32 *fifo`: タイムアウトした時にデータを入れる FIFO キュー
	- `int data`: タイムアウト時に FIFO キューに入れるデータの値


### `timer_alloc` 関数
```c
struct TIMER *timer_alloc(void)
```
- 処理内容
	- 未使用 (フラグが `TIMER_FLAGS_UNUSE`) なタイマを `TIMERCTL` 構造体の `timers0` 配列からとってくる
	- フラグは `TIMER_FLAGS_ALLOC` に変更する


### `timer_free` 関数
```c
void timer_free(struct TIMER *timer);
```
- 処理内容
	- タイマを解放する。
	- フラグを `TIMER_FLAGS_UNUSE` に設定する
- その他
	- おそらく使用中のタイマに対して、これを実行するとバグが発生する


### `timer_init` 関数
```c
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);
```
- 処理内容
	- `timer_alloc` で確保した `TIMER` 構造体に、FIFO キューとタイムアウト時に報告する値を設定する
- その他
	- `timer_alloc` の際に合わせて設定するように設定してしまっても良さそう


### `timer_settime` 関数
```c
void timer_settime(struct TIMER *timer, unsigned int timeout);
```
- 処理内容
	- タイマに対して、タイムアウト値を設定して使用中にする
	- 使用中のタイマは Linked List になっており、タイムアウト値でソートされているため、適切な場所に挿入する


### `inthandler20` 関数
```c
void inthandler20(int *esp);
```
- タイマ割り込みが発生した時に実行される関数
- 処理内容
	- IRQ0 に割り込み完了を通知する
	- タイムアウトしていたタイマに対して、以下の操作を行う
		- タイマの状態を割り当て済み (`TIMER_FLAGS_ALLOC`) に設定する
		- マルチタスク用のタイマじゃない場合、FIFO キューに予め設定していたデータを入れる
		- マルチタスク用のタイマ (`task_timer`) の場合、最後にタスクスイッチする



## mtask.c
### `TSS32` 構造体
```c
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};
```
- フィールド: 後述の TSS を参照
- その他
	- GDT に設定する
	- 全体で 104[B]


### `TASKCTL` 構造体
```c
#define MAX_TASKS		1000
#define MAX_TASKLEVELS	10

struct TASKCTL {
	int now_lv;
	char lv_change;
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};
```
- フィールド
	- `int now_lv`: 実行中のタスクレベル
	- `char lv_change`: 次回タスクスイッチ時に、タスクレベルも変更した方が良いか
	- `struct TASKLEVEL level[MAX_TASKLEVELS]`: `TASKLEVEL` 構造体の配列
	- `struct TASK task0[MAX_TASKS]`: 全タスクを保持する配列


### `TASKLEVEL` 構造体
```c
#define MAX_TASKS_LV	100

struct TASKLEVEL {
	int running;
	int now;
	struct TASK *tasks[MAX_TASKS_LV];
};
```
- フィールド
	- `int running`: `TASK_READY` の状態になっているタスクの数
	- `int now`: 現在実行しているタスク
		- 具体的には、以下の `tasks` 配列のインデックス
	- `struct TASK *tasks[MAX_TASKS_LV]`: このタスクレベルに属するタスクへのポインタの配列


### `TASK` 構造体
```c
struct TASK {
	int sel, flags; /* selはGDTの番号のこと */
	int level, priority;
	struct FIFO32 fifo;
	struct TSS32 tss;
};
```
- フィールド
	- `int sel`: GDT のセグメントセレクタ
	- `int flags`: タスクの状態フラグ
	- `int level`: タスクレベル
	- `int priority`: 優先度
		- 割り当てられる CPU 時間
		- `task_timer` のタイムアウト値として使用される
	- `struct FIFO32 fifo`: 
	- `struct TSS32 tss`: TSS


### `task_init` 関数
```c
struct TASK *task_init(struct MEMMAN *memman);
```
- 処理内容
	- `TASKCTL` 構造体のメモリ領域を確保する
	- `TASKCTL` 構造体を初期化する
		- 全タスクに TSS セグメントのセグメントセレクタを登録する
		- GDT 内の TSS セグメント自体を初期化する
		- `TASKLEVEL` 構造体を初期化する
	- 現在実行されているタスク (OS) 自体を設定する
	- 番兵としてアイドルタスクを設定する


### `task_alloc` 関数
```c
struct TASK *task_alloc(void);
```
- 処理内容
	- 未使用の `TASK` 構造体を取得する
	- 取得した `TASK` 構造体に以下を設定する
		- `flags` フィールドを `TASK_ALLOC` に設定する
		- `tss` フィールドを設定する


### `task_run` 関数
```c
void task_run(struct TASK *task, int level, int priority);
```
- 処理内容
	- `level` 引数が負の値の場合は、`task` 引数に設定されているタスクレベルを使用する
	- `priority` 引数が正の値の場合は、`task` 引数に新しい優先度を設定する
	- 実行可能状態のタスクかつ、新しい `level` と元のタスクレベルが異なる場合は、既存のタスクレベルからタスクを取り除く (このタイミングでタスクの状態フラグが `TASK_ALLOC` になる)
	- 実行可能状態のタスクでない場合は、タスクを指定したタスクレベルに実行中のタスクとして追加する
	- 新しい実行中のタスクが増えているため、タスクスイッチの際にタスクレベルを見直す
- その他
	- 以下の場合に使用する
		- 新しいタスクを実行する場合
		- スリープ中のタスクを実行する場合
		- タスクの優先度を変更する場合


### `task_sleep` 関数
```c
void task_sleep(struct TASK *task);
```
- 処理内容
	- 実行可能状態じゃない場合は特に何もせず、実行可能状態の場合は以降の処理を行う
	- タスクを現在のタスクレベルから取り除く
	- 現在実行中のタスクと、取り除く対象のタスクが一致している場合、タスクスイッチを行う。


### `task_now` 関数
```c
struct TASK *task_now(void);
```
- 処理内容
	- 現在実行中のタスクレベルから、実行中のタスクを取得する


### `task_add` 関数
```c
void task_add(struct TASK *task);
```
- 処理内容
	- `task` 引数には、事前に設定されたタスクレベルにタスクを追加する


### `task_remove` 関数
```c
void task_remove(struct TASK *task);
```
- 処理内容
	- 削除対象のタスクが `TASKLEVEL` 構造体の `tasks` 配列のどこにあるかを探索する
	- 対象のタスクの状態フラグを `TASK_ALLOC` にする
	- 必要に応じて `TASKLEVEL` 構造体の `tasks` 配列をずらす処理を行う
- その他
	- `task_sleep` 関数から呼び出される


### `task_idle` 関数
```c
void task_idle(void);
```
- 処理内容
	- `HLT` するだけ
- その他
	- 番兵として使用される


### `task_switch` 関数
```c
void task_switch(void);
```
- 処理内容
	- 現在実行中の `TASKLEVEL` 構造体の `now` フィールドを更新する
	- もし `TASKCTL` 構造体の `lv_change` がセットされている場合、`task_switchsub` 関数を実行して、最上位のタスクレベルを取得する
	- 最上位のタスクレベルから、次に実行するタスクを取得する
	- 次に実行するタスクの優先度に基づいて、タイマを設定する (CPU 時間を割り当てる)
	- 新しいタスクとこれまで実行していたタスクが一致しない場合は、Far-JMP する


### `task_switchsub` 関数
```c
void task_switchsub(void);
```
- 処理内容
	- 実行可能状態なタスクを持つ、一番上のタスクレベルを探索する
	- `TASKCTL` 構造体の `now_lv` フィールドに一番上のタスクレベルを設定する
	- `TASKCTL` 構造体の `lv_change` フィールドを `0` に設定する




# General Knowledge
## GDT / LDT / IDT / TSS
### Segment Descriptor
- GDT と LDT で使用されるセグメントディスクリプタ
- 構成要素
	- Base Address (32[bit]): セグメントの開始アドレス
	- D/B (1[bit])
		- `0`: 16-bit モード
		- `1`: 32-bit モード
	- G - Granularity (1[bit]): セグメントリミットの単位
		- `0`: 1[B] 単位
		- `1`: 4[KB] 単位
	- Segment Limit (20[bit]): セグメントリミット (セグメントサイズ - 1)
	- AVL - Available (1[bit]): システムソフトウェア (OS 等) が自由に利用できる
	- P - Segment-Present (1[bit]): セグメント存在
		- `0`: セグメントがメモリ内にない
		- `1`: セグメントがメモリ内にある
	- DPL - Descriptor Privilege Level (2[bit]): セグメントの特権レベル (0 ~ 3)
	- S - Descriptor Type (1[bit]): ディスクリプタタイプ
		- `0`: システムセグメント
		- `1`: コードセグメントまたはデータセグメント
	- Type (4[bit]): セグメントタイプ
		- データセグメント
			- `0EWA`: E (Expand-Direction), W (Write-Enable), A (Accessed)
			- `0000`: Read-Only
			- `0001`: Read-Only, Accessed
			- `0010`: Read/Write
			- `0011`: Read/Write, Accessed
			- `0100`: Read-Only, Expand-Down
			- `0101`: Read-Only, Expand-Down, Accessed
			- `0110`: Read/Write, Expand-Down
			- `0111`: Read/Write, Expand-Down, Accessed
		- コードセグメント
			- `1CRA`: C (Conforming), R (Read Enable), A (Accessed)
			- `1000`: Execute-Only
			- `1001`: Execute-Only, Accessed
			- `1010`: Execute/Read
			- `1011`: Execute/Read, Accessed
			- `1100`: Execute-Only, Conforming
			- `1101`: Execute-Only, Comforming, Accessed
			- `1110`: Execute/Read, Conforming
			- `1111`: Execute/Read, Conforming, Accessed
		- システムセグメント
			- `0001`: 16-bit TSS (Available)
			- `0010`: LDT
			- `0011`: 16-bit TSS (Busy)
			- `0100`: 16-bit Call Gate
			- `0101`: Task Gate
			- `0110`: 16-bit Interrupt Gate
			- `0111`: 16-bit Trap Gate
			- `1001`: 32-bit TSS (Available)
			- `1011`: 32-bit TSS (Busy)
			- `1100`: 32-bit Call Gate
			- `1110`: 32-bit Interrupt Gate
			- `1111`: 32-bit Trap Gate
```
 63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32
+-----------------------+-----+-----+-----------+--+-----+--+-----------+-----------------------+
|                       |  |D |  |A |  Segment  |  |     |  |           |                       |
| Base Address (24-31)  |G |/ |0 |V |   Limit   |P | DPL |S |   Type    | Base Address (23-16)  |
|                       |  |B |  |L |  (19-16)  |  |     |  |           |                       |
+-----------------------+-----+-----+--------------+-----+--+-----------+-----------------------+
|                                               |                                               |
|              Base Address (0-15)              |             Segment Limit (0-15)              |
|                                               |                                               |
+-----------------------------------------------+-----------------------------------------------+
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
```


### GDT (Global Descritor Table)
- セグメントディスクリプタを保持するテーブル
- システム全体で 1 つだけ作成され、全てのプログラムから使用される
- 最大エントリ数は 8192 個 (先頭エントリは 0 で埋めているため、実際は 8191 個)
- 具体的なアクセス手順
	1. GDTR より GDT のリニアアドレスを取得
	2. セグメントレジスタから GDT のインデックスを取得
	3. セグメントディスクリプタサイズは 8[B] なので、`GDT のリニアアドレス + 8 x インデックス` でアクセス可能
- ディスクリプタの種類
	- Code Segment Descriptor
	- Data Segment Descriptor
	- System Segment Descriptor
		- LDT (Local Descriptor Table) Segment Descriptor
		- TSS (Task State Segment) Descriptor
		- Call Gate Descriptor
		- Interrupt Gate Descriptor
		- Task Gate Descriptor
		- Trap Gate Descriptor

#### GDTR
- GDT に関する情報を保持する CPU のレジスタ
- 構成要素
	- Table Limit (16[bit]): GDT のエントリ数
	- Linear Base Address (32[bit]): GDT が保管されているリニアベースアドレス
```
 47                 16 15　　　　　  0
+---------------------+-------------+
| Linear Base Address | Table Limit |
+---------------------+-------------+
```


### LDT (Local Descriptor Table)
- アプリケーション同士は同じ特権レベルを持つため、特権レベルによる保護だけでは、他のアプリケーションのメモリ領域にアクセスすることが可能となってしまう
- LDT は、タスク毎に GDT に相当するディスクリプタテーブルとして機能することで、アプリケーションのセキュリティを高める
- 1 つの LDT を複数または全てのタスクで共有することもできる
- 具体的なアクセス手順
	1. LDTR からの値から、GDT 内の LDT セグメントディスクリプタを取得
	2. LDT セグメントディスクリプタから、LDT のリニアベースアドレスやリミットを取得
	3. 各セグメントレジスタで TI が 1 になっている場合は、この LDT に基づいてセグメントを取得
- TSS は LDTR を保持しているため、LDT が自動的に切り替えられる。

#### LDTR
- LDT に関する情報を保持する CPU のレジスタ
- 構成要素
	- Segment Selector (16[bit]): GDT 内の LDT セグメントディスクリプタのセグメントセレクタ
```
 15　　　　　       0
+------------------+
| Segment Selector |
+------------------+
```


### IDT (Interrupt Descriptor Table)
- 最大エントリ数は 256 個
- ディスクリプタの種類
	- Task Gate Descriptor
	- Interrupt Gate Descriptor
	- Trap Gate Descriptor
```
Task Gate Descriptor
 63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32
+-----------------------------------------------+--+-----+--+--+--+--+--+-----------------------+
|                                               |  |     |  |  |  |  |  |                       |
|                     Unused                    |P | DPL |0 |0 |1 |0 |1 |         Unused        |
|                                               |  |     |  |  |  |  |  |                       |
+--------------------------------------------------+-----+--+--+--+--+--+-----------------------+
|                                               |                                               |
|              TSS Segment Selector             |                     Unused                    |
|                                               |                                               |
+-----------------------------------------------+-----------------------------------------------+
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00

Interrupt Gate Descriptor
 63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32
+-----------------------------------------------+--+-----+--+--+--+--+--+--+--+--+--------------+
|                                               |  |     |  |  |  |  |  |  |  |  |              |
|             Offset Address (16-31)            |P | DPL |0 |D |1 |1 |0 |0 |0 |0 |    Unused    |
|                                               |  |     |  |  |  |  |  |  |  |  |              |
+--------------------------------------------------+-----+--+--+--+--+--+--+--+--+--------------+
|                                               |                                               |
|                Segment Selector               |             Offset Address (0-15)             |
|                                               |                                               |
+-----------------------------------------------+-----------------------------------------------+
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00

Trap Gate Descriptor
 63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32
+-----------------------------------------------+--+-----+--+--+--+--+--+--+--+--+--------------+
|                                               |  |     |  |  |  |  |  |  |  |  |              |
|             Offset Address (16-31)            |P | DPL |0 |D |1 |1 |1 |0 |0 |0 |    Unused    |
|                                               |  |     |  |  |  |  |  |  |  |  |              |
+--------------------------------------------------+-----+--+--+--+--+--+--+--+--+--------------+
|                                               |                                               |
|                Segment Selector               |             Offset Address (0-15)             |
|                                               |                                               |
+-----------------------------------------------+-----------------------------------------------+
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
```

#### IDTR
- IDT に関する情報を保持する CPU のレジスタ
- 構成要素
	- Table Limit (16[bit]): IDT のエントリ数
	- Linear Base Address (32[bit]): IDT が保管されてるリニアベースアドレス
```
 47                 16 15　　　　　  0
+---------------------+-------------+
| Linear Base Address | Table Limit |
+---------------------+-------------+
```


### Segment Register
- リアルモード
	- 20[bit]のリニアアドレスの上位 16[bit]を保持する
- プロテクトモード
	- セグメントセレクタを保持する
	- 論理アドレスは、セグメントセレクタ (16[bit]) とオフセット (32[bit]) で表される
	- セグメントセレクタに含まれるインデックスの値から GDT / LDT のエントリ (セグメントディスクリプタ) を参照し、セグメントのベースリニアアドレスを取得し、これにオフセットを足すことで、リニアアドレスに変換することができる。
	- 構成要素
		- RPL (Request Privilege Level): 特権レベル
		- TI (Table Indicator): セグメントが GDT (0) / LDT (1) のどちらにあるのか
		- Index: GDT / LDT のインデックス
			- 下位 3[bit]をマスクする（`AND 0xFFF8`）ことで、自動的にインデックスの値を 8 倍した値となり、GDT / LDT の先頭から対象エントリまでのバイト数が表現される。
```
 15            3  2 1 0
+---------------+--+---+
|     Index     |TI|RPL|
+---------------+--+---+
```


### TSS
- TSS はタスクの状態の保存・復元に使用するデータ構造である
- タスク 1 つにつき 1 つ TSS を用意する
- 特権レベルの遷移があった場合、CPU は TSS からスタックポインタおよびスタックセグメントを自動的にロードして、特権レベルごとに違うスタックを使用する
- 構成要素
	- Backlink (Previous Task Link): CALL 命令や割り込みでタスクを切り替えた時に、元のタスクのセレクタ値を CPU が保存する領域
	- ESP0: 特権レベル 0 に移行した時のスタックポインタ
	- SS0: 特権レベル 0 に移行した時のスタックセグメント
	- ESP1: 特権レベル 1 に移行した時のスタックポインタ
	- SS1: 特権レベル 1 に移行した時のスタックセグメント
	- ESP2: 特権レベル 2 に移行した時のスタックポインタ
	- SS2: 特権レベル 2 に移行した時のスタックセグメント
	- CR3: タスク毎のページテーブルのアドレス
	- EIP, EFLAGS, EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI: 32[bit] レジスタ
	- ES, CS, SS, DS, FS, GS: 16[bit] レジスタ
	- LDT Segment Selector (LDTR): GDT 内の LDT セグメントディスクリプタのセレクタ
	- T (Trap Bit): セットされている場合、ハードウェアタスクスイッチ時にデバッグ例外 (`INT 1`) が発生する
	- I/O Map Base Address: TSS の先頭から I/O 許可ビットマップまでのバイト数
```
+----------------------+--------------------+-+
| I/O Map Base Address |       Reserved     |T|
+-------------------------------------------+-+
|       Reserved       | LDT Segment Selector |
+---------------------------------------------+
|       Reserved       |          GS          |
+---------------------------------------------+
|       Reserved       |          FS          |
+---------------------------------------------+
|       Reserved       |          DS          |
+---------------------------------------------+
|       Reserved       |          SS          |
+---------------------------------------------+
|       Reserved       |          CS          |
+---------------------------------------------+
|       Reserved       |          ES          |
+----------------------+----------------------+
|                     EDI                     |
+---------------------------------------------+
|                     ESI                     |
+---------------------------------------------+
|                     EBP                     |
+---------------------------------------------+
|                     ESP                     |
+---------------------------------------------+
|                     EBX                     |
+---------------------------------------------+
|                     EDX                     |
+---------------------------------------------+
|                     ECX                     |
+---------------------------------------------+
|                     EAX                     |
+---------------------------------------------+
|                    EFLAGS                   |
+---------------------------------------------+
|                     EIP                     |
+---------------------------------------------+
|                     CR3                     |
+----------------------+----------------------+
|       Reserved       |         SS2          |
+----------------------+----------------------+
|                     ESP2                    |
+----------------------+----------------------+
|       Reserved       |         SS1          |
+----------------------+----------------------+
|                     ESP1                    |
+----------------------+----------------------+
|       Reserved       |         SS0          |
+----------------------+----------------------+
|                     ESP0                    |
+----------------------+----------------------+
|       Reserved       |  Previous Task Link  |
+----------------------+----------------------+
```

#### TR (Task Register)
- TSS に関する情報を保持する CPU のレジスタ
- 構成要素
	- Segment Selector (16[bit]): GDT 内の TSS セグメントディスクリプタのセグメントセレクタ
```
 15　　　　　       0
+------------------+
| Segment Selector |
+------------------+
```



## Jump / Call
### Jump
- Near-Jump
- Far-Jump


### CALL
- Near-Jump
- Far-Jump

