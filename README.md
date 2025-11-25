
# Wireless Hand-Gesture Controlled Robot
A 4WD wireless rover controlled through a gesture-based wearable glove, featuring progressive speed control, differential steering, ultrasonic obstacle avoidance, and real-time feedback.

# Features

Proportional Gesture Control using MPU6050
Smooth acceleration based on hand tilt (PWM 85–200), not simple ON/OFF.

Differential Steering Logic
Blends throttle + steering for precise turning.

Long-Range Wireless Communication
NRF24L01 modules with reliable packet transmission.

Intelligent Obstacle Avoidance

Emergency braking under 15 cm

Servo-mounted ultrasonic scanner

“Parking sensor”-style buzzer feedback

Fail-Safe Protection
Motors stop automatically if the radio signal is lost or obstacle is detected.

---

## Components Used
Active Buzzer	1	Audio feedback
2× 18650 Batteries	1 pack	High-current power

| Component           | Quantity | Description                               |
|---------------------|----------|-------------------------------------------|
| Transmiter          | 1        | The Glove                                 |
| Arduino Nano        | 1        | MCU for gesture detection                 |
| MPU6050             | 1        | Gyro & Accelerometer                      |
| NRF24L01            | 1        | Long-range wireless module                |
| 470µF Capacitor     | 1        | Stabilization & good comunication         |
| 9V Battery          | 1        | Power source                              |
|---------------------|----------|-------------------------------------------|
| Receiver            | 1        | The Robot itself                          |
| Arduino Mega 2560   | 1        | Central controller                        |
| L298N Motor Driver  | 1        | For motrol controls and direction         |
| DC MOTORS           | 4        | 4WD propulsion                            |
| NRF24L01            | 1        | Long-range wireless module                |
| HC-SR04             | 1        | Distance sensor                           |
| SG90 Servo          | 1        | Rotates the sensor for scanning           |
| BUZZER              | 1        | Audio feedback                            |
| 3× 18650 Batteries  | 1        | High-current power                        |


---

## How It Works

The glove detects hand tilt using the MPU6050.

NRF24L01 transmits throttle + steering values to the robot.

The robot applies progressive PWM acceleration and differential steering.

The ultrasonic scanner monitors obstacles:

Under 15 cm → robot brakes instantly

Servo sweeps left/right to find a free direction

Buzzer beeps with increasing frequency as the distance decreases

If radio link fails, motors shut down automatically.

---

### Hardware Setup (Wiring Overview)
Receiver – Arduino Mega

Component	Pin Name	Arduino Pin	Notes

L298N	IN1 / IN2	D5 / D4	Left motors (PWM)

L298N	IN3 / IN4	D3 / D2	Right motors (PWM)

NRF24L01	CE / CSN	D7 / D11	Radio control

NRF24L01	SCK / MOSI / MISO	52 / 51 / 50	Hardware SPI

NRF24L01	VCC / GND	3.3V / GND	Add capacitor

Servo	Signal	A0	Ultrasonic scanning

HC-SR04	Trig / Echo	A3 / A4	Distance reading

Buzzer	VCC	D6	Audio alerts

Transmitter – Arduino Nano

Component	Pin Name	Arduino Pin

NRF24L01	CE / CSN	D7 / D8

NRF24L01	SCK / MOSI / MISO	D13 / D11 / D12

MPU6050	SDA / SCL	A4 / A5

---

## Software Dependencies

Install via Arduino IDE Library Manager:

RF24 by TMRh20

MPU6050_tockn

Servo (built-in)

SPI (built-in)

Wire (built-in)

---

## Configuration & Calibration
Gesture Sensitivity
int deadzoneAccel = 20;   
int maxAccelAngle = 45;   
int minPWM = 85;          
int maxPWM = 200;         

Direction Fix
float dirFataSpate = 1.0;
float dirStangaDreapta = 1.0;

---

## How to Run

Assemble the Hardware using the wiring tables.

Install all required libraries.

Upload code:

Transmitter_Code.ino → Glove

Receiver_Code.ino → Robot

Calibrate:
Place glove on a flat surface → press reset → wait ~3 seconds.

Drive:

Tilt forward = accelerate

Backward = reverse

Left/Right = steer

---

## Troubleshooting
Robot stutters or resets

Add a 470µF capacitor to NRF24L01 VCC/GND.

Motors spin in the wrong direction

Adjust the direction variables instead of rewiring.

Constant buzzer beeping

Obstacle detected < 12 cm.

Robot doesn’t move at low tilt

Increase minPWM.

---

## License

This project is open-source under the MIT License.
Feel free to use, modify, and contribute!

