/*****************************************************************************
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
*****************************************************************************/

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <SdFat.h>
#include <SdFatConfig.h>
// File system object.
SdFat sd;  // reassigns all sd functions to SdFat functions

// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The library version this example was written for
const char *libraryVersion = "0.23.2";
// The name of this file
const char *sketchName = "sensors_mayfly.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "proto_temp";
// How frequently (in minutes) to log data
//desired interval is 5, using 1 for debugging purposes
const uint8_t loggingInterval = 1;
// The number of readings to average
const uint8_t measurementsToAverage = 10;
// Your logger's timezone.
const int8_t timeZone = -8;  // Pacific Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long serialBaud = 115200;   // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;        // MCU pin for the green LED (-1 if not applicable)
const int8_t redLED = 9;          // MCU pin for the red LED (-1 if not applicable)
const int8_t buttonPin = 21;      // MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = A7;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin = -1;    // MCU SD card power pin (-1 if not applicable)
const int8_t sdCardSSPin = 12;     // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = -1;  // MCU pin controlling main sensor power (-1 if not applicable)

// Create the main processor chip "sensor" - for general metadata
// look at the back of your board for the version information
const char *mcuBoardVersion = "v0.5";
ProcessorStats mcuBoard(mcuBoardVersion);


// ==========================================================================
//    Settings for Additional Serial Ports
// ==========================================================================

// The modem and a number of sensors communicate over UART/TTL - often called "serial".
// "Hardware" serial ports (automatically controlled by the MCU) are generally
// the most accurate and should be configured and used for as many peripherals
// as possible.  In some cases (ie, modbus communication) many sensors can share
// the same serial port.

// External serial connection is needed for Arduino to commincate with gsm
// Here, AltSoftSerial is used because it far more efficient than the standard
// Arduino softwareserial library.
// Mayfly specification with AltSoftSerial is pin 6 for receiving (Rx) and
// pin 5 for transmitting (Tx). Pin 4 is a reserved pin for AltSoftSerial. Do not
// use this pin when using AltSoftSerial.

// Unfortunately, the Rx and Tx pins are on different Grove plugs on both the Mayfly
// making AltSoftSerial somewhat inconvenient to use.

// AltSoftSerial by Paul Stoffregen (https://github.com/PaulStoffregen/AltSoftSerial)
// is the most accurate software serial port for AVR boards.
// AltSoftSerial can only be used on one set of pins on each board so only one
// AltSoftSerial port can be used.
// Not all AVR boards are supported by AltSoftSerial.
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;


// ==========================================================================
//    Wifi/Cellular Modem Settings
// ==========================================================================

// Create a reference to the serial port for the modem
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section


AltSoftSerial &modemSerial = altSoftSerial;  // For software serial if needed

// Modem Pins - Describe the physical pin connection of your modem to your board
const int8_t modemVccPin = -1;      // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemStatusPin = -1;   // MCU pin used to read modem status (-1 if not applicable)
const int8_t modemResetPin = -1;    // MCU pin connected to modem reset pin (-1 if unconnected)
const int8_t modemSleepRqPin = -1;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem status (-1 if unconnected)

// Network connection information
const char *apn = "AT+CSTT=\"Ultra\",\"\",\"\"";  // The APN for the gprs connection, unnecessary for WiFi
// const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
// const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs


// ==========================================================================

// For almost anything based on the SIMCom SIM7000
#include <modems/SIMComSIM7000.h>
const long modemBaud = 9600;  //  SIM7000 does auto-bauding by default
SIMComSIM7000 modem900(&modemSerial,
                        modemVccPin, modemStatusPin,
                        modemResetPin, modemSleepRqPin,
                        apn);
// Create an extra reference to the modem by a generic name (not necessary)
SIMComSIM7000 modem = modem900;


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
// The I2C Maxim DS3231 real time clock (RTC) is absolutely required for
// time-keeping on all AVR boards. This library also makes use of it for
// its on-board temperature sensor. The DS3231 requires a 3.3V power supply.

#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(measurementsToAverage);
Variable *ds3231Temp = new MaximDS3231_Temp(&ds3231);

// ==========================================================================
//     Maxim DS18S20 One Wire Temperature Sensor
// ==========================================================================
//#include <sensors/MaximDS18.h>
//const int8_t OneWirePower = sensorPowerPin; // Pin to switch power on and off (-1 if unconnected)
//const int8_t OneWireBus = 2;  // Pin attached to the OneWire Bus (-1 if unconnected) (D24 = A0)

// Create a Maxim DS18 sensor object (use this form for a single sensor on bus with an unknown address)
// Use this constructor to save yourself the trouble of finding the address
//MaximDS18 ds18(OneWirePower, OneWireBus, measurementsToAverage);

