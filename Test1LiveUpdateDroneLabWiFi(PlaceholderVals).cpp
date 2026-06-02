#include <WiFi.h>
#include <CPU.h>

// For Temp.
CPU cpu;
float tempC;

// Wifi Settings
const char* ssid = "Drone_Lab";
const char* password = "123456789";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  cpu.begin();
  delay(3000);
  
    Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("================================");
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("================================");

  server.begin();
}

void loop() {
  WiFiClient client = server.available();


  if (!client) {
    return;
  }

  Serial.println("Client connected");

  // Read first line of HTTP request
  String request = client.readStringUntil('\r');
  Serial.println(request);

  // Clear remaining request data
  while (client.available()) {
    client.read();
  }

  //--------------------------------------------------
  // TEMPERATURE ENDPOINT
  //--------------------------------------------------
  if (request.indexOf("GET /temp") >= 0) {

    float tempC = cpu.getTemperature();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();

    client.println(tempC);

    client.stop();

    Serial.print("Temperature sent: ");
    Serial.println(tempC);

    return;
  }

  //--------------------------------------------------
  // MAIN WEBPAGE
  //--------------------------------------------------
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html>");
  client.println("<html>");

  client.println("<head>");
  client.println("<title>Pico Dashboard</title>");

  client.println("<script>");

  client.println("async function updateTemp() {");
  client.println("  let response = await fetch('/temp');");
  client.println("  let temp = await response.text();");
  client.println("  document.getElementById('temp').innerText = temp;");
  client.println("}");

  client.println("setInterval(updateTemp, 1000);");

  client.println("window.onload = updateTemp;");

  client.println("</script>");

  client.println("</head>");

  client.println("<body>");

  client.println("<h1>Pico 2 W Sensor Dashboard</h1>");

  client.println("<p>");
  client.println("CPU Temperature: ");
  client.println("<span id='temp'>Loading...</span>");
  client.println(" &deg;C");
  client.println("</p>");

  client.println("</body>");
  client.println("</html>");

  client.stop();

  Serial.println("Main page served");
}