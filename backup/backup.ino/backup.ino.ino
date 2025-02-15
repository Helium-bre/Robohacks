#include <Servo.h>  // Include the Servo library

Servo myServo;  // Create a servo object

#define cout 12
#define cs0 2 
#define cs0 A4
#define cs1 5
#define cs2 3
#define cs3 4
#define irLeft A3
#define irRight A2




enum colorEnum {
  BLACK,
  RED,
  BLUE,
  GREEN,
  MIXED,
  WHITE,
}colorEnum;

typedef struct color {
  char* name;
  enum colorEnum colEnum;
  int s2;
  int s3;
  int min;
  int max;
  int value;
} color;

// Define colors
color reds = {"red",RED,LOW,LOW,12,100,0};
color blues = {"blue",BLUE,LOW,HIGH,12,70,0};
color greens = {"green",GREEN,HIGH,HIGH,14,120,0};
color colorList[] = {reds, blues, greens};

// IR data
int irmove = 0;
int irdata = 0;

// Motor pins
int ena = 5; // enable A = controle de vitesse du moteur gauche
int enb = 6; // enable B = controle de vitesse du moteur droit
int leftForward = 8;
int leftReverse = 11;        // BRO JUST USE #DEFINE WTF IS THAT
int RightReverse = 9;      // WHY IS RIGHT UPPERCASE AND LEFT LOWERCASEEEE
int RightForward = 7;

int enaSpeed = 70;
int enbSpeed = 90;