// Create the temperature variable object for the DS18 and return a variable-type pointer to it
//MaximDS18_Temp(&ds18);  // Temperature in °C, optional UUID and variable code argument
// Resolution is between 0.0625°C (12 bit) and 0.5°C (9-bit)
// Accuracy is ±0.5°C from -10°C to +85°C for DS18S20 and DS18B20, ±2°C for DS1822 and MAX31820
// Range is -55°C to +125°C (-67°F to +257°F)


// ==========================================================================
//     DecagonCTD Meter Hydros 21
// ==========================================================================
// The Meter Environmental Hydros 21 communicates with the board using the
// SDI-12 protocol (and the Arduino SDI-12 library). It requires a 3.5-12V
// power supply, which can be turned off between measurements. While contrary
// to the manual, they will run with power as low as 3.3V. On the 5TM with a
// stereo cable, the power is connected to the tip, data to the ring, and ground
// to the sleeve. On the bare-wire version, the power is connected to the white cable,
// data to red, and ground to the unshielded cable.

#include <sensors/DecagonCTD.h>
const int8_t SDI12Power = sensorPowerPin;
const int8_t SDI12Data = 7;

// Create the Decagon CTD sensor object
DecagonCTD ctd(SDI12Power, SDI12Data, measurementsToAverage);


// ==========================================================================
//    Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
#include <VariableArray.h>

// FORM1: Create pointers for all of the variables from the sensors,
// at the same time putting them into an array
// NOTE:  Forms one and two can be mixed
Variable *variableList[] = {
    new ProcessorStats_Battery(&mcuBoard),  // battery voltage. Range is 0 to 5 V
// Create the Conductivity, Temperature, and Water Depth variables for the CTD
// and return variable-type pointers to them
    new DecagonCTD_Cond(&ctd),  // Conductivity in µS/cm
//  Resolution is 0.001 mS/cm = 1 µS/cm
//  Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
//  Range is 0 – 120 mS/cm (bulk)
    new DecagonCTD_Temp(&ctd),  // Temperature in °C
//  Resolution is 0.1°C
//  Accuracy is ±1°C
//  Range is -11°C to +49°C
    new DecagonCTD_Depth(&ctd)  // Water depth in mm
//  Resolution is 2 mm
//  Accuracy is ±0.05% of full scale
//  Range is 0 to 5 m or 0 to 10 m, depending on model
  };

// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);


// ==========================================================================
//     The Logger Object[s]
// ==========================================================================
#include <LoggerBase.h>

// Create a new logger instance
Logger dataLogger(LoggerID, loggingInterval, &varArray);


// ==========================================================================
//    A Publisher to Monitor My Watershed / EnviroDIY Data Sharing Portal
// ==========================================================================
// Device registration and sampling feature information can be obtained after
// registration at https://monitormywatershed.org or https://data.envirodiy.org
//const char *registrationToken = "12345678-abcd-1234-ef00-1234567890ab";   // Device registration token
//const char *samplingFeature = "12345678-abcd-1234-ef00-1234567890ab";     // Sampling feature UUID

// Create a data publisher for the EnviroDIY/WikiWatershed POST endpoint
//#include <publishers/EnviroDIYPublisher.h>
//EnviroDIYPublisher EnviroDIYPOST(dataLogger, &modem.gsmClient, registrationToken, samplingFeature);


// ==========================================================================
//    ThingSpeak Data Publisher
// ==========================================================================
// Create a channel with fields on ThingSpeak in advance
// The fields will be sent in exactly the order they are in the variable array.
// Any custom name or identifier given to the field on ThingSpeak is irrelevant.
// No more than 8 fields of data can go to any one channel.  Any fields beyond the
// eighth in the array will be ignored.
// const char *thingSpeakMQTTKey = "***KEY***";  // Your MQTT API Key from Account > MyProfile.
// const char *thingSpeakChannelID = "***KEY***";  // The numeric channel id for your channel
// const char *thingSpeakChannelKey = "***KEY***";  // The Write API Key for your channel

// Create a data publisher for ThingSpeak
// #include <publishers/ThingSpeakPublisher.h>
// ThingSpeakPublisher TsMqtt(dataLogger, &modem.gsmClient, thingSpeakMQTTKey, thingSpeakChannelID, thingSpeakChannelKey);


// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, uint8_t rate = 75)
{
    for (uint8_t i = 0; i < numFlash; i++) {
        digitalWrite(greenLED, HIGH);
        digitalWrite(redLED, LOW);
        delay(rate);
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
        delay(rate);
    }
    digitalWrite(redLED, LOW);
}


