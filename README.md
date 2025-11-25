
# Wireless Hand-Gesture Controlled Robot
With Obstacle Avoidance & Progressive Speed Control

A fully wireless 4WD robotic rover controlled through a wearable gesture-based glove.
The system uses an Arduino Mega on the robot and an Arduino Nano/Uno on the transmitter, communicating through NRF24L01 modules.

The robot features progressive PWM acceleration, differential steering, active ultrasonic obstacle avoidance, and real-time audio feedback.

 Key Features
 Proportional Gesture Control

Speed is fully analog — not just ON/OFF.

The robot accelerates smoothly (PWM 85–255) based on hand tilt using the MPU6050.

 Differential Steering

Smooth turning using a mix of throttle + steering input for precise directional control.

 Long-Range Wireless Link

NRF24L01 modules ensure stable, low-latency communication with packet integrity checks.

 Smart Obstacle Avoidance

Automatic emergency braking under 15 cm

Servo-mounted HC-SR04 performs scanning for free direction

“Parking sensor”-style beeping for distance awareness

 Fail-Safe Systems

Motors stop instantly if:

Radio signal is lost

A close obstacle is detected

 Hardware Requirements
 Transmitter (Wearable Glove)

Arduino Nano / Uno

MPU6050 IMU (Gyroscope + Accelerometer)

NRF24L01 + PA/LNA radio

(Recommended: add 10 µF capacitor between VCC & GND)

Power source: 9V battery or power bank

Status LED (pin 13) for heartbeat/diagnostics

 Receiver (Robot Car)

Arduino Mega 2560

L298N Motor Driver

4× DC Gear Motors (TT yellow motors)

NRF24L01 module (with capacitor)

HC-SR04 ultrasonic sensor

SG90 micro-servo for scanning

Active buzzer

Power: 2× 18650 Li-Ion cells (7.4V) or other high-current supply

 Wiring & Pinout
Receiver (Arduino Mega 2560)
Component	Pin Name	Arduino Pin	Note
L298N	IN1 / IN2	D5 / D4	Left motors (PWM)
L298N	IN3 / IN4	D3 / D2	Right motors (PWM)
NRF24L01	CE / CSN	D7 / D11	Radio control
NRF24L01	SCK / MOSI / MISO	52 / 51 / 50	Hardware SPI
NRF24L01	VCC / GND	3.3V / GND	Never use 5V
Servo	Signal	A0	Sensor scanning
HC-SR04	Trig / Echo	A3 / A4	Distance measurement
Buzzer	VCC	D6	Positive pin
Transmitter (Arduino Nano / Uno)
Component	Pin Name	Arduino Pin
NRF24L01	CE / CSN	D7 / D8
NRF24L01	SCK / MOSI / MISO	D13 / D11 / D12
MPU6050	SDA / SCL	A4 / A5
📚 Software Dependencies

Install these via Arduino IDE → Library Manager:

RF24 by TMRh20

MPU6050_tockn

Servo (built-in)

SPI (built-in)

Wire (built-in)

 Configuration & Tuning
Transmitter Sensitivity

Inside the transmitter code:

int deadzoneAccel = 20;   // Tilt needed to start moving
int maxAccelAngle = 45;   // Tilt angle for max speed
int minPWM = 85;          // Minimum motor power
int maxPWM = 255;         // Maximum speed

Fixing Direction Inversion

If forward/backward or steering feels reversed:

float dirFataSpate = -1.0;      // Set to 1.0 if inverted
float dirStangaDreapta = -1.0;  // Set to 1.0 if inverted

 How to Run

Assemble all hardware
Follow the wiring tables above. Ensure common ground between modules.

Install all required libraries

Upload the code

Upload Transmitter_Code.ino to the glove Arduino

Upload Receiver_Code.ino to the Arduino Mega
(Disconnect the NRF24 modules if uploading causes errors)

Calibrate the glove

Place on a flat surface

Reset Arduino

Wait ~3 seconds for MPU6050 calibration

Drive!

Tilt forward → accelerate

Tilt backward → reverse

Tilt left/right → steer

 Troubleshooting
Robot stutters or resets

NRF24L01 has voltage spikes → Add 10µF or 100µF capacitor on VCC/GND.

Motors spin the wrong direction

Do not rewire.
Adjust direction variables in code or swap logical pins in your motor function.

Buzzer beeps constantly

Ultrasonic sensor sees an object <15 cm.
Clear the path.

Robot doesn’t move at low tilt

Increase:

minPWM = 90;

 License

This project is open-source.
Feel free to modify, extend, and contribute!
