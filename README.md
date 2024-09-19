# 赤外線光で物体の有無を検出をしよう
## 始めに
このリポジトリは2024/9/21,22に開催されたMaker Faire Tokyo 2024で展示されたロボットで実装している機能をより詳しく理解できるよう、**補足情報の位置付け**で作成したものです。
「フォトリフレクタ」というセンサを使って、**「物体の有無」を検出するやり方**を紹介します。

1. フォトリフレクタを使う上で必要となる、LEDの点灯方法やセンサ値の取得方法をマイコンボードを動かし実験することで理解を深めます。
2. フォトリフレクタの設定を変えたり、光の環境を変えるなどして、センサ値へどのような影響を与えるのかを実験で学べます。
3. 環境が変わってもセンサ値を安定して取得できるようにする方法を紹介します。

![test_circuit_overview](/image/test_circuit_image_00.jpg)

フォトリフレクタは赤外線などを発光し、その反射光を受け取ることで物体の有無などを観測できるセンサです。光を使っているため、物理スイッチのように触れる必要なく、非接触で物体を検出できます。

例えば、車輪ロボットに搭載すれば、進行方向に壁があるかどうか、壁があるときロボットから壁までの距離がどれくらいあるかを非接触で知ることができ、壁にぶつからず移動できるようになります。

この記事の対象者は電子工作でArduino, M5STACKを使っている初級レベルの人を想定しています。
Arduino, M5STACKは手軽に電子工作が楽しめるように作られており、インターネット上に使い方に関する情報も多いため、これらのマイコンボードを使ったことがない人でも、やりたいことを検索し、問題解決できる方であれば十分扱えると思います。

回路はブレッドボードでジャンパ線を使い製作します。はんだづけは行いません。

ソフトはマイコンボードに制御プログラムを書き込みます。

### 注意
前述したように、このリポジトリは2024/9/21,22に開催されたMaker Faire Tokyo 2024で展示されたロボットで実装している機能をより詳しく理解できるよう、**補足情報の位置付け**で作成したものです。疑問点など出来る限り自己解決頂きたく思います。

このリポジトリの内容によって生じた損害やけがに対して、一切の責任を負いません。実験に不安のある方は本リポジトリの記載を読むにとどめる、もしくは電子工作に精通している方の協力を得てください。

また、本リポジトリは予告なく変更、削除する可能性があります。ご留意ください。

## 事前準備
### 1.準備するもの

実験に使用する電子部品、その他必要なものを記載します。
ブレッドボードやマイコンボードなどの、その他必要なものはご自身で使いやすいものを準備ください。

ただし以降の説明では、マイコンボードはM5STACKのATOMS3 Liteを使用する前提で記載します。
類似製品にATOMS3（サイズ違い。ディスプレイやIMU付き）があり、同様のピンの割り当てになっているため以下の実験は行えます。
ATOMS3 Liteよりも価格が高いですが、センサ値をディスプレイに表示するなど応用もできるでしょう。

■ 電子部品

| No. | 部品名 | 型番等 | 個数 | 備考 | URL |
| --- | --- | --- | --- | --- | --- |
| 1 | フォトリフレクタ | LBR-127HLD | 1 | センサ(D1+Q1) | |
| 2-1 | 抵抗 | 51Ω | 1 | LED電流制限(R1 or R3) | |
| 2-2 | 抵抗 | 75Ω | 1 | LED電流制限(R1 or R3) | |
| 2-3 | 抵抗 | 100Ω | 1 | LED電流制限(R1 or R3) | |
| 3-1 | 抵抗 | 3kΩ | 1 | フォトトランジスタ感度調整(R2) | |
| 3-2 | 抵抗 | 18kΩ | 1 | フォトトランジスタ感度調整(R2) | |
| 3-3 | 抵抗 | 33kΩ | 1 | フォトトランジスタ感度調整(R2) | |
| 4 | LED | GREEN | 1 | 点灯確認、感度実験用(D2) | |

■ その他必要なもの