// Read's the battery voltage
// NOTE: This will actually return the battery level from the previous update!
float getBatteryVoltage()
{
    if (mcuBoard.sensorValues[0] == -9999) mcuBoard.update();
    return mcuBoard.sensorValues[0];
}


// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
    // Wait for USB connection to be established by PC
    // NOTE:  Only use this when debugging - if not connected to a PC, this
    // could prevent the script from starting
    #if defined SERIAL_PORT_USBVIRTUAL
      while (!SERIAL_PORT_USBVIRTUAL && (millis() < 10000)){}
    #endif

    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);
    Serial.println();

    Serial.print(F("Using ModularSensors Library version "));
    Serial.println(MODULAR_SENSORS_VERSION);

    if (String(MODULAR_SENSORS_VERSION) !=  String(libraryVersion))
        Serial.println(F(
            "WARNING: THIS EXAMPLE WAS WRITTEN FOR A DIFFERENT VERSION OF MODULAR SENSORS!!"));

    // Start the serial connection with the modem
    modemSerial.begin(modemBaud);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set up some of the power pins so the board boots up with them off
    // NOTE:  This isn't necessary at all.  The logger begin() function
    // should leave all power pins off when it finishes.
    if (modemVccPin >= 0)
    {
        pinMode(modemVccPin, OUTPUT);
        digitalWrite(modemVccPin, LOW);
    }
    if (sensorPowerPin >= 0)
    {
        pinMode(sensorPowerPin, OUTPUT);
        digitalWrite(sensorPowerPin, LOW);
    }

    // Set the timezones for the logger/data and the RTC
    // Logging in the given time zone
    Logger::setLoggerTimeZone(timeZone);
    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    Logger::setRTCTimeZone(0);

    // Attach the modem and information pins to the logger
    dataLogger.attachModem(modem);
    modem.setModemLED(modemLEDPin);
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin, greenLED);

    // Begin the logger
    dataLogger.begin();

    // Note:  Please change these battery voltages to match your battery
    // Check that the battery is OK before powering the modem
    if (getBatteryVoltage() > 3.7)
    {
        modem.modemPowerUp();
        modem.wake();
        modem.setup();

        // At very good battery voltage, or with suspicious time stamp, sync the clock
        // Note:  Please change these battery voltages to match your battery
        if (getBatteryVoltage() > 3.8 ||
            dataLogger.getNowEpoch() < 1546300800 ||  /*Before 01/01/2019*/
            dataLogger.getNowEpoch() > 1735689600)  /*After 1/1/2025*/
        {
            // Synchronize the RTC with NIST
            Serial.println(F("Attempting to connect to the internet and synchronize RTC with NIST"));
            if (modem.connectInternet(120000L))
            {
                dataLogger.setRTClock(modem.getNISTTime());
            }
        }
    }

    // Set up the sensors, except at lowest battery level
    if (getBatteryVoltage() > 3.4)
    {
        Serial.println(F("Setting up sensors..."));
        varArray.setupSensors();
    }

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct
    // Writing to the SD card can be power intensive, so if we're skipping
    // the sensor setup we'll skip this too.
    if (getBatteryVoltage() > 3.4)
    {
        Serial.println(F("Setting up file on SD card"));
        dataLogger.turnOnSDcard(true);  // true = wait for card to settle after power up
        dataLogger.createLogFile(true);  // true = write a new header
        dataLogger.turnOffSDcard(true);  // true = wait for internal housekeeping after write
    }

    // Power down the modem - but only if there will be more than 15 seconds before
    // the first logging interval - it can take the LTE modem that long to shut down
    if (Logger::getNowEpoch() % (loggingInterval*60) > 15 ||
        Logger::getNowEpoch() % (loggingInterval*60) < 6)
    {
        Serial.println(F("Putting modem to sleep"));
        modem.modemSleepPowerDown();
    }
    else
    {
        Serial.println(F("Leaving modem on until after first measurement"));
    }

    // Call the processor sleep
    Serial.println(F("Putting processor to sleep"));
    dataLogger.systemSleep();
}


// ==========================================================================
// Main loop function
// ==========================================================================

// Use this short loop for simple data logging and sending
// /*
void loop()
{
    // Note:  Please change these battery voltages to match your battery
    // At very low battery, just go back to sleep
    if (getBatteryVoltage() < 3.4)
    {
        dataLogger.systemSleep();
    }
    // At moderate voltage, log data but don't send it over the modem
    else if (getBatteryVoltage() < 3.7)
    {
        dataLogger.logData();
    }
    // If the battery is good, send the data to the world
    else
    {
        dataLogger.logDataAndPublish();
    }
}
