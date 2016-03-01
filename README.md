# SquareMote
Main idea behind SquareMote (a.k.a. in Mote^2) is an ultra-low-power battery powered remote sensor that has as much devices already on board as possible to make it easy to deploy as an independent sensor, but also have ability to expand its functionality with add-on “shields”.

These are main features of the device:

- Onboard 3.3V voltage regulator allows device to be powered by external power source of up to 16V
- 3 Position physical switch that allows to bypass voltage regulator for low voltage source (3V coin cell), use normal power source with voltage regulator or turn power off
- Onboard 3V replaceable CR2032 battery
- Onboard RFM radio transceiver (RFM69W or RFM12B working at 433 or 915Mhz)
- Temperature and Humidity sensor
- ATMega328p processor running at 8 Mhz for low power (can  work under 2V)
- Onboard 16Mhz resonator which allows MCU to also work at 16Mhz when low power operation not required
- Onboard LEDs
- Onboard buttons (currently 6)
- Small size (5×5 cm)
- Two 14 pin headers that allow access to all AVR pins
- External power header
 
Initially this device was meant to be a remote weather sensor for my Xronos Clocks, but it grew into something more.

No need for bulky headers or wires, just insert 3V coin cell battery and drop this sensor where you want to monitor temperature/humidity. Of course it’s not waterproof, so you can’t technically “drop” it anywhere outside.

Long battery life was top priority and under ideal conditions Mote^2 can run for about a year without needing new battery. Of course it all depends on ambient temperature, and frequency of updates.  Cold is battery’s worst enemy and slashes it’s life significantly. However in warmer months (from April to November) I had a sensor outside sending updates every 5 minutes without seeing end to battery life in near future.

Few hardware “tricks” were used to lower power usage.

- Battery bypass for Linear Voltage Regulator. This provides most power saving, as Voltage Regulator created huge drain for power sources below 3.3V
- ATmega328p running at 8Mhz.
This reduced power drain, and also allows chip to run at voltages as low as 1.8V
- TH02 sensor power disconnect.
Temperature and Humidity Sensor’s (TH02) VCC connected to ATMega’s digial pin, allowing to switch sensor on and off at will
