// ***************************************************************************
// Example of connection to AWS IoT from ESP8266 with Arduino IDE
// using MQTT with client certificate
//
// Written by Javier Yanez
//
// based on the example of PubSubClient:
// https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_esp8266/mqtt_esp8266.ino
// ***************************************************************************

#include <ESP8266WiFi.h>
#include <PubSubClient.h> //https://pubsubclient.knolleary.net/

//incomplete array
unsigned char private_der[] = {
  0x30, 0x82, 0x04, 0xa3, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
  0xae, 0x59, 0x05, 0xf3, 0x01, 0x27, 0x6f, 0xab, 0x3d, 0x6c, 0xdf, 0x1a,
  0x2b, 0x42, 0x06, 0x5e, ...
};
unsigned int private_der_len = 1191;

//incomplete array
unsigned char cert_der[] = {
  0x30, 0x82, 0x03, 0x5a, 0x30, 0x82, 0x02, 0x42, 0xa0, 0x03, 0x02, 0x01,
  0x02, 0x02, 0x15, 0x00, 0xba, 0x70, 0x77, 0x25, 0xdd, 0x0f, 0x56, 0xcb,
  0x6c, 0xb7, 0x17, 0x0f, ...
};
unsigned int cert_der_len = 862;

const char* ssid = "your_ssid";
const char* password = "your_password";
const char* mqtt_server = "a18fbw2ljplhhm.iot.eu-west-1.amazonaws.com";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);

  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      mqttClient.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_wifiClient() {
  wifiClient.setCertificate(cert_der, cert_der_len);
  wifiClient.setPrivateKey(private_der, private_der_len);
}

void setup_mqttClient() {
  mqttClient.setServer(mqtt_server, 8883);
  mqttClient.setCallback(callback);  
}

void setup() {
  Serial.begin (115200);
  
  setup_wifi();

  setup_wifiClient();

  setup_mqttClient();
}

void loop() {
  
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    mqttClient.publish("outTopic", msg);
  }
}