| No. | 部品名 | 型番等 | 個数 | 備考 | URL |
| --- | --- | --- | --- | --- | --- |
| 1 | ブレッドボード | - | 1 | - | - |
| 2 | ジャンパ線 | - | 約10本 | ブレッドボード上の配線 | |
| 3 | マイコンボード | ATOMS3 lite | 1 | - | https://docs.m5stack.com/en/core/AtomS3%20Lite |
| 4 | USBケーブル | - | 1 | AtomS3 lite書き込み | |
| 5 | PC | - | 1 | Arduino IDEでのソフト開発 | |


※フォトリフレクタの実験では使っていませんが、応用例の中で下記の部品を使用しています。参考に記載します。

| No. | 部品名 | 型番等 | 個数 | 備考 | URL |
| --- | --- | --- | --- | --- | --- |
| 1 | サーボモータ | SG92R | 1 | - | - |
| 2 | ピンヘッダ | - | 1 | 3ピン | - |


### 2.PCにArduino IDEをインストール

PCのセットアップをします。今回使用するM5STACKのウェブサイトにArduino IDEのインストール方法, M5Stackのマイコンボードを使用するセットアップ方法が記載されています。

https://docs.m5stack.com/en/arduino/arduino_ide

今回マイコンボードはATOMS3 liteを使用する前提のため、以下の項目では「M5AtomS3」を選択してください。

    2.Installing Arduino Board Management
    3.Installing Arduino Libraries

#### <参考情報>
Arduino IDEでプログラムをATOMS3へ書き込むことを行いますが、少し時間がかかります（2,3分等。PCのスペックによる可能性もあります）

少し発展的ではありますが、開発環境をVisual Studio Code（略VSC）とPlatformIOにすると比較的早く書き込みができるようになります。

VSCのインストールやPlatformIO、マイコンボードの設定などを行う必要がありますので、インターネットで検索しやり方がわかるようであれば使うのもよいでしょう。
その場合、本リポジトリのプログラムはArduino IDEを使う前提になっていますのでソースコードの中身をmainにコピーするなどが必要です。

### 3.ブレッドボードに回路を作成

回路図、写真を参考にブレッドボードに回路を組みます。

この回路はUSBでPCと接続されたM5ATOMS3 Liteから電源を供給して動作します。
ジャンパ線で回路図、画像を参考に配線してみてください。

AtomS3 liteの接続は、ジャンパ線を直接端子の穴に挿す。
もしくはピンヘッダをお持ちであれば、ブレッドボードに写真のように取り付けられます。

フォトリフレクタの赤外線LED(D1)、可視光LED(D2)の抵抗は、部品リストのNo.2-*から選びます。
ここでは「D1につながるR1にNo.2-1の51Ω」、「D2につながるR3にNo.2-3の100Ω」をつけます。

フォトリフレクタのフォトトランジスタ(Q1)の抵抗は、部品リストのNo.3-*から選びます。
ここでは「Q1につながるR2にNo.3-3の33kΩ」をつけます。

・[回路図（フォトリフレクタ実験）](/schematic/reflective_sensor_circuit.pdf)

・ジャンパ線でAtomS3 Liteを配線したイメージ
![test_circuit_w_jumper](/image/test_circuit_image_01.jpg)

・ピンヘッダでAtomS3 Liteを配線したイメージ
![test_circuit_w_pinheader_0](/image/test_circuit_image_02.jpg)
![test_circuit_w_pinheader_1](/image/test_circuit_image_03.jpg)


### 4.実験のプログラムについて
実験は次のようにカテゴリーを分けています。プログラムのファイル名とカテゴリの対応を確認ください。
プログラム名の末尾に記載された" * " には、番号が入ります。

| カテゴリ | プログラム名 | 備考 |
| --- | --- | --- |
| 基礎 | basic_*.ino | 基本的な動作を見る |
| 発展 | practice_*.ino | 対象が変わったりしたときの影響を見る |
| 応用 | application_*.ino | フォトリフレクタを使った応用例 |

プログラムの中身についてはあまり触れませんが、コードとしてはArduino, M5STACKの基本的なコマンドを使用して作成していますので、インターネットで検索すると多々の情報を得られます。

