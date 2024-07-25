#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>

WiFiMulti WiFiMulti;
SocketIOclient socketIO_Server;

char* ssid = "ECLAutomatizacion_2.4Gnormal";
char* pass = "KD6JVB8kmCRe";

long tiempo1 = 0;
long tiempo2 = 0;
long tiempoSegundos = 0;
long tiempoReset = 0;
byte joined = 0;

/***********************************************************/

StaticJsonDocument<200> getData(String data) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, data);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  return doc;
}

/***********************************************************/
void socketIOEvent(socketIOmessageType_t type, uint8_t* payload, size_t length) {
  switch (type) {


    case sIOtype_DISCONNECT:
      Serial.printf("[IOc] Disconnected!\n");
      socketIO_Server.onEvent(socketIOEvent);
      break;


    case sIOtype_CONNECT:
      Serial.printf("[IOc] Connected to url: %s\n", payload);
      socketIO_Server.send(sIOtype_CONNECT, "/");
      break;


    case sIOtype_EVENT:
      {
        char* sptr = NULL;
        int id = strtol((char*)payload, &sptr, 10);
        Serial.printf("[IOc] get event: %s id: %d\n", payload, id);

        String str = (char*)payload;
        Serial.print(str);
      }
      break;


    case sIOtype_ACK:
      Serial.printf("[IOc] get ack: %u\n", length);
      break;


    case sIOtype_ERROR:
      Serial.printf("[IOc] get error: %u\n", length);
      break;


    case sIOtype_BINARY_EVENT:
      Serial.printf("[IOc] get binary: %u\n", length);
      break;


    case sIOtype_BINARY_ACK:
      Serial.printf("[IOc] get binary ack: %u\n", length);
      break;
  }
}

/////////// SETUP ////////////////
void setup() {
  Serial.begin(115200);
  	WiFiMulti.addAP("CyberSphere2.4", "KD6JVB8kmCRe");

	//WiFi.disconnect();
    Serial.println("Conectado");
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}
  Serial.println("Conectado");
  socketIO_Server.begin("192.168.100.163", 5000, "/socket.io/?EIO=4");
  socketIO_Server.onEvent(socketIOEvent);
  Serial.println("Conectado a websocket socketio");
  tiempo1 = millis();
}
/////////// LOOP ////////////////
void loop() {
  socketIO_Server.loop();
  tiempo2 = millis();
  if (tiempo2 > (tiempo1 + 1000)) {
    tiempo1 = millis();
    //Este tiempo es utilizado para el envio de acuse hacia el servidor cada 30 sg
    tiempoSegundos++;
    //Este tiempo se utiliza para reiniciar el microcontrolador si no recibe una respuesta del servidor cada 60 sg
    if (tiempoSegundos >= 10) {
      tiempoSegundos = 0;
      if (joined == 0) {
        joinGroup();
        joined++;
      }
      SendFeedBack();
    }
  }
  ///////////////////////////////////////////////////////////////////////////////
}
void SendFeedBack() {
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("sendMessageToGroup");
  JsonObject param1 = array.createNestedObject();
  param1["groupName"] = "group1";
  param1["message"] = "holaGrupo1";
  String output;
  serializeJson(doc, output);
  socketIO_Server.sendEVENT(output);
  //Serial.println(output);
}

void joinGroup() {
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("joinGroup");
  JsonObject param1 = array.createNestedObject();
  param1["group"] = "group1";
  String output;
  serializeJson(doc, output);
  socketIO_Server.sendEVENT(output);
  //Serial.println(output);
}
