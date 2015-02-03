# ESP8266-webservo
Servo controlled by a simple webserver on an arduino with the ESP8266 module

The program begins with a 3 second delay to get the module ready. The pins 0 and 1 (RX and TX) are reserved for arduino's serial, so pins 2 and 3 are used using the [SoftwareSerial Library](http://arduino.cc/en/Reference/SoftwareSerial).
External power should be supplied to drive the servo since power from USB alone would probably not be sufficient for both the module and the servo.


Pins
=======
The ESP8266 modules runs on 3.3v. Connecting 5v will KILL the module! The power and ground from the arduino are connected to the module.
Rest of the pins should be connected to power for normal operation.
The module's RX and TX pins are connected to pins 3 and 2, respectively. Optionally, a separate USB to Serial module can be connected to those pins as well to monitor the communication between the arduino and the module.

The information about the module, its pins, and AT commands can be found [here](https://nurdspace.nl/ESP8266).
Also, there seems to be some problems running both the SoftwareSerial and the new servo libraries together. Quick fix for using 1-2 servos was addressed [here](http://forum.arduino.cc/index.php?topic=28456.0).
