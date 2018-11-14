#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

//###################################################################
//WIFI configuration
SoftwareSerial esp(6 ,7); // RX, TX

char ssid[] = "Namli";            // your network SSID (name)
char pass[] = "9903098610";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char mqttServer[] = "mqtt.thingspeak.com";
//char mqttServer[] = "171.253.28.46";
char ClientID[] = "Station1";
String arrivedData;

WiFiEspClient espClient;
PubSubClient client(espClient);

//###################################################################
//DHT configuration
DHT dht(5, DHT11);

//######################################################################
//Delay
int count;
unsigned long sec;

void delay_ms (uint16_t millisecond) {
  sec = ((16000000/12)/1000)*millisecond;
  for (count = 0; count < sec; count ++);
}
//###############################################################

void RobotInfor(){
  
  // print the SSID of the network you're attached to
  Serial.print("###ClientID: ");
  Serial.print (ClientID);

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("---IP Address: ");
  Serial.println(ip);
}

void ConnectToWiFi (){
    esp.begin(9600);
  // initialize ESP module
  WiFi.init(&esp);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("ESP is not connected");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
}

void callback(char* topic, byte* payload, unsigned int length) {
  arrivedData = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    arrivedData += (char)payload[i];
  }
  Serial.println();
  Serial.println("########################");
  Serial.println(arrivedData);
  Serial.println("########################");
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println ("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect(ClientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay_ms(5000);
    }
  }
}

void mqttPublish (){
  Serial.println ("Sending data");
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  // Data
  String Data = String(h) + String (";") + String(t);
  int length = Data.length();
  char Buff[length];
  Data.toCharArray (Buff, length + 1);
  //Publish packet
  client.publish( ClientID, Buff );
  Serial.println (Data);
  return;
}

void setup()
{
  //###################
  //General 
  Serial.begin(9600);
  //####################
  //ESP
  ConnectToWiFi ();
  client.setServer (mqttServer, 1883);
  client.setCallback (callback);
  //####################
  //DHT
  dht.begin ();
  RobotInfor();
  
}

void loop(){
    if (!client.connected()) {
    reconnect();
    client.subscribe ("System", 0);
  }
  client.loop();
  delay_ms (2000);
  mqttPublish();
  return;
}

//void loop (){
//  Serial.println ("1");
//  delay_ms (1000);
//  Serial.println ("0");
//  delay_ms (1000);
//}

