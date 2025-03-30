# inas sensor sense

A simple sensor device that can be used to measure temperature, tds, and more. This device is based on the INAS sensor and the ESP32 microcontroller.

## Features

- Measure voltage
- Measure temperature
- Measure TDS (Total Dissolved Solids)

## Hardware

- ESP32S3 Sense

## Setup

1. Clone this repository
2. copy default.env.user.ini to .env.user.ini
   And change the values to match your setup
3. And you can use Makefile to build and upload the firmware

```bash
make build
make upload
```

See more details in the [Makefile](Makefile) or `make help` command. 

## Demo

Soon...

<!-- ![Demo](demo.gif) -->

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
