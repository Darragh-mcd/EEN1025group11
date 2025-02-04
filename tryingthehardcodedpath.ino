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


const int default_speed = 225;
const int turning_speed = default_speed+25;
const int turning_tight_speed = 255;
const int white_thresh = 1000;
const int Obstacle_thresh = 3500; // threshold for in parking level rmps up fairly quick
int Parking_Thresh = 2500;
int  consecutiveReadings = 0;
int  consecutiveReadings2 = 0;
int i = 0; // used to grab directions from array
//int stage = 0; // used for hard coded path testing
//------------------------------------------------------ needed for logic checks

const int MAX = 8;
const int INF = 9999;
//L F R 180 L F 180 F Park
//int hard_code_route[] = {LEFT,FORWARD,RIGHT,TURN_AROUND,LEFT,FORWARD,TURN_AROUND,FORWARD,PARK};
//-------------------------------------------------------------------------------------------- this is a hardcoded test route
//For Dijkstras 
int graph[MAX][MAX] = {
    {0, 10, 10, 10, 2, 10, 10, 2}, //0
    {10, 0, 10, 10, 10, 10, 1, 2}, //1
    {10, 10, 0, 2, 10, 10, 10, 2},//2
    {10, 10, 2, 0, 10, 10, 3, 10},//3
    {2, 10, 10, 10, 0, 10, 3, 10},//4
    {10, 10, 10, 10, 10, 10, 3, 10},//5
    {10, 1, 10, 3, 3, 3, 0, 10},//6
    {2, 2, 2, 10, 10, 10, 10, 0}//7
};

    //int start = 0; // Starting node
    //int end = 1;   // Destination node
    //int pathLength;
int stage = 1;
//int directionofcar = 1; // 1 is east, -1 is west
int currentPathIndex = 0; // Index in the current path
//int path[MAX]; // Holds the path nodes

//dynamic movement
typedef enum { NORTH = 0, EAST = 90, SOUTH = 180, WEST = 270 } Orientation;
Orientation CurrentOrientation = WEST; //Start coming from the west going to east
//BROYG
    int hardcodedPath[] = {0, 4, 2, 4, 1, 5};
    int numHardcoded = sizeof(hardcodedPath) / sizeof(hardcodedPath[0]);
    int fullPath[MAX * MAX];
    int fullPathSize = 0;

