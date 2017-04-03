
// training program with gripper button working.

#include <Servo.h>  //arduino library
#include <math.h>   //standard c library

Servo baseServo;  
Servo shoulderServo;  
Servo elbowServo; 
Servo gripperServo;

int command;

struct jointAngle{
  int base;
  int shoulder;
  int elbow;
};

int desiredGrip = 30;

int basePotPin = A0;
int shoulderPotPin = A1;
int elbowPotPin = A2; 
int gripPin = A3;

int desiredDelay = 5;

int potValHolder1;
int potValHolder2;
int potValHolder3;

int recordPin = 2;

bool gripped = 1;

bool holder = 1;

struct jointAngle desiredAngle; //desired angles of the servos

//+++++++++++++++FUNCTION DECLARATIONS+++++++++++++++++++++++++++

int servoParallelControl (int thePos, Servo theServo );
int potRead (int potPin);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void setup()
{ 
  Serial.begin(9600);
  baseServo.attach(5);        // attaches the servo on pin 5 to the servo object 
  shoulderServo.attach(4);
  elbowServo.attach(3);
  gripperServo.attach(2);
  
  Serial.setTimeout(50);      //ensures the the arduino does not read serial for too long
  //Serial.println("started");
  baseServo.write(90);        //intial positions of servos
  shoulderServo.write(150);
  elbowServo.write(110);

  pinMode(gripPin, INPUT);
  pinMode(recordPin, INPUT);
  
} 

//primary arduino loop
void loop() 
{ 

    //Serial.println("++++++++++++++++++++++++++++++++++++++++");
    // Take average of the value
    potValHolder1 = potRead(basePotPin);
    if (potValHolder1 < 500){
        desiredAngle.base = map(potValHolder1, 1, 50, 175, 5);
        if (desiredAngle.base > 175) desiredAngle.base = 175;
        if (desiredAngle.base < 5) desiredAngle.base = 5;
    }

    
    potValHolder2 = potRead(shoulderPotPin);
    if (potValHolder2 < 500){
       desiredAngle.shoulder = map(potValHolder2, 100, 255, 175, 5);
     
       if (desiredAngle.shoulder > 175) desiredAngle.shoulder = 175;
       if (desiredAngle.shoulder < 5) desiredAngle.shoulder = 5;
         //Serial.println("shoulder+++++++++++++++++++++++++++++++++++++++++++++");
      // Serial.println(desiredAngle.shoulder);
    }

    
    potValHolder3 = potRead(elbowPotPin);
    if (potValHolder3 < 500){
       desiredAngle.elbow = map(potValHolder3, 1, 180, 175, 5);
       if (desiredAngle.elbow > 175) desiredAngle.shoulder = 175;
       if (desiredAngle.elbow < 5) desiredAngle.shoulder = 5;
    }


    //Serial.println("shoulder");
     //Serial.println(desiredAngle.shoulder);
      //Serial.println(desiredAngle.elbow);


    // read the gripper button to define position. 
    // when the button is pressed the gripper changes positions
    if ((digitalRead(gripPin) == HIGH) ){
      delay(100); // ensure that the signal is not random
      if ((digitalRead(gripPin) == HIGH) ){
        gripped = !gripped;    
  
        if (gripped == 1){
          desiredGrip = 3;
        }
        if (gripped == 0){
          desiredGrip = 75;
        }
      }
    }

  int status1 = 0;
  int status2 = 0;
  int status3 = 0;
  int status4 = 0;
  int done = 0 ; 
  
  while( done == 0){  
    //move the servo to the desired position
    //Serial.println("hello");
    status1 = servoParallelControl(desiredAngle.base, baseServo, desiredDelay);
    status2 = servoParallelControl(desiredAngle.shoulder,  shoulderServo, desiredDelay);
    status3 = servoParallelControl(desiredAngle.elbow, elbowServo, desiredDelay);      
    status4 = servoParallelControl(desiredGrip, gripperServo, desiredDelay);  
    
    if (status1 == 1 & status2 == 1 & status3 == 1 & status4 == 1){
      done = 1;
      
    }
        
  }// end of while
  
  //delay(500);
  

}

//++++++++++++++++++++++++++++++FUNCTION DEFITNITIONS++++++++++++++++++++++++++++++++++++++++++

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
    
}

int potRead (int potPin){
    // Take average of the value
    int potValHolder = 0;
    int basicRead = 0;
    int rightShift = 0;
    int averageReading = 0;
    
    for (int i = 0; i< 25; i++){
      basicRead = analogRead(potPin); //read the potentiometer
      rightShift = basicRead >> 2;    // right shift the value
      potValHolder += rightShift;      // add to cumulative value
      
    }
    // final average
    averageReading = potValHolder/25;

    //print the outcome
    // Serial.println("++++++");
    //Serial.println(averageReading);
    //Serial.println("-----");
    
    return averageReading;

}


