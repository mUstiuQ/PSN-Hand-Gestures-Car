#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

// ---- NRF24 ----
RF24 radio(7, 11);
const byte address[6] = "00001";

// ---- MOTOR DRIVER ----
#define IN1 5
#define IN2 4
#define IN3 3
#define IN4 2

// ---- SERVO + SENSOR ----
#define SERVO_PIN A0
#define TRIG A3
#define ECHO A4

#define BUZZER 6


Servo scanner;

// structura pachetului primit
struct Packet {
  int accel;   // 0..255
  int brake;   // 0..255
  int steer;   // -100..100
};

Packet data;

// masurare distanta
long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 30000);
  return duration * 0.034 / 2;
}

// functii motoare
void motors(int left, int right) {
  // stanga
  if (left > 0) { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); }
  else if (left < 0) { digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); }
  else { digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); }

  // dreapta
  if (right > 0) { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }
  else if (right < 0) { digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
  else { digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); }
}

void stopAll() {
  motors(0, 0);
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  scanner.attach(SERVO_PIN);
  scanner.write(90);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);


  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  Serial.println("Receiver ready!");
}

void loop() {
  // citire pachet NRF
  if (radio.available()) {
    radio.read(&data, sizeof(data));

    Serial.print("ACC=");
    Serial.print(data.accel);
    Serial.print(" BRK=");
    Serial.print(data.brake);
    Serial.print(" STR=");
    Serial.println(data.steer);
  }

  // distanta
  int dist = getDistance();
  Serial.print("Dist: ");
  Serial.println(dist);


  if (dist > 0 && dist < 10) {
  digitalWrite(BUZZER, HIGH);
  delay(80);
  digitalWrite(BUZZER, LOW);
  delay(80);
  stopAll();
  return;
  } else {
  digitalWrite(BUZZER, LOW);
} 




  // LOGICA DE MISCARE
  int forward = data.accel - data.brake; // accel pozitiv, frana negativ
  forward = constrain(forward, -255, 255);

  // steering - afecteaza diferit stanga/dreapta
  int leftMotor  = forward - (data.steer * 1.3);
  int rightMotor = forward + (data.steer * 1.3);

  // limitare
  leftMotor = constrain(leftMotor, -255, 255);
  rightMotor = constrain(rightMotor, -255, 255);

  // conducere efectivă
  motors(leftMotor, rightMotor);

  delay(20);
}
