#include <Wire.h>
#include <math.h>

#define COMPRIMENTO_CONJUNTO_DE_TREINO 10
#define COMPRIMENTO_BUFFER_CONJUNTO_DE_TREINO (COMPRIMENTO_CONJUNTO_DE_TREINO/2)
#define PORTA_COMUNICACAO_MASTER    5
#define PORTA_FINAL_EXPEDIENTE   11


int conter;
struct a {
  int sensor1;
  int sensor2;
  int sensor3;
  int sensor4;
  int sensor5;
  int classe;
};

struct distance {
  float distancia;
  int classe;
};

distance conjunto_distancias[7];
a valor_teste;
a valores_treinamento[COMPRIMENTO_BUFFER_CONJUNTO_DE_TREINO];
int contador_treinamento;

void setup() {
  Wire.begin(8);                // join i2c bus with address 8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  pinMode(PORTA_COMUNICACAO_MASTER,OUTPUT);
  digitalWrite(PORTA_COMUNICACAO_MASTER,HIGH);
  pinMode(PORTA_FINAL_EXPEDIENTE,OUTPUT);
  digitalWrite(PORTA_FINAL_EXPEDIENTE,HIGH);
  contador_treinamento = 0;     // start serial for output
}

void loop() {
  delay(100);
    if (contador_treinamento == COMPRIMENTO_BUFFER_CONJUNTO_DE_TREINO+1) {
      int i;
      for (i=0;i<contador_treinamento;i++) {
        conjunto_distancias[i].distancia = calc_distance(valores_treinamento[i].sensor1,valores_treinamento[i].sensor2,valores_treinamento[i].sensor3,valores_treinamento[i].sensor4,valores_treinamento[i].sensor5,valor_teste.sensor1,valor_teste.sensor2,valor_teste.sensor3,valor_teste.sensor4,valor_teste.sensor5);
        conjunto_distancias[i].classe =valores_treinamento[i].classe;
      }
      contador_treinamento = 0;
      digitalWrite(PORTA_FINAL_EXPEDIENTE,LOW);
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  digitalWrite(PORTA_COMUNICACAO_MASTER,LOW);
  char c = Wire.read();
  if (c == 'A') { //Vamos receber o dado do conjunto de teste!
    contador_treinamento = 0; // Resete a contagem do conjunto de treino!
    char dado[4];
    memset(dado,0,4);
    char aux;
    int contador1 =0;
    int contador2 =1;
    while (0 < Wire.available()) { // loop percorrendo todos
      aux = Wire.read();
      while (aux != ',' and Wire.available()) {
        dado[contador1] = aux; // recebe o byte;
        contador1++;
        aux = Wire.read();
      }
      contador1 = 0;
        if (contador2 == 1) {
          valor_teste.sensor1 = atoi(dado);
          memset(dado,0,4);    
        }
        if (contador2 == 2) {
          valor_teste.sensor2 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 3) {
          valor_teste.sensor3 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 4) {
          valor_teste.sensor4 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 5) {
          valor_teste.sensor1 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 6) {
          valor_teste.classe = atoi(dado);
          break;
        }
        contador2++;
    }  
  }
  if (c=='B') { //Vamos receber um dado do conjunto de treino!
    char dado[5];
    memset(dado,0,4);
    char aux;
    int contador1 =0;
    int contador2 =1;
    while (0 < Wire.available()) { // loop percorrendo todos
      aux = Wire.read();
      while (aux != ',' and Wire.available() and contador1 != 4) {
        dado[contador1] = aux; // recebe o byte;
        contador1++;
        aux = Wire.read();
        if (contador2==6) break;
      }
      if (contador2==6) dado[0] = aux;
      contador1 = 0;
        if (contador2 == 1) {
          valores_treinamento[contador_treinamento].sensor1 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 2) {
          valores_treinamento[contador_treinamento].sensor2 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 3) {
          valores_treinamento[contador_treinamento].sensor3 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 4) {
          valores_treinamento[contador_treinamento].sensor4 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 5) {
          valores_treinamento[contador_treinamento].sensor5 = atoi(dado);
          memset(dado,0,4);
        }
        if (contador2 == 6) {
          valores_treinamento[contador_treinamento].classe = atoi(dado);
          contador_treinamento++;
          break;
        }
        contador2++;
    }
  }
  digitalWrite(PORTA_COMUNICACAO_MASTER,HIGH);
}

double calc_distance(int s11,int s12,int s13,int s14,int s15,int s21,int s22,int s23,int s24,int s25)
{
  Serial.println("a");
  double square_difference_s1 = fabs(s21 - s11);
  double square_difference_s2 = fabs(s22 - s12);
  double square_difference_s3 = fabs(s23 - s13);
  double square_difference_s4 = fabs(s24 - s14);
  double square_difference_s5 = fabs(s25 - s15);  
  
    double sum = fabs(square_difference_s1 + square_difference_s2 + square_difference_s3 + square_difference_s4 + square_difference_s5);
    return sum;
}

void requestEvent() {
  int alfa;
  for (alfa=0;alfa<COMPRIMENTO_BUFFER_CONJUNTO_DE_TREINO;alfa++) {
    byte *b = (byte *) &conjunto_distancias[alfa].distancia;
    Wire.write(b[0]);
    Wire.write(b[1]);
    Wire.write(b[2]);
    Wire.write(b[3]);
    Wire.write(conjunto_distancias[alfa].classe);
   }
  digitalWrite(PORTA_FINAL_EXPEDIENTE,LOW);
}
