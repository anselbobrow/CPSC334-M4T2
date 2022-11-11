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
#define CURTAIN_PIN_2 21
#define BUTTON 14

// initialize motors
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
Servo cloudMotor;
Servo curtainMotor1;
Servo curtainMotor2;

// variables for movement
int distance = 50;
int speed = 5;
int cloudPos = 0;
int mapSpeed;
unsigned long start;
unsigned long curtain_movement; 
unsigned long sun_moving;
unsigned long offset;

// states
int on = 0;

// multithreading
TaskHandle_t clouds;
TaskHandle_t stars;

void setup() {
  myStepper.setSpeed(5);
  pinMode(BUTTON, INPUT);
  cloudMotor.attach(CLOUD_PIN);
  curtainMotor1.attach(CURTAIN_PIN_1);
  curtainMotor2.attach(CURTAIN_PIN_2);
  Serial.begin(115200);

  // https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/ 
    // multithreading for task 0 (rotating stepper)
  xTaskCreatePinnedToCore(
      stageLeft, /* Function to implement the task */
      "stars", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      tskIDLE_PRIORITY,  /* Priority of the task */
      &stars,  /* Task handle. */
      0); /* Core where the task should run */

  // multithreading for task 1 (rotating servo)
  xTaskCreatePinnedToCore(
      stageRight, /* Function to implement the task */
      "clouds", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &clouds,  /* Task handle. */
      1); /* Core where the task should run */

  // reset motors
  curtainMotor1.write(0);
  curtainMotor2.write(180);
}

void loop() {} 

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
void stageRight(void * parameter) {
  // includes stars and curtain motor 2
    // curtain angles: open = 180, closed = 0
  
  int timing = 14000;
  while (true) { 
    int button = digitalRead(BUTTON);
    if (button == HIGH && on == 0) {
      on = 1;
      start = millis();
    } else if (on == 1) {
      Serial.println(millis() - start);
      if (millis() - start > 500) {
        curtain_movement = millis();
        on = 2; // moving curtains  
      } else {
        cloudMove(0, 5);
        cloudMove(50, 5);
      }
    } else if (on == 2) {
      if (millis() - curtain_movement > 2000) {
        curtainMotor2.write(0); 
        curtainMotor1.write(180); 
        on = 3;
        sun_moving = millis();
      } else {
        cloudMove(0, 5);
        cloudMove(50, 5);
      }
    } else if (on == 3) {
      cloudMove(0, 5);
      cloudMove(50, 5);
      if (millis() - sun_moving > timing) {
        on = 4; 
        curtain_movement = millis();
        if (millis() - sun_moving > timing + 300) {
          curtainMotor2.write(180);
          curtainMotor1.write(0);
        }
      }
    } else if (on == 4) {
      if (millis() - curtain_movement > 3000) {
        on = 5;
        curtainMotor2.write(0);
        curtainMotor1.write(180);
        sun_moving = millis();
      }
      else {
        cloudMove(0, 5);
        cloudMove(50, 5);
      }
    } else if (on == 5) {
      if (millis() - sun_moving > timing) {
        on = 0;
        curtainMotor2.write(180);
        curtainMotor1.write(0);
      } else {
        cloudMove(0, 5);
        cloudMove(50, 5);
      }
    }
  }
}

void stageLeft(void *parameter) {
  // includes clouds and curtain motor 1
    // curtain angles: open = 0, closed = 180
  
  int open = 1;
  bool offset_bool = false;
  while (true) {
    Serial.println(on);
    if (on == 1 || on == 2 || on == 3 || on == 4) {
      myStepper.step(stepsPerRevolution);  
    }
  }
}
