#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "ESPAsyncTCP.h"
#include <SoftwareSerial.h>

const char* ssid = "June";
const char* password = "wifipassword";

AsyncServer server(23);
AsyncClient *client;
SoftwareSerial mySerial(D1, D2); // RX, TX

void setup(){
  Serial.begin(115200);
  mySerial.begin(9600); // Connect to ir controller
  
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.printf("WiFi Connected!\n");
  Serial.println(WiFi.localIP());

  // start tcp server
  server.onClient(onClient, 0);
  server.begin();
}

void sendOK() {
  String buf = "OK!";
  if(client && client->connected()) {
    const char* foo = buf.c_str();
    char bar[buf.length() + 1];
    strcpy(bar, foo);
    bar[buf.length()] = '\n'; //'\0';
    client->write(bar, sizeof(bar));
  }
}

void onPoll(void *obj, AsyncClient* c){
  Serial.printf("onPoll:%lu\n", millis());

}

void onError(void *obj, AsyncClient* c, int8_t error){
  Serial.printf("onError:%d\n", error);
}

void onAck(void *obj, AsyncClient* c, size_t len, uint32_t time){
  Serial.printf("onAck:%u:%u\n", len, time);
}

void onDisconnect(void *obj, AsyncClient* c){
  Serial.printf("onDisconnect\n");
  c->free();
  delete c;
  client = NULL;
}

void onTimeout(void *obj, AsyncClient* c, uint32_t time){
  Serial.printf("onTimeout:%u\n", time);
  client->close();
}

void onData(void *obj, AsyncClient* c, void *data, size_t len){
  ((char * )data)[len] = 0;
  String content = String((char *) data);

  Serial.print("onData:");
  Serial.println(content);
  
  if(content.substring(0,3) == "227") {
      Serial.println("Send IR code ..");
      
      uint8_t irArray[512];
      unsigned int idx = 0;
      char *irCode = const_cast<char*>(content.c_str());
      char *str;
      
      while ((str = strtok_r(irCode, ",", &irCode)) != NULL) {
        uint8_t ir = (uint8_t)atoi(str + '\0');
        irArray[idx++] = ir;
      }
      
      mySerial.write((uint8_t*)irArray, idx);
      sendOK();
  }   
}

void loop(){
  if(client && client->connected()) {
      while(!client->canSend()) yield();
  }
}

void onClient(void *obj, AsyncClient* c){
  if(client){
    os_printf("r\n");
    if(client->connected())
      client->close();
  }
  os_printf("c\n");
  client = c;
  client->onError(onError);
  client->onAck(onAck);
  client->onDisconnect(onDisconnect);
  client->onTimeout(onTimeout);
  client->onData(onData);
  //client->onPoll(onPoll);
}
