# ParticlePN532

A Particle library port for the PN532 v3 Module by Elechouse.


Most of the original code in this repo was ported from [elechouse's official PN532 repo](https://github.com/elechouse/PN532/tree/master).
The original author created it as an Arduino library, so I adjusted the source to work in the Particle enviornment.


All testing was done on a Photon 2.


## Usage

This snippet shows the basics of setting up the necessary objects to read from the PN532 chip.
You should use Particle's `Wire`(I2C) interface unless you know what you're doing.
```
// Initialize I2C interface
PN532_I2C pn532_i2c(Wire);
ParticlePN532 nfc(pn532_i2c);

void setup() {
    nfc.begin();
}
```

For more specific examples, see the [examples](examples) folder.


## Installation

You can install the `ParticlePN532` library using the Particle CLI or in the Web IDE.


In the Web IDE, from the libraries tab, search `ParticlePN532` in the community libraries section and install the latest version.


## NDEF

If you want to use NDEF records, additionally use the `ParticleNDEF` library ([found here](https://github.com/jacksonhorton/ParticleNDEF)).

## LICENSE
[MIT License](https://github.com/jacksonhorton/ParticlePN532/blob/master/LICENSE) (c) 2024, Jackson Horton
