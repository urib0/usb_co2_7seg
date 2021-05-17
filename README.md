# usb_co2_7seg

![usb_co2](usb_co2_7seg.jpg)

Mac, Linux マシンに USB で接続できて追加ドライバ不要で値を読める CO2 と 7 セグ表示器

## 使い方(macOS)

まず、USB 機器をすべてコンピュータから取り外す。

次に、usb_co2 を USB ポートに接続する。

次に、コンピュータに接続されているシリアルポートの一覧を表示する。

```
$ ls -1 /dev/cu.*
/dev/cu.Bluetooth-Incoming-Port
/dev/cu.iMiminashi5s-WirelessiAP
/dev/cu.usbmodem14101
```

上記の例の場合、`cu.usbmodem14101` というデバイスファイルが usb_co2 を示す。

次に、`stty`コマンドを実行してシリアルポートの設定を行う。

```sh
stty -f /dev/cu.usbmodem14101 raw 9600
```

最後に、`cat`コマンドでデバイスファイルを読み込むと CO2 の値(ppm)が表示される。

```
$ cat /dev/cu.usbmodem14101
co2=399;status=0
co2=399;status=0
co2=399;status=0
co2=399;status=0
co2=399;status=0
co2=399;status=0
```

## 使い方(Linux)

まず、USB 機器をすべてコンピュータから取り外す。

次に、usb_co2 を USB ポートに接続する。

次に、コンピュータに接続されているシリアルポートの一覧を表示する。

```
$ ls -1 /dev/tty*
/dev/tty
.
. (中略)
.
/dev/ttyACM0
/dev/ttyAMA0
/dev/ttyprintk
/dev/ttyUSB0
```

上記の例の場合、`/dev/ttyACM0` というデバイスファイルが usb_co2 を示す。

次に、`stty`コマンドを実行してシリアルポートの設定を行う。

```sh
stty -F /dev/ttyACM0 raw 9600
```

最後に、`cat`コマンドでデバイスファイルを読み込むと CO2 の値(ppm)が表示される。

```
$ cat /dev/ttyACM0
co2=399;status=0
co2=399;status=0
co2=399;status=0
co2=399;status=0
co2=399;status=0
```
