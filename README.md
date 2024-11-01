# PN532

A Particle library for the PN532 v3 Module by Hiletgo.


Most of the original code in this repo was sourced from [this repo](https://github.com/elechouse/PN532/tree/PN532_HSU).
The oridinal author created it as an Arduino library, so I adjusted the source to work in the Particle enviornment.


All testing was done on a Photon 2 device. I was able to read and write to an NFC tag while communicating with the PN532 over I2C.


## Usage

```
// TODO: Add example snippet
```

See the [examples](examples) folder for more details.


## Wiring

When using Particle's `Wire` for the I2C interface, wire it like so:

| PN532 Module | Photon 2 |
| -------- | ------- |
| GND | GND |
| VCC | 3V3 |
| SDA | D0 |
| SCL | D1 |

***Note: make sure the jumper on the PN532 board is configured for I2C and you add the 4.7kΩ pullup resistors to SDA and SCL if they aren't on your module's board.***

## LICENSE
Copyright 2024 Jackson Horton

Licensed under the MIT license
