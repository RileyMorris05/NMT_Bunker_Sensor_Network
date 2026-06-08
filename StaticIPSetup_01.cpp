// Trying to Setup StaticIP
#include <WiFi.h>
#include <CPU.h>

CPU cpu;
float tempC;

// Settings for WiFi Connecting to
const char* ssid = "Drone_Lab";
const char* password = "123456789";

WiFiServer server(80);

// =========================================
// Sensor Configuration
// =========================================
const int SensorNumber = 1;    // Please enter a whole Number and Signify on the Sensor which # this is


// Setting unique Static IP address for sensor
IPAddress localIP(192, 168, 1, 201);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

void setup() {
  Serial.begin(115200);
  cpu.begin();
  WiFi.config(localIP, gateway, subnet);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("================================");
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("================================");

  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.subnetMask());

  server.begin();
}

void loop() {

  WiFiClient client = server.available();

  tempC = cpu.getTemperature();
  

  if (!client) {
    return;
  }

  String request = client.readStringUntil('\r');


  //--------------------------------------------------
  // Data Endpoint (For Pulling Data for Excel Spreadsheet)
  //--------------------------------------------------
  if (request.indexOf("GET /GetData") >= 0) {

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();

    client.println(SensorNumber);
    client.println(",");
    client.println(tempC);

    client.stop();
    return;
  }

  //--------------------------------------------------
  // Webpage (For Manual Sensor Watching)
  //--------------------------------------------------
  client.println("HTTP/1.1 200 OK");    // Sends HTTP Status line and suceeds the request
  client.println("Content-Type: text/html");    // Tells Browser to Render as Webpage
  client.println("Connection: close");    // "After I send this, I will close the connection"
  client.println();   // Blank Line

  // Declare and start the html document
  client.println("<!DOCTYPE html>");
  client.println("<html>");

  // Head section
  client.println("<head>");
  client.println("<title>Dashboard for Sensor ");
  client.println(SensorNumber);
  client.println("</title>");

  //Begins a Javascript block in the webpage
  client.println("<script>");

  client.println("async function GetData() {"); // Defines an asynchronous function XYZ
  client.println("  let response = await fetch('/GetData');");    // Asks for ABC, which goes to earlier if statement
  client.println("  let tempC = await response.text();");    // Waits for response from ABC and sets it as plain text
  client.println("  document.getElementById('BLANK').innerText = BLANK;");    // Finds HTML element and replaces text with new value
  client.println("}");    // Ends the XYZ function

  // Updating XYZ every second
  client.println("setInterval(GetData, 1000);");
  // Runs XYZ immediately when the page loads
  client.println("window.onload = GetData;");

  // End Javascript Section
  client.println("</script>");

  // Closes the Head Section
  client.println("</head>");
  client.println("<body>");

  //client.println("</h1>");

  client.println("<p>");    // Starts Paragraph Block
  client.println("CPU Temperature: ");    // Static Label
  client.println("<span id='tempC'>Loading...</span>");    // Says Loading then replaces with the Float value
  client.println("</p>");   // Ends Paragraph Block

  client.println("</body>");
  client.println("</html>");

  client.stop();
  
}