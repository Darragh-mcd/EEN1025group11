const int Parking_Thresh = 2500; // threshold for in parking level rmps up fairly quick
const int Parking_Sensor = 16; // analog pin set
int Parking_val  = 0; //
int Consecutive_Readings = 0;
void Park() {
  const int Required_Readings = 5;    
  Parking_val = analogRead(Parking_Sensor);


  if (Parking_val >= Parking_Thresh) {
    Consecutive_Readings++; // Increment counter for valid readings
    if (Consecutive_Readings >= Required_Readings) {
      halt(); // Stop the robot after multiple readings
    }
  } else {
    Consecutive_Readings = 0; 
    forward();            
  }
}







