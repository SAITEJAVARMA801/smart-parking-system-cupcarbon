#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

// ====== USER CONFIG ======
#define WIFI_SSID     "OPPO_RENO_8"
#define WIFI_PASS     "STV@9996"

//  spot ID
#define SPOT_ID 2

// MQTT broker (central computer IP)
#define MQTT_HOST     "10.217.88.250"
#define MQTT_PORT     1883

// Pins
#define LED_PIN   15
#define NUMPIXELS 16
#define TRIG_PIN   2
#define ECHO_PIN   4

#define DIST_THRESHOLD_CM 50

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiClient espClient;
PubSubClient mqtt(espClient);

bool reserved = false;

String topicSpot() { return String("parking/spot/") + String(SPOT_ID); }
String topicControl() { return String("parking/control/") + String(SPOT_ID); }

void setAll(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

long readUltrasonicCm() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  return (long)(duration * 0.034 / 2.0);
}

void publishStatus(const char* status, float distance) {
  char payload[200];
  snprintf(payload, sizeof(payload),
    "{\"spot\":%d,\"status\":\"%s\",\"distance_cm\":%.1f,\"ip\":\"%s\"}",
    SPOT_ID, status, distance, WiFi.localIP().toString().c_str()
  );
  mqtt.publish(topicSpot().c_str(), payload, true);
  Serial.println(payload);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  if (msg == "reserved") {
    reserved = true;
  } else if (msg == "clear") {
    reserved = false;
  }
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
    setAll(0, 0, 255); // Blue = offline
  }
  Serial.println("\nWiFi connected");
}

void mqttReconnect() {
  while (!mqtt.connected()) {
    String clientId = "esp32-spot-" + String(SPOT_ID);
    if (mqtt.connect(clientId.c_str())) {
      mqtt.subscribe(topicControl().c_str());
      Serial.println("MQTT connected");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pixels.begin();
  pixels.clear();
  pixels.show();

  wifiConnect();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setAll(0, 0, 255); // Blue = offline
    wifiConnect();
  }
  if (!mqtt.connected()) mqttReconnect();
  mqtt.loop();

  long d = readUltrasonicCm();
  bool occupied = (d > 0 && d < DIST_THRESHOLD_CM);

  if (reserved) {
    if (occupied) {
      setAll(255, 0, 0); // Red
      publishStatus("reserved_occupied", d);
    } else {
      setAll(255, 0, 255); // Purple
      publishStatus("reserved_free", d);
    }
  } else {
    if (occupied) {
      setAll(255, 0, 0); // Red
      publishStatus("occupied", d);
    } else {
      setAll(0, 255, 0); // Green
      publishStatus("free", d);
    }
  }

  delay(500); // update every 0.5 sec for low latency
}
