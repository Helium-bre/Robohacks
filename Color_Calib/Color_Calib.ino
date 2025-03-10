//#include <Servo.h>  // Include the Servo library

//Servo myServo;  // Create a servo object

#define cout 12

#define cs0 2 

#define cs0 A4

#define cs1 5
#define cs2 3
#define cs3 4
#define ir1 A2
#define ir2 A3




enum colorEnum{
  BLACK,
  RED,
  BLUE,
  GREEN,
  MIXED,
  WHITE,
}colorEnum;


typedef struct color{
  char* name;
  enum colorEnum colEnum;
  int s2;
  int s3;
  int min;
  int max;
  int value;
} color;


typedef struct pid{
  int leftSpeed;
  int rightSpeed;
  int error;
  int integral;
}pid;


// int red = 0;
// int blue = 0;
// int green = 0;

color reds = {"red",RED,LOW,LOW,12,100,0};
color blues = {"blue",BLUE,LOW,HIGH,12,70,0};
color greens = {"green",GREEN,HIGH,HIGH,14,120,0};

color colorList[] = {reds,blues,greens};


//ir data
int irmove = 0;
int irdata = 0;

int ena = 9;
int enb =10;

int leftForward = 11;
int leftReverse = 8;
int RightReverse = 6;
int RightForward = 7;


void setup() {
  // put your setup code here, to run once:
  pinMode(cs0, OUTPUT);
  pinMode(cs1,OUTPUT);
  pinMode(cs2, OUTPUT);
  pinMode(cs3, OUTPUT);
  pinMode(ena,OUTPUT);
  pinMode(RightForward,OUTPUT);
  pinMode(leftForward,OUTPUT);
  //myServo.attach(2);
  Serial.begin(9600);
}




color calibrate(color c ){
  // calibrate a color (ISSUE WITH BLUE)
  char command;
  int max = 0;
  int min = 255;
  Serial.print("Reading max, press 'B' to stop\n");
  while( command != 'B'){
    command = Serial.read();
    int val = setBound(c);
    if (val > max){
      max = val;
    }
  }
  c.max = max;
  Serial.print(c.max);
  Serial.print("Reading of max done\n ");
  while ( command != 'C'){
    command = Serial.read();
  }
  Serial.print("Reading of min started. Press 'W' to stop \n");
  while ( command != 'W'){
    command = Serial.read();
    
    int val = setBound(c);
    if (val < min){
      min = val;
    }
  }
  c.min = min;
  Serial.print(c.min);
  return c;
}

int setBound(color c){
  
  digitalWrite(cs2, c.s2);
  digitalWrite(cs3, c.s3);
  return pulseIn(cout,LOW);
}



enum colorEnum getColor(){
  // get the detected color (MORE TUNING REQUIRED)
  enum colorEnum colorDetected = BLACK;   // enum defined at the beginning of the program

  for (int i = 0; i < 3; i ++){
    digitalWrite(cs2, colorList[i].s2); // cs2 and cs3 are S2 and S3 ports of the color sensor
    digitalWrite(cs3, colorList[i].s3);
   
    colorList[i].value = constrain(map(pulseIn(cout, LOW),colorList[i].min,colorList[i].max,255,0),0,255); // colorList is the list of colors detected by the sensor ( defined at the beginning of the program)
    color c = colorList[i];
    //Serial.print('\n');
    Serial.print("/  ");
    Serial.print(c.name);
    Serial.print(" =   ");
    Serial.print(c.value);
    if ((c.value > 200 )){ //&& c.colEnum != BLUE) || (c.value > 225 && c.colEnum == BLUE)){

      if (colorDetected == BLACK){
        colorDetected = c.colEnum;
        
      }
      else if (colorDetected == MIXED){
        colorDetected = WHITE;
        
      }
      else {
        colorDetected = MIXED;
        
      }
      
    }
    
  }
  return colorDetected;
}



// pid PID(pid lastPID){
//   int mleft = MOTOR1; // CHANGE TO ACTUAL PIN
//   int mright = MOTOR2; // CHANGE TO ACTUAL PIN
//   int error = lastPID.left - lastPID.right;

//   lastPID.integral += error;
//   int derivative = error - lastPID.error;
//   float correction =  (Kp * error) + (Ki * integral) + (Kd * derivative) // Cstes are defined at the beginning of the program
//   lastPID.leftSpeed = constrain(basespeed  - correction,0,255);
//   lastPID.rightSpeed = constrain(basespeed + correction,0,255);
//   analogWrite(mleft, lastPID.leftSpeed);
//   analogWrite(mright, lastPID.rightSpeed);
//   return lastPID;
// }




void loop() {
  digitalWrite(cs0, HIGH); // set frequency to 100 % DO NOT CHANGE. Maybe change it if  
  digitalWrite(cs1,HIGH);
  // analogWrite(enb,100);
  //digitalWrite(leftForward,HIGH);

  // followTrack();

  // input
  char command = Serial.read();
  if (command == 'R'){
    Serial.print("   Red calibration \n ");
    reds = calibrate(reds);
  }
  else if ( command == 'G' ){
    Serial.print(" Green Calibration");
    greens = calibrate(greens);
  }
  else if (command == 'B'){
    Serial.print(" Blue calibration");
    blues = calibrate(blues);
  }
  
  
  enum colorEnum colorDetected = getColor();
  Serial.print('\n');
  if (colorDetected == WHITE){
    Serial.print("White");
  }
  else if (colorDetected == MIXED){
    Serial.print("Mixed");
  }
  else if (colorDetected == BLACK){
    Serial.print("Black");
  }
  else {
    for (int i = 0; i < 3; i ++){
      if (colorList[i].colEnum == colorDetected){
        Serial.print(colorList[i].name);
      }
    }
  }
  
  Serial.print("   / IR  motion = ");
  irmove = pulseIn(ir1,LOW);
  Serial.print(irmove);
  Serial.print("   / IR  presence = ");
  irdata = digitalRead(ir1);
  Serial.print(irdata);

  Serial.print('\n');
  
  delay(200);
  
}

// void followTrack(){
//     delay(100);
//     enum colorEnum colorDetected = getColor();
//     if (colorDetected == WHITE){
//       Serial.println("White");
//     }
//     else if (colorDetected == MIXED){
//       Serial.println("Mixed");
//     }
//     else if (colorDetected == BLACK){
//       Serial.println("Black");
//     }
// }

// void servoPush(){
//  myServo.write(0);
//  delay(1000);
//  myServo.write(180);
// }