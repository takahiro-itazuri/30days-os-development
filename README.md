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



## フォルトファイル
[『30日でできる！OS自作入門』のメモ](https://vanya.jp.net/os/haribote.html) より変換ずみの hankaku.c を使用しました。



## sprintf
[sprintfを実装する | OS自作入門 5日目-2 【Linux】 | サラリーマンがハッカーを真剣に目指す](http://bttb.s1.valueserver.jp/wordpress/blog/2017/12/17/makeos-5-2/) より sprintf 関数の実装を使用しました。
（少しだけ変更を加えています。）



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
- [【学習メモ#3rd】12ステップで作る組込みOS自作入門](https://www.slideshare.net/sandai/312os)
