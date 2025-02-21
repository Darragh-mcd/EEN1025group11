/*
This code is designed to control a robotic vehicle using sensor inputs and motor outputs.
It reads values from multiple sensors to detect obstacles and navigate accordingly.
Predefined movement commands (FORWARD, REVERSE, RIGHT, LEFT, TURN_AROUND, PARK) are used
to guide the robot through its path. The code also incorporates logic for parking detection.
Unnecessary functions have been removed for efficiency, and comments have been added for clarity.
*/
//--------------------------------------------
#define FORWARD 1
#define REVERSE 2
#define RIGHT 3
#define LEFT 4
#define TURN_AROUND 5
#define PARK 6
//---------------------------------------------- Macros for using Switch cases


const int Motor1_Pwm = 37;
const int Motor1_Ph = 38;
const int Motor2_Pwm = 39;
const int Motor2_Ph = 20;
const int brownsensor = 4;
const int redsensor = 5;
const int orangesensor = 6;
const int yellowsensor = 7;
const int greensensor = 15;
const int parkingsensor = 16;


//--------------------------------------------------- pins


int rightmost_sensor = 0;
int left_sensor = 0;
int middle_sensor = 0;
int right_sensor = 0;
int Leftmost_sensor = 0;
int Parking_val = 0;
int sensor_sum = 0;
//--------------------------------------------------- sensor vals


int default_speed = 200;
const int turning_speed = default_speed+25;
const int turning_tight_speed = 255;
const int white_thresh = 1000;
const int Obstacle_thresh = 2000; // threshold for in parking level rmps up fairly quick
int Parking_Thresh = 2000;
int  consecutiveReadings = 0;
int  consecutiveReadings2 = 0;
int i = 0; // used to grab directions from array
//------------------------------------------------------ needed for logic checks

const int MAX = 8;
const int INF = 9999;
int nodeblocked = 0;
//L F R 180 L F 180 F Park

//-------------------------------------------------------------------------------------------- this is a hardcoded test route
//For Dijkstras 
int graph[MAX][MAX] = {
    { 0, INF, INF, INF, 4, INF, INF, 4 },    
    { INF, 0, INF, INF, INF, INF, 2, 4 },    
    { INF, INF, 0, 4, INF, INF, INF, 4 },    
    { INF, INF, 4, 0, INF, INF, 7, INF },    
    { 4, INF, INF, INF, 0, INF, 7, INF },    
    { INF, INF, INF, INF, INF, INF, 3, INF },  
    { INF, 2, INF, 7, 7, 3, 0, INF },      
    { 4, 4, 4, INF, INF, INF, INF, 0 }      
};
int externalsave[MAX][MAX] = {
    { 0, INF, INF, INF, 4, INF, INF, 4 },    
    { INF, 0, INF, INF, INF, INF, 2, 4 },    
    { INF, INF, 0, 4, INF, INF, INF, 4 },    
    { INF, INF, 4, 0, INF, INF, 7, INF },    
    { 4, INF, INF, INF, 0, INF, 7, INF },    
    { INF, INF, INF, INF, INF, INF, 3, INF },  
    { INF, 2, INF, 7, 7, 3, 0, INF },      
    { 4, 4, 4, INF, INF, INF, INF, 0 }      
};

  
int stage = 1;

int currentPathIndex = 0; // Index in the current path

//dynamic movement
typedef enum { NORTH = 0, EAST = 90, SOUTH = 180, WEST = 270 } Orientation;
Orientation CurrentOrientation = WEST; //Start coming from the west going to east
//BROYG
    int hardcodedPath[] = {0, 2, 4, 3 , 5,0};
    int numHardcoded = sizeof(hardcodedPath) / sizeof(hardcodedPath[0]);
    int fullPath[MAX * MAX];
    int fullPathSize = 0;

//BROYG (sensor layout)
        int current ;
        int next ;
        int previous ;
        int setupid = 0;
  int node_no = 1;

