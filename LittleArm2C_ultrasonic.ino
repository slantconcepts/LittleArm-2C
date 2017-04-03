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

#define echoPin A0 // Echo Pin
#define trigPin A1 // Trigger Pin


bool holder = 1;
int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long readDistance; // the output distance from the sensor


//+++++++++++++++FUNCTION DECLARATIONS+++++++++++++++++++++++++++
int servoParallelControl (int thePos, Servo theServo );
int ultraSensor(int theEchoPin, int theTrigPin);
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
  
} 

//primary arduino loop
void loop() 
{ 

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
   desiredDelay = 3;
   readDistance = ultraSensor(echoPin, trigPin);
   Serial.println(readDistance);
   ready = 1;
   if ((readDistance > 3 )&& (readDistance < 30)){
      Serial.println("Hello");
      //move the elbow and shoulder some amount
      desiredAngle.elbow = map(readDistance, 5, 30, 175, 70);
      desiredAngle.shoulder = map(readDistance, 5, 30, 5, 100);
      desiredAngle.base = 90;
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
    //int theSpeed = speed;
    
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
   //this fucntion caluclates and returns the distance in cm
    
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
