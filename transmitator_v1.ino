#include <Wire.h>
#include <MPU6050_tockn.h>
#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE=7, CSN=8
const byte address[6] = "00001";

MPU6050 mpu(Wire);

struct Packet {
  int accel;
  int brake;
  int steer;
};

Packet data;

// --- smoothing heavy ---
float ax_f = 0;
float ay_f = 0;

// FILTRU EXPONENTIAL
float smooth(float prev, float now, float k) {
  return prev + (now - prev) * k;
}

void setup() {
  Serial.begin(115200);

  Wire.begin();
  mpu.begin();
  delay(2000);
  mpu.calcGyroOffsets();

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("Transmitter ULTRA HEAVY READY!");
}

void loop() {
  mpu.update();

  float ax = mpu.getAngleX(); // forward/back
  float ay = mpu.getAngleY(); // left/right

  // --- MASSIVE FILTER ---
  ax_f = smooth(ax_f, ax, 0.06); // mai agresiv
  ay_f = smooth(ay_f, ay, 0.06);

  // ============================
  //  DEADZONES EXTREME
  // ============================
  int deadzoneAccel = 20; // mai mare pentru miscare heavy
  int maxAccelAngle = 40;

  if (abs(ax_f) < deadzoneAccel) {
    data.accel = 0;
    data.brake = 0;
  }
  else if (ax_f >= deadzoneAccel) {
    // accel progresiva
    data.accel = map(ax_f, deadzoneAccel, maxAccelAngle, 0, 120);
    data.brake = 0;
  }
  else if (ax_f <= -deadzoneAccel) {
    data.brake = map(abs(ax_f), deadzoneAccel, maxAccelAngle, 0, 120);
    data.accel = 0;
  }

  data.accel = constrain(data.accel, 0, 120);
  data.brake = constrain(data.brake, 0, 120);

  // ============================
  //   ULTRA SAFE STEERING
  // ============================
  int deadzoneSteer = 20; // mare pentru heavy control
  int maxSteerAngle = 45;

  if (abs(ay_f) < deadzoneSteer) {
    data.steer = 0;
  }
  else {
    data.steer = map(ay_f, -maxSteerAngle, maxSteerAngle, -30, 30);
  }

  data.steer = constrain(data.steer, -30, 30);

  // trimite pachetul
  radio.write(&data, sizeof(data));

  // debug
  Serial.print("ACC=");
  Serial.print(data.accel);
  Serial.print("  BRK=");
  Serial.print(data.brake);
  Serial.print("  STR=");
  Serial.println(data.steer);

  delay(50);
}
