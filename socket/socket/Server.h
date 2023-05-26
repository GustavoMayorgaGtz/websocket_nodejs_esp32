#include <WiFi.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>

SocketIOclient socketIO;
WiFiClient client;

#define ssid "ECLAutomatizacion_2.4Gnormal"
#define pass "KD6JVB8kmCRe"

long tiempo1 = 0;
long tiempo2 = 0;
long tiempoSegundos = 0;
long tiempoReset = 0;
byte joined = 0;

/***********************************************************/

StaticJsonDocument<200> getData(String data)
{
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, data);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  return  doc;
}

/***********************************************************/
void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch (type) {


    case sIOtype_DISCONNECT:
      Serial.printf("[IOc] Disconnected!\n");
      socketIO.onEvent(socketIOEvent);
      break;


    case sIOtype_CONNECT:
      Serial.printf("[IOc] Connected to url: %s\n", payload);
      socketIO.send(sIOtype_CONNECT, "/");
      break;


    case sIOtype_EVENT:
      {
        char * sptr = NULL;
        int id = strtol((char *)payload, &sptr, 10);
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

/***********************************************************/
/***********************************************************/
void ServerSetup()
{
  WiFi.begin(ssid, pass);
  Serial.println("Conectando a la red");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(3000);
 
  }
  Serial.println("Conectado");
  socketIO.begin("192.168.100.187", 5000, "/socket.io/?EIO=4");
  socketIO.onEvent(socketIOEvent);
  tiempo1 = millis();
}

/***********************************************************/
//Mandar acuse de conexion con servidor
// void Acuse()
// {
//   // creat JSON message for Socket.IO (event)
//   DynamicJsonDocument doc(1024);
//   JsonArray array = doc.to<JsonArray>();

//   // add evnet name
//   // Hint: socket.on('Light', ....
//   array.add("Light");
//   // add payload (parameters) for the event
//   JsonObject param1 = array.createNestedObject();
//   param1["Acuse"] = true;

//   // JSON to String (serializion)
//   String output;
//   serializeJson(doc, output);

//   // Send event
//   socketIO.sendEVENT(output);

//   // Print JSON for debugging
//   Serial.println(output);
// }
/***********************************************************/
void SendFeedBack()
{
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("sendMessageToGroup");
  JsonObject param1 = array.createNestedObject();
  param1["groupName"] = "group1";
  param1["message"] = "holaGrupo1";
  String output;
  serializeJson(doc, output);
  socketIO.sendEVENT(output);
  //Serial.println(output);
}

void joinGroup()
{
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("joinGroup");
  JsonObject param1 = array.createNestedObject();
  param1["group"] = "group1";
  String output;
  serializeJson(doc, output);
  socketIO.sendEVENT(output);
  //Serial.println(output);
}

void ServerLoop()
{
  socketIO.loop();
  tiempo2 = millis();
  if (tiempo2 > (tiempo1 + 1000)) {
    tiempo1 = millis();
    //Este tiempo es utilizado para el envio de acuse hacia el servidor cada 30 sg
    tiempoSegundos++;
    //Este tiempo se utiliza para reiniciar el microcontrolador si no recibe una respuesta del servidor cada 60 sg
    if (tiempoSegundos >= 10)
    {
      tiempoSegundos = 0;
      if(joined == 0){
        joinGroup();
        joined++;
      }
    SendFeedBack();
    }
  }
}
