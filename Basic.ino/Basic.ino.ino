#define cout 12
#define cs0s1 2
#define cs2 3
#define cs3 4
#define ir 11
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
};

int red = 0;
int blue = 0;


int green = 0;

color reds = {"red",RED,LOW,LOW,10,70,0};
color blues = {"blue",BLUE,LOW,HIGH,5,50,0};
color greens = {"green",GREEN,HIGH,HIGH,10,70,0};

color colorList[] = {reds,blues,greens};
int irmove = 0;
int irdata = 0;
void setup() {
  // put your setup code here, to run once:

  pinMode(cs0s1, OUTPUT);
  pinMode(cs2, OUTPUT);
  pinMode(cs3, OUTPUT);
  Serial.begin(9600);
}


color calibrate(color c ){
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



void loop() {
  digitalWrite(cs0s1, HIGH); // set frequency to 100 % DO NOT CHANGE

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

  enum colorEnum colorDetected = BLACK;

  for (int i = 0; i < 3; i ++){
    //color c = colorList[i];
    digitalWrite(cs2, colorList[i].s2);
    digitalWrite(cs3, colorList[i].s3);
    // Serial.print("  /  ");
    // Serial.print(c.name );
    // Serial.print(" = ");
    colorList[i].value = constrain(map(pulseIn(cout, LOW),colorList[i].min,colorList[i].max,255,0),0,255);
    color c = colorList[i];
    Serial.print(c.value);
    Serial.print('\n');
    if (c.value > 200){
      //Serial.print("Color detected");
      if (colorDetected == BLACK){
        colorDetected = c.colEnum;
        Serial.print("it is not black anymore    ");
        if (colorDetected == BLACK){
          Serial.print("error");
        }
      }
      else if (colorDetected == MIXED){
        colorDetected = WHITE;
        Serial.print("it is now white");
      }
      else {
        colorDetected = MIXED;
        Serial.print("It is now mixed");
      }
      Serial.print('\n');
    }
    //Serial.print(c.value);
  }
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
  Serial.print('\n');
  

  // digitalWrite(cs2, LOW);
  // digitalWrite(cs3, LOW);
  // Serial.print("    Red  value= ");
  // red = map(pulseIn(cout, LOW),reds.min,reds.max,255,0);
  // Serial.print(red);
  // digitalWrite(cs2, LOW);
  // digitalWrite(cs3, HIGH);
  // Serial.print("/  Blue  value= ");
  // blue = map(pulseIn(cout, LOW),blues.min,blues.max,255,0);
  // Serial.print(blue);
  // digitalWrite(cs2, HIGH);
  // digitalWrite(cs3, HIGH);
  // Serial.print("/  Green  value= ");
  // green = map(pulseIn(cout, LOW),blues.min,blues.max,255,0);
  // Serial.print(green);

  Serial.print("   / IR  motion = ");
  irmove = pulseIn(ir,LOW);
  Serial.print(irmove);
  Serial.print("   / IR  presence = ");
  irdata = digitalRead(ir);
  Serial.print(irdata);

  Serial.print('\n');
  
  delay(200);
}
