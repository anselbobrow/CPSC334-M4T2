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

//variable for noting differnt times of day in the sequence
int off = 2;

//motors
Stepper myStepper(stepsPerRev, IN1, IN3, IN2, IN4);
Servo myservo;
int angle = 0;

//tasks for multithreading
TaskHandle_t belt;
TaskHandle_t day;


void setup() {
  pinMode(BUTTON, INPUT);
  // set speed of stepper
  myStepper.setSpeed(15);
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

//sun servo code
void sun(void * parameter) {
  while(true){
    //
    int night = 0;
    int button = digitalRead(BUTTON);
    //if the day/night cycle is initiated
    if(off == 2 && button == HIGH){
      off = 0;
      delay(4000);
      night = 1;
    }
    //increment servo angle if daytime
    if (off == 0){
      angle++;
      delay(100);
      //move servo
      myservo.write(angle);
      if(angle == 180){
        off = 1;
      }
    }
    //nighttime
    else if (off == 1){
      if(night == 1){
        delay(5000);
        night = 0;
      }
      //reset sun - do day portion backwards
      angle--;
      delay(100);
      //move servo
      myservo.write(angle);
      //cycle is over
      if(angle == 0){
        off = 2;
      }
    }
  }
}

//moving the character
void conveyor(void * parameter){
  int night = 0;
  int nothing = 0;
  while(true){
    //initial nighttim and then daytime movement
    if(off == 0){
      if(night == 0){
        delay(4000);
        night = 1;
      }
      myStepper.step(stepsPerRev);
    }
    else if(off == 1 || off == 2){
      //nightime
      if(off == 1){
        if(night == 1){
          delay(4000);
          night = 0;
          myStepper.setSpeed(17);
        }
        //send character back to start
        myStepper.step(-stepsPerRev);
      }
      //day/night cycle is over
      while(off == 2){
        nothing++;
      }
      myStepper.setSpeed(15);
    }
  }
}
