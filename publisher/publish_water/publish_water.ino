/*
 * ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Publish Example
 * Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
 * Made as part of my MQTT Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
 */
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

//água
float vazao; //Variável para armazenar o valor em L/min
float media=0; //Variável para tirar a média a cada 1 minuto
int contaPulso; //Variável para a quantidade de pulsos
int i=0; //Variável para contagem
int pinSCT = D2; 

// WiFi
// Make sure to update this for your own WiFi network!
const char* ssid = "Oi_Velox_WiFi_0C60";//"Your SSID";
const char* wifi_password = "pedronat";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "192.168.0.11";//"MQTT Broker IP Address";
//const char* mqtt_server = "192.168.56.1";//"MQTT Broker IP Address";
const char* mqtt_topic_water = "agua";
const char* mqtt_username = "pi";
const char* mqtt_password = "raspberry";
//const char* mqtt_username = "PEDRO";
//const char* mqtt_password = "eremcd123";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "04061995";

long randNumber;

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 3001, wifiClient); // 3001 is the listener port for the Broker

void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // água
  pinMode(D2, INPUT);
  attachInterrupt(digitalPinToInterrupt(D2), inpulso, RISING);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  } 
}
void loop() {
  pubAgua();
}


void pubAgua(){
  contaPulso = 0;   //Zera a variável para contar os giros por segundos
  sei();      //Habilita interrupção
  delay (1000); //Aguarda 1 segundo
  cli();      //Desabilita interrupção
  
  vazao = contaPulso / 5.5; //Converte para L/min
  media=media+vazao; //Soma a vazão para o calculo da media
  i++;
  
  Serial.print(vazao); //Imprime na serial o valor da vazão
  Serial.print(" L/min - "); //Imprime L/min
  Serial.print(i); //Imprime a contagem i (segundos)
  Serial.println("s");

  if(i==60)
  {
    media = media/60; //Tira a media dividindo por 60
    Serial.print("\nMedia por minuto = "); //Imprime a frase Media por minuto =
    Serial.print(media); //Imprime o valor da media
    Serial.println(" L/min - "); //Imprime L/min
    String SerialData="";
    SerialData = String(media);
    const char* fluxo = SerialData.c_str();
    // PUBLISH to the MQTT Broker (topic = mqtt_topic_energy, defined at the beginning)
    // Here, "Button pressed!" is the Payload, but this could be changed to a sensor reading, for example.
    if (client.publish(mqtt_topic_water, fluxo)) {
      Serial.println("Valor da potencia enviado pro broken");
    }
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(mqtt_topic_water, fluxo);
    }
    media = 0; //Zera a variável media para uma nova contagem
    i=0; //Zera a variável i para uma nova contagem
    Serial.println("\n\nInicio\n\n");
  }
}

void inpulso (){ 
  contaPulso++; //Incrementa a variável de contagem dos pulsos
} 
