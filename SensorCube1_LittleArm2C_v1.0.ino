// Created at Slant Robotics
// www.slantrobotics.com
// Updated on 2/15/2017

//This program allows control of the LittleArm 2C with the SensorCube
//This program utilizes proximity data from the SensorCube to control 
// the LittleArm 2C in 3 dimensions and the gripper.

#include <Servo.h>  //arduino library
#include <math.h>   //standard c library

#define PI 3.141

Servo baseServo;  
Servo shoulderServo;  
Servo elbowServo; 
Servo gripperServo;

struct jointAngle{
  int base;
  int shoulder;
  int elbow;
};

struct jointAngle desiredAngle; //desired angles of the servos

int desiredGrip;
int gripperPos;
int command;
int desiredDelay = 16;

int ready = 0;

//+++++++++++++++ULTRASONIC VARIABLES++++++++++++++++++++++++++++

//First Sensor Using the Native Board Plug
#define echoPin A2 // Echo Pin
#define trigPin A3 // Trigger Pin

// The Second Sensor using the spare pins on the board
#define echoPin2 A0
#define trigPin2 A1

#define IRSensorPin A6  

bool holder = 1;

//+++++++++++++++FUNCTION DECLARATIONS+++++++++++++++++++++++++++
int servoParallelControl (int thePos, Servo theServo );
int ultraSensor(int theEchoPin, int theTrigPin);
int IRSensor(int sensorPin);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void setup()
{ 
  Serial.begin(9600);
  baseServo.attach(5);        // attaches the servo on pin 5 to the servo object 
  shoulderServo.attach(4);
  elbowServo.attach(3);
  gripperServo.attach(2);
  
  Serial.setTimeout(50);      //ensures the the arduino does not read serial for too long
  Serial.println("started");
  baseServo.write(90);        //intial positions of servos
  shoulderServo.write(150);
  elbowServo.write(110);
  gripperServo.write(80);
  ready = 0;

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(IRSensorPin, INPUT);
  
} 

//primary arduino loop
void loop() 
{ 

  // Local Variables
  int maximumRange = 30;      // Maximum range needed
  int minimumRange = 5;       // Minimum range needed
  long readDistance1;         // the output distance from the sensor
  long readDistance2;         // the second output that controls the base rotation
  int maximumRangeIR = 500;   // Maximum range needed
  int minimumRangeIR = 250;   // Minimum range needed
  long readDistanceIR;        // the second output that controls the base rotation
  
  // read a usb command if available
  if (Serial.available()){
    ready = 1;
    desiredAngle.base = Serial.parseInt();
    desiredAngle.shoulder = Serial.parseInt();
    desiredAngle.elbow = Serial.parseInt();
    desiredGrip = Serial.parseInt();
    desiredDelay = Serial.parseInt();

    if(Serial.read() == '\n'){              // if the last byte is 'd' then stop reading and execute command 'd' stands for 'done'
        Serial.flush();                     //clear all other commands piled in the buffer
        Serial.print('d');                  //send completion of the command
    }
  }
  
  else{
   //read the distance read by the sensor
   desiredDelay = 16;
   readDistance1 = ultraSensor(echoPin, trigPin);
   readDistance2 = ultraSensor(echoPin2, trigPin2);
   readDistanceIR = analogRead(IRSensorPin);

   // Print Data is needed to reference
   // Serial.print("read distance 1 = ");
   //Serial.println(readDistance1);
   //Serial.print("read distance 2 =" );
   //Serial.println(readDistance2);
   //Serial.print("read distance IR =" );
   //Serial.println(readDistanceIR);
   //Serial.println("----------------");
   
   ready = 1;
   if (((readDistance1 > 3 )&& (readDistance1 < 30))){ 
      //move the elbow and shoulder some amount
      desiredAngle.elbow = map(readDistance1, 5, 30, 175, 70);
      desiredAngle.shoulder = map(readDistance1, 5, 30, 5, 100);
   }

   if ( (readDistance2 > 3 )&& (readDistance2 < 30) )  {  
      desiredAngle.base = map(readDistance2, 5, 30, 5, 175); 
   }

   if ( (readDistanceIR > minimumRangeIR )&& (readDistanceIR < maximumRangeIR) )  {  
      desiredGrip = map(readDistanceIR, minimumRangeIR, maximumRangeIR, 5, 110); 
   }
    
  } // end of ultrasonic else
  
  int status1 = 0;
  int status2 = 0;
  int status3 = 0;
  int status4 = 0;
  int done = 0 ; 
  
  while(done == 0 && ready == 1){  
    //move the servo to the desired position
    status1 = servoParallelControl(desiredAngle.base, baseServo, desiredDelay);
    status2 = servoParallelControl(desiredAngle.shoulder,  shoulderServo, desiredDelay);
    status3 = servoParallelControl(desiredAngle.elbow, elbowServo, desiredDelay);      
    status4 = servoParallelControl(desiredGrip, gripperServo, desiredDelay);  
    
    if (status1 == 1 & status2 == 1 & status3 == 1 & status4 == 1){
      done = 1;
    }      
  }// end of while
}

//++++++++++++++++++++++++++++++FUNCTION DEFINITIONS++++++++++++++++++++++++++++++++++++++++++

int servoParallelControl (int thePos, Servo theServo, int theSpeed ){
  
    int startPos = theServo.read();        //read the current pos
    int newPos = startPos;
    
    //define where the pos is with respect to the command
    // if the current position is less that the actual move up
    if (startPos < (thePos-5)){
          
       newPos = newPos + 1;
       theServo.write(newPos);
       delay(theSpeed);
       return 0;
           
    }
  
   else if (newPos > (thePos + 5)){
      newPos = newPos - 1;
      theServo.write(newPos);
      delay(theSpeed);
      return 0;  
    }  
    
    else {
        return 1;
    }  
   
} //end servo parallel control

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int ultraSensor(int theEchoPin, int theTrigPin){
  
   //this function calculates and returns the distance in cm
   long duration, distance; // Duration used to calculate distance
   /* The following trigPin/echoPin cycle is used to determine the
   distance of the nearest object by bouncing soundwaves off of it. */ 
   digitalWrite(theTrigPin, LOW); 
   delayMicroseconds(2); 
  
   digitalWrite(theTrigPin, HIGH);
   delayMicroseconds(10); 
   
   digitalWrite(theTrigPin, LOW);
   duration = pulseIn(theEchoPin, HIGH);
   
   //Calculate the distance (in cm) based on the speed of sound.
   distance = duration/58.2;  
   return distance;
}

int IRSensor(int sensorPin){
  
  int IRdist;
  //read the value on the sensor Pin (D12 if using Meped Board)
  IRdist = analogRead(sensorPin);
  return IRdist;
}
