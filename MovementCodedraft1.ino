const int Motor1_Pwm = 37;
const int Motor1_Ph = 38;
const int Motor2_Pwm = 39;
const int Motor2_ph = 20;

bool reversing = false; // State to track motor direction
unsigned long lastSwitchTime = 0; // Last time direction changed
const unsigned long switchInterval = 2000; // Interval between direction changes in ms

void setup() {
  Serial.begin(57600);
  pinMode(Motor1_Pwm, OUTPUT);
  pinMode(Motor1_Ph, OUTPUT); 
  pinMode(Motor2_Pwm, OUTPUT);
  pinMode(Motor2_ph, OUTPUT);
  
}
void loop() {
unsigned long currentTime = millis();
  
  // Check if it's time to switch direction
  if (currentTime - lastSwitchTime >= switchInterval) {
    reversing = !reversing; // Toggle the direction
    lastSwitchTime = currentTime; // Update the last switch time


  // Move the motors based on the current direction
  if (reversing) {
    reverse();
  } else {
    forward();
  }
}
void forward() {
  digitalWrite(Motor2_ph, HIGH);
  digitalWrite(Motor1_Ph, HIGH);
  analogWrite(Motor1_Pwm, 200);
  analogWrite(Motor2_Pwm, 200);

}
void reverse() {
  analogWrite(Motor1_Pwm, 200);
  analogWrite(Motor2_Pwm, 200);
  digitalWrite(Motor1_Ph, LOW);
  digitalWrite(Motor2_ph, LOW);

}

void halt() {
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  analogWrite(Motor1_Ph, LOW);
  analogWrite(Motor2_ph, LOW);
}
