#include <WiFi.h>
#include <PubSubClient.h>

// ====== USER CONFIG ======
#define WIFI_SSID     "OPPO_RENO_8"
#define WIFI_PASS     "STV@9996"
#define SPOT_ID       5

#define MQTT_HOST     "10.173.206.250"
#define MQTT_PORT     1883

// ====== PINS ======
// Ultrasonic
#define TRIG_PIN   5
#define ECHO_PIN   18

// RGB LED (common cathode)
#define RED_PIN    15
#define GREEN_PIN  2
#define BLUE_PIN   4

#define DIST_THRESHOLD_CM 15   // Spot occupied if distance < 10 cm

WiFiClient espClient;
PubSubClient mqtt(espClient);

bool reserved = false;

// ===== MQTT Topics =====
String topicSpot()    { return String("parking/spot/") + String(SPOT_ID); }
String topicControl() { return String("parking/control/") + String(SPOT_ID); }

// ===== LED Control =====
void setLED(bool r, bool g, bool b) {
  digitalWrite(RED_PIN, r);
  digitalWrite(GREEN_PIN, g);
  digitalWrite(BLUE_PIN, b);
}

// ===== Ultrasonic Distance =====
long readUltrasonicCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30 ms
  long distance = duration * 0.034 / 2;           // convert to cm
  return distance;
}

// ===== MQTT Callback =====
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i=0; i<length; i++) msg += (char)payload[i];

  if (msg == "reserved") {
    reserved = true;
  } else if (msg == "clear") {
    reserved = false;
  }
}

// ===== WiFi & MQTT Connect =====
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void connectMQTT() {
  while (!mqtt.connected()) {
    if (mqtt.connect("ESP32Client")) {
      mqtt.subscribe(topicControl().c_str());
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  connectWiFi();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(callback);
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  long distance = readUltrasonicCm();
  bool occupied = (distance > 0 && distance < DIST_THRESHOLD_CM);

  // LED logic
  if (reserved && occupied) {
    setLED(1,1,1);   // Yellow
  } else if (reserved && !occupied) {
    setLED(0,0,1);   // Blue
  } else if (!reserved && occupied) {
    setLED(1,0,0);   // Red
  } else {
    setLED(0,1,0);   // Green
  }

  // Status
  String status;
  if (reserved && occupied)       status = "reserved_occupied";
  else if (reserved && !occupied) status = "reserved_free";
  else if (occupied)              status = "occupied";
  else                            status = "free";

  // Publish JSON
  String payload = "{\"spot\":" + String(SPOT_ID) +
                   ",\"status\":\"" + status + "\"" +
                   ",\"distance_cm\":\"" + String(distance) + "\"" +
                   ",\"ip\":\"" + WiFi.localIP().toString() + "\"}";

  mqtt.publish(topicSpot().c_str(), payload.c_str());

  delay(2000);
}
