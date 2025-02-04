#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "Iot";
const char* password = "YourPassword";

// Server URL
const char* serverUrl = "http://3.250.38.184:8000/api/arrived/TEAM-ID"; // Replace TEAM-ID with your team ID

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

  // Send POST request
  sendPostRequest(0); // Example: Notify arrival at position 0
}

void loop() {
  // No repetitive actions in this example
}

// Function to send POST request and display response
void sendPostRequest(int position) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Initialize HTTP request
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare POST data
    String postData = "position=" + String(position);

    // Send POST request
    int httpResponseCode = http.POST(postData);

    // Check HTTP response code
    if (httpResponseCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);

      // Print server headers
      Serial.println("Server Headers:");
      for (int i = 0; i < http.headers(); i++) {
        Serial.print(http.headerName(i) + ": ");
        Serial.println(http.header(i));
      }

      // Print response body
      String responseBody = http.getString();
      Serial.println("Response Body:");
      Serial.println(responseBody);
    } else {
      Serial.printf("Error sending POST request: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    // End HTTP connection
    http.end();
  } else {
    Serial.println("Wi-Fi not connected");
  }
}

Similar example
https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-post


Sending Messages Example:
// post body
String postBody("position=");
postBody += position;
// send post request and headers
client.println("POST /api/arrived/INSERT_YOUR_TEAM_ID_HERE HTTP/1.1");
client.println("Content-Type: application/x-www-form-urlencoded");
client.print("Content-Length: ");
client.println(postBody.length());
client.println("Connection: close");
client.println();
// send post body
client.println(postBody);
Full code:
#include <WiFi.h>

// Wi-Fi credentials
const char* ssid = "YourSSID";
const char* password = "YourPassword";

// Server details
const char* serverAddress = "3.250.38.184"; // IP address
const int serverPort = 8000;               // Port

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

  // Create a connection to the server
  WiFiClient client;
  if (client.connect(serverAddress, serverPort)) {
    Serial.println("Connected to server");

    // Construct the POST body
    String postBody("position=0");

    // Send the POST request
    client.println("POST /api/arrived/TEAM-ID HTTP/1.1"); // Replace TEAM-ID
    client.println("Host: 3.250.38.184");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postBody.length());
    client.println("Connection: close");
    client.println();
    client.println(postBody);

    // Read the server's response
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line.length() == 1 && line[0] == '\r') break; // End of headers
      Serial.println(line);
    }

    // Read the response body
    String responseBody = client.readString();
    Serial.println("Response Body:");
    Serial.println(responseBody);

    client.stop(); // Close the connection
  } else {
    Serial.println("Connection to server failed");
  }
}

void loop() {}


