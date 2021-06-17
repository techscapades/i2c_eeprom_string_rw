# i2c eeprom string rw
read and write strings when using multiple i2c eeprom with arduino, esp8266, eps32

EEPROM_PAGE_WRITE

   Example program showing a method for writing large amounts of
   data to an eeprom (24LC256/512 etc), whilst avoiding page write
   boundaries

   Original sketch: copyright www.hobbytronics.co.uk 2012,
   https://www.hobbytronics.co.uk/eeprom-page-write

   Modified sketch: @techscapades 2021,
   https://github.com/techscapades/i2c_eeprom_string_rw/tree/main

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   any later version. see <http://www.gnu.org/licenses/>

   Modifications by @techscapades:
   1. updateEEPROM function to prevent unnecessary writes
   2. Input and Output String instead of Char directly (which was annoying)
   3. AutoUpdate bool for update confirmation
   4. Length of data to be read isnt required anymore so you can 
      read directly from an address as well, BUT you need to avoid 
      using a special character called: terminating_char, when saving 
      strings because its used as a terminating flag for automatic reading

   ####LIMITATIONS####
   CURRENT IMPLEMENTATION CAN ONLY WRITE UP TO 12 CHARACTER LONG STRINGS,
   it is defined as maxChar, hopefully I or someone can improve this to match
   the original sketch's function
   
   If you want to save numbers etc.. use String(your_number) and pass it
   into updateEEPROM. Use (mystring).toInt() or toDouble etc.. to convert
   the string back into your original data type
   
   
