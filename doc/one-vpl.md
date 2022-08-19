# oneVPL (oneAPI Video Processing Library) への対応について

Sora C++ SDK は [Intel Media SDK](https://github.com/Intel-Media-SDK/MediaSDK#intel-media-sdk) の後継である [oneVPL (oneAPI Video Processing Library)](https://github.com/oneapi-src/oneVPL) に対応しています。

oneVPL は Intel Media SDK を利用する環境でも動作します。oneVPL のディスパッチャーにより、適切に Intel Media SDK / oneVPL が使用されます。
詳細については [OneVPL のページ](https://github.com/oneapi-src/oneVPL#onevpl-architecture) をご確認ください。

## 対応プラットフォーム

- Windows 10 x86_64
- Ubuntu 20.04 x86_64 : libmfx1 (MediaSDK) のみ対応 (*)
- Ubuntu 22.04 x86_64 : libmfx1 (MediaSDK) または libmfx-gen1.2 (oneVPL-intel-gpu) に対応

(*) Ubuntu 20.04 x86_64 は libmfx-gen1.2 が apt で利用できないため対象外としています。

## 動作確認ができた環境

### Windows

- GPD WIN3 (Core i7-1195G7)

### Ubuntu 20.04 x86_64

- GPD WIN3 (Core i7-1195G7)
    - libmfx-gen1 を使用

### Ubuntu 22.04 x86_64

- GPD WIN3 (Core i7-1195G7)
    - libmfx-gen.1.2 を使用
- DELL XPS 15 (Core i9-9980HK)
    - libmfx-gen1 を使用
