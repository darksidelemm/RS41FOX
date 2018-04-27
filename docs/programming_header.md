# Vaisala RS41 Expansion Connector Pinout
Based on work by DF8OE


Viewed into port from outside
```
----------|     |----------
|  9    7    5    3    1  |
|                         |
|  10   8    6    4    2  |
---------------------------
```

Suitable Plug: Molex 87568-1073  (element14 order code 1365756)

* 1 - GND
* 2 - I2C_SDA (PB11)
* 3 - I2C_SCL (PB10)
* 4 - +VDD_MCU
* 5 - MCU Switched 3.3V out to external device
* 6 - +V_Battery
* 7 - RST
* 8 - SWCLK (PA14)
* 9 - SWDIO (PA13)
* 10 - GND


