# i2c_eeprom_string_rw
read and write strings when using i2c eeprom with arduino, esp8266, eps32

EEPROM_PAGE_WRITE

   Example program showing a method for writing large amounts of
   data to an eeprom (24LC256/512 etc), whilst avoiding page write
   boundaries

   Original sketch: copyright www.hobbytronics.co.uk 2012,
   https://www.hobbytronics.co.uk/eeprom-page-write

   Programme modified by @techscapades 2021, modified
   sketch: 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   any later version. see <http://www.gnu.org/licenses/>

   Modifications by @techscapades:
   1. updateEEPROM function to prevent unnecessary writes
   2. Input and Output String instead of Char directly
   3. AutoUpdate bool for update confirmation

   ####LIMITATIONS####
   CURRENT IMPLEMENTATION CAN ONLY WRITE UP TO 12 CHARACTER LONG STRINGS
