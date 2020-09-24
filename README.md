# OS 自作入門 for Mac
## 背景 / モチベーション
これまで以下のようなプロジェクトにおいて、macOS で「OS 自作入門」を扱えました。
- [tatsumack/30nichideosjisaku: 『30日でできる！ OS自作入門』のmacOS開発環境構築](https://github.com/tatsumack/30nichideosjisaku)

ただし、nask が 32bit アプリであるために、最新の macOS では利用することができなくなってしまいました。

実行すると、以下のようなエラーが出ます。
```
Bad CPU type in executable
```

nask 自体は、nasm という無料のアセンブラの文法の多くを真似て、自動最適化能力を高めたアセンブラということですので、最新の macOS でも利用できる nasm を使用する方法に置き換えようと考えました。

とはいえ、そこまで難しくはなく、Linux 上で nasm を使って実現されている方々がブログを残されており、また C 言語になれば修正が必要な部分はほとんどありません。  
このプロジェクトでは、これらのブログを参考に、macOS での環境構築や全てのソースコードを含めた形で記録して残しておくことを目的としています。



## 必要なツール類のインストール
- [nasm](https://www.nasm.us/): nask の代わりに使用するアセンブラ
- [qemu](https://www.qemu.org/): 作成した OS の起動実験用のエミュレータ
- [mtools](https://www.gnu.org/software/mtools/): edimg の代わりに使用するフロッピーディスク用のイメージ作成ツール
- [i386-elf-toolchain (i386-elf-gcc, i386-elf-binutils)](https://github.com/nativeos/homebrew-i386-elf-toolchain): cc1 の代わりに使用する C 言語のコンパイラ (Mac の gcc は clang のため、`-T` でリンカスクリプトを渡せない)

```
brew install qemu nasm mtools
brew tap nativeos/i386-elf-toolchain
brew install i386-elf-binutils i386-elf-gcc
```

バージョンが表示されていれば、インストールできています。
```
qemu-system-i386 --version
nasm --version
mtools --version
i386-elf-gcc --version
```



## 変更点一覧
|変更前 (nask)|変更後 (nasm)|コメント・説明|
|:--|:--|:--|
|`RESB 18`|`TIMES 18 DB 0`|`RESB` を使うと警告が出ます。|
|`RESB 0x7dfe-$`|`TIMES 0x1fe-($-$$) DB 0`||
|`JMP entry`|`JMP SHORT entry`|特にエラーや警告は出ませんでしたが、参考にした文献では意図しないコードになりえるとのこと。|
|`[INSTRSET “i486p”]`|削除|エラーが発生します。必要ないらしいです。|
|`[FORMAT "WCOFF"]`|削除|エラーが発生します。必要ないらしいです。|
|`[FILE "naskfunc.nas"]`|削除|エラーが発生します。必要ないらしいです。|
|`_io_hlt`|`io_hlt`|アンダーバーがあるとエラーが発生します。他の関数も同様。|



## フォントファイル
[『30日でできる！OS自作入門』のメモ](https://vanya.jp.net/os/haribote.html) より変換ずみの hankaku.c を使用しました。



## sprintf
[sprintfを実装する | OS自作入門 5日目-2 【Linux】 | サラリーマンがハッカーを真剣に目指す](http://bttb.s1.valueserver.jp/wordpress/blog/2017/12/17/makeos-5-2/) より sprintf 関数の実装をベースに使用しました。

上記の実装に加えて、以下の機能を追加しています。
- `%X`(16 進数表示の際のアルファベットの大文字表記) への対応
- 文字数指定
- ゼロパディング



## リンカスクリプト
リンカスクリプトは、[『30日でできる！OS自作入門』のメモ](https://vanya.jp.net/os/haribote.html#hrb) に紹介されていたものを、そのまま使用しています。



## QEMU
13 日目の harib10c でタイマの性能測定をする際に、なぜか割り込み処理が実行されなくなってしまいました。  
Mac の Activity Monitor を見ると、qemu の CPU 使用率はほぼ 100 % で張り付いていて、`count` は正常にインクリメントされているようでした。  
そこで、`count++` 後に適用な画面描画を挟むと、正常に割り込み処理が行われるようになりました。  
しかし、この場合は測定結果の誤差が大きくなってしまい、性能測定の目的が果たせませんでした。

QEMU のせいなのかは不明ですが、[OS自作入門してみた＆やりきった - ハラミTech](https://blog.haramishio.xyz/entry/hariboteos) を参考にしたところ、`-enable-kvm` (`-machine accel=kvm`) をつけることで解消したとのことでした。  
しかし、KVM は Linux で利用できる機能ですので、Mac では利用できません。
```
$ qemu-system-i386 -m 32M -drive file=haribote.img,if=floppy,format=raw -boot a -enable-kvm
qemu-system-i386: invalid accelerator kvm
```
[macos - How to enable KVM on a Mac for Qemu? - Stack Overflow](https://stackoverflow.com/questions/53778106/how-to-enable-kvm-on-a-mac-for-qemu) を参考にしたところ、`-machine accel=hvf` を代わりにアクレラレータとして利用できるとのことでした。  
しかし、`qemu-system-i386` では利用できないようでした。
```
$ qemu-system-i386 -m 32M -drive file=haribote.img,if=floppy,format=raw -boot a -machine accel=hvf
qemu-system-i386: invalid accelerator hvf
```

そこで、以下のコマンドを試してみました。
```
$ qemu-system-x86_64 -m 32M -drive file=haribote.img,if=floppy,format=raw -boot a -machine accel=hvf
qemu-system-x86_64: warning: host doesn't support requested feature: CPUID.80000001H:ECX.svm [bit 2]
```
CPUID 命令に関する警告メッセージは出ていますが、とりあえずソースコードがそのまま動作するようになりました。  

ただし、結局のところ、測定結果の誤差が非常に大きくなってしまい、性能測定の目的は果たせませんでした。
測定回数を十分に大きくすることで多少の誤差は無視できるようになるかもしれないですが、どうしても性能測定をしたい場合は、実機で行うのがやはり良さそうです。



## 動作確認環境
- macOS Catalina 10.15.6
- NASM version 2.15.05
- QEMU emulator version 5.1.0
- mtools (GNU mtools) 4.0.24
- 386-elf-gcc (GCC) 9.2.0



## 参考文献
- [tools/nask - hrb-wiki](http://hrb.osask.jp/wiki/?tools/nask)
- [『30日でできる！OS自作入門』のメモ](https://vanya.jp.net/os/haribote.html)
- [『30日でできる！OS自作入門』を macOS Catalina で実行する - Qiita](https://qiita.com/noanoa07/items/8828c37c2e286522c7ee)
- [noanoa07/myHariboteOS: 『30日でできる！OS自作入門』 for macOS Catalina](https://github.com/noanoa07/myHariboteOS)
- [sprintfを実装する | OS自作入門 5日目-2 【Linux】 | サラリーマンがハッカーを真剣に目指す](http://bttb.s1.valueserver.jp/wordpress/blog/2017/12/17/makeos-5-2/)
- [OS自作入門してみた＆やりきった - ハラミTech](https://blog.haramishio.xyz/entry/hariboteos)
- [【学習メモ#3rd】12ステップで作る組込みOS自作入門](https://www.slideshare.net/sandai/312os)



## その他・メモ
### メモリマップ
|変数名等|開始アドレス|終了アドレス|サイズ|説明|
|:--|:--|:--|:--|:--|
|			|`0x0000_0000`|`0x0000_03ff`|`0x0000_0400` (1[KB])		|リアルモード用割り込みベクタテーブル|
|			|`0x0000_7c00`|`0x0000_7dff`|`0x0000_0200` (512[B])		|IPL (先頭セクタ)|
|`DSKCAC0`	|`0x0000_8000`|`0x0003_5fff`|`0x0002_d000` (180[KB])	|ディスクキャッシュ (10 シリンダ分)|
|`[VRAM]`	|`0x000a_0000`|`0x000b_ffff`|`0x0002_0000` (128[KB])	|VRAM (320 x 200 x 8bit カラー)|
|			|`0x000c_0000`|`0x000c_7fff`|`0x0000_8000` (32[KB])		|ビデオ BIOS|
|			|`0x000c_8000`|`0x000e_ffff`|`0x0002_8000` (160[KB])	|拡張 BIOS|
|			|`0x000f_0000`|`0x000f_ffff`|`0x0001_0000` (64[KB])		|マザーボード BIOS|
|`DSKCAC`	|`0x0010_0000`|`0x0026_7fff`|`0x0016_8000` (1440[KB])	|ディスクキャッシュ (FDの大きさ分)|
|`ADR_IDT`	|`0x0026_8000`|`0x0027_ffff`|`0x0000_0800` (2[KB])		|IDT|
|`ADR_GDT`	|`0x0027_0000`|`0x0027_ffff`|`0x0001_0000` (64[KB])		|GDT|
|`BOTPAK`	|`0x0028_0000`|`0x002f_ffff`|`0x0008_0000` (512[KB])	|OS 本体 (bootpack.hrb)|
|			|`0x0030_0000`|`0x0030_ffff`|`0x0001_0000` (64[KB])		|スタック|
|			|`0x0031_0000`|`0x003b_ffff`|`0x000b_0000` (704[KB])	|空き領域|
|			|`0x003c_0000`|`0x003c_7fff`|`0x0000_8000` (32[KB])		|メモリマネージャ|
|			|`0x003c_8000`|`0x003f_ffff`|`0x0003_8000` (224[KB])	|空き領域|
|			|`0x0040_0000`|				|							|空き領域|
|`[VRAM]`	|`0xfd00_0000`|`0xfdff_ffff`|`0x0100_0000` (16[MB])		|VRAM (VESA BIOS Extension)|


### GDT / IDT
#### GDT (Global Descritor Table)
- セグメントディスクリプタを保持するテーブル
- 最大エントリ数は 8192 個 (先頭エントリは 0 で埋めているため、実際は 8191 個)
- ディスクリプタの種類
	- Code Segment Descriptor
	- Data Segment Descriptor
	- Local Descriptor Table (LDT) Segment Descriptor
	- Task-State Segment (TSS) Descriptor
	- Call-Gate Descriptor
	- Interrupt-Gate Descriptor
	- Trap-Gate Descriptor
	- Task-Gate Descriptor
- 構成要素
	- Segment Limit: セグメントサイズ
	- Base Address: セグメントの開始アドレス
	- Type: セグメントタイプ
		- データセグメント (最上位ビットが `0`)
			- `EWA`: E (Expand-Direction), W (Write-Enable), A (Accessed)
			- `000`: Read-Only
			- `001`: Read-Only, Accessed
			- `010`: Read/Write
			- `011`: Read/Write, Accessed
			- `100`: Read-Only, Expand-Down
			- `101`: Read-Only, Expand-Down, Accessed
			- `110`: Read/Write, Expand-Down
			- `111`: Read/Write, Expand-Down, Accessed
		- コードセグメント (最上位ビットが `1`)
			- `CRA`: C (Conforming), R (Read Enable), A (Accessed)
			- `000`: Execute-Only
			- `001`: Execute-Only, Accessed
			- `010`: Execute/Read
			- `011`: Execute/Read, Accessed
			- `100`: Execute-Only, Conforming
			- `101`: Execute-Only, Comforming, Accessed
			- `110`: Execute/Read, Conforming
			- `111`: Execute/Read, Conforming, Accessed
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
	- S (Descriptor Type): ディスクリプタタイプ
		- `0`: システムセグメント
		- `1`: コードセグメントまたはデータセグメント
	- DPL (Descriptor Privilege Level): セグメントの特権レベル
	- P (Segment-Present): セグメント存在
		- `0`: セグメントがメモリ内にない
		- `1`: セグメントがメモリ内にある
	- AVL (Available): システムソフトウェアが自由に利用できる
	- D/B
		- `0`: 
		- `1`: 
	- G (Granularity): セグメントリミットの単位
		- `0`: 1[B] 単位
		- `1`: 4[KB] 単位
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

#### IDT (Interrupt Descriptor Table)
- 最大エントリ数は 256 個
- ディスクリプタの種類
	- Task Gate Descriptor
	- Interrupt Gate Descriptor
	- Trap Gate Descriptor


### GDTR / IDTR
- GDT と IDT は、ともに CPU にとっては非常に大きなデータであるため、メモリ上に保管される。
- どこに GDT と IDT を設定したかを記憶するために、CPU 内の GDTR と IDTR を設定する必要がある。

#### GDTR 
- `LGDT` 命令で設定できる。
- 構成要素
	- Table Limit (16[bit]): GDT のエントリ数
	- Linear Base Address (32[bit]): GDT が保管されてるリニアアドレス
```
 47                 16 15　　　　　  0
+---------------------+-------------+
| Linear Base Address | Table Limit |
+---------------------+-------------+
```

#### IDTR
- `LIDT` 命令で設定できる。
- 構成要素
	- Table Limit (16[bit]): IDT のエントリ数
	- Linear Base Address (32[bit]): IDT が保管されてるリニアアドレス
```
 47                 16 15　　　　　  0
+---------------------+-------------+
| Linear Base Address | Table Limit |
+---------------------+-------------+
```


### セクメントレジスタ
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
```
+----------------------+----------------------+
| I/O Map Base Address |       Reserved       |
+---------------------------------------------+
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