#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

//###################################################################
//WIFI configuration
SoftwareSerial esp(6,7); // RX, TX

char ssid[] = "Namli";            // your network SSID (name)
char pass[] = "9903098610";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

//char mqttServer[] = "mqtt.thingspeak.com";
char mqttServer[] = "192.168.137.48";
char ClientID[] = "Robot3";
String arrivedData;

WiFiEspClient espClient;
PubSubClient client(espClient);

void RobotInfor(){
  
  // print the SSID of the network you're attached to
  Serial.print("ClientID: ");
  Serial.println (ClientID);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.println(rssi);
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

//void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//  for (int i=0;i<length;i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//}

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
      delay(5000);
    }
  }
}

void mqttPublish (String Data){
  Serial.println ("Sending data");
  // Data
  int length = Data.length();
  char Buff[length];
  Data.toCharArray (Buff, length + 1);
  Serial.println (Buff);
  //Publish packet
  client.publish( ClientID, Buff );

  //delay(1000);
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
  RobotInfor();
  //client.subscribe ("System", 0);
  pinMode (13, OUTPUT);
}

void loop(){
  if (!client.connected()) {
    reconnect();
    client.subscribe ("System", 0);
  }
  client.loop();
  delay(100);
  //mqttPublish ("123");
  if (arrivedData == "1"){
    digitalWrite (13, HIGH);
  }else{
    digitalWrite (13, LOW);
  }
  return;
}
