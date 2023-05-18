# esp32_lichtwecker

Lichtwecker mit WebServer

## Hardware
- ESP 32 AZ Delivery DevKit v4
- Nextion Display NX4832K035_011
- LED SK6812 RGBW (neutral)
- MAX98357 I2S Amp

## Wireing
|Modul|GPIO|Modul|Port
| ------------- | ------------- | ------------- | ------------- |
|ESP32|12|MAX|BCLK
|ESP32|22|MAX|DIN
|ESP32|23|MAX|LRCLK
|ESP32|27|LED|DAT
|ESP32|16|NEXTION|TX
|ESP32|17|NEXTION|RX

|MAX:|
| ------------- |
|SD_MODE left auf +5V|
|SD_MODE right über R 39k auf +5V|

## Software
- Software
- SimpleTimer
- ESP 32 Digital RGB LED Drivers
- Audio-Tools
- LibHelix
- WifiManager

## Bild
![plot](./doc/IMG20230518091455.jpg)
