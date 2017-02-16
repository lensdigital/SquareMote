##Firmware for Squaremote Based Temperature/Humidity remote sensors

###To upload hex firmware:

* You will need a 6 pin serial to USB converter (i.e. FTDI adapter)
* copy AVRDUDE version 6.3 to know location (i.e. C:\Xronos\AVR). Are required files included with Arduino IDE 1.6.10
* save all *.bat and *.hex files to some folder on your computer
* modify update_squaremote.bat and specify AVRPath and AVRConfig paths if different.
* RENAME .hex file based on your radio chip type (RFM12b vs RFM69)
 * If you have RFM12b, rename RFM12b file (i.e. sensor_R2_915_RFM12b.hex) to sensor_R2_915.hex (or 433 depending on your frequency)
 * For RFM69 radio, similary rename RFM69 to sensor_R2_915.hex (or 433). 
 * Same as above applies to R2.5 revisions
* Choose your firmeware version and run specific .bat file. It will ask you for COM port (single digit) and Frequency (915 or 433) Mhz
  * start_here_R2.bat will update R2 squaremote with standard sensor firmware (Network 1, Gateway 1, Node 5)
  * th02_heater.bat will start heating up TH02 sensor elemement to fix issues with stuck humidity. Please run it for at least 15 minutes. Do not use battery for this one, as it will drain it quickly. Upload normal firmware after sensor has been "baked" for sufficient time.
  
  