// this constant won't change. It's the pin number of the sensor's output:

long duration, inches, cm;
bool reachednode = true;
bool inside = true;
int consecutiveReadings3 = 0;
bool finishednode5 = true;
int turned_away = 0;
int blockednode;

void setup() {
  Serial.begin(57600);
  pinMode(Motor1_Pwm, OUTPUT);
  pinMode(Motor1_Ph, OUTPUT);
  pinMode(Motor2_Pwm, OUTPUT);
  pinMode(Motor2_Ph, OUTPUT);
  pinMode(redsensor, INPUT);
  pinMode(orangesensor, INPUT);
  pinMode(yellowsensor, INPUT);
  pinMode(brownsensor, INPUT);
  pinMode(greensensor,INPUT);
  pinMode(parkingsensor,INPUT);
  dijkstrasetup();
  for (int i = 0; i < fullPathSize; i++){ Serial.printf("\n%d",fullPath[i]);}
  
}
void dijkstrasetup(){
      // makes sure that everytime this is called the current node is always 0 and it doesnt for the rerouting
    if(setupid == 0){
      current = 0;
    }
    // Reset the fullPathSize to 0
    fullPathSize = 0;

    // Clear the fullPath array (optional, but good practice)
    for (int i = 0; i < MAX * MAX; i++) {
        fullPath[i] = 0; // Reset each element to 0 or any default value
    }

//clears subpath and connects the full route 
      for (int i = 0; i < numHardcoded - 1; i++) {
        int subPath[MAX];
        int subPathSize = 0;
        if(i == 0 ){
          dijkstra(graph, current, hardcodedPath[i +1], subPath, subPathSize);
        }else{dijkstra(graph, hardcodedPath[i - 1], hardcodedPath[i], subPath, subPathSize);}
        if (i > 0) fullPathSize--;
        for (int j = 0; j < subPathSize; j++) {
            fullPath[fullPathSize++] = subPath[j];
        }

    }
    currentPathIndex = 0;
    setupid++;
    for (int i = 0; i < fullPathSize; i++){ Serial.printf("\n%d",fullPath[i]);}
}

void loop() {
  while(finishednode5){
  while(reachednode){
  check_sensors();
  Adjustment();
  //checks and removes the nodes already to make the rerouting a lot easier 
  checkAndRemoveHardcodedNode(current);// do i need to unblock it? or would it make a difference ?
   if (isNodeBlocked()) {
        Serial.printf("\n%d Node %d is blocked",__LINE__, next); 
        perform180(); // Perform a 180-degree turn
        switchCurrentOrientation(); 
        Serial.printf("\n%d currentiorientation: %d", __LINE__, CurrentOrientation);
        // by having this system as the 
        graph[current][next] = node_no*100; // Mark the edge as blocked in the graph using a some sorts of counter which will take in consideration which node is this
        graph[next][current] = node_no*100;
        dijkstrasetup(); // Recalculate the path
        nodeblocked = 1;
        for (int i = 0; i < fullPathSize; i++) {
            Serial.printf("\n%d path: %d",__LINE__,fullPath[i]);
        }
        node_no++;
        return;
    }
  consecutiveReadings = 0;
  consecutiveReadings2 = 0;
  if(current == 6 && next == 5){
    Serial.printf("\n %d current %d next %d",__LINE__,current,next);
    reachednode = false;
  }
  }
  Park();
  }
  halt();
  }
