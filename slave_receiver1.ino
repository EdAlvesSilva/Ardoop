#include <Wire.h>
#include <math.h>

#define COMPRIMENTO_CONJUNTO_DE_TREINO 10
#define COMPRIMENTO_BUFFER_CONJUNTO_DE_TREINO (COMPRIMENTO_CONJUNTO_DE_TREINO/2)
#define PORTA_COMUNICACAO_MASTER    5


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
  uint32_t distancia;
  int classe;
};

distance conjunto_distancias[5];
a valor_teste;
a valores_treinamento[COMPRIMENTO_BUFFER_CONJUNTO_DE_TREINO];
int contador_treinamento;

void setup() {
  // Mudar dependendo de qual mapper
  Wire.begin(8);                // join i2c bus with address 8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);
  pinMode(PORTA_COMUNICACAO_MASTER,OUTPUT);
  digitalWrite(PORTA_COMUNICACAO_MASTER,HIGH);
  contador_treinamento = 0;     // start serial for output
}

void loop() {
  delay(100);
    if (contador_treinamento == COMPRIMENTO_BUFFER_CONJUNTO_DE_TREINO+1) {
      int i;
      for (i=0;i<contador_treinamento;i++) {
        conjunto_distancias[i].distancia = calc_distance(valores_treinamento[i].sensor1,valores_treinamento[i].sensor2,valores_treinamento[i].sensor3,valores_treinamento[i].sensor4,valores_treinamento[i].sensor5,valor_teste.sensor1,valor_teste.sensor2,valor_teste.sensor3,valor_teste.sensor4,valor_teste.sensor5);
        conjunto_distancias[i].classe =valores_treinamento[i].classe;
        Serial.print(valores_treinamento[i].sensor1);
        Serial.print(";");
        Serial.print(valores_treinamento[i].sensor2);
        Serial.print(";");
        Serial.print(valores_treinamento[i].sensor3);
        Serial.print(";");
        Serial.print(valores_treinamento[i].sensor4);
        Serial.print(";");
        Serial.print(valores_treinamento[i].sensor5);
        Serial.println(";");
        Serial.println("-----------------------------------------------");
        Serial.print(valor_teste.sensor1);
        Serial.print(";");
        Serial.print(valor_teste.sensor2);
        Serial.print(";");
        Serial.print(valor_teste.sensor3);
        Serial.print(";");
        Serial.print(valor_teste.sensor4);
        Serial.print(";");
        Serial.print(valor_teste.sensor5);
        Serial.println(";");
        Serial.print("Distancia : ");
        Serial.println(conjunto_distancias[i].distancia);
        Serial.print("Classe : ");
        Serial.println(conjunto_distancias[i].classe);
      }
      contador_treinamento = 0;
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
        Serial.println(contador2);
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

uint32_t calc_distance(int s11,int s12,int s13,int s14,int s15,int s21,int s22,int s23,int s24,int s25)
{
  double square_difference_s1 = (s21 - s11) * (s21 - s11);
  double square_difference_s2 = (s22 - s12) * (s22 - s12);
  double square_difference_s3 = (s23 - s13) * (s23 - s13);
  double square_difference_s4 = (s24 - s14) * (s24 - s14);
  double square_difference_s5 = (s25 - s15) * (s25 - s15);  
  
    double sum = square_difference_s1 + square_difference_s2 + square_difference_s3 + square_difference_s4 + square_difference_s5;
    double value = sqrt(sum);
    return value;
}
