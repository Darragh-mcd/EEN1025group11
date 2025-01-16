const int Motor1_Pwm = 37;
const int Motor1_Ph = 38;
const int Motor2_Pwm = 39;
const int Motor2_Ph = 20;
const int brownsensor = 4;
const int redsensor = 5;
const int orangesensor = 6;
const int yellowsensor = 7;
const int greensensor = 15;
int brown_sensor = 0;
int green_val = 0;
int red_val = 0;
int orange_val = 0;
int yellow_val = 0;
const int sensorPins[5] = {brownsensor, redsensor, orangesensor, yellowsensor, greensensor};
const int default_speed = 100;
const int turning_speed = 150;
const int turning_tight_speed = 225;
const int white_thresh = 1000;


bool reversing = false; // State to track motor direction
unsigned long lastSwitchTime = 0; // Last time direction changed
const unsigned long switchInterval = 2000; // Interval between direction changes in ms
//BROYG


void setup() {
  Serial.begin(57600);
  //pinMode(H1_H2_ENABLE, OUTPUT);
  pinMode(Motor1_Pwm, OUTPUT);
  pinMode(Motor1_Ph, OUTPUT);
  pinMode(Motor2_Pwm, OUTPUT);
  pinMode(Motor2_Ph, OUTPUT);
  pinMode(redsensor, INPUT);
  pinMode(orangesensor, INPUT);
  pinMode(yellowsensor, INPUT);
  pinMode(brownsensor, INPUT);
  pinMode(greensensor,INPUT);
}
void loop() {
  int count = 0;
  /*bool sensors[5] ={0,0,0,0,0};
  for (int ii = 0; ii < 5; ii++) {
    sensors[ii] = analogRead(sensorPins[ii]);
  }*/
  uint red_val = analogRead(redsensor);
  uint orange_val = analogRead(orangesensor);
  uint yellow_val = analogRead(yellowsensor);
  uint brown_val = analogRead(brownsensor);
  uint green_val = analogRead(greensensor);




    if(red_val == HIGH && orange_val == LOW && yellow_val == HIGH){
    forward();
    }
  //if(red_val ==LOW && yellow_val == LOW){
  //  halt();
  //}
  if(red_val <= white_thresh && yellow_val >= white_thresh){
    left();
      }
  if(red_val >= white_thresh && yellow_val <= white_thresh){
    right();
  }
  if(orange_val >= white_thresh && red_val <= white_thresh){
    left();
  }
  if(orange_val >= white_thresh && yellow_val <= white_thresh){
    right();
  }
  if (brown_val <= white_thresh && green_val >= white_thresh){
    tight_left();
  }
  if (brown_val >= white_thresh && green_val <= white_thresh){
    tight_right();
  }
  if(orange_val <= white_thresh && yellow_val <= white_thresh && red_val <= white_thresh && green_val <= white_thresh && brown_val <= white_thresh){
    count += 1;
    if (count == 1)
      perform180();
    if (count == 2)
      perform90R();
    if (count == 3)
      perform180();
    if (count == 4)
      perform90L();  
    }
  }




void reverse() {
  digitalWrite(Motor2_Ph, HIGH);
  digitalWrite(Motor1_Ph, HIGH);
  analogWrite(Motor1_Pwm, default_speed);
  analogWrite(Motor2_Pwm, default_speed);




}
void forward() {
  analogWrite(Motor1_Pwm, default_speed);
  analogWrite(Motor2_Pwm, default_speed);
  digitalWrite(Motor1_Ph, LOW);
  digitalWrite(Motor2_Ph, LOW);




}




void left() {
  analogWrite(Motor1_Pwm, default_speed);
  digitalWrite(Motor1_Ph, LOW);
  digitalWrite(Motor2_Ph, LOW);
  analogWrite(Motor2_Pwm, turning_speed);
}
void tight_left(){
  analogWrite(Motor1_Pwm, 0);
  digitalWrite(Motor1_Ph, LOW);
  digitalWrite(Motor2_Ph, LOW);
  analogWrite(Motor2_Pwm, turning_tight_speed);
}






void right(){
  analogWrite(Motor1_Pwm, turning_speed);
  digitalWrite(Motor1_Ph,LOW);
  digitalWrite(Motor2_Ph,LOW);
  analogWrite(Motor2_Pwm, default_speed);
}
void tight_right(){
  analogWrite(Motor1_Pwm, turning_tight_speed);
  digitalWrite(Motor1_Ph,LOW);
  digitalWrite(Motor2_Ph,LOW);
  analogWrite(Motor2_Pwm, 0);
}
void halt() {
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  analogWrite(Motor1_Ph, HIGH);
  analogWrite(Motor2_Ph, HIGH);
}
void perform180(){
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  delay(1000);
  digitalWrite(Motor1_Ph, LOW);
  digitalWrite(Motor2_Ph, HIGH);
  analogWrite(Motor1_Pwm, 150);
  analogWrite(Motor2_Pwm, 150);
  delay(1000);
}
void perform90L(){
  forward();
  delay(200);
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  delay(1000);
  digitalWrite(Motor1_Ph, LOW);
  digitalWrite(Motor2_Ph, HIGH);
  analogWrite(Motor1_Pwm, 150);
  analogWrite(Motor2_Pwm, 150);
  delay(500);
}
void perform90R(){
  forward();
  delay(200);
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  delay(1000);
  digitalWrite(Motor1_Ph, HIGH);
  digitalWrite(Motor2_Ph, LOW);
  analogWrite(Motor1_Pwm, 150);
  analogWrite(Motor2_Pwm, 150);
  delay(500);
}