void switchCurrentOrientation(){
  //array which shows the relation of each node to other nodes and where are they based on the compass 
  int directionSS[MAX][4] = {
    //north/west/east/south
    {-1, 4, 7, -1}, // Node 0 
    {-1, 6, 7, -1}, // Node 1 
    {-1, 3, 7, -1}, // Node 2 
    {-1, 6, 2, -1}, // Node 3 
    {-1, 6, 0, -1}, // Node 4 
    {-1, -1, -1, -1}, // Node 5 
    {3, 5, 1, 4}, // Node 6 
    {2, 1, -1, 0}  // Node 7 
    };
        if (blockednode == directionSS[current][0] && nodeblocked == 0) {
            Serial.println("forward");
            CurrentOrientation = NORTH;
                        Serial.println("NORTH");
            halt();
            delay(1000);
        } else if (blockednode == directionSS[current][1] && nodeblocked == 0) {

            CurrentOrientation = WEST;
            //perform90L();
            Serial.println("WEST");
            halt();
            delay(1000);
        } else if (blockednode == directionSS[current][2] && nodeblocked == 0) {
          CurrentOrientation = EAST;
            Serial.println("EAST");
            //perform90R();
        } else if (blockednode == directionSS[current][3] && nodeblocked == 0) {
          CurrentOrientation = SOUTH;
            Serial.println("SOUTH");
            //perform180();
        }
}
  //look over this function
void retryBlockedNodes() {
            // Restore original graph weight (assuming 3 was original weight)
                if (graph[previous][current] > 4 ) { // change to previous current cause the node is already passed and it will make sense to reset it 
                //Serial.printf("\n%d Node %d is now unblocked", __LINE__, next);
                    graph[previous][current] = externalsave[previous][current]; // Reset to default
                    graph[current][previous] = externalsave[current][previous];
                    Serial.printf("\n%d Node %d is now unblocked", __LINE__, next);
                }

    }
 void checkAndRemoveHardcodedNode(int current) {
    //for (int i = 0; i < numHardcoded; i++) {// no need for for loop might delete the next nodes if there are duplicates
        if (current == hardcodedPath[0]) {

            // Shift elements left to remove the node
            for (int j = 0; j < numHardcoded - 1; j++) {
                hardcodedPath[j] = hardcodedPath[j + 1];
                Serial.printf("\n%d:%d ",__LINE__, hardcodedPath[j]);
            }
            
            numHardcoded--; // Reduce size of hardcodedPath
            //break; // Exit loop after deletion
            Serial.printf("\n%d Node has been removed %d",__LINE__, current);
            
        }

}

