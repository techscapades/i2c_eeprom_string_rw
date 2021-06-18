/*
   EEPROM_PAGE_WRITE

   Example program showing a method for writing String
   data to an eeprom (24LC256/512 etc)

   copyright www.hobbytronics.co.uk 2012, original sketch:
   https://www.hobbytronics.co.uk/eeprom-page-write

   Programme modified by @techscapades 2021, modified
   sketch: https://github.com/techscapades/i2c_eeprom_string_rw/tree/main

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   any later version. see <http://www.gnu.org/licenses/>

   Modifications by @techscapades:
   1. updateEEPROM function to prevent unnecessary writes
   2. Input and Output String instead of Char directly
   3. AutoUpdate bool for update confirmation
   4. Length of data to be read isnt required anymore
      BUT you need to avoid using a special character called: terminating_char
      when saving strings because its used as a terminating flag for automatic reading

   ####LIMITATIONS####
   CURRENT IMPLEMENTATION CAN ONLY WRITE UP TO 12 CHARACTER LONG STRINGS PER ADDRESS, 
   it is defined as maxChar, hopefully I or someone can improve this to match
   the original sketch's function, for now, split your strings at index 12 and 
   write them to consecutive addresses, read from them accordingly :/

   If you want to save numbers etc.. use String(your_number) and pass it
   into updateEEPROM. Use (mystring).toInt() or toDouble etc.. to convert
   the string back into your original data type

*/

#include <Wire.h>
#define eeprom1 0x50    //Address of 24LC256 eeprom chip
bool eeprom_autoupdate = true; //specify if you wanna control updates
const String terminating_char = "|"; //ensure its something you wont use in any String!!!!!!
const int maxChar = 12; //WARNING DO NOT CHANGE THIS!


void setup(void)
{
  Serial.begin(115200);
  Wire.begin();
  delay(500);
  Serial.println();

  String test = "0123456789ab";

  updateEEPROM(eeprom1, 0, test); //(i2c device address of eeprom, address to be written to, String to be written)
  Serial.println();
  String resultString = readEEPROM(eeprom1, 0); //(i2c device address of eeprom, address to be read from)
  Serial.println(resultString);

}


void updateEEPROM(int deviceaddress, unsigned int eeaddress, String writeInput ) {
  Serial.println("======================== UPDATE START ========================");

  char str_len = 0;
  String readOutput = ""; //string to store the read output
  char writeData[] = {}; //empty char buffer to hold string to char output
  String writeDataOutput = ""; //string to show String(writeData) for debugging

  //currently limited to 12 character long strings only;
  if (writeInput.length() > maxChar) {
    Serial.print("String longer than ");
    Serial.print(maxChar);
    Serial.println(" characters, update failed");
  } else {

    if (writeInput.length() < maxChar) {
      writeInput = writeInput + terminating_char ;
    }

    int writeData_len = writeInput.length() + 1;
    writeInput.toCharArray(writeData, writeData_len);

    //find number of characters in writeData and save it to str_len
    do {
      str_len++;
    } while (writeData[str_len]);

    for (int j = 0; j < str_len; j++) {
      writeDataOutput += String(char(writeData[j]));
    }
    Serial.println("writeDataOutput");
    Serial.println(writeDataOutput);

    Serial.println("writeInput before update");
    Serial.println(writeInput);

    //do a read for the data at the address
    readOutput = readEEPROM(deviceaddress, eeaddress);
    readOutput = readOutput + terminating_char;

    Serial.println("readOutput before update");
    Serial.println(readOutput);

    //do a String comparison and write only if its not the same
    if (writeInput == readOutput) {
      Serial.println("No variable update required");
    } else {
      if (eeprom_autoupdate == true) {
        Serial.println("updating");
        writeEEPROM(deviceaddress, eeaddress, writeData);
        Serial.println("updated");
      }
      else {
        //manual update code, can also set a button input or some other flag
        while (Serial.available()) Serial.read();
        Serial.println("Update EEPROM ?  press Y to confirm");
        while (!Serial.available());
        char res =  Serial.read();
        if (res == 'Y' || res == 'y') {
          Serial.println("updating");
          writeEEPROM(deviceaddress, eeaddress, writeData);
          Serial.println("updated");
        } else {
          Serial.println("update cancelled");
        }
      }
    }
  }
  Serial.println("======================== UPDATE END ==========================");

}

String readEEPROM(int deviceaddress, unsigned int eeaddress)
{
  Serial.println("================== READ START ==================");
  unsigned char i = 0;
  String outputString = "";
  unsigned char data[32]; //a char buffer to store the read output

  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(deviceaddress, maxChar);

  while (Wire.available()) {
    data[i++] = Wire.read();
  }

  //convert the read data into a String and save it to readOutput
  for (int j = 0; j < maxChar; j++) {
    outputString += String(char(data[j]));
  }

  //block for automatic reading without needing to specify length of string
  if (outputString.indexOf(terminating_char) == -1) {
    Serial.println("terminating_char not needed");
  } else {
    Serial.println("index of terminating charachter");
    Serial.println(outputString.indexOf(terminating_char));
    outputString.remove(outputString.indexOf(terminating_char));
  }


  if (outputString.length() <= maxChar) {
    //Serial.println("readEEPROMString function output string");
    Serial.println(outputString);
    Serial.println("================== READ END ====================");
    return outputString;
  }

}


void writeEEPROM(int deviceaddress, unsigned int eeaddress, char* data)
{
  // Uses Page Write for 24LC256
  // Allows for 64 byte page boundary
  // Splits string into max 16 byte writes
  unsigned char i = 0, counter = 0;
  unsigned int  address;
  unsigned int  page_space;
  unsigned int  page = 0;
  unsigned int  num_writes;
  unsigned int  data_len = 0;
  unsigned char first_write_size;
  unsigned char last_write_size;
  unsigned char write_size;

  // Calculate length of data
  do {
    data_len++;
  } while (data[data_len]);

  // Calculate space available in first page
  page_space = int(((eeaddress / 64) + 1) * 64) - eeaddress;

  // Calculate first write size
  if (page_space > 16) {
    first_write_size = page_space - ((page_space / 16) * 16);
    if (first_write_size == 0) first_write_size = 16;
  }
  else
    first_write_size = page_space;

  // calculate size of last write
  if (data_len > first_write_size)
    last_write_size = (data_len - first_write_size) % 16;

  // Calculate how many writes we need
  if (data_len > first_write_size)
    num_writes = ((data_len - first_write_size) / 16) + 2;
  else
    num_writes = 1;

  i = 0;
  address = eeaddress;
  for (page = 0; page < num_writes; page++)
  {
    if (page == 0) write_size = first_write_size;
    else if (page == (num_writes - 1)) write_size = last_write_size;
    else write_size = 16;

    Wire.beginTransmission(deviceaddress);
    Wire.write((int)((address) >> 8));   // MSB
    Wire.write((int)((address) & 0xFF)); // LSB
    counter = 0;
    do {
      Wire.write((byte) data[i]);
      i++;
      counter++;
    } while ((data[i]) && (counter < write_size));
    Wire.endTransmission();
    address += write_size; // Increment address for next write

    delay(6);  // needs 5ms for page write
  }
}

void loop() {
}
