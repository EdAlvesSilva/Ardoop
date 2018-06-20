#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <string.h>

#define COMPRIMENTO_CONJUNTO_DE_TESTE 5
#define COMPRIMENTO_CONJUNTO_DE_TREINO 10
#define ENDERECO_I2C_PRIMEIRO_MAPPER  8
#define ENDERECO_I2C_SEGUNDO_MAPPER  6
#define PORTA_COMUNICACAO_PRIMEIRO_MAPPER 31
#define PORTA_COMUNICACAO_SEGUNDO_MAPPER 32
#define COMPRIMENTO_MAXIMO_LINHA_ARQUIVO  26  

const int chipSelect = 53;

char *testSet[5];

struct b {
  int sensor1;
  int sensor2;
  int sensor3;
  int sensor4;
  int sensor5;
  int classe;
  char *string;
};
  
b conjuntoTeste[COMPRIMENTO_CONJUNTO_DE_TESTE];

void setup() {
  pinMode(PORTA_COMUNICACAO_PRIMEIRO_MAPPER,INPUT_PULLUP);
  pinMode(PORTA_COMUNICACAO_SEGUNDO_MAPPER,INPUT_PULLUP);
  // Open serial communications and wait for port to open:
  Wire.begin();
  delay(3000);
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
char first_buffer[21];
int len = 0;
int last_total_sum = -1;
int total_sum = -1;
char *lvalue;
  if (dataFile) {
    while (dataFile.available()) {
      c = dataFile.read();
      while (c != '\r' and c != '\0') {
          first_buffer[len] = c;
          len++;
          c = dataFile.read();
          }
      c = dataFile.read();
      if (c=='\0') break;
      first_buffer[len] = ',';
      total_sum += len;
      if (last_total_sum == total_sum){
        break;
      }
      else {
      if (i==0) {
        Wire.beginTransmission(ENDERECO_I2C_PRIMEIRO_MAPPER);
        Wire.write('A');
        Wire.write(first_buffer);
        Wire.endTransmission();

        delay(5000); // Pra da tempo de ler se foi certinho
        
        Wire.beginTransmission(ENDERECO_I2C_SEGUNDO_MAPPER);
        Wire.write('A');
        Wire.write(first_buffer);
        Wire.endTransmission();
      }
      last_total_sum = total_sum;
      char * copia_first_buffer = strdup(first_buffer);
      conjuntoTeste[i].sensor1 = atoi(strtok_r(copia_first_buffer,",",&lvalue));
      conjuntoTeste[i].sensor2 = atoi(strtok_r(NULL,",",&lvalue));
      conjuntoTeste[i].sensor3 = atoi(strtok_r(NULL,",",&lvalue));
      conjuntoTeste[i].sensor4 = atoi(strtok_r(NULL,",",&lvalue));
      conjuntoTeste[i].sensor5 = atoi(strtok_r(NULL,",",&lvalue));
      conjuntoTeste[i].classe = atoi(strtok_r(NULL,",",&lvalue));
      i++;  
      len = 0;
      free(copia_first_buffer);
      }
    }
    dataFile.close();
    free(first_buffer);
}
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
  Serial.println("Encerrado o setup"); //for debugging purpose
 }

void loop() {

  
  int test_set_counter;
  for (test_set_counter=0;test_set_counter <= COMPRIMENTO_CONJUNTO_DE_TESTE-1; test_set_counter++ ) {
    
    delay(5000);
    
    File dataFile = SD.open("/trainlog.txt");
    int t; //Trasnmition status
    int line_counter = 0;
    char c;

     
    while (dataFile.available()) {
      if (line_counter <= COMPRIMENTO_CONJUNTO_DE_TREINO/2) {
          Wire.beginTransmission(ENDERECO_I2C_PRIMEIRO_MAPPER);
          Wire.write('B');        
        }
        else {
          if (line_counter==COMPRIMENTO_CONJUNTO_DE_TREINO) break;
          Wire.beginTransmission(ENDERECO_I2C_SEGUNDO_MAPPER);
          Wire.write('B');
        }
      c = dataFile.read();
      while (c != '\r' and c != '\n'){
        Wire.write(c);
        c = dataFile.read();
      }
      if (c =='\r') c = dataFile.read();
        if (line_counter < COMPRIMENTO_CONJUNTO_DE_TREINO/2) {
          while (digitalRead(PORTA_COMUNICACAO_PRIMEIRO_MAPPER) != HIGH);
        }
        else {
          while (digitalRead(PORTA_COMUNICACAO_SEGUNDO_MAPPER) != HIGH);
        }
        t = Wire.endTransmission();
        delay(1);
        line_counter++;
      if (c=='\0' and line_counter==COMPRIMENTO_CONJUNTO_DE_TREINO) break;
    }
  Wire.endTransmission();
  delay(1450000); // 
  }
} 


