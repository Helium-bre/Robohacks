#define cout 12
#define cs0 2
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
};


typedef struct color{
  char* name;
  enum colorEnum colEnum;
  int s2;
  int s3;
  int min;
  int max;
  int value;
} color;



// int red = 0;
// int blue = 0;
// int green = 0;

color reds = {"red",RED,LOW,LOW,10,100,0};
color blues = {"blue",BLUE,LOW,HIGH,5,60,0};
color greens = {"green",GREEN,HIGH,HIGH,10,100,0};

color colorList[] = {reds,blues,greens};


//ir data
int irmove = 0;
int irdata = 0;



void setup() {
  // put your setup code here, to run once:
  pinMode(cs0, OUTPUT);
  pinMode(cs1,OUTPUT);
  pinMode(cs2, OUTPUT);
  pinMode(cs3, OUTPUT);
  Serial.begin(9600);
}


color calibrate(color c ){
  // calibrate a color (ISSUE WITH BLUE)
  char command ;
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
    if ((c.value > 200 && c.colEnum != BLUE) || (c.value > 245 && c.colEnum == BLUE)){

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
    return colorDetected;
  }
}


void loop() {
  digitalWrite(cs0, HIGH); // set frequency to 100 % DO NOT CHANGE. Maybe change it if  
  digitalWrite(cs1,HIGH);

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
  
  //

  enum colorEnum colorDetected = getColor();
  
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
