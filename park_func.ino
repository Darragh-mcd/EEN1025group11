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







