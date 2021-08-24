# MKRGSM library

The [MKR GSM library](https://www.arduino.cc/en/Reference/MKRGSM) can be installed using the [Arduino library manager](https://www.arduino.cc/en/Guide/Libraries).

With the [Arduino MKR GSM 1400](https://store.arduino.cc/mkr-gsm-1400) and this library you can do most of the operations you can do with a GSM phone: place and receive voice calls, send and receive SMS, and connect to the internet over a GPRS network. The on board module, operates in 3G with a 2G fallback.

Arduino MKR GSM 1400 has a modem that transfers data from a serial port to the GSM network. The modem executes operations via a series of AT commands. The library abstracts low level communications between the modem and SIM card. It relies on the [Serial library](https://www.arduino.cc/en/Reference/Serial) for communication between the modem and Arduino.

Typically, each individual command is part of a larger series necessary to execute a particular function. The library can also receive information and return it to you when necessary.

This library is based on the [GSM](https://www.arduino.cc/en/Reference/GSM) library of the Arduino GSM shield, adding new features like UDP, SSL and DTMF tone recognition.

To use this library
```
#include <MKRGSM.h>
```

## Library structure

As the library enables multiple types of functionality, there are a number of different classes.

- The `GSM` class takes care of commands to the radio modem. This handles the connectivity aspects of the module and registers your system in the GSM infrastructure. All of your GSM/GPRS programs will need to include an object of this class to handle the necessary low level communication.
- Voice call handling, managed by the `GSMVoiceCall` class.
- Send/receive SMS messages, managed by the `GSM_SMS` class.
- The `GPRSClass` is for connecting to the internet.
- `GSMClient` includes implementations for a client, similar to the Ethernet and WiFi libraries.
- `GSMServer` includes implementations for a server, similar to the Ethernet and WiFi libraries. NB : A number of network operators do not allow for incoming connections from the public internet, but will allow them from inside their own. Check with your operator to see what restrictions there are on data use.
- A number of utility classes such as GSMScanner and GSMModem

## Library compatibility

The library tries to be as compatible as possible with the current Ethernet and WiFi101 library. Porting a program from an Arduino Ethernet or WiFi101 library to an Arduino with the MKR GSM 1400 should be fairly easy. While it is not possible to simply run Ethernet or WiFi101 compatible code on the MKR GSM 1400 as-is, some minor, library specific, modifications will be necessary, like including the GSM and GPRS specific libraries and getting network configuration settings from your cellular network provider.