## 基礎 1：LEDを点灯させる
LEDの点灯をさせる実験です。プログラムはbasic_1.inoを使います。

点灯させるのは、回路図上のD2（色付きのLED）になります。
フォトリフレクタの赤外線LEDを点灯させてもよいのですが、人間は目で赤外線を認識することができないので、可視光のLEDを使って点灯しているかを確認します。

プログラムでは点灯と消灯を繰り返す動作を確認できます。

## 基礎 2：フォトトランジスタで光を検出する
フォトトランジスタで光を検出する実験です。プログラムはbasic_2.inoを使います。

フォトリフレクタの赤外線LEDを基礎1のように点灯、消灯させています。この時のセンサ値をArduino IDEのシリアルモニタに出力させています。
（シリアルモニタはArduino IDEのメニューバーのツールタブの中にあります。）

LEDが点灯しているときに、フォトリフレクタの手前に手（物体）を近づけてみるとセンサ値が大きくなることを確認できます。

LEDから発せられた光が物体に当たり、反射光をフォトトランジスタが受け、電流が流れることで抵抗に電圧が発生します。この発生した電圧をマイコンボードでAD変換して、電圧の大きさをセンサ値としています。ATOMS3はAD変換の基準電圧が3.3Vです。AD変換の分解能12bit（4096）の最大値が3.3Vになります。

一方、LEDが消灯しているときは、センサ値がほぼ変化していないことを確認できます。これはLEDが消灯している（そもそも光を発していない）タイミングのため、反射光がなく、フォトトランジスタは電流を流していないため、抵抗に電圧がほぼ発生せず0V付近にいるため、センサ値が低いままとなります。

また、フォトリフレクタの手前に近づけている手（物体）の位置を徐々に動かしてみましょう。フォトリフレクタに近づけるとセンサ値が上がり、遠ざけるとセンサ値が下がることを確認できます。

これは感覚的にも理解できると思いますが、遠くになるほど光は広がり暗くなり、近くになるほど光る箇所が限定され明るくなるためです。フォトトランジスタが受ける光の大きさが変わり、センサ値が変化します。

## 基礎 3：LEDの明るさを変えてみる
フォトリフレクタの赤外線LED(D1)に繋がった抵抗(R1)の大きさを変えることで、LEDの明るさを変えることができます。
プログラムは基礎2と同じbasic_2.inoを使います。

ブレッドボードについているLEDの抵抗を、他の抵抗（抵抗値が異なるもの）に変え、基礎2でやったようにフォトリフレクタの上に物体を近づけてみると、センサ値の大きさが変化したことを確認できます。

これはLEDが発する光の強さが変化し、フォトトランジスタが受け取る反射光の強さも変化したためです。このようにLEDの光の強さが変化するとセンサ値の値も変化します。

## 基礎 4：フォトトランジスタの感度を変えてみる
フォトリフレクタのフォトトランジスタ(Q1)に繋がった抵抗(R2)を変えることで、センサ値の感度を変えることができます。
プログラムは基礎2と同じbasic_2.inoを使います。

ブレッドボードについているフォトトランジスタの抵抗を、他の抵抗（抵抗値が異なるもの）に変え、基礎2でやったようにフォトリフレクタの上に物体を近づけてみると、センサ値の大きさが変化したことを確認できます。

これはフォトトランジスタが光を受け取り電流を流しますが、抵抗に流れる電流対して発生する電圧の大きさが変化するためです。オームの法則から理解できると思います。
（ex. 1mAの電流が流れているときに、抵抗値が1kΩである場合は1V、抵抗値が2kΩである場合は2Vが発生します）

## 基礎 5：光を反射させる物体によるセンサ値の影響をみる
身の回りにあるもので「白いもの、黒いもの」を準備して下さい。
プログラムは基礎2と同じbasic_2.inoを使います。

フォトリフレクタの前に近づけてみると、白いものの方がセンサ値が大きいことを確認できると思います。これはお分かりのように白色の方が光を反射しやすいためです。

このようにセンサ値は反射させる物体の状態にも影響を受けてしまうということも覚えておきましょう。

