// Live Updating Webpage Code with Placeholders

// The purpose of this code is to have a template for adding sensors and readings to the webpage output.

#include <WiFi.h>

// Wifi Settings
const char* ssid = "WIFI_SSID";
const char* password = "PLACEHOLDER_PASSWORD";

void setup() {
  Serial.begin(115200);
  delay(3000):

  Serial.println("Starting...");

  WiFi.softAP(ssid, password);

  // Tells user WiFi Name, Password, and Browser Address to Connect To in the Serial Monitor
  Serial.println();
  Serial.println("================================");
  Serial.println("Connect your phone to: ");
  Serial.println(ssid);
  Serial.println("The Password is: ");
  Serial.println(password);
  Serial.print("Open browser at: http://");
  Serial.println(WiFi.softAPIP());
  Serial.println("================================");

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }
  Serial.println("Client connected");

  // Read first line of HTTP request then Clearing remaining data (For Debugging)
  String request = client.readStringUntil('\r');
  Serial.println(request);
  while (client.available()) {
    client.read();
  }

  //================================================================================================
  // ANSWERING REQUEST FOR VALUE
  //================================================================================================
  if (request.indexOf("GET /ABC") >= 0) {

    // Sends response header and blank line
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();

    // Sends the result itself
    // Have a float value inside the bracket here for the data that is sent
    client.println(FLOAT);

    client.stop();
    return;
  }

  //================================================================================================
  // MAIN WEBPAGE
  //================================================================================================
  client.println("HTTP/1.1 200 OK");    // Sends HTTP Status line and suceeds the request
  client.println("Content-Type: text/html");    // Tells Browser to Render as Webpage
  client.println("Connection: close");    // "After I send this, I will close the connection"
  client.println();   // Blank Line

  // Declare and start the html document
  client.println("<!DOCTYPE html>");
  client.println("<html>");

  // Head section
  client.println("<head>");
  client.println("<title>Zohrab Dashboard</title>");

  //Begins a Javascript block in the webpage
  client.println("<script>");

  client.println("async function XYZ() {"); // Defines an asynchronous function XYZ
  client.println("  let response = await fetch('/ABC');");    // Asks for ABC, which goes to earlier if statement
  client.println("  let FLOAT = await response.text();");    // Waits for response from ABC and sets it as plain text
  client.println("  document.getElementById('BLANK').innerText = BLANK;");    // Finds HTML element and replaces text with new value
  client.println("}");    // Ends the XYZ function

  // Updating XYZ every second
  client.println("setInterval(XYZ, 1000);");

  // Runs XYZ immediately when the page loads
  client.println("window.onload = XYZ;");

  // End Javascript Section
  client.println("</script>");

  // Closes the Head Section
  client.println("</head>");

  // Statys the Body Section (visible part of webpage)
  client.println("<body>");

  //Displays large heading on the page
  client.println("<h1>Pico 2 W Sensor Dashboard</h1>");

  client.println("<p>");    // Starts Paragraph Block
  client.println("CPU Temperature: ");    // Static Label
  client.println("<span id='FLOAT'>Loading...</span>");    // Says Loading then replaces with the Float value
  client.println("</p>");   // Ends Paragraph Block

  // Closes visible page content then ends the html document
  client.println("</body>");
  client.println("</html>");

  client.stop();    //Closes TCP connection to Browser
}