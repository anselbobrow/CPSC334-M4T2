#include <Stepper.h>
#include <ESP32Servo.h>

const int stepsPerRev = 2048;

// ULN2003 Motor Driver Pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17
#define SERVO_PIN 26
#define BUTTON 27
#define SUN 33

int off = 2;

Stepper myStepper(stepsPerRev, IN1, IN3, IN2, IN4);
Servo myservo;
int angle = 0;


TaskHandle_t belt;
TaskHandle_t day;

void setup() {
  pinMode(BUTTON, INPUT);
  pinMode(SUN, OUTPUT);
  // set speed of stepper
  myStepper.setSpeed(17);
  //pin the servo is at
  myservo.attach(SERVO_PIN, 400, 2400);
  //set servo angle to 0
  if (myservo.read() != angle){
    myservo.write(angle);
  }
  Serial.begin(115200);
  // multithreading for task 1 (rotating stepper)
  xTaskCreatePinnedToCore(
      conveyor, /* Function to implement the task */
      "belt", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      tskIDLE_PRIORITY,  /* Priority of the task */
      &belt,  /* Task handle. */
      0); /* Core where the task should run */

      // multithreading for task 2 (rotating servo)
  xTaskCreatePinnedToCore(
      sun, /* Function to implement the task */
      "day", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &day,  /* Task handle. */
      1); /* Core where the task should run */

}

void loop(){}

void sun(void * parameter) {
  while(true){
    //Serial.println(off);
    int button = digitalRead(BUTTON);
    //Serial.println(button);
    if(off == 2 && button == HIGH){
      off = 0;
    }
    //increment servo angle
    if (off == 0){
      digitalWrite(SUN, LOW);
      angle++;
      delay(100);
      //move servo
      myservo.write(angle);
      if(angle == 180){
        off = 1;
      }
    }
    else if (off == 1){
      digitalWrite(SUN, HIGH);
      angle--;
      delay(100);
      //move servo
      myservo.write(angle);
      if(angle == 0){
        off = 2;
      }
    }
  }
}

void conveyor(void * parameter)
{
  int nothing;
  while(true){
    if(off == 0){
      myStepper.step(stepsPerRev);
    }
    else if(off == 1 || off == 2)
    {
      if(off == 1){
        myStepper.step(-stepsPerRev);
      }
      while(off == 2)
      {
        nothing++;
      }
    }
  }
}