void Park() {
  default_speed = 150; // Set the default speed for parking
  Serial.printf("\nIts parking");
  // Initialize sensor values to a high number (INF)
  rightmost_sensor = INF;
  left_sensor = INF;
  middle_sensor = INF;
  right_sensor = INF;
  Leftmost_sensor = INF;
  Parking_val = INF;
  sensor_sum = INF;

  const int requiredReadings = 5; // Number of consecutive readings required to confirm parking
  bool parked = false; // Flag to indicate if the robot has parked successfully

  while (inside == true) {

    Parking_val = analogRead(parkingsensor); // Read the parking sensor
    // Check if the parking sensor detects the parking spot
    if (Parking_val >= Parking_Thresh) {
      consecutiveReadings++; // Increment counter for valid readings
      if (consecutiveReadings >= requiredReadings) {
        halt(); // Stop the robot
        Serial.printf("\n%d Parked", __LINE__);
        parked = true; // Set the parked flag to true
        delay(20000); // Wait for 20 seconds (simulating parking)
        inside = false; // Exit the parking loop
        break; // Exit the while loop
      }
    } else {
      // Reset counter if the condition isn't met
      consecutiveReadings = 0;
      forward(); // Continue moving forward
    }
  }

  // If the robot has parked, stop permanently
  if (parked) {
    haltforever(); // Stop the robot permanently
    Serial.printf("\n%d Parking complete. Robot halted.", __LINE__);
  } else {
    Serial.printf("\n%d Parking failed. Resuming normal operation.", __LINE__);
  }
}
void haltforever(){
  while(true){
  analogWrite(Motor1_Ph, 0);
  analogWrite(Motor2_Ph, 0);
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  delay(200000);
  }
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
//sets the sensors 
void check_sensors(){
  left_sensor = analogRead(redsensor);
  middle_sensor = analogRead(orangesensor);
  right_sensor = analogRead(yellowsensor);
  Leftmost_sensor = analogRead(brownsensor);
  rightmost_sensor = analogRead(greensensor);
  Parking_val = analogRead(parkingsensor);
  sensor_sum = middle_sensor+Leftmost_sensor+rightmost_sensor+left_sensor+right_sensor;
  }


//does the adjustment when following the line
void Adjustment(){
  if(left_sensor > white_thresh && middle_sensor < white_thresh && right_sensor > white_thresh){
      forward();
     
    }
  if((left_sensor <= white_thresh && right_sensor >= white_thresh)&&(middle_sensor >= white_thresh && left_sensor <= white_thresh)){
    left();
   
      }
  if((left_sensor >= white_thresh && right_sensor <= white_thresh)&&(middle_sensor >= white_thresh && right_sensor <= white_thresh)){
    right();
   
  }
   
  if (Leftmost_sensor <= white_thresh && rightmost_sensor >= white_thresh){
     tight_left();
     
  }
  if (Leftmost_sensor >= white_thresh && rightmost_sensor <= white_thresh){
    tight_right();
   
    }
  if((sensor_sum >= 1500)&&(sensor_sum <= 6500)){
   
    halt();
    delay(100);
    
    getdirections(fullPath, fullPathSize, currentPathIndex);
    currentPathIndex++;
    forward();
    delay(5);
    }
    
}

void getdirections(int fullPath[], int fullPathSize, int currentPathIndex){
   int directioncoordinates[MAX][4] = {
    //north/west/east/south
    {-1, 4, 7, -1}, // Node 0 
    {-1, 6, 7, -1}, // Node 1 
    {-1, 3, 7, -1}, // Node 2 
    {-1, 6, 2, -1}, // Node 3 
    {-1, 6, 0, -1}, // Node 4 
    {-1, -1, -1, -1}, // Node 5 
    {3, 5, 1, 4}, // Node 6 
    {2, 1, -1, 0}  // Node 7 
    };
    printdirectionofcar(fullPath, fullPathSize, directioncoordinates, currentPathIndex);
}

//find the direction the car is coming from
void printdirectionofcar(int fullPath[], int fullPathSize, int directions[MAX][4], int currentPathIndex) {
    if (currentPathIndex < fullPathSize - 1) {
      
        current = fullPath[currentPathIndex];
        next = fullPath[currentPathIndex + 1];
        previous = (currentPathIndex > 0) ? fullPath[currentPathIndex - 1] : INF;


        // Debugging info
        Serial.print("Current node: ");
        Serial.println(current);
        Serial.print("Next node: ");
        Serial.println(next);
       
        Serial.println("At start node");


        if (previous == directions[current][0] && nodeblocked == 0) {
              CurrentOrientation = NORTH;
              Serial.println("NORTH");
            halt();
            delay(1000);
        } else if (previous == directions[current][1] && nodeblocked == 0) {
            CurrentOrientation = WEST;
            Serial.println("WEST");
            halt();
            delay(1000);
        } else if (previous == directions[current][2] && nodeblocked == 0) {
          CurrentOrientation = EAST;
            Serial.println("EAST");
            
        } else if (previous == directions[current][3] && nodeblocked == 0) {
          CurrentOrientation = SOUTH;
            Serial.println("SOUTH");
        
        }
         else {
          //during special occasions like rerouting nodeblocked makes it so that the direction is kept 
            nodeblocked = 0;
            Serial.printf("\n%d Keep Current Direction %d",__LINE__,CurrentOrientation);
            halt();
            delay(1000);
        }


        //calls the funciton which does the left right forward or 180 functions
    printCarMovements(fullPath, fullPathSize, currentPathIndex, previous, next, current, directions);
        // Debugging info for path progression
        Serial.print("Updated currentPathIndex: ");
        Serial.println(currentPathIndex);
    } else {
        Serial.println("Reached final node. Halting.");
        halt();
    }
        // Debugging info for path progression
        Serial.print("Updated currentPathIndex: ");
        Serial.println(currentPathIndex);
}
 //calls the funciton which does the left right forward or 180 functions
void printCarMovements(int fullPath[], int fullPathSize, int currentPathIndex,int previous, int next, int current, int directions[MAX][4]){
          // Find the direction of the next node
          int moveDirection = 0;
        for (int j = 0; j < 4; j++) {
            if (directions[current][j] == next) {
                moveDirection = j;
                break;
            }
        }
        //debugging 
     switch (CurrentOrientation) {
            case NORTH: printf("North: "); break;
            case EAST: printf("East: "); break;
            case SOUTH: printf("South: "); break;
            case WEST: printf("West: "); break;
        }
       // Determine the movement action
        if (moveDirection == -1) {
        Serial.print("Current node: ");
        Serial.println(current);
        Serial.print("Next node: ");
        Serial.println(next);
        Serial.print("Previous node: ");
                return;
        }
        //switch case base on the orientation and the future needed orientation
  switch (CurrentOrientation) {
            case EAST:
                if (moveDirection == 1) { Serial.printf("\n%d east goes",__LINE__);forward();delay(250);}
                else if (moveDirection == 3) {Serial.printf("\n%d east goes",__LINE__); perform90L(); }
                else if (moveDirection == 0) {Serial.printf("\n%d east goes",__LINE__); perform90R(); }
                else if (moveDirection == 2) {Serial.printf("\n%d east goes",__LINE__); perform180(); }
                break;

            case SOUTH:
                if (moveDirection == 0) {Serial.printf("\n%d south goes",__LINE__); forward();delay(250);}
                else if (moveDirection == 1) {Serial.printf("\n%d south goes",__LINE__); perform90L(); }
                else if (moveDirection == 2) { Serial.printf("\n%d south goes",__LINE__);perform90R(); }
                else if (moveDirection == 3) { Serial.printf("\n%d south goes",__LINE__);perform180(); }
                break;

            case WEST:
                if (moveDirection == 2) { Serial.printf("\n%d west goes",__LINE__);forward();delay(250);}
                else if (moveDirection == 0) { Serial.printf("\n%d west goes",__LINE__);perform90L(); }
                else if (moveDirection == 3) { Serial.printf("\n%d west goes",__LINE__);perform90R(); }
                else if (moveDirection == 1) { Serial.printf("\n%d west goes",__LINE__);perform180(); }
                break;

            case NORTH:
                if (moveDirection == 3) {Serial.printf("\n%d north goes",__LINE__); forward();delay(250); }
                else if (moveDirection == 2) {Serial.printf("\n%d north goes",__LINE__); perform90L(); }
                else if (moveDirection == 1) {Serial.printf("\n%d north goes",__LINE__); perform90R(); }
                else if (moveDirection == 0) {Serial.printf("\n%d north goes",__LINE__); perform180(); }
                break;
        }

}

//chjecks if the node is blocked based on the sensor
bool isNodeBlocked() {
    const int requiredReadings = 5; // Number of consecutive readings required
    static int consecutiveReadings2 = 0; // Counter for consecutive readings

    // Check if the parking sensor value exceeds the obstacle threshold
    if (Parking_val >= Obstacle_thresh) {
        consecutiveReadings2++; // Increment counter for valid readings
        if (consecutiveReadings2 >= requiredReadings) {
            // If the required number of consecutive readings is met, the node is blocked
            halt(); // Stop the car
            //perform180(); // Perform a 180-degree turn
            //forward(); // Move forward after turning
            consecutiveReadings2 = 0; // Reset the counter
            return true; // Node is blocked
        }
    } else {
        // Reset the counter if the condition isn't met
        consecutiveReadings2 = 0;
    }

    // If the node is not blocked, continue checking sensors and adjusting movement
    check_sensors();
    //Adjustment(); // Continue moving forward
    return false; // Node is not blocked
}


//Calls dikjstras to recalculate a shortest path 
void dijkstra(int graph[MAX][MAX], int start, int end, int path[], int &pathSize) {
    int dist[MAX];
    bool visited[MAX] = {false};
    int parent[MAX];
    
    for (int i = 0; i < MAX; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[start] = 0;

    for (int count = 0; count < MAX - 1; count++) {
        int u = findMinDistance(dist, visited, MAX);
        visited[u] = true;

        for (int v = 0; v < MAX; v++) {
            if (!visited[v] && graph[u][v] && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
    pathSize = 0;
    constructPath(parent, end, path, pathSize);
}
// Function to construct the shortest path recursively
void constructPath(int parent[], int target, int path[], int &pathSize) {
    if (parent[target] == -1) {
        path[pathSize++] = target;
        return;
    }
    constructPath(parent, parent[target], path, pathSize);
    path[pathSize++] = target;
}
// Function to find the node with the smallest distance
int findMinDistance(int dist[], bool visited[], int n) {
    int min = INF, min_index = -1;
    for (int i = 0; i < n; i++) {
        if (!visited[i] && dist[i] <= min) {
            min = dist[i];
            min_index = i;
        }
    }
    return min_index;
}


//rest of the functions are the ones controling the movements
void reverse() {
  analogWrite(Motor2_Ph, 255);
  analogWrite(Motor1_Ph, 255);
  analogWrite(Motor1_Pwm, default_speed);
  analogWrite(Motor2_Pwm, default_speed);
}
void forward() {
  analogWrite(Motor1_Ph, 0);
  analogWrite(Motor2_Ph, 0);
  analogWrite(Motor1_Pwm, default_speed);
  analogWrite(Motor2_Pwm, default_speed+5);
}
void left() {
  analogWrite(Motor1_Ph, 0);
  analogWrite(Motor2_Ph, 0);
  analogWrite(Motor1_Pwm, default_speed);
  analogWrite(Motor2_Pwm, turning_speed);
}
void tight_left(){
  analogWrite(Motor1_Ph, 0);
  analogWrite(Motor2_Ph, 0);
  analogWrite(Motor1_Pwm, 50);
  analogWrite(Motor2_Pwm, turning_tight_speed);
}
void right(){
  digitalWrite(Motor1_Ph,0);
  digitalWrite(Motor2_Ph,0);
  analogWrite(Motor1_Pwm, turning_speed);
  analogWrite(Motor2_Pwm, default_speed);
}
void tight_right(){
  analogWrite(Motor1_Ph,0);
  analogWrite(Motor2_Ph,0);
  analogWrite(Motor1_Pwm, turning_tight_speed);
  analogWrite(Motor2_Pwm, 50);
}
void halt() {
  analogWrite(Motor1_Ph, 0);
  analogWrite(Motor2_Ph, 0);
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
}
void perform180() {
  forward();
  delay(250);
  Serial.printf("\n%d 180",__LINE__);
  int ctr = 0;
  halt();
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  delay(5);
  analogWrite(Motor1_Ph, 255);
  analogWrite(Motor2_Ph, 0);
  analogWrite(Motor1_Pwm, 180);
  analogWrite(Motor2_Pwm, 180);
  delay(600);
  while (1) {
    ctr++;
    check_sensors();
    if (middle_sensor <= white_thresh||ctr >10000) {
      Adjustment();
      break;
    }
  }
}

void perform90L(){
  Serial.printf("\n%d Turn right",__LINE__);
  forward();
  delay(200);
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  analogWrite(Motor1_Ph, 0);
  analogWrite(Motor2_Ph, 255);
  analogWrite(Motor1_Pwm, 255);
  analogWrite(Motor2_Pwm, 255);
  delay(300);
 


}
void perform90R(){
  Serial.printf("\n%d Turn left",__LINE__);
  forward();
  delay(200);
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  analogWrite(Motor1_Ph, 255);
  analogWrite(Motor2_Ph, 0);
  analogWrite(Motor1_Pwm, 255);
  analogWrite(Motor2_Pwm, 255);
  delay(300);
}