## 発展 1：光の色（波長）によるセンサ値の影響をみる
LEDが発光する光の色によるセンサ値の影響を確認します。
プログラムはpractice_1.inoを使います。

今回使用しているフォトリフレクタは赤外線に反応するように作られてます。具体的には光の波長940nmを受け取りやすく設計されています。

基礎2で確認したように赤外線LEDが消灯した状態（赤外線が出ていない環境）ではセンサ値が大きくなりませんでした。
それでは基礎1で使用した可視光LEDが光るとセンサ値はどうなるでしょうか。

ATOMS3のボタンを押し続けると、シリアルモニタに「Pressed button. LED changed.」と表示され、発光するLEDを変更できるようにしています。
現在光っているLEDが何かは、センサ値を見ているシリアルモニタ上に表示されています。

このソースコートではコードの動きをわかりやすくするため、センサ値の取得時にdelay関数を使い動きを止めています。そのためボタンを押した判定がタイミングによっては取得できないことがあるため、ボタンを長押しすることで対処しています。

可視光のLEDに変更しセンサ値を観察してみるとほとんど値は変わらないことを確認できると思います。

このようにこのリフレクタに搭載されているフォトトランジスタは赤外線940nmをピークに持っているため、可視光のLEDではほとんどセンサ値が変化しないことがわかります。

さて、次に以下に記載したような光をフォトリフレクタを近づけてみて実験してみると、センサ値が変わったり、変わらなかったりすることを確認できると思います。

* 家の中の照明（蛍光灯、白熱球、スタンドライト、懐中電灯等）
* 太陽光

家の中の照明でセンサ値に大きな値が出たものがあれば、赤外線を出している照明です。
ものによりますが、赤外線はLED照明では少なく、白熱球では多いです。

また太陽光は様々な波長の光を含んでいます。地上に届く光には地球の大気の影響で特定の波長が少なくなっていますが、可視光以外の波長の光も多く降り注いでいます。

実は今回使用しているフォトリフレクタの940nmは地球の大気で大きく減衰する波長になっています。これは野外で使ったときも太陽光の影響を受けにくくするための工夫です。
赤外線のフォトセンサには可視光に近い800nm台のセンサもあるため、素子選定する際は感度がどこにあるかを確認しましょう。

確認したように、人間の目には同じく白色に見えるかもしれませんが、場所によっては見えていない波長を多く出している環境があります。
このようにフォトトランジスタに入る光の色（波長）の違い、環境光の違いによってセンサ値が変化します。

## 発展 2：環境光（外乱光）によるセンサ値の影響を低減する
発展2では環境光による影響を低減させ、センサ値を安定させる方法を紹介します。
プログラムはpractice_2.inoを使います。

発展1で環境光によってセンサ値が変化することを確認しました。
これではフォトリフレクタを搭載したロボットを運用したいとき、場所や時間帯（太陽光が入る・入らない）によってセンサ値が変化してしまい、想定外の動作をしてしまう可能性があります。これでは安定した動作を行うことができません。対策が必要です。

フォトリフレクタのデータシートを確認してみましょう（ネットでLBR-127HLDと検索してください）。

データシートの中で様々なグラフが書かれているページの中央あたりに「Collector current vs Forward current」というものがあります。
これからForward current（LEDの電流値）とCollector current（フォトトランジスタの電流値）の関係がほぼ線形で変化する特性があることを確認できます。
これはLEDの電流値は光の明るさ、フォトトランジスタの電流値は抵抗を介してセンサ値となると考えられるため、センサ値は明るさによって線形に変化すると理解できます。

ここで「環境光によるセンサ値」と「LEDが点灯した時のセンサ値」を考えてみましょう。

「環境光によるセンサ値」は「環境光」がフォトトランジスタに入ることで得られたものです。
一方、「LEDが点灯した時のセンサ値」はどのような光がフォトトランジスタに入った時のものでしょうか？
それは「LEDによる発せられた光」+「環境光」の2つがフォトトランジスタに入ったものです。

センサ値は光の明るさに対して線形に反応することを考えると、「LEDが点灯しているとき」と「LEDが点灯していないとき」のセンサ値の差をとれば、環境光による影響を排除できることがわかります。

