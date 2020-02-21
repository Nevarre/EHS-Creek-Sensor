# EHS-Creek-Sensor

Current setup process
-> Create new project in your PlatformIO Home
-> Clone repository
-> Change `platform.ini` settings

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

upload_port = /dev/cu.usbserial*
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
