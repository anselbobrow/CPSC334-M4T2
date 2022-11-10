// https://randomnerdtutorials.com/esp32-stepper-motor-28byj-48-uln2003/
#include <Stepper.h>
#include <ESP32Servo.h>

const int stepsPerRevolution = 2048;

// ULN2003 Motor Driver Pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17
#define CLOUD_PIN 26 
#define CURTAIN_PIN_1 22

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
Servo cloudMotor;
Servo curtainMotor1;
int distance = 50;
int speed = 5;
int cloudPos = 0;
int curtainPos1 = 0;
int mapSpeed;

TaskHandle_t clouds;

void setup() {
  myStepper.setSpeed(5);
  cloudMotor.attach(CLOUD_PIN);
  curtainMotor1.attach(CURTAIN_PIN_1);
  Serial.begin(115200);

  // multithreading for task 1 (rotating servo)
  // https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/ 
  xTaskCreatePinnedToCore(
      cloudsFunction, /* Function to implement the task */
      "clouds", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &clouds,  /* Task handle. */
      0); /* Core where the task should run */

      Serial.println(xPortGetCoreID());

  for (int i = 0; i < 360; i += 1) {
    curtainMotor1.write(i);
    delay(10);
  }
}

void loop() {
  // step one revolution in one direction:
  // myStepper.step(stepsPerRevolution);
  // Serial.println(xPortGetCoreID());

  // curtain 1

  // moveTo(0, speed);
  // moveTo(50, speed);
}

// rotates servo to position at specified speed
int pos1;
void cloudMove(int position, int speed) {
  mapSpeed = map(speed, 0, 30, 30, 0);
  if (position > cloudPos) {
    for (cloudPos = pos1; cloudPos <= position; cloudPos += 1) {
      cloudMotor.write(cloudPos);
      pos1 = cloudPos;
      delay(mapSpeed);
    }    
  } else {
    for (cloudPos = pos1; cloudPos >= position; cloudPos -= 1) {
      cloudMotor.write(cloudPos);
      pos1 = cloudPos;
      delay(mapSpeed);
    }
  }
}

// clouds
void cloudsFunction( void * parameter) {
  for(;;) {
    cloudMove(0, 5);
    cloudMove(50, 5);
  }
}
