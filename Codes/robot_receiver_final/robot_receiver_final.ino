//prima data bagam bibliotecile pt servo(care nu-l mai foloses), modulu radio si comunicatiile SPI
#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(7,11); //creem obiectul radio setat pe pinii 7 si 11
const byte address[6] ="00001"; //adresa radio a robotului



//definimi pinii PWM
 //motoare stanga
#define IN1 5
#define IN2 4
//motoare dreapta
#define IN3 3
#define IN4 2

#define SERVO_PIN A0
#define TRIG A3
#define ECHO A4
#define BUZZER 6

Servo scanner; //obiectul pt servo

//structura pt datele care le da manusa
struct Telecomanda{
  int acceleratie;
  int frana;
  int viraj;
};
Telecomanda date; 


//logica pentru buzzer cand vede un obiect
unsigned long lastBuzzerTime=0;
int buzzerState = LOW;
const long buzzerInterval = 150; //timp in milisecunde

//functii pt senzorul ultrasonic
long getDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 30000);
  //daca nu capteaza nimic pe o distanta de 999 cm se face conversie microsecunde in cm
  if(duration == 0)
    return 999;
  return duration * 0.034/2;
}

//logica motoarelor
void motors(int left, int right) {
  left = constrain(left, -255, 255);
  right = constrain(right, -255, 255);

  //PT STANGA
  //motor inainte
  if(left>0)
  {
    digitalWrite(IN1, LOW);
    analogWrite(IN2, left);
  }
  //motor inapoi
  else if (left < 0)
  {
    analogWrite(IN1, abs(left));
    digitalWrite(IN2, LOW);
  }
  //motor oprit
  else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  //PT DREAPTA
  //motor inainte
   if (right > 0) { 
    analogWrite(IN3, right); 
    digitalWrite(IN4, LOW); 
  }
  //motor inapoi
  else if (right < 0) { 
    digitalWrite(IN3, LOW); 
    analogWrite(IN4, abs(right)); 
  }
  //motor oprit
  else { 
    digitalWrite(IN3, LOW); 
    digitalWrite(IN4, LOW); 
  }
}


//functie care opreste tot
void stopAll(){
  motors(0,0);
}


void setup() {
  Serial.begin(115200);

  //setam pinii motoarele ca outputuri
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  //senzorul ultrasonic seteaza directiile
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  //pune servoul in pozitia initiala (90 grade)
  scanner.attach(SERVO_PIN);
  scanner.write(90);
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  //pornim modulul radio ca fiind un receiver
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

}


  //daca exista impuls de la telecomanda il citeste
void loop() {
  if(radio.available()) {
    radio.read(&date, sizeof(date));
  }

  int dist = getDistance();
  unsigned long currentMili = millis(); //distanta si timpul curent

  //logica bipait buzzer
  if(dist>0 && dist <12) {
  //verificam daca a trecut timpul pentru urmatorul bip
  if(currentMili - lastBuzzerTime >= buzzerInterval)
  {
    lastBuzzerTime = currentMili; //resetam cronometrul

    //inversam starea (daca e stins il aprindem, daca e aprins il stingem)
    if(buzzerState == LOW) 
    {
      buzzerState = HIGH;
    }
    else{
      buzzerState = LOW;
    }
    digitalWrite(BUZZER, buzzerState);
  }

  //conditie de siguranta pentru a prevenii un accident (daca merge in fata si e ceva, il oprim)
  if(date.acceleratie > 0) {
    stopAll();
  }
  else {
    //cu spatele poate sa mearga ca nu are senzor si aco (desi ar trebuii)
    int forward = date.acceleratie - date.frana;
    int steerAmount = date.viraj;
    motors(forward - steerAmount, forward + steerAmount);
  }
 } else {
  //daca nu e obsactol , oprim buzzerul si resetam starea
  digitalWrite(BUZZER, LOW);
  buzzerState = LOW;

  //miscare normala
  int forward = date.acceleratie - date.frana;
  int steerAmount = date.viraj;
  int leftMotor = forward - steerAmount;
  int rightMotor = forward + steerAmount;
  motors(leftMotor, rightMotor);
  }
  
  delay(10);
}




