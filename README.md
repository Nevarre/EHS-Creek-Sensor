# EHS-Creek-Sensor
The UC Berkeley Environment, Healty, and Safety (EHS) team is working on a real-time water quality monitoring system to preserve the wildlife and landscape of Strawberry Creek. 

Table of contents
=================
<!--ts-->
   * [About](#about)
   * [Table of contents](#table-of-contents)
   * [Installation](#installation)
   * [Hardware](#hardware)
      * [Sensors](#sensors)
   * [Future Work](#future-work)
   * [Dependency](#dependency)
<!--te-->

## About

Data monitoring integrates hardware and software, our do-it-yourself water monitoring system uses the [Mayfly Data Logger](https://www.envirodiy.org/mayfly/). For more information on working with this board, we recommend the [Learn EnviroDIY Programming](https://envirodiy.github.io/LearnEnviroDIY/index.html) tutorial. Along with the tutorial, we also recommend using the [ModularSensors](https://github.com/EnviroDIY/ModularSensors) library, a "common interface of functions for use with Arduino-compatible dataloggers". 


## Installation

The original Arduino Integrated Development Environment (IDE) has poor library managment. We recommend using the [PlaformIO](https://platformio.org/install) IDE that supports Arduino library managment. Text-editors such as [Visual Studio Code](https://code.visualstudio.com/) or [Atom](https://atom.io/) supports PlatformIO. For more information on how to use and install these platforms, refer to the [EnvrioDIY tutorial](https://envirodiy.github.io/LearnEnviroDIY/04-GetPlatformIO/index.html).

Once you have installed [Git](https://git-scm.com/downloads) and PlatformIO, follow these instructions to setup the current foundations of our code. 

1. Create new project in your PlatformIO Home
2. Clone repository
3. Change `platform.ini` settings

Current `platform.ini` settings.

```
[platformio]
src_dir = EHS-Creek-Sensor/sensors_mayfly

[env:mayfly]
monitor_speed = 115200
board = mayfly
platform = atmelavr
framework = arduino
; any port that starts with /dev/cu.usbserial
; change this for your respective port
;/dev/ttyUSB0 - Serial port (Unix-based OS)
;COM3 - Serial port (Windows OS)
;192.168.0.13 - IP address when using OTA
;/media/disk - physical path to media disk/flash drive (mbed enabled boards)
;D: - physical path to media disk/flash drive (Windows OS).

upload_port = /dev/*
lib_ldf_mode = deep+
lib_ignore = RTCZero
    ;-DigiXBee
build_flags =
    -DSDI12_EXTERNAL_PCINT
    -DNEOSWSERIAL_EXTERNAL_PCINT
    -DMQTT_MAX_PACKET_SIZE=240
    -DTINY_GSM_RX_BUFFER=64
    -DTINY_GSM_YIELD_MS=2
    -DENABLE_SERIAL2
    -DENABLE_SERIAL3
lib_deps =
    EnviroDIY_ModularSensors@>=0.6.7
    SDFat
;  ^^ Use this when working from an official release of the library
;    https://github.com/EnviroDIY/ModularSensors.git#develop
;  ^^ Use this when if you want to pull from the develop branch
;    EnviroDIY_ModularSensors@=0.17.2
;  ^^ Use this when if you want to pull from a specific release
;    https://github.com/EnviroDIY/ModularSensors.git#a84934eebc1fadef8b372dc0251cb3b127c8f71a
;  ^^ Use this when if you want to pull from a specific commit by referencing the commit after the #
    https://github.com/PaulStoffregen/AltSoftSerial.git
;  libraries used for the tds sensor
    https://github.com/DFRobot/GravityTDS.git
;  libraries used for testing alternative RTC method from
;   https://www.envirodiy.org/mayfly/software/sleeping-mayfly-logger-example/
;   https://github.com/adafruit/SD.git
    https://github.com/SodaqMoja/RTCTimer.git
    https://github.com/SodaqMoja/Sodaq_PcInt.git
    https://github.com/greiman/SdFat.git

```


## Hardware

![mayfly](https://3qzcxr28gq9vutx8scdn91zq-wpengine.netdna-ssl.com/wp-content/uploads/2016/05/mayfly_topview_labels3.jpg)

We are using the [Mayfly Data Logger](https://www.envirodiy.org/mayfly/), a "powerful, user-programmable microprocessor board that is fully compatible with the Arduino IDE software". The data logger requires a MicroSD card and a CR1220 Lithium Coin Cell 3V Battery. Data-logging also requires using an external real time clock (RTC) because the main data logger takes sleep cycles to record data. Our [DS3231 RTC](https://www.maximintegrated.com/en/products/analog/real-time-clocks/DS3231.html) is supported by the ModularSensors library.

### Sensors
Water-quality monitoring requires analysis of various physical and chemical parameters such as temperature, turbidity, and conductivity. We are experimenting with various sensors for their functionality and accessiblity. Many of theses sensors are supported by the ModularSensors library.

- [HYDROS 21](https://www.metergroup.com/environment/products/hydros-21-water-level-monitoring/) (Conductiviy, temperature, depth)
- [Atlas Scientific K.01](https://www.atlas-scientific.com/probes/conductivity-probe-k-0-1/) (Conductivity)
- [DS18S20 Digital Thermometer](https://www.maximintegrated.com/en/products/sensors/DS18S20.html) (Temperature)
- [Gravity: Analog Turbidity Sensor](https://www.dfrobot.com/product-1394.html) (Turbidity)


## Furture Work
We hope to share the live water-quality data of Strawberry Creek with the [Monitor My Watershed](https://monitormywatershed.org/) community.

## Common Errors:
#### SCons.dblite.dblite object

```
Exception OSError: (2, 'No such file or directory', '/path/to/dir/.pio/build/.sconsign.dblite') in <bound method dblite.__del__ of <SCons.dblite.dblite object at 0x10bfe5610>> ign
ored

```

Try removing `.pio` folder from the projects folder. 
-> Go to terminal and `cd /path/to/project/folder `
-> Since this is a hidden folder use the command `ls -a -ltr` to see the `.pio` folder.
-> Remove by `rm -rf .pio`. Check it is removed `ls -a -ltr` command again.

Refer to this link for more information: 
[https://community.platformio.org/t/issues-after-vscode-install/8885](https://community.platformio.org/t/issues-after-vscode-install/8885)


#### avrdude: stk500_recv(): programmer is not responding
```
Configuring upload protocol...
AVAILABLE: arduino
CURRENT: upload_protocol = arduino
Looking for upload port...
Auto-detected: /dev/cu.usbserial-A1062BH2
Uploading .pio/build/mayfly/firmware.hex
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 2 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 3 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 4 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 5 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 6 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 7 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 8 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 9 of 10: not in sync: resp=0x00
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 10 of 10: not in sync: resp=0x00

avrdude done.  Thank you.
```

I have encountered this issue many times. It is generally a port connection issue between the computer and the device. Your usb cable might even be the cause.

From Arduino Stack Exchange: "This is caused by a generic connection error between your computer and the Arduino, and can result from many different specific problems." 

Refer here for more details: [https://arduino.stackexchange.com/questions/17/avrdude-stk500-getsync-not-in-sync-resp-0x00-aka-some-dude-named-avr-won/22541](https://arduino.stackexchange.com/questions/17/avrdude-stk500-getsync-not-in-sync-resp-0x00-aka-some-dude-named-avr-won/22541)

Other suggestions:
- ensure latest FTDI drivers (applicable if COM port not visible)
- burn bootloader
- try manually resetting by button press just before uploading

#### Library not found
Try installing/reinstalling the library from the PlatformIO library search. Otherwise find the original github repository to the library and add the repository link to your `platform.ini` settings under `lib_deps`.
