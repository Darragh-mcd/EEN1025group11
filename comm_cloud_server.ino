#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "iot";
const char* password = "hoidenish91dippier";

// Server details
const char* serverAddress = "3.250.38.184"; // IP address
const int serverPort = 8000;               // Port

// Hardcoded positions (predefined route)
int positions[] = {0, 1, 2, 3, 4, 5}; // Example route
int currentPositionIndex = 0;         // Start from the first position

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 10000; // Send update every 10 seconds

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {
  // Check if it's time to send the next position
  if ((millis() - lastTime) > timerDelay) {
    // Send the current position to the server
    sendPostRequest(positions[currentPositionIndex]);

    // Move to the next position in the array
    currentPositionIndex = (currentPositionIndex + 1) % (sizeof(positions) / sizeof(positions[0]));

    // Update the last time
    lastTime = millis();
  }
}

void sendPostRequest(int position) {
  WiFiClient client;

  if (client.connect(serverAddress, serverPort)) {
    Serial.println("Connected to server");

    // Construct the POST body
    String teamId = "pllk3327"; // Replace with your actual team ID
    String endpoint = "/api/arrived/" + teamId;
    String postBody = "position=" + String(position);

    // Send the POST request
    client.println("POST " + endpoint + " HTTP/1.1");
    client.println("Host: " + String(serverAddress));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postBody.length());
    client.println("Connection: close");
    client.println();
    client.println(postBody);

    // Read the server's response
    Serial.println("Server response:");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }

    client.stop(); // Close the connection
  } else {
    Serial.println("Connection to server failed");
  }
}