bool motorEnabled = false;  // Flag to track motor state
int dropseed = 0;
void setup() {
  pinMode(cs0, OUTPUT);
  pinMode(cs1, OUTPUT);
  pinMode(cs2, OUTPUT);
  pinMode(cs3, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(RightForward, OUTPUT);
  pinMode(leftForward, OUTPUT);
  pinMode(RightReverse, OUTPUT);
  pinMode(leftReverse, OUTPUT);

  myServo.attach(2); // definition du servo sur le port DIGITAL 2
  Serial.begin(9600);

    digitalWrite(RightForward, HIGH); // mettre en marche le moteur droit 
    digitalWrite(leftForward, HIGH); // mettre en marche le moteur gauche



}

color calibrate(color c) {
  char command;
  int maxVal = 0;
  int minVal = 255;
  
  Serial.println("Reading max, press 'B' to stop");
  
  while (true) {
    if (Serial.available() > 0) {
      command = Serial.read();
      if (command == 'B') break;
    }
    
    int val = setBound(c);
    if (val > maxVal) maxVal = val;
  }
  
  c.max = maxVal;
  Serial.print("Max: ");
  Serial.println(c.max);
  Serial.print("Setup for measuring min. Press 'C' when ready");
  while (true) {
    if (Serial.available() > 0) {
      command = Serial.read();
      if (command == 'C') break;
    }
  }
  Serial.println("Reading min, press 'W' to stop");
  while (true) {
    if (Serial.available() > 0) {
      command = Serial.read();
      if (command == 'W') break;
    }
    
    int val = setBound(c);
    if (val < minVal) minVal = val;
  }

  c.min = minVal;
  Serial.print("Min: ");
  Serial.println(c.min);
  
  return c;
}

int setBound(color c) {
  digitalWrite(cs2, c.s2);
  digitalWrite(cs3, c.s3);
  return pulseIn(cout, LOW);
}

enum colorEnum getColor() {
  enum colorEnum colorDetected = BLACK;

  for (int i = 0; i < 3; i++) {
    digitalWrite(cs2, colorList[i].s2);
    digitalWrite(cs3, colorList[i].s3);

    colorList[i].value = constrain(map(pulseIn(cout, LOW), colorList[i].min, colorList[i].max, 255, 0), 0, 255);
    
    if ((colorList[i].value > 200 && colorList[i].colEnum != BLUE) || (colorList[i].value > 245 && colorList[i].colEnum == BLUE)) {
      if (colorDetected == BLACK) {
        colorDetected = colorList[i].colEnum;
      } else if (colorDetected == MIXED) {
        colorDetected = WHITE;
      } else {
        colorDetected = MIXED;
      }
    }
  }
  
  return colorDetected;
}

void loop() {
  digitalWrite(cs0, HIGH); // set frequency of color sensor to 100% DO NOT TOUCH I DON'T KNOW WHAT THAT SHIT DOES
  digitalWrite(cs1, HIGH);
  digitalWrite(leftForward,HIGH);
  digitalWrite(RightForward,HIGH);
  enum colorEnum detectedColor = getColor();
  if ((detectedColor == BLUE || detectedColor == GREEN) && dropseed == 0){ // if green or blue thingy
    dropseed = 1;
    digitalWrite(leftForward,LOW);
    digitalWrite(leftReverse,LOW);
    digitalWrite(RightForward,LOW);
    digitalWrite(RightReverse,LOW);
    servoPush();
  }
  else {
    if (!(detectedColor == BLUE || detectedColor == GREEN)){
      dropseed = 0;
    }
    followTrack();
  }
  //digitalWrite(RightForward, HIGH);
  //digitalWrite(leftForward, HIGH);

  
    /*
    digitalWrite(leftForward, LOW);
    digitalWrite(RightForward, LOW);
    digitalWrite(RightReverse, LOW);
    digitalWrite(leftReverse, LOW);
    */

    //servoPush();

  // Handle Serial Commands


  if (Serial.available() > 0) { // wtf is this my brother in Christ
    char command = Serial.read();
    
    if (command == 'R') {
      Serial.println("Red calibration");
      reds = calibrate(reds);
    } else if (command == 'G') {
      Serial.println("Green Calibration");
      greens = calibrate(greens);
    } else if (command == 'B') {
      Serial.println("Blue Calibration");
      blues = calibrate(blues);
    }
  }

// detect color type shit
  // enum colorEnum colorDetected = getColor();
  
  // if (colorDetected == WHITE) {
  //   Serial.println("White");
  // } else if (colorDetected == MIXED) {
  //   Serial.println("Mixed");
  // } else if (colorDetected == BLACK) {
  //   Serial.println("Black");
  // } else {
  //   for (int i = 0; i < 3; i++) {
  //     if (colorList[i].colEnum == colorDetected) {
  //       Serial.println(colorList[i].name);
  //     }
  //   }
  // }
// detect IR type shit
  // Serial.print("IR motion: ");
  // irmove = pulseIn(irLeft, LOW);
  // Serial.println(irmove);

  // Serial.print("IR presence: ");
  // irdata = digitalRead(irLeft);
  // Serial.println(irdata);

  delay(50); // delay b4 next iteration
  

}

void followTrack() { // I SPENT 2 HOURS DEBUGGING THIS MF BECAIUSE OF A FCKING IF ELSE STATEMENT
  int threshold = 96;
  int leftVal = analogRead(irLeft);
  //Serial.print(leftVal);
  //Serial.print(" <- Left \n");
  int rightVal = analogRead(irRight);
  //Serial.print(rightVal);
  //Serial.print(" <- Right \n");

  
  if ((leftVal < threshold) && (rightVal < threshold)){
    analogWrite(ena, enaSpeed);
    analogWrite(enb, enbSpeed);
    
  }

  else if (leftVal < threshold) {
    // if the reading from the left IR is below the threshold, it means WHITE is detected
    // Steer right
      //analogWrite(ena, 255);
    // shortlty turn very high speed to trigger the motor
      //delay(1);
    digitalWrite(leftForward,HIGH);
    digitalWrite(leftReverse,LOW);
    digitalWrite(RightForward,HIGH);
    digitalWrite(RightReverse,LOW);
    analogWrite(ena, enaSpeed);
    analogWrite(enb, 0);
     // eteindre le moteur oppose pour augmenter la rotation
    Serial.println("steer right");
  }
  else if (rightVal < threshold) {
    // if the reading from the right IR is below the threshold, it means WHITE is detected
    // Steer left
    //analogWrite(enb, 255);
    // shortlty turn very high speed to trigger the motor
    //delay(1);
    //digitalWrite(leftForward,LOW);
    digitalWrite(leftForward,HIGH);
    digitalWrite(leftReverse,LOW);
    digitalWrite(RightForward,HIGH);
    digitalWrite(RightReverse,LOW);
    analogWrite(enb, enbSpeed);
    analogWrite(ena, 0);
    // eteindre le moteur oppose pour augmenter la rotation
    Serial.println("steer left");
  } 
  else {
    // Stop
    //Serial.print("forward");
    digitalWrite(leftForward,HIGH);
    digitalWrite(leftReverse,LOW);
    digitalWrite(RightForward,HIGH);
    digitalWrite(RightReverse,LOW);
    analogWrite(ena, enaSpeed);
    analogWrite(enb, enbSpeed);
    // maintenir une vitesse constante lorsque le robot est sur la ligne noire
  }
}

void servoPush() {
  //myServo.write(0);
  Serial.print("PUSH TYPE SHIT");
  delay(1000);
  //myServo.write(180);
}
