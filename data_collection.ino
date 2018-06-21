#include <SPI.h>
#include <SD.h>
#include <Rtc_Pcf8563.h>

const int chipSelect = 53;
Rtc_Pcf8563 rtc;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";

  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin <= 5; analogPin++) {
    if (analogPin < 5) {
      int sensor = analogRead(analogPin);
      dataString += String(sensor);
      dataString += ",";
    }
    if (analogPin == 5) {
      dataString += String((rtc.formatTime()));
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("19052018.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
  }
}

