#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <string.h>

const int chipSelect = 53;

struct a {
  int sensor1;
  int sensor2;
  int sensor3;
  int sensor4;
  int sensor5;
  int classe;
};

a conjuntoTeste[5];

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  String dataInput = "";

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("/datalog.txt");

  // if the file is available, write to it:
int t;
int i = 0;
char c = ' ';
char intermediario[21];
int len = 0;
int last_acumulado = -1;
int acumulado = -1;
char *lvalue;
  if (dataFile) {
    while (dataFile.available()) {
      c = dataFile.read();
      while (c != '\r' and c != '\0') {
          intermediario[len] = c;
          len++;
          c = dataFile.read();
          Serial.println(c);
          }
      c = dataFile.read();
      if (c=='\0') break;
      intermediario[len] = ',';
      acumulado += len;
      Serial.print("Acumulado : ");
      Serial.println(acumulado);
      if (last_acumulado == acumulado){
        Serial.print(intermediario);
        break;
      }
      else {
      last_acumulado = acumulado;
      char * copia_intermediario = strdup(intermediario);
      conjuntoTeste[i].sensor1 = atoi(strtok_r(copia_intermediario,",",&lvalue));
      conjuntoTeste[i].sensor2 = atoi(strtok_r(NULL,",",&lvalue));
      conjuntoTeste[i].sensor3 = atoi(strtok_r(NULL,",",&lvalue));
      conjuntoTeste[i].sensor4 = atoi(strtok_r(NULL,",",&lvalue));
      conjuntoTeste[i].sensor5 = atoi(strtok_r(NULL,",",&lvalue));
      conjuntoTeste[i].classe = atoi(strtok_r(NULL,",",&lvalue));
      i++;    
      len = 0;
      }
    }
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
  for (i=0;i<=4;i++) {
    Serial.println(conjuntoTeste[i].sensor1);
    Serial.println(conjuntoTeste[i].sensor2);
    Serial.println(conjuntoTeste[i].sensor3);
    Serial.println(conjuntoTeste[i].sensor4);
    Serial.println(conjuntoTeste[i].sensor5);
    Serial.println(conjuntoTeste[i].classe);
    Serial.println();
  }
}

void loop() {

}


