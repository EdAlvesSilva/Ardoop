#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <string.h>

#define COMPRIMENTO_CONJUNTO_DE_TESTE 5
#define COMPRIMENTO_CONJUNTO_DE_TREINO 10
#define ENDERECO_I2C_PRIMEIRO_MAPPER  8
#define ENDERECO_I2C_SEGUNDO_MAPPER  12
#define PORTA_COMUNICACAO_PRIMEIRO_MAPPER 31
#define PORTA_COMUNICACAO_SEGUNDO_MAPPER 32
#define COMPRIMENTO_MAXIMO_LINHA_ARQUIVO  26
#define PORTA_CONCLUSAO_DE_TAREFA_PRIMEIRO  7
#define PORTA_CONCLUSAO_DE_TAREFA_SEGUNDO   6
 

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

unsigned long tempo1;

typedef union
{
 double number;
 uint8_t bytes[4];
} doubleunion_t;

struct distance {
  doubleunion_t distancia;
  int classe;
};

distance conjunto_distancias[COMPRIMENTO_CONJUNTO_DE_TREINO];


void setup() {
  pinMode(PORTA_COMUNICACAO_PRIMEIRO_MAPPER,INPUT_PULLUP);
  pinMode(PORTA_COMUNICACAO_SEGUNDO_MAPPER,INPUT_PULLUP);
  pinMode(PORTA_CONCLUSAO_DE_TAREFA_PRIMEIRO,INPUT_PULLUP);
  pinMode(PORTA_CONCLUSAO_DE_TAREFA_SEGUNDO,INPUT_PULLUP);
  
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
          }
      c = dataFile.read();
      if (c=='\0') break;
      intermediario[len] = ',';
      acumulado += len;
      if (last_acumulado == acumulado){
        break;
      }
      else {
      if (i==0) {
        Wire.beginTransmission(ENDERECO_I2C_PRIMEIRO_MAPPER);
        Wire.write('A');
        Wire.write(intermediario);
        Wire.endTransmission();

        Wire.beginTransmission(ENDERECO_I2C_SEGUNDO_MAPPER);
        Wire.write('A');
        Wire.write(intermediario);
        Wire.endTransmission();

        break;
      }
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
      free(copia_intermediario);
      }
    }
    dataFile.close();
    free(intermediario);
}
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
  Serial.println("Encerrado o setup");
  tempo1 = millis();
  Serial.println(tempo1);
 }

void loop() {
  int test_set_counter;
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
        line_counter++;
      if (c=='\0' and line_counter==COMPRIMENTO_CONJUNTO_DE_TREINO) break;
    }
  Wire.endTransmission();
  while(digitalRead(PORTA_CONCLUSAO_DE_TAREFA_PRIMEIRO)!=LOW);
  Wire.requestFrom(ENDERECO_I2C_PRIMEIRO_MAPPER, 32); // O numero de bytes que eu solicito nao importa pra nada, entao estou solicitando o maximo que posso
  int couting_var = 0; 
  int i;
  while (Wire.available()>0) {
    for (i=0;i<4;i++) {
      conjunto_distancias[couting_var].distancia.bytes[i] = Wire.read();
    }
    conjunto_distancias[couting_var].classe = Wire.read();
    couting_var++;
  }
  while(digitalRead(PORTA_CONCLUSAO_DE_TAREFA_SEGUNDO)!=LOW) ;
  Wire.requestFrom(ENDERECO_I2C_SEGUNDO_MAPPER, 32);
    while (Wire.available()>0) {
    for (i=0;i<4;i++) {
      conjunto_distancias[couting_var].distancia.bytes[i] = Wire.read();
    }
    conjunto_distancias[couting_var].classe = Wire.read();
    couting_var++;
  }
  Serial.println("Valor: ");
  unsigned long tempo2 = millis();
  Serial.println(tempo2);
} 


