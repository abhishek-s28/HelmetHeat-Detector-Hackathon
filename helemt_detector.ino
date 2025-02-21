#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

const char *ssid = "DhavanilPhone";
const char *password = "njlw8553";
int buzzerPin = 25;

WebServer server(80);
DHT dht(26, DHT11);

float temperatureReadings[5];
int readingIndex = 0;

// Method to read temperature and calculate average
float readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  
  // Store temperature readings for average calculation
  temperatureReadings[readingIndex] = t;
  readingIndex = (readingIndex + 1) % 5;  // Loop back after 5 readings

  // Calculate average temperature
  float sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += temperatureReadings[i];
  }
  float avgTemp = sum / 5.0;

  Serial.println("Temperature: " + String(t) + " °C");
  Serial.println("Average Temperature: " + String(avgTemp) + " °C");

  return avgTemp;
}

// Method to read humidity
float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  return h;
}

// Root page handler
void handleRoot() {
  char msg[3000];

  float temperature = readDHTTemperature();
  float humidity = readDHTHumidity();

  snprintf(msg, 3000,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='1'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; color: #333; margin-bottom: 30px; }\
    .card {\
      display: inline-block;\
      background-color: #fff;\
      border-radius: 15px;\
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);\
      margin: 15px;\
      padding: 20px;\
      width: 250px;\
      text-align: center;\
      transform: scale(0.95);\
      transition: transform 0.3s ease-in-out;\
    }\
    .card:hover {\
      transform: scale(1);\
      box-shadow: 0 8px 20px rgba(0, 0, 0, 0.15);\
    }\
    .icon {\
      font-size: 3rem;\
      margin-bottom: 10px;\
      transition: color 0.3s ease-in-out;\
    }\
    .temperature { color: #ca3517; }\
    .humidity { color: #00add6; }\
    .value { font-size: 2rem; font-weight: bold; }\
    .units { font-size: 1.2rem; color: #777; }\
    .dht-labels { font-size: 1.5rem; padding-bottom: 15px; }\
    .average-card {\
      background-color: #4CAF50;\
      color: white;\
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);\
      margin-top: 30px;\
    }\
    .average-card .value { font-size: 2.5rem; }\
    </style>\
  </head>\
  <body>\
      <h2>Helmet Heat Detector Sensor!</h2>\
      <div class='card'>\
        <i class='fas fa-thermometer-half icon temperature'></i>\
        <div class='dht-labels'>Temperature</div>\
        <span class='value'>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </div>\
      <div class='card'>\
        <i class='fas fa-tint icon humidity'></i>\
        <div class='dht-labels'>Humidity</div>\
        <span class='value'>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </div>\
      <div class='card average-card'>\
        <div class='dht-labels'>Average Temperature</div>\
        <span class='value'>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </div>\
  </body>\
</html>", temperature, humidity, temperature);

  server.send(200, "text/html", msg);
}

void setup(void) {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
float avgTemperature = readDHTTemperature();

  // Check if the average temperature is above or equal to 25°C
  if(avgTemperature >= 25.0){
    tone(buzzerPin, 1000);   // Activate the buzzer at 1000 Hz
    delay(2000);             // Wait for 2 seconds
          // Stop the buzzer
  }
  noTone(buzzerPin); 
  server.handleClient();
  delay(2); // Allow the CPU to switch to other tasks
}