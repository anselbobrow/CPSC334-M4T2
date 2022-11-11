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
int curtainPos1 = 0;
int mapSpeed;

// states
int on = 0;

// multithreading
TaskHandle_t clouds;

void setup() {
  myStepper.setSpeed(5);
  pinMode(BUTTON, INPUT);
  cloudMotor.attach(CLOUD_PIN);
  curtainMotor1.attach(CURTAIN_PIN_1);
  curtainMotor2.attach(CURTAIN_PIN_2);
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

  // reset motors
  curtainMotor1.write(0);
  curtainMotor2.write(0);
}

void loop() {
  // read start button
  int button = digitalRead(BUTTON);
  if (button == LOW && on == 0) {
    on = 1;
  } else if (on == 1) {
    long int start = millis();
    // wait 3 seconds in night mode
    while (millis() - start < 3000) {
        myStepper.step(stepsPerRevolution);
    }
    on = 2;

    // close curtains for day mode
    curtainMotor1.write(180);
  } 


  // step one revolution in one direction:
  // myStepper.step(stepsPerRevolution);

  // Serial.println(state);
  // while (state == "closed") {
  // curtain 1
    // curtainMotor1.write(180);
    // delay(1000);
    // curtainMotor1.write(0);
    // delay(1000);
    // curtainMotor1.write()
  // }

  

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
  if (on == 1) {
    long int start = millis();
    while (millis() - start < 3000) {
    //   Serial.println(millis(), start);
      cloudMove(0, 5);
      cloudMove(50, 5);
    //   curtainMotor2.write(0);
    //   delay(2000);
    //   curtainMotor2.write(180);
    //   delay(2000);
    }

    // curtain 2
    // state = "closed";
    curtainMotor2.write(180);
    delay(2000);

    for (;;) {
      cloudMove(0, 5);
      cloudMove(50, 5);   
      // curtainMotor2.write(0);
      // delay(2000);
      // curtainMotor2.write(180);
      // delay(2000);     
    }
  }
}
