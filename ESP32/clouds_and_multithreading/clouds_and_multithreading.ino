// https://randomnerdtutorials.com/esp32-stepper-motor-28byj-48-uln2003/
#include <Stepper.h>
#include <ESP32Servo.h>

const int stepsPerRevolution = 2048;

// ULN2003 Motor Driver Pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17
#define SERVO_PIN 26 

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
Servo servoMotor;
int distance = 50;
int speed = 5;
int pos = 0;
int mapSpeed;

TaskHandle_t clouds;

void setup() {
  myStepper.setSpeed(5);
  servoMotor.attach(SERVO_PIN);
  Serial.begin(115200);

  // multithreading for task 1 (rotating servo)
  xTaskCreatePinnedToCore(
      clouds, /* Function to implement the task */
      "clouds", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &clouds,  /* Task handle. */
      0); /* Core where the task should run */
}

void loop() {
  // step one revolution in one direction:
  myStepper.step(stepsPerRevolution);
}

// rotates servo to position at specified speed
int pos1 = 0;
void moveTo(int position, int speed) {
  mapSpeed = map(speed, 0, 30, 30, 0);
  if (position > pos) {
    for (pos = pos1; pos <= position; pos += 1) {
      servoMotor.write(pos);
      pos1 = pos;
      delay(mapSpeed);
    }    
  } else {
    for (pos = pos1; pos >= position; pos -= 1) {
      servoMotor.write(pos);
      pos1 = pos;
      delay(mapSpeed);
    }
  }
}

// clouds
void clouds( void * parameter) {
  for(;;) {
    moveTo(0, speed);
    moveTo(50, speed);
  }
}