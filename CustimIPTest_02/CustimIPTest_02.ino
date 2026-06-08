// Sensor Node WIP Code
#include <WiFi.h>
#include <CPU.h>

CPU cpu;
float tempC;
float o2Percent;
float humidity;
float dustSensor;
float pressure;
float co2;
float multichannelGasSensor;
float airQualitySensor;

// Settings for WiFi Connecting to
const char* ssid = "Drone_Lab";
const char* password = "123456789";

WiFiServer server(80);

// =========================================
// Sensor Configuration
// =========================================
const int SensorNumber = 1;    // Please enter a whole Number and Signify on the Sensor which # this is
//




//Setting unique Static IP address for sensor
IPAddress localIP(192, 168, 0, 202);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  cpu.begin();
  Serial.println("I am Alive");
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
  o2Percent = random(195, 210) / 10.0;
  humidity = random(250, 750) / 10.0;
  dustSensor = random(0, 500) / 10.0;
  pressure = random(9800, 10400) / 10.0;
  co2 = random(400, 2000);
  multichannelGasSensor = random(0, 1000) / 10.0;
  airQualitySensor = random(0, 500);
//    tempC = random(10, 99);
    //Serial.println(tempC);
  if (!client) {
    return;
  }

  String request = client.readStringUntil('\r');
  client.readStringUntil('\n');
  Serial.println(request);
  String requestLower = request;
  requestLower.toLowerCase();

  while (client.connected()) {
    String headerLine = client.readStringUntil('\n');
    if (headerLine == "\r" || headerLine.length() == 0) {
      break;
    }
  }


  //--------------------------------------------------
  // Data Endpoint (For Pulling Data for Excel Spreadsheet)
  //--------------------------------------------------
  if (requestLower.indexOf("get /getdata") >= 0) {

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Cache-Control: no-store");
    client.println("Connection: close");
    client.println();

    client.print(SensorNumber);
    client.print(",");
    client.print(tempC);
    client.print(",");
    client.print(o2Percent);
    client.print(",");
    client.print(humidity);
    client.print(",");
    client.print(dustSensor);
    client.print(",");
    client.print(pressure);
    client.print(",");
    client.print(co2);
    client.print(",");
    client.print(multichannelGasSensor);
    client.print(",");
    client.println(airQualitySensor);

    client.flush();
    delay(10);
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

  client.println("async function GetData() {");
  client.println("  let xhr = new XMLHttpRequest();");
  client.println("  xhr.onreadystatechange = function() {");
  client.println("    if (xhr.readyState == 4) {");
  client.println("      if (xhr.status == 200) {");
  client.println("        let parts = xhr.responseText.trim().split(',');");
  client.println("        if (parts.length >= 9) {");
  client.println("          document.getElementById('sensorNumber').innerText = parts[0];");
  client.println("          document.getElementById('tempC').innerText = parts[1] + ' C';");
  client.println("          document.getElementById('o2Percent').innerText = parts[2] + ' %';");
  client.println("          document.getElementById('humidity').innerText = parts[3] + ' %';");
  client.println("          document.getElementById('dustSensor').innerText = parts[4] + ' ug/m3';");
  client.println("          document.getElementById('pressure').innerText = parts[5] + ' hPa';");
  client.println("          document.getElementById('co2').innerText = parts[6] + ' ppm';");
  client.println("          document.getElementById('multichannelGasSensor').innerText = parts[7];");
  client.println("          document.getElementById('airQualitySensor').innerText = parts[8];");
  client.println("        } else {");
  client.println("          document.getElementById('tempC').innerText = 'Bad data';");
  client.println("        }");
  client.println("      } else {");
  client.println("        document.getElementById('tempC').innerText = 'Request failed';");
  client.println("      }");
  client.println("    }");
  client.println("  };");
  client.println("  xhr.open('GET', '/GetData', true);");
  client.println("  xhr.send();");
  client.println("}");
    // Ends the XYZ function

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

  client.println("<p>");
  client.println("Sensor Number: ");
  client.println("<span id='sensorNumber'>Loading...</span>");
  client.println("</p>");

  client.println("<p>");
  client.println("CPU Temperature: ");
  client.println("<span id='tempC'>Loading...</span>");
  client.println("</p>");

  client.println("<p>");
  client.println("O2: ");
  client.println("<span id='o2Percent'>Loading...</span>");
  client.println("</p>");

  client.println("<p>");
  client.println("Humidity: ");
  client.println("<span id='humidity'>Loading...</span>");
  client.println("</p>");

  client.println("<p>");
  client.println("Dust Sensor: ");
  client.println("<span id='dustSensor'>Loading...</span>");
  client.println("</p>");

  client.println("<p>");
  client.println("Pressure: ");
  client.println("<span id='pressure'>Loading...</span>");
  client.println("</p>");

  client.println("<p>");
  client.println("CO2: ");
  client.println("<span id='co2'>Loading...</span>");
  client.println("</p>");

  client.println("<p>");
  client.println("Multichannel Gas Sensor: ");
  client.println("<span id='multichannelGasSensor'>Loading...</span>");
  client.println("</p>");

  client.println("<p>");
  client.println("Air Quality Sensor: ");
  client.println("<span id='airQualitySensor'>Loading...</span>");
  client.println("</p>");

  client.println("</body>");
  client.println("</html>");

  client.flush();
  delay(10);
  client.stop();
  
}
