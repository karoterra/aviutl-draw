# aviutl-draw

[AviUtl](http://spring-fragrance.mints.ne.jp/aviutl/)
拡張編集スクリプトで仮想バッファの代替として使える DLL です。

## 使い方

```lua
local KD = require("KaroterraDraw")

-- バッファをクリアする
-- KD.clear(obj.screen_w, obj.screen_h)
-- 画像データをセットしてバッファを準備する
KD.setimage(obj.getpixeldata())

-- 描画オプションを設定する
KD.setcomposite(3)
KD.setblend(0)
KD.setinterpolate(1)

-- 描画する
obj.load("figure", "円", 0xffffff, 100)
local data, w, h = obj.getpixeldata()
KD.draw(data, w, h)

-- 描画結果を取得する
data,w,h = KD.getimage()
obj.setoption("drawtarget", "tempbuffer", w, h)
obj.load("tempbuffer")
obj.putpixeldata(data)
obj.setoption("drawtarget", "framebuffer")
```

## 関数

### `clear(w, h)`
DLL内で保持しているバッファを透明な画像として初期化する。
`obj.setoption("drawtarget", "tempbuffer", w, h)` 相当
- 引数
  - w: 幅
  - h: 高さ
- 戻り値: なし

### `setimage(data, w, h)`
DLL内で保持しているバッファに画像を送る。
この画像でバッファが初期化される。
- 引数
  - data: 画像データ
  - w: 幅
  - h: 高さ
- 戻り値: なし

### `getimage()`
DLL内で保持しているバッファから画像を取得する。
- 戻り値
  - 戻り値1: 画像データ
  - 戻り値2: 幅
  - 戻り値3: 高さ

### `setcomposite(value)`
アルファチャンネルの計算方法を指定する。
- 引数
  - value: コンポジットの種類
- 戻り値: なし

| value | コンポジットの種類    |
|------:|:----------------------|
|     0 | Clear                 |
|     1 | Copy                  |
|     2 | Destination           |
|     3 | Source Over (default) |
|     4 | Destination Over      |
|     5 | Source In             |
|     6 | Destination In        |
|     7 | Source Out            |
|     8 | Destination Out       |
|     9 | Source Atop           |
|    10 | Destination Atop      |
|    11 | XOR                   |
|    12 | Lighter               |

### `setblend(value)`
ブレンドモードを指定する。
- 引数
  - value: ブレンドモード
- 戻り値: なし

| value | ブレンドモード |
|------:|:---------------|
|     0 | 通常 (default) |
|     1 | 加算           |
|     2 | 減算           |
|     3 | 乗算           |
|     4 | スクリーン     |
|     5 | オーバーレイ   |
|     6 | 比較(明)       |
|     7 | 比較(暗)       |
|     8 | 輝度           |
|     9 | 色差           |
|    10 | 陰影           |
|    11 | 明暗           |
|    12 | 差分           |

### `setinterpolate(value)`
画像の補完方法を指定する。
- 引数
  - value: 補完方法
- 戻り値: なし

| value | 補完方法           |
|------:|:-------------------|
|     0 | Nearest Neighbor   |
|     1 | Bilinear (default) |

### `draw(data,w,h [,ox,oy,zoom,alpha,rotate])`
DLL内で保持しているバッファに画像を描画する。
- 引数
  - data: 画像データ
  - w: 幅
  - h: 高さ
  - ox: x座標(省略時は0)
  - oy: y座標(省略時は0)
  - zoom: 拡大率(省略時は1)
  - alpha: 不透明度(省略時は1)
  - rotate: 回転(省略時は0)
- 戻り値: なし

### `drawperspective(data,w,h, x0,y0,x1,y1,x2,y2,x3,y3, u0,v0,u1,v1,u2,v2,u3,v3,alpha)`
DLL内で保持しているバッファに画像を射影変換して描画する。
- 引数
  - data: 画像データ
  - w: 幅
  - h: 高さ
  - x0,y0, x1,y1, x2,y2, x3,y3: 射影元の座標
  - u0,v0, u1,v1, u2,v2, u3,v3: 射影先の座標
  - alpha: 不透明度(省略時は1)
- 戻り値: なし

## ライセンス

このソフトウェアは MIT ライセンスのもとで公開されます。
詳細は [LICENSE](LICENSE) を参照してください。

使用したライブラリ等については [CREDITS](CREDITS.md) を参照してください。
