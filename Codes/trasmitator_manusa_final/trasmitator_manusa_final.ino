#include <Wire.h> //folosit pt I2C de MPU
#include <MPU6050_tockn.h>
#include <SPI.h> //pt SPI de nrf
#include <RF24.h>

RF24 radio(7,8); //creeaza obiectul pt CE SI CSN
const byte address[6] = "00001";

MPU6050 mpu(Wire); //creem obiectul MPU pt magistrala I2C

//strctura datelor trimise catre robot
struct Telecomanda{
  int acceleratie;
  int frana;
  int viraj;
};

Telecomanda date; //instanta telecomezii

//pt axa XY am creat un smoothing puternic care se mai poate regla, asta pt a avea un calibru mai bun de control
float ax_f=0;
float ay_f=0;

//funtie de filtru exponential pt netezirea valorilor brute
float smooth(float prev, float now, float k) {
  return prev + (now - prev) * k;
}


void setup() {
  Serial.begin(115200);

  Wire.begin();
  mpu.begin();
  delay(2000);
  mpu.calcGyroOffsets(); //calibrare giroscop 

  radio.begin();
  radio.setPALevel(RF24_PA_LOW); //folosim o putere mai mica dar sigura pt un consum redus
  radio.openWritingPipe(address); //adresa pipe-ului prin care comunica radioul
  radio.stopListening(); //modulul intra in mid trasnmisie

  Serial.println("Transmitator Pornit si calibrat");

}

void loop() {
  mpu.update(); //actualizeaza citirele de miscare din senzor

  float ax = mpu.getAngleX(); //unghi pt fata/sapte
  float ay = mpu.getAngleX(); //unghi pt stanga/dreapta

  ax = ax * 1.0;
  ay = ay * 1.0;

  //aplica filtrare puternica ca sa nu fie prea senzitiva manusa
  ax_f = smooth(ax_f, ax, 0.10);
  ax_f = smooth(ay_f, ay, 0.10);

  //zone moarte + viteze
  int deadzoneAcc = 20; //daca e la un unghi de sub 20 de grade e oprit
  int maxAccAngle = 45; //unghi maxim inclinat pt acceleratie

  int minPWM = 80; //putere minima pe care o ia robotu la start
  int maxPWM = 200; //putere maxim trimisa spre robot (n-am folosit 255 pt a proteja motoarele)

  //logica acceleratie/dat cu spatele
  if(abs(ax_f) < deadzoneAcc) {
    date.acceleratie = 0;
    date.frana = 0;
  
  }
  else if (ax_f >= deadzoneAcc) {
    date.acceleratie = map(ax_f, deadzoneAcc, maxAccAngle, minPWM, maxPWM); 
    date.frana = 0;
  }
  else if (ax_f <= -deadzoneAcc) {
    date.frana = map(abs(ax_f), deadzoneAcc, maxAccAngle, minPWM, maxPWM);
    date.acceleratie = 0;
  }

  //limitari pt acceleratie si frana
  date.acceleratie = constrain(date.acceleratie, 0, maxPWM);
  date.frana = constrain(date.frana, 0, maxPWM);

  //pt viraje
  int deadzoneSteer = 20;
  int maxSteerAngle = 45;
  int steerPower = 110; //putere pt a intoarce robotul

  if(abs(ay_f) < deadzoneSteer) { //zona moarta pt viraj
    date.viraj = 0;
  }
  else {
    date.viraj = map(ay_f, -maxSteerAngle, maxSteerAngle, -steerPower, steerPower); //mapare unghi + putere viraj
  }

  date.viraj = constrain(date.viraj, -steerPower, steerPower); //limitam virajul

  radio.write(&date, sizeof(date)); //trimitem structura de date la robot 

  delay(50);
}