このプログラムでは、3種類の値をシリアルモニタに表示させています。

1. LEDが点灯しているときのセンサ値（環境光 + LED光）
2. LEDが点灯していないときのセンサ値（環境光のみ）
3. 1から2を引いた値（環境光を排除したセンサ値 ≒ LED光のみ）

赤外線を出している照明を光らせてみると、1,2のセンサ値の上昇に対し、3のセンサ値の上昇は小さく環境光が変わっても変化が少ないことを確認できると思います。

このようにLEDの点灯有無のセンサ値をそれぞれ検出し、差分をとることで環境光が変化しても安定したセンサ値を得ることができ、誤作動を減らすことができます。

## 応用例 1：非接触スイッチ機能
今回紹介したフォトリフレクタを使って簡単な機能を作ってみたいと思います。
プログラムはapplication_1.inoを使い、上記で実験に使った回路をそのまま使用します。

ここではフォトリフレクタの上に手かざし、ある距離まで近づいたらLEDが点灯する機能を作ります。
センサ値がある閾値を超えたら、ブレッドボードの可視光LEDを点灯させることにしました。

またATOMS3 Liteには3色LEDが搭載されていますので、一緒に点灯させてみましょう。
ATOMS3 Lite内のLEDは、FastLEDというライブラリを使い点灯させることができます。
プログラム内ではブレッドボードの可視光LEDと同じタイミングで青色に光るようにしています。

このようにセンサが検出した値を監視し、センサ値によって動きを変えられます。
このプログラムではLEDの点灯・消灯を制御しましたが、もちろん他の部品を動かすようなプログラミングをすれば、モータの速度や角度を変えるなどより複雑な動きをさせることができます。

## 応用例 2：センサ値を使ったサーボモータの角度制御
実験に使用した回路にサーボモータ（角度を決められるモータ：SG92R）を追加して、フォトリフレクタが検出したセンサ値を使い、サーボモータの角度を変える機能を作ろうと思います。

フォトリフレクタに手を近づけていくにあわせてサーボモータの角度を変化させるようにします。

プログラムはapplication_2.inoを使います。

回路にはサーボモータを追加します。回路図、以下の画像を参考してください。

・[回路図（フォトリフレクタ実験回路にサーボを追加）](/schematic/reflective_sensor_circuit_with_servo.pdf)

・ブレッドボードの回路にサーボモータを取り付けたイメージ
![application_w_servo](/image/test_circuit_w_servo.jpg)

## 応用例 3：接近アピール機能
実験に使用した回路にサーボモータ（角度を決められるモータ：SG92R）を追加して、フォトリフレクタが検出したセンサ値を使い、サーボモータの速度を変える機能を作ろうと思います。

サーボモータは手を振るようにある角度を行ったり来たりする動きをしています。
フォトリフレクタのセンサ値をによって速度を変更します。
フォトリフレクタに物体を近づけセンサ値が大きくなると素早く動き、接近をアピールするようにしました。

プログラムはapplication_3.inoを使います。

## 最後に
本リポジトリではフォトリフレクタの使い方やセンサ値へ影響を与えることなどを紹介をしました。さらにセンサ値を使った簡単な機能を作成しました。
フォトリフレクタを使えば非接触で物体の有無・距離の検出ができます。
Maker Faire Tokyoで展示していた移動ロボットのように壁から一定の距離を保ち走行したり、壁の有無を判断して知的に迷路を解くなどに応用できます。

今回は赤外線を用いたフォトトランジスタを使い物体の有無・距離を検出しましたが、距離を測定するデバイスはToFセンサや超音波センサなど、フォトリフレクタ以外もあります。
フォトトランジスタでは～10cmあたりで検出ができましたが、ToFを使うと数mなどの距離を測定することができたりします。しかし検出速度はフォトリフレクタの方が速いなど特徴があります。作りたいものに必要な特性を把握して、目的にあったセンサを選び活用しましょう。

以上

Rofumi

This repository is licensed under the Apache License, Version2.0