//BROYG (sensor layout)




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
  for (int i = 0; i < fullPathSize; i++){ Serial.println(fullPath[i]);}
  //dijkstra(graph, start, end, path, &pathLength);
}
void dijkstrasetup(){
      for (int i = 0; i < numHardcoded - 1; i++) {
        int subPath[MAX];
        int subPathSize = 0;
        dijkstra(graph, hardcodedPath[i], hardcodedPath[i + 1], subPath, subPathSize);
        if (i > 0) fullPathSize--;
        for (int j = 0; j < subPathSize; j++) {
            fullPath[fullPathSize++] = subPath[j];
            //Serial.println(fullPath[j]);
        }

    }
}
void loop() {
  check_sensors();
  Adjustment();
 
  consecutiveReadings = 0;
  consecutiveReadings2 = 0;
 
  }
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
  analogWrite(Motor2_Pwm, default_speed-5);
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
  analogWrite(Motor1_Pwm, 60);
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
void perform180(){
  halt();
  analogWrite(Motor1_Pwm, 0);
  analogWrite(Motor2_Pwm, 0);
  delay(5);
  analogWrite(Motor1_Ph, 255);
  analogWrite(Motor2_Ph, 0);
  analogWrite(Motor1_Pwm, 255);
  analogWrite(Motor2_Pwm, 255);
  delay(600);
  halt();
  forward();
  delay(200);
}
void perform90R(){
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
void perform90L(){
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
void Park() {
  const int requiredReadings = 0;    // Number of consecutive readings required
 
 


  while(1==1){
    Parking_val = analogRead(parkingsensor);
    if (Parking_val >= Parking_Thresh) {
       consecutiveReadings++; // Increment counter for valid readings
       if (consecutiveReadings >= requiredReadings) {
       halt();
       break; // Stop the robot after consistent readings
    }
  } else{
   // Reset counter if the condition isn't met
      forward();             // Continue moving forward
  }}
}
void Obstacle_detect(){




  const int requiredReadings = 5;    // Number of consecutive readings required
 
  Parking_val = analogRead(parkingsensor);


 
  if (Parking_val >= Obstacle_thresh) {
    consecutiveReadings2++; // Increment counter for valid readings
    if (consecutiveReadings2 >= requiredReadings) {
    halt();
    perform180();
    forward();
    }
  } {
   // Reset counter if the condition isn't met
  check_sensors();
  Adjustment();               // Continue moving forward
  }
}
void check_sensors(){
  left_sensor = analogRead(redsensor);
  middle_sensor = analogRead(orangesensor);
  right_sensor = analogRead(yellowsensor);
  Leftmost_sensor = analogRead(brownsensor);
  rightmost_sensor = analogRead(greensensor);
  Parking_val = analogRead(parkingsensor);
  sensor_sum = middle_sensor+Leftmost_sensor+rightmost_sensor+left_sensor+right_sensor;
  }
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
  if((sensor_sum >= 800)&&(sensor_sum <= 2000)){
   
    halt();
    delay(100);
    getdirections(fullPath, fullPathSize, currentPathIndex);
    currentPathIndex++;
    forward();
    delay(5);
    }
    
  /*else if (sensor_sum >3500)  {
    reverse();
  }*/
}
void getdirections(int fullPath[], int fullPathSize, int currentPathIndex){
   int directioncoordinates[MAX][4] = {
    //north/west/east/south
    {-1, 4, 7, -1}, // Node 0 
    {-1, 6, 7, -1}, // Node 1 
    {-1, 3, 7, -1}, // Node 2 (7 connects from East) 
    {-1, 6, 2, -1}, // Node 3 (6 connects from West) 
    {-1, 6, 0, -1}, // Node 4 
    {-1, -1, -1, -1}, // Node 5 
    {3, 5, 1, 4}, // Node 6 (4 connects from South, 3 connects from West) 
    {2, 1, -1, 0}  // Node 7 (1 connects from West, 2 connects from East, 0 connects from South)
    };
    printdirectionofcar(fullPath, fullPathSize, directioncoordinates, currentPathIndex);
}
void printdirectionofcar(int fullPath[], int fullPathSize, int directions[MAX][4], int currentPathIndex) {
    if (currentPathIndex < fullPathSize - 1) {
      
        int current = fullPath[currentPathIndex];
        int next = fullPath[currentPathIndex + 1];
        int previous = fullPath[currentPathIndex -1];


        // Debugging info
        Serial.print("Current node: ");
        Serial.println(current);
        Serial.print("Next node: ");
        Serial.println(next);
       
        Serial.println("At start node");


        if (previous == directions[current][0]) {
            Serial.println("forward");
            //forward();
            CurrentOrientation = NORTH;
            //perform90L();
                        Serial.println("NORTH");
            halt();
            delay(1000);
        } else if (previous == directions[current][1]) {

            CurrentOrientation = WEST;
            //perform90L();
            Serial.println("WEST");
            halt();
            delay(1000);
        } else if (previous == directions[current][2]) {
          CurrentOrientation = EAST;
            Serial.println("EAST");
            //perform90R();
        } else if (previous == directions[current][3]) {
          CurrentOrientation = SOUTH;
            Serial.println("SOUTH");
            //perform180();
        }// else if (current)
         else {
            Serial.println("Invalid direction. Halting.");
            halt();
            delay(1000);
            //return; // Stop execution to avoid incrementing index incorrectly
        }


        // Increment the path index after a valid move
    printCarMovements(fullPath, fullPathSize, currentPathIndex, next, current, directions);
        //currentPathIndex++;


        // Debugging info for path progression
        Serial.print("Updated currentPathIndex: ");
        Serial.println(currentPathIndex);


    } else {
        Serial.println("Reached final node. Halting.");
        halt();
    }

      //printCarMovements(path, pathLength, CurrentPathIndex, next, current)
            // Increment the path index after a valid move
        //currentPathIndex++;


        // Debugging info for path progression
        Serial.print("Updated currentPathIndex: ");
        Serial.println(currentPathIndex);
}
void printCarMovements(int fullPath[], int fullPathSize, int currentPathIndex, int next, int current, int directions[MAX][4]){
          // Find the direction of the next node
          int moveDirection = 0;
        for (int j = 0; j < 4; j++) {
            if (directions[current][j] == next) {
                moveDirection = j;
                break;
            }
        }
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
        //Serial.println(previous);
            return;
        }
  switch (CurrentOrientation) {
            case EAST:
                if (moveDirection == 1) { forward();}
                else if (moveDirection == 3) { perform90L(); }
                else if (moveDirection == 0) { perform90R(); }
                else if (moveDirection == 2) { perform180(); }
                break;

            case SOUTH:
                if (moveDirection == 0) { forward();}
                else if (moveDirection == 1) { perform90L(); }
                else if (moveDirection == 2) { perform90R(); }
                else if (moveDirection == 3) { perform180(); }
                break;

            case WEST:
                if (moveDirection == 2) { forward();}
                else if (moveDirection == 0) { perform90L(); }
                else if (moveDirection == 3) { perform90R(); }
                else if (moveDirection == 1) { perform180(); }
                break;

            case NORTH:
                if (moveDirection == 3) { forward(); }
                else if (moveDirection == 2) { perform90L(); }
                else if (moveDirection == 1) { perform90R(); }
                else if (moveDirection == 0) { perform180(); }
                break;
        }

}
  // Dijkstra's Algorithm
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